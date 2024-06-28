﻿/**
 * @FilePath     : /cola/src/UtilsCtrl/ThreadPool/Thread/UThreadPrimary.h
 * @Description  : 核心线程，处理任务中
 * @Author       : naonao
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-20 19:54:09
 **/
#ifndef NAO_UTHREADPRIMARY_H
#define NAO_UTHREADPRIMARY_H

#include "../UThreadObject.h"
#include "UThreadBase.h"
#include <mutex>
#include <vector>


NAO_NAMESPACE_BEGIN

class UThreadPrimary : public UThreadBase
{
protected:
    explicit UThreadPrimary()
    {
        index_        = NAO_SECONDARY_THREAD_COMMON_ID;
        pool_threads_ = nullptr;
        type_         = NAO_THREAD_TYPE_PRIMARY;
    }


    NStatus init() override
    {
        NAO_FUNCTION_BEGIN
        NAO_ASSERT_INIT(false)
        NAO_ASSERT_NOT_NULL(config_)

        is_init_ = true;
        metrics_.reset();
        buildStealTargets();
        thread_ = std::move(std::thread(&UThreadPrimary::run, this));
        setSchedParam();
        setAffinity(index_);
        NAO_FUNCTION_END
    }


    /**
     * 注册线程池相关内容，需要在init之前使用
     * @param index
     * @param poolTaskQueue
     * @param poolThreads
     * @param config
     */
    NStatus setThreadPoolInfo(int index, UAtomicQueue<UTask>* poolTaskQueue,
                              std::vector<UThreadPrimary*>* poolThreads,
                              UThreadPoolConfigPtr          config)
    {
        NAO_FUNCTION_BEGIN
        NAO_ASSERT_INIT(false)   // 初始化之前，设置参数
        NAO_ASSERT_NOT_NULL(poolTaskQueue, poolThreads, config)

        this->index_           = index;
        this->pool_task_queue_ = poolTaskQueue;
        this->pool_threads_    = poolThreads;
        this->config_          = config;
        NAO_FUNCTION_END
    }


    /**
     * 线程执行函数
     * @return
     */
    NStatus run() final
    {
        NAO_FUNCTION_BEGIN
        NAO_ASSERT_INIT(true)
        NAO_ASSERT_NOT_NULL(pool_threads_)

        /**
         * 线程池中任何一个primary线程为null都不可以执行
         * 防止线程初始化失败的情况，导致的崩溃
         * 理论不会走到这个判断逻辑里面
         */
        if (std::any_of(pool_threads_->begin(), pool_threads_->end(), [](UThreadPrimary* thd) {
                return nullptr == thd;
            })) {
            NAO_RETURN_ERROR_STATUS("primary thread is null")
        }

        status = loopProcess();
        NAO_FUNCTION_END
    }


    NVoid processTask() override
    {
        UTask task;
        if (popTask(task) || popPoolTask(task) || stealTask(task)) {
            runTask(task);
        }
        else {
            fatWait();
        }
    }


    NVoid processTasks() override
    {
        UTaskArr tasks;
        if (popTask(tasks) || popPoolTask(tasks) || stealTask(tasks)) {
            // 尝试从主线程中获取/盗取批量task，如果成功，则依次执行
            runTasks(tasks);
        }
        else {
            fatWait();
        }
    }


    /**
     * 如果总是进入无task的状态，则开始休眠
     * 休眠一定时间后，然后恢复执行状态，避免出现异常情况导致无法唤醒
     */
    NVoid fatWait()
    {
        cur_empty_epoch_++;
        metrics_.fleet_wait_times_++;
        NAO_YIELD();
        if (cur_empty_epoch_ >= config_->primary_thread_busy_epoch_) {
            NAO_UNIQUE_LOCK lk(mutex_);
            cv_.wait_for(lk, std::chrono::milliseconds(config_->primary_thread_empty_interval_));
            metrics_.deep_wait_times_++;
            cur_empty_epoch_ = 0;
        }
    }


    /**
     * 依次push到任一队列里。如果都失败，则yield，然后重新push
     * @param task
     * @return
     */
    NVoid pushTask(UTask&& task)
    {
        while (!(primary_queue_.tryPush(std::move(task)) ||
                 secondary_queue_.tryPush(std::move(task)))) {
            metrics_.local_push_yield_times_++;
            NAO_YIELD();
        }
        cur_empty_epoch_ = 0;
        metrics_.local_push_real_num_++;
        cv_.notify_one();
    }


    /**
     * 写入 task信息，是否上锁由
     * @param task
     * @param enable 确认是否有
     * @param lockable true 的时候需要上锁，false 的时候会解锁
     * @return
     */
    NVoid pushTask(UTask&& task, NBool enable, NBool lockable)
    {
        secondary_queue_.push(std::move(task),
                              enable,
                              lockable);   // 通过 second 写入，主要是方便其他的thread 进行steal操作
        if (enable && !lockable) {
            cur_empty_epoch_ = 0;
            cv_.notify_one();
        }
        metrics_.local_push_yield_times_++;
        metrics_.local_push_real_num_++;
    }


