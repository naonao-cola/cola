/**
 * @FilePath     : /cola/src/UtilsCtrl/ThreadPool/Thread/UThreadSecondary.h
 * @Description  :
 * @Author       : naonao
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-20 19:54:17
 **/
#ifndef NAO_UTHREADSECONDARY_H
#define NAO_UTHREADSECONDARY_H

#include "UThreadBase.h"

NAO_NAMESPACE_BEGIN

class UThreadSecondary : public UThreadBase
{
public:
    explicit UThreadSecondary()
    {
        cur_ttl_ = 0;
        type_    = NAO_THREAD_TYPE_SECONDARY;
    }


protected:
    NStatus init() override
    {
        NAO_FUNCTION_BEGIN
        NAO_ASSERT_INIT(false)
        NAO_ASSERT_NOT_NULL(config_)

        cur_ttl_ = config_->secondary_thread_ttl_;
        is_init_ = true;
        thread_  = std::move(std::thread(&UThreadSecondary::run, this));
        setSchedParam();
        NAO_FUNCTION_END
    }


    /**
     * 设置pool的信息
     * @param poolTaskQueue
     * @param poolPriorityTaskQueue
     * @param config
     * @return
     */
    NStatus setThreadPoolInfo(UAtomicQueue<UTask>*         poolTaskQueue,
                              UAtomicPriorityQueue<UTask>* poolPriorityTaskQueue,
                              UThreadPoolConfigPtr         config)
    {
        NAO_FUNCTION_BEGIN
        NAO_ASSERT_INIT(false)   // 初始化之前，设置参数
        NAO_ASSERT_NOT_NULL(poolTaskQueue, poolPriorityTaskQueue, config)

        this->pool_task_queue_          = poolTaskQueue;
        this->pool_priority_task_queue_ = poolPriorityTaskQueue;
        this->config_                   = config;
        NAO_FUNCTION_END
    }


    NStatus run() final
    {
        NAO_FUNCTION_BEGIN
        NAO_ASSERT_INIT(true)

        status = loopProcess();
        NAO_FUNCTION_END
    }


    NVoid processTask() override
    {
        UTask task;
        if (popPoolTask(task)) {
            runTask(task);
        }
        else {
            // 如果单次无法获取，则稍加等待
            waitRunTask(config_->queue_emtpy_interval_);
        }
    }


    NVoid processTasks() override
    {
        UTaskArr tasks;
        if (popPoolTask(tasks)) {
            runTasks(tasks);
        }
        else {
            waitRunTask(config_->queue_emtpy_interval_);
        }
    }


    /**
     * 有等待的执行任务
     * @param ms
     * @return
     * @notice 目的是降低cpu的占用率
     */
    NVoid waitRunTask(NMSec ms)
    {
        auto task = this->pool_task_queue_->popWithTimeout(ms);
        if (nullptr != task) {
            (*task)();
        }
    }


    /**
     * 判断本线程是否需要被自动释放
     * @return
     */
    bool freeze()
    {
        if (likely(is_running_)) {
            cur_ttl_++;
            cur_ttl_ = std::min(cur_ttl_, config_->secondary_thread_ttl_);
        }
        else {
            cur_ttl_--;   // 如果当前线程没有在执行，则ttl-1
        }

        return cur_ttl_ <= 0 && done_;   // 必须是正在执行的线程，才可以被回收
    }

private:
    NSec cur_ttl_ = 0;   // 当前最大生存周期

    friend class UThreadPool;
};

using UThreadSecondaryPtr = UThreadSecondary*;

NAO_NAMESPACE_END

#endif   // NAO_UTHREADSECONDARY_H
