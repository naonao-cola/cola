/**
 * @FilePath     : /cola/src/UtilsCtrl/ThreadPool/UThreadPoolConfig.h
 * @Description  :
 * @Author       : naonao
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-20 19:35:18
 **/
#ifndef NAO_UTHREADPOOLCONFIG_H
#define NAO_UTHREADPOOLCONFIG_H

#include "UThreadObject.h"
#include "UThreadPoolDefine.h"

NAO_NAMESPACE_BEGIN

struct UThreadPoolConfig : public NStruct
{
    /** 具体值含义，参考UThreadPoolDefine.h文件 */
    int   default_thread_size_           = NAO_DEFAULT_THREAD_SIZE;
    int   secondary_thread_size_         = NAO_SECONDARY_THREAD_SIZE;
    int   max_thread_size_               = NAO_MAX_THREAD_SIZE;
    int   max_task_steal_range_          = NAO_MAX_TASK_STEAL_RANGE;
    int   max_local_batch_size_          = NAO_MAX_LOCAL_BATCH_SIZE;
    int   max_pool_batch_size_           = NAO_MAX_POOL_BATCH_SIZE;
    int   max_steal_batch_size_          = NAO_MAX_STEAL_BATCH_SIZE;
    int   primary_thread_busy_epoch_     = NAO_PRIMARY_THREAD_BUSY_EPOCH;
    NMSec primary_thread_empty_interval_ = NAO_PRIMARY_THREAD_EMPTY_INTERVAL;
    NSec  secondary_thread_ttl_          = NAO_SECONDARY_THREAD_TTL;
    NSec  monitor_span_                  = NAO_MONITOR_SPAN;
    NMSec queue_emtpy_interval_          = NAO_QUEUE_EMPTY_INTERVAL;
    int   primary_thread_policy_         = NAO_PRIMARY_THREAD_POLICY;
    int   secondary_thread_policy_       = NAO_SECONDARY_THREAD_POLICY;
    int   primary_thread_priority_       = NAO_PRIMARY_THREAD_PRIORITY;
    int   secondary_thread_priority_     = NAO_SECONDARY_THREAD_PRIORITY;
    bool  bind_cpu_enable_               = NAO_BIND_CPU_ENABLE;
    bool  batch_task_enable_             = NAO_BATCH_TASK_ENABLE;
    bool  monitor_enable_                = NAO_MONITOR_ENABLE;

    NStatus check() const
    {
        NAO_FUNCTION_BEGIN
        if (default_thread_size_ < 0 || secondary_thread_size_ < 0) {
            NAO_RETURN_ERROR_STATUS("thread size cannot less than 0")
        }

        if (default_thread_size_ + secondary_thread_size_ > max_thread_size_) {
            NAO_RETURN_ERROR_STATUS("max thread size is less than default + secondary thread. [" +
                                    std::to_string(max_thread_size_) + "<" +
                                    std::to_string(default_thread_size_) + "+" +
                                    std::to_string(secondary_thread_size_) + "]");
        }

        if (monitor_enable_ && monitor_span_ <= 0) {
            NAO_RETURN_ERROR_STATUS("monitor span cannot less than 0")
        }
        NAO_FUNCTION_END
    }

protected:
    /**
     * 计算可盗取的范围，盗取范围不能超过默认线程数-1
     * @return
     */
    int calcStealRange() const
    {
        int range = std::min(this->max_task_steal_range_, this->default_thread_size_ - 1);
        return range;
    }

    friend class UThreadPrimary;
    friend class UThreadSecondary;
};

using UThreadPoolConfigPtr = UThreadPoolConfig*;

NAO_NAMESPACE_END

#endif   // NAO_UTHREADPOOLCONFIG_H