    /**
     * 从本地弹出一个任务
     * @param task
     * @return
     */
    NBool popTask(UTaskRef task)
    {
        auto result = primary_queue_.tryPop(task) || secondary_queue_.tryPop(task);
        metrics_.calcLocal(result, 1);
        return result;
    }


    /**
     * 从本地弹出一批任务
     * @param tasks
     * @return
     */
    NBool popTask(UTaskArrRef tasks)
    {
        NBool result   = primary_queue_.tryPop(tasks, config_->max_local_batch_size_);
        auto  leftSize = config_->max_local_batch_size_ - tasks.size();
        if (leftSize > 0) {
            // 如果凑齐了，就不需要了。没凑齐的话，就继续
            result |= (secondary_queue_.tryPop(tasks, leftSize));
        }
        metrics_.calcLocal(result, tasks.size());
        return result;
    }


    /**
     * 从其他线程窃取一个任务
     * @param task
     * @return
     */
    NBool stealTask(UTaskRef task)
    {
        if (unlikely(pool_threads_->size() < config_->default_thread_size_)) {
            /**
             * 线程池还未初始化完毕的时候，无法进行steal。
             * 确保程序安全运行。
             */
            return false;
        }

        /**
         * 窃取的时候，仅从相邻的primary线程中窃取
         * 待窃取相邻的数量，不能超过默认primary线程数
         */
        NBool result = false;
        for (auto& target : steal_targets_) {
            /**
             * 从线程中周围的thread中，窃取任务。
             * 如果成功，则返回true，并且执行任务。
             * steal 的时候，先从第二个队列里偷，从而降低触碰锁的概率
             */
            if (likely((*pool_threads_)[target]) &&
                    (((*pool_threads_)[target])->secondary_queue_.trySteal(task)) ||
                ((*pool_threads_)[target])->primary_queue_.trySteal(task)) {
                result = true;
                break;
            }
        }

        metrics_.calcSteal(result, 1);
        return result;
    }


    /**
     * 从其他线程盗取一批任务
     * @param tasks
     * @return
     */
    NBool stealTask(UTaskArrRef tasks)
    {
        if (unlikely(pool_threads_->size() != config_->default_thread_size_)) {
            return false;
        }

        NBool result = false;
        for (auto& target : steal_targets_) {
            if (likely((*pool_threads_)[target])) {
                result = ((*pool_threads_)[target])
                             ->secondary_queue_.trySteal(tasks, config_->max_steal_batch_size_);
                auto leftSize = config_->max_steal_batch_size_ - tasks.size();
                if (leftSize > 0) {
                    result |= ((*pool_threads_)[target])->primary_queue_.trySteal(tasks, leftSize);
                }

                if (result) {
                    /**
                     * 在这里，我们对模型进行了简化。实现的思路是：
                     * 尝试从邻居主线程(先secondary，再primary)中，获取 x(=max_steal_batch_size_)
                     * 个task， 如果从某一个邻居中，获取了 y(<=x) 个task，则也终止steal的流程
                     * 且如果如果有一次批量steal成功，就认定成功
                     */
                    break;
                }
            }
        }

        metrics_.calcSteal(result, tasks.size());
        return result;
    }


    /**
     * 构造 steal 范围的 target，避免每次盗取的时候，重复计算
     * @return
     */
    NVoid buildStealTargets()
    {
        steal_targets_.clear();
        for (int i = 0; i < config_->calcStealRange(); i++) {
            auto target = (index_ + i + 1) % config_->default_thread_size_;
            steal_targets_.push_back(target);
        }
        steal_targets_.shrink_to_fit();
    }


    ~UThreadPrimary() override
    {
        /**
         * 在开启展示宏的时候，会在主线程退出的时候，打印相关内容
         * 默认情况下，不会开启
         */
        metrics_.show("thread" + std::to_string(index_));
    }

private:
    int                       index_;                 // 线程index
    int                       cur_empty_epoch_ = 0;   // 当前空转的轮数信息
    UWorkStealingQueue<UTask> primary_queue_;         // 内部队列信息
    UWorkStealingQueue<UTask> secondary_queue_;   // 第二个队列，用于减少触锁概率，提升性能
    std::vector<UThreadPrimary*>* pool_threads_;    // 用于存放线程池中的线程信息
    std::vector<int>              steal_targets_;   // 被偷的目标信息

    friend class UThreadPool;
    friend class UAllocator;
};

using UThreadPrimaryPtr = UThreadPrimary*;

NAO_NAMESPACE_END

#endif   // NAO_UTHREADPRIMARY_H
