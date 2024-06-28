/**
 * @FilePath     : /cola/src/Dag/DagElement/DGroup/DSome/DSome.inl
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-26 15:26:20
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-26 15:53:29
 */

#ifndef NAO_DSOME_INL
#define NAO_DSOME_INL

#include "DSome.h"
NAO_NAMESPACE_BEGIN

template<NInt TriggerNum>
DSome<TriggerNum>::DSome()
{
    element_type_ = DElementType::SOME;
    session_      = URandom<>::generateSession(NAO_STR_SOME);
}


template<NInt TriggerNum>
NStatus DSome<TriggerNum>::addElement(DElementPtr element)
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_INIT(false)
    NAO_ASSERT_NOT_NULL(element)

    /**
     * GSome中插入的元素，必须要有 timeout的设定，如果没有的话，强行写到max值。
     * 用于确保在pipeline run执行完成之前，所有的 element 都会被回收
     */
    if (NAO_DEFAULT_ELEMENT_TIMEOUT == element->timeout_) {
        element->setTimeout(NAO_MAX_BLOCK_TTL, DElementTimeoutStrategy::HOLD_BY_PIPELINE);
    }

    group_elements_arr_.emplace_back(element);
    NAO_FUNCTION_END
}


template<NInt TriggerNum>
NStatus DSome<TriggerNum>::run()
{
    NAO_FUNCTION_BEGIN

    left_num_   = TriggerNum;   // 还剩n个，就完成当前GSome的执行逻辑
    cur_status_ = CStatus();

    /**
     * 1. 并发的执行，其中的所有逻辑信息
     * 2. 等待 Trigger Num 个element执行完成
     * 3. 将所有未执行完的element 设置为timeout
     * 4. 赋返回值
     */
    for (auto* element : group_elements_arr_) {
        thread_pool_->commit(
            [this, element] {
                auto curStatus = element->fatProcessor(NFunctionType::RUN);
                {
                    NAO_UNIQUE_LOCK lock(lock_);
                    cur_status_ += curStatus;
                    left_num_--;
                }
                cv_.notify_one();
            },
            NAO_POOL_TASK_STRATEGY);
    }

    NAO_UNIQUE_LOCK lock(lock_);
    cv_.wait(lock, [this] { return left_num_ <= 0 || cur_status_.isErr(); });

    for (DElementPtr element : group_elements_arr_) {
        if (!element->done_) {
            element->cur_state_.store(DElementState::TIMEOUT, std::memory_order_release);
        }
    }
    status = cur_status_;
    NAO_FUNCTION_END
}


template<NInt TriggerNum>
NBool DSome<TriggerNum>::isSerializable() const
{
    return false;   // 情况较为复杂，默认不可以
}


template<NInt TriggerNum>
NVoid DSome<TriggerNum>::dump(std::ostream& oss)
{
    dumpElement(oss);
    dumpGroupLabelBegin(oss);
    oss << 'p' << this << "[shape=point height=0];\n";
    oss << "color=blue;style=dashed;\n";   // 蓝色虚线

    for (const auto& element : group_elements_arr_) {
        element->dump(oss);
    }

    dumpGroupLabelEnd(oss);

    for (const auto& element : run_before_) {
        dumpEdge(oss, this, element);
    }
}


template<NInt TriggerNum>
NBool DSome<TriggerNum>::isHold()
{
    // 这里固定是不可以 hold的
    return false;
}


template<NInt TriggerNum>
NStatus DSome<TriggerNum>::checkSuitable()
{
    NAO_FUNCTION_BEGIN
    status = DElement::checkSuitable();
    NAO_FUNCTION_CHECK_STATUS

    NAO_RETURN_ERROR_STATUS_BY_CONDITION((NAO_DEFAULT_LOOP_TIMES != loop_), "DSome cannot set loop > 1.")
    NAO_RETURN_ERROR_STATUS_BY_CONDITION((0 >= TriggerNum), "trigger num must bigger than 0.")
    NAO_RETURN_ERROR_STATUS_BY_CONDITION((group_elements_arr_.size() < TriggerNum),
                                         "this DSome need at least [" + std::to_string(TriggerNum) + "] element.")
    NAO_RETURN_ERROR_STATUS_BY_CONDITION(
        std::any_of(group_elements_arr_.begin(), group_elements_arr_.end(), [](DElementPtr ptr) { return !ptr->isAsync(); }),
        "DSome contains async node only.")

    NAO_FUNCTION_END
}

NAO_NAMESPACE_END

#endif   // NAO_DSOME_INL