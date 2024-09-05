/**
 * @FilePath     : /cola/cola/Dag/DagElement/_DEngine/DDynamicEngine/DDynamicEngine.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-26 11:36:19
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-09-05 10:22:34
 **/

#include "DDynamicEngine.h"

NAO_NAMESPACE_BEGIN

NStatus DDynamicEngine::setup(const DSortedDElementPtrSet& elements)
{
    NAO_FUNCTION_BEGIN
    /**
     * 1. 标记数据，比如有多少个结束element等
     * 2. 标记哪些数据，是linkable 的
     * 3. 分析当前dag类型信息
     */
    mark(elements);
    link(elements);
    analysisDagType(elements);
    NAO_FUNCTION_END
}


NStatus DDynamicEngine::run()
{
    NAO_FUNCTION_BEGIN
    cur_status_.reset();

    switch (dag_type_) {
    case internal::DEngineDagType::COMMON:
    {
        commonRunAll();
        break;
    }
    case internal::DEngineDagType::ALL_SERIAL:
    {
        serialRunAll();
        break;
    }
    case internal::DEngineDagType::ALL_PARALLEL:
    {
        parallelRunAll();
        break;
    }
    default:
        NAO_RETURN_ERROR_STATUS("unknown engine dag type")
    }
    status = cur_status_;
    NAO_FUNCTION_END
}


NStatus DDynamicEngine::afterRunCheck()
{
    NAO_FUNCTION_BEGIN
    /**
     * 纯串行和纯并行 是不需要做结果校验的
     * 但是普通的dag，后期还是校验一下为好
     * 这里也可以通过外部接口来关闭
     */
    if (internal::DEngineDagType::COMMON == dag_type_) {
        for (DElementCPtr element : total_element_arr_) {
            NAO_RETURN_ERROR_STATUS_BY_CONDITION(!element->done_, element->getName() + ": dynamic engine, check not run it...")
        }
    }

    NAO_FUNCTION_END
}


NVoid DDynamicEngine::commonRunAll()
{
    /**
     * 1. 执行没有任何依赖的element
     * 2. 在element执行完成之后，进行裂变，直到所有的element执行完成
     * 3. 等待异步执行结束
     */
    finished_end_size_ = 0;
    for (const auto& element : front_element_arr_) {
        process(element, element == front_element_arr_.back());
    }

    fatWait();
}

NVoid DDynamicEngine::mark(const DSortedDElementPtrSet& elements)
{
    total_element_arr_.clear();
    front_element_arr_.clear();
    total_end_size_ = 0;

    for (DElementPtr element : elements) {
        if (element->run_before_.empty()) {
            total_end_size_++;
        }

        if (element->dependence_.empty()) {
            front_element_arr_.emplace_back(element);
        }
        total_element_arr_.emplace_back(element);
    }
}


NVoid DDynamicEngine::analysisDagType(const DSortedDElementPtrSet& elements)
{
    if (total_element_arr_.empty() || (front_element_arr_.size() == 1 && total_element_arr_.size() - 1 == linked_size_)) {
        /**
         * 如果所有的信息中，只有一个是非linkable。则说明只有最后的那个是的，且只有一个开头
         * 故，这里将其认定为一条 lineal 的情况
         * ps: 只有一个或者没有 element的情况，也会被算到 ALL_SERIAL 中去
         */
        dag_type_ = internal::DEngineDagType::ALL_SERIAL;
    }
    else if (total_element_arr_.size() == total_end_size_ && front_element_arr_.size() == total_end_size_) {
        dag_type_ = internal::DEngineDagType::ALL_PARALLEL;
    }
    else {
        dag_type_ = internal::DEngineDagType::COMMON;
    }
}


NVoid DDynamicEngine::process(DElementPtr element, NBool affinity)
{
    if (unlikely(cur_status_.isErr())) {
        return;
    }

    const auto& exec = [this, element] {
        element->beforeRun();
        const NStatus& curStatus = element->fatProcessor(NFunctionType::RUN);
        if (unlikely(curStatus.isErr())) {
            // 当且仅当整体状正常，且当前状态异常的时候，进入赋值逻辑。确保不重复赋值
            NAO_LOCK_GUARD lk(status_lock_);
            cur_status_ += curStatus;
        }
        afterElementRun(element);
    };

     if (affinity && element->isDefaultBinding()) {
        // 如果 affinity=true，表示用当前的线程，执行这个逻辑。以便增加亲和性
        exec();
    } else {
        thread_pool_->execute(exec, calcIndex(element));
    }
}


