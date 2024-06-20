﻿/**
 * @FilePath     : /cola/src/UtilsCtrl/ThreadPool/Thread/UThreadBase.h
 * @Description  :
 * @Author       : naonao
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-20 19:53:51
 **/
#ifndef CGRAPH_UTHREADBASE_H
#define CGRAPH_UTHREADBASE_H

#include <thread>

#include "../Metrics/UMetrics.h"
#include "../Queue/UQueueInclude.h"
#include "../Task/UTaskInclude.h"
#include "../UThreadObject.h"
#include "../UThreadPoolConfig.h"



NAO_NAMESPACE_BEGIN

class UThreadBase : public UThreadObject
{
protected:
    explicit UThreadBase()
    {
        done_                     = true;
        is_init_                  = false;
        is_running_               = false;
        pool_task_queue_          = nullptr;
        pool_priority_task_queue_ = nullptr;
        config_                   = nullptr;
        total_task_num_           = 0;
    }


    ~UThreadBase() override { reset(); }


    NStatus destroy() override
    {
        NAO_FUNCTION_BEGIN
        NAO_ASSERT_INIT(true)

        reset();
        NAO_FUNCTION_END
    }


    /**
     * 从线程池的队列中，获取任务
     * @param task
     * @return
     */
    virtual NBool popPoolTask(UTaskRef task)
    {
        NBool result = pool_task_queue_->tryPop(task);
        if (!result && NAO_THREAD_TYPE_SECONDARY == type_) {
            // 如果辅助线程没有获取到的话，还需要再尝试从长时间任务队列中，获取一次
            result = pool_priority_task_queue_->tryPop(task);
        }
        metrics_.calcPool(result, 1);
        return result;
    }


    /**
     * 从线程池的队列中中，获取批量任务
     * @param tasks
     * @return
     */
    virtual NBool popPoolTask(UTaskArrRef tasks)
    {
        NBool result = pool_task_queue_->tryPop(tasks, config_->max_pool_batch_size_);
        if (!result && NAO_THREAD_TYPE_SECONDARY == type_) {
            result = pool_priority_task_queue_->tryPop(tasks, 1);   // 从优先队列里，最多pop出来一个
        }

        metrics_.calcPool(result, tasks.size());
        return result;
    }


    /**
     * 执行单个任务
     * @param task
     */
    NVoid runTask(UTask& task)
    {
        is_running_ = true;
        task();
        total_task_num_++;
        is_running_ = false;
    }


    /**
     * 批量执行任务
     * @param tasks
     */
    NVoid runTasks(UTaskArr& tasks)
    {
        is_running_ = true;
        for (auto& task : tasks) {
            task();
        }
        total_task_num_ += tasks.size();
        is_running_ = false;
    }


    /**
     * 清空所有任务内容
     */
    NVoid reset()
    {
        done_ = false;
        cv_.notify_one();   // 防止主线程 wait时间过长，导致的结束缓慢问题
        if (thread_.joinable()) {
            thread_.join();   // 等待线程结束
        }
        is_init_        = false;
        is_running_     = false;
        total_task_num_ = 0;
    }

    /**
     * 执行单个消息
     * @return
     */
    virtual NVoid processTask() = 0;


    /**
     * 获取批量执行task信息
     */
    virtual NVoid processTasks() = 0;


    /**
     * 循环处理任务
     * @return
     */
    NStatus loopProcess()
    {
        NAO_FUNCTION_BEGIN
        NAO_ASSERT_NOT_NULL(config_)

        if (config_->batch_task_enable_) {
            while (done_) {
                processTasks();   // 批量任务获取执行接口
            }
        }
        else {
            while (done_) {
                processTask();   // 单个任务获取执行接口
            }
        }

        NAO_FUNCTION_END
    }


    /**
     * 设置线程优先级，仅针对非windows平台使用
     */
    NVoid setSchedParam()
    {
#ifndef _WIN32
        int priority = NAO_THREAD_SCHED_OTHER;
        int policy   = NAO_THREAD_MIN_PRIORITY;
        if (type_ == NAO_THREAD_TYPE_PRIMARY) {
            priority = config_->primary_thread_priority_;
            policy   = config_->primary_thread_policy_;
        }
        else if (type_ == NAO_THREAD_TYPE_SECONDARY) {
            priority = config_->secondary_thread_priority_;
            policy   = config_->secondary_thread_policy_;
        }

        auto        handle = thread_.native_handle();
        sched_param param  = {calcPriority(priority)};
        int         ret    = pthread_setschedparam(handle, calcPolicy(policy), &param);
        if (0 != ret) {
            NAO_ECHO("warning : set thread sched param failed, system error code is [%d]", ret);
        }
#endif
    }

    /**
     * 设置线程亲和性，仅针对linux系统
     */
    NVoid setAffinity(int index)
    {
#if defined(__linux__) && !defined(__ANDROID__)
        if (!config_->bind_cpu_enable_ || NAO_CPU_NUM == 0 || index < 0) {
            return;
        }

        cpu_set_t mask;
        CPU_ZERO(&mask);
        CPU_SET(index % NAO_CPU_NUM, &mask);

        auto handle = thread_.native_handle();
        int  ret    = pthread_setaffinity_np(handle, sizeof(cpu_set_t), &mask);
        if (0 != ret) {
            NAO_ECHO("warning : set thread affinity failed, system error code is [%d]", ret);
        }
#endif
    }


private:
    /**
     * 设定计算线程调度策略信息，
     * 非OTHER/RR/FIFO对应数值，统一返回OTHER类型
     * @param policy
     * @return
     */
    static int calcPolicy(int policy)
    {
        return (NAO_THREAD_SCHED_OTHER == policy || NAO_THREAD_SCHED_RR == policy ||
                NAO_THREAD_SCHED_FIFO == policy)
                   ? policy
                   : NAO_THREAD_SCHED_OTHER;
    }


    /**
     * 设定线程优先级信息
     * 超过[min,max]范围，统一设置为min值
     * @param priority
     * @return
     */
    static int calcPriority(int priority)
    {
        return (priority >= NAO_THREAD_MIN_PRIORITY && priority <= NAO_THREAD_MAX_PRIORITY)
                   ? priority
                   : NAO_THREAD_MIN_PRIORITY;
    }


protected:
    bool          done_;                 // 线程状态标记
    bool          is_init_;              // 标记初始化状态
    bool          is_running_;           // 是否正在执行
    int           type_           = 0;   // 用于区分线程类型（主线程、辅助线程）
    unsigned long total_task_num_ = 0;   // 处理的任务的数字

    UAtomicQueue<UTask>* pool_task_queue_;   // 用于存放线程池中的普通任务
    UAtomicPriorityQueue<UTask>*
        pool_priority_task_queue_;   // 用于存放线程池中的包含优先级任务的队列，仅辅助线程可以执行
    UThreadPoolConfigPtr config_ = nullptr;   // 配置参数信息
    UMetrics             metrics_;            // 线程中的指标信息

    std::thread             thread_;   // 线程类
    std::mutex              mutex_;
    std::condition_variable cv_;
};

NAO_NAMESPACE_END

#endif   // NAO_UTHREADBASE_H
