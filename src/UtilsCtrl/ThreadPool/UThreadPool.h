/**
 * @FilePath     : /cola/src/UtilsCtrl/ThreadPool/UThreadPool.h
 * @Description  :
 * @Author       : naonao
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-20 19:50:36
 **/
#ifndef CGRAPH_UTHREADPOOL_H
#define CGRAPH_UTHREADPOOL_H

#include <algorithm>
#include <functional>
#include <future>
#include <list>
#include <map>
#include <memory>
#include <thread>
#include <vector>


#include "Queue/UQueueInclude.h"
#include "Task/UTaskInclude.h"
#include "Thread/UThreadInclude.h"
#include "UThreadObject.h"
#include "UThreadPoolConfig.h"


NAO_NAMESPACE_BEGIN

class UThreadPool : public UThreadObject
{
public:
    /**
     * 通过默认设置参数，来创建线程池
     * @param autoInit 是否自动开启线程池功能
     * @param config
     */
    explicit UThreadPool(NBool                    autoInit = true,
                         const UThreadPoolConfig& config   = UThreadPoolConfig()) noexcept;

    /**
     * 析构函数
     */
    ~UThreadPool() override;

    /**
     * 设置线程池相关配置信息，需要在init()函数调用前，完成设置
     * @param config
     * @return
     * @notice 通过单例类(UThreadPoolSingleton)开启线程池，则线程池默认init。需要 destroy
     * 后才可以设置参数
     */
    NStatus setConfig(const UThreadPoolConfig& config);

    /**
     * 获取线程池配置信息
     * @return
     */
    UThreadPoolConfig getConfig() const;

    /**
     * 开启所有的线程信息
     * @return
     */
    NStatus init() final;

    /**
     * 提交任务信息
     * @tparam FunctionType
     * @param task
     * @param index
     * @return
     */
    template<typename FunctionType>
    auto commit(const FunctionType& task, NIndex index = NAO_DEFAULT_TASK_STRATEGY)
        -> std::future<decltype(std::declval<FunctionType>()())>;

    /**
     * 向特定的线程id中，提交任务信息
     * @tparam FunctionType
     * @param task
     * @param tid 线程id。如果超出主线程个数范围，则默认写入pool的通用队列中
     * @param enable 是否启用上锁/解锁功能
     * @param lockable 上锁(true) / 解锁(false)
     * @return
     */
    template<typename FunctionType>
    auto commitWithTid(const FunctionType& task, NIndex tid, NBool enable,
                       NBool lockable) -> std::future<decltype(std::declval<FunctionType>()())>;

    /**
     * 根据优先级，执行任务
     * @tparam FunctionType
     * @param task
     * @param priority 优先级别。自然序从大到小依次执行
     * @return
     * @notice 建议，priority 范围在 [-100, 100] 之间
     */
    template<typename FunctionType>
    auto commitWithPriority(const FunctionType& task,
                            int priority) -> std::future<decltype(std::declval<FunctionType>()())>;

    /**
     * 执行任务组信息
     * 取taskGroup内部ttl和入参ttl的最小值，为计算ttl标准
     * @param taskGroup
     * @param ttl
     * @return
     */
    NStatus submit(const UTaskGroup& taskGroup, NMSec ttl = NAO_MAX_BLOCK_TTL);

    /**
     * 针对单个任务的情况，复用任务组信息，实现单个任务直接执行
     * @param task
     * @param ttl
     * @param onFinished
     * @return
     */
    NStatus submit(NAO_DEFAULT_CONST_FUNCTION_REF func, NMSec ttl = NAO_MAX_BLOCK_TTL,
                   NAO_CALLBACK_CONST_FUNCTION_REF onFinished = nullptr);

    /**
     * 获取根据线程id信息，获取线程index信息
     * @param tid
     * @return
     * @notice 辅助线程返回-1
     */
    NIndex getThreadIndex(NSize tid);

    /**
     * 释放所有的线程信息
     * @return
     */
    NStatus destroy() final;

    /**
     * 判断线程池是否已经初始化了
     * @return
     */
    NBool isInit() const;

    /**
     * 生成辅助线程。内部确保辅助线程数量不超过设定参数
     * @param size
     * @return
     */
    NStatus createSecondaryThread(NInt size);

    /**
     * 删除辅助线程
     * @param size
     * @return
     */
    NStatus releaseSecondaryThread(NInt size);

protected:
    /**
     * 根据传入的策略信息，确定最终执行方式
     * @param origIndex
     * @return
     */
    virtual NIndex dispatch(NIndex origIndex);

    /**
     * 监控线程执行函数，主要是判断是否需要增加线程，或销毁线程
     * 增/删 操作，仅针对secondary类型线程生效
     */
    NVoid monitor();

    NAO_NO_ALLOWED_COPY(UThreadPool)

private:
    NBool               is_init_{false};   // 是否初始化
    NInt                cur_index_ = 0;    // 记录放入的线程数
    UAtomicQueue<UTask> task_queue_;       // 用于存放普通任务
    UAtomicPriorityQueue<UTask>
        priority_task_queue_;   // 运行时间较长的任务队列，仅在辅助线程中执行
    std::vector<UThreadPrimaryPtr> primary_threads_;                   // 记录所有的主线程
    std::list<std::unique_ptr<UThreadSecondary>> secondary_threads_;   // 用于记录所有的辅助线程
    UThreadPoolConfig    config_;                                      // 线程池设置值
    std::thread          monitor_thread_;                              // 监控线程
    std::map<NSize, int> thread_record_map_;                           // 线程记录的信息
    std::mutex           st_mutex_;   // 辅助线程发生变动的时候，加的mutex信息
};

using UThreadPoolPtr = UThreadPool*;

NAO_NAMESPACE_END

#include "UThreadPool.inl"

#endif   // NAO_UTHREADPOOL_H