NVoid DDynamicEngine::afterElementRun(DElementPtr element)
{
    element->done_ = true;
    if (!element->run_before_.empty() && cur_status_.isOK()) {
        if (internal::DElementShape::LINKABLE == element->shape_) {
            // 针对linkable 的情况，做特殊判定
            process(*(element->run_before_.begin()), true);
        }
        else {
            DElementPtr reserved = nullptr;
            for (auto* cur : element->run_before_) {
                if (--cur->left_depend_ <= 0) {
                    if (reserved) {
                        process(cur, false);
                    }
                    else {
                        reserved = cur;   // 留一个作为亲和性的，在当前线程运行
                    }
                }
            }
            reserved ? process(reserved, true) : void();
        }
    }
    else {
        NAO_LOCK_GUARD lock(lock_);
        /**
         * 满足一下条件之一，则通知wait函数停止等待
         * 1，无后缀节点全部执行完毕(在运行正常的情况下，只有无后缀节点执行完成的时候，才可能整体运行结束)
         * 2，有节点执行状态异常
         */
        if ((element->run_before_.empty() && (++finished_end_size_ >= total_end_size_)) || cur_status_.isErr()) {
            cv_.notify_one();
        }
    }
}


NVoid DDynamicEngine::fatWait()
{
    NAO_UNIQUE_LOCK lock(lock_);
    cv_.wait(lock, [this] {
        /**
         * 遇到以下条件之一，结束执行：
         * 1，执行结束
         * 2，状态异常
         */
        return (finished_end_size_ >= total_end_size_) || cur_status_.isErr();
    });
}


#ifdef _NAO_PARALLEL_MICRO_BATCH_ENABLE_
NVoid GDynamicEngine::parallelRunAll()
{
    // 微任务模式，主要用于性能测试的场景下
    const UThreadPoolConfig& config = thread_pool_->getConfig();
    NSize                    thdNum = config.default_thread_size_ + config.secondary_thread_size_;
    NAO_THROW_EXCEPTION_BY_CONDITION(thdNum <= 0, "default thread size cannot smaller than 1");

    std::vector<std::future<NStatus>> futures;
    NSize                             taskNumPerThd = total_end_size_ / thdNum + (NSize)(0 != total_end_size_ % thdNum);
    for (int i = 0; i < thdNum; i++) {
        DElementPtrArr elements;
        for (int j = 0; j < taskNumPerThd; j++) {
            auto cur = i * taskNumPerThd + j;
            if (cur < total_end_size_) {
                elements.emplace_back(front_element_arr_[cur]);
            }
        }
        auto curFut = thread_pool_->commit([elements] {
            NAO_FUNCTION_BEGIN
            for (auto* element : elements) {
                status += element->fatProcessor(NFunctionType::RUN);
                NAO_FUNCTION_CHECK_STATUS
            }
            NAO_FUNCTION_END;
        });
        futures.emplace_back(std::move(curFut));
    }

    for (auto& fut : futures) {
        cur_status_ += fut.get();
    }
}
#else
NVoid DDynamicEngine::parallelRunAll()
{
    /**
     * 主要适用于dag是纯并发逻辑的情况
     * 直接并发的执行所有的流程，从而减少调度损耗
     * 实测效果，在32路纯并行的情况下，整体耗时从 21.5s降低到 12.5s
     * 非纯并行逻辑，不走此函数
     */
    std::vector<std::future<NStatus>> futures;
    futures.reserve(total_end_size_);
    for (int i = 0; i < total_end_size_; i++) {
        futures.emplace_back(std::move(thread_pool_->commit([this, i] {
            return total_element_arr_[i]->fatProcessor(NFunctionType::RUN);
        }, calcIndex(total_element_arr_[i]))));
    }

    for (auto& fut : futures) {
        cur_status_ += fut.get();
    }
}
#endif


NVoid DDynamicEngine::serialRunAll()
{
    /**
     * 如果分析出来 dag是一个链式的，则直接依次执行element
     * 直到所有element都执行完成，或者有出现错误的返回值
     */
    DElementPtr cur = (front_element_arr_.empty()) ? nullptr : front_element_arr_.front();
    while (cur) {
        cur_status_ += cur->fatProcessor(NFunctionType::RUN);
        if (cur->run_before_.empty() || cur_status_.isErr()) {
            break;
        }
        cur = *(cur->run_before_.begin());
    }
}

NAO_NAMESPACE_END
