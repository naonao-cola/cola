/**
 * @FilePath     : /cola/src/UtilsCtrl/ThreadPool/Queue/UAtomicPriorityQueue.h
 * @Description  : 线程安全的优先队列。因为 priority_queue和queue的弹出方式不一致，故暂时不做合并
 * @Author       : naonao
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-20 19:51:23
 **/
#ifndef NAO_UATOMICPRIORITYQUEUE_H
#define NAO_UATOMICPRIORITYQUEUE_H

#include <queue>

#include "UQueueObject.h"

NAO_NAMESPACE_BEGIN

template<typename T>
class UAtomicPriorityQueue : public UQueueObject
{
public:
    UAtomicPriorityQueue() = default;

    /**
     * 尝试弹出
     * @param value
     * @return
     */
    NBool tryPop(T& value)
    {
        NBool result = false;
        if (mutex_.try_lock()) {
            if (!priority_queue_.empty()) {
                value = std::move(*priority_queue_.top());
                priority_queue_.pop();
                result = true;
            }
            mutex_.unlock();
        }

        return result;
    }


    /**
     * 尝试弹出多个任务
     * @param values
     * @param maxPoolBatchSize
     * @return
     */
    NBool tryPop(std::vector<T>& values, int maxPoolBatchSize)
    {
        NBool result = false;
        if (mutex_.try_lock()) {
            while (!priority_queue_.empty() && maxPoolBatchSize-- > 0) {
                values.emplace_back(std::move(*priority_queue_.top()));
                priority_queue_.pop();
                result = true;
            }
            mutex_.unlock();
        }

        return result;
    }


    /**
     * 传入数据
     * @param value
     * @param priority 任务优先级，数字排序
     * @return
     */
    NVoid push(T&& value, int priority)
    {
        std::unique_ptr<T> task(c_make_unique<T>(std::move(value), priority));
        NAO_LOCK_GUARD     lk(mutex_);
        priority_queue_.push(std::move(task));
    }


    /**
     * 判定队列是否为空
     * @return
     */
    NBool empty()
    {
        NAO_LOCK_GUARD lk(mutex_);
        return priority_queue_.empty();
    }

    NAO_NO_ALLOWED_COPY(UAtomicPriorityQueue)

private:
    std::priority_queue<std::unique_ptr<T>> priority_queue_;   // 优先队列信息，根据重要级别决定先后执行顺序
};

NAO_NAMESPACE_END

#endif   // NAO_UATOMICPRIORITYQUEUE_H
