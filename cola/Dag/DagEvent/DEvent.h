/**
 * @FilePath     : /cola/src/Dag/DagEvent/DEvent.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 15:13:06
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 15:55:21
 **/
#ifndef NAO_DEVENT_H
#define NAO_DEVENT_H

#include <future>
#include <mutex>
#include <vector>

#include "DEventDefine.h"
#include "DEventObject.h"


NAO_NAMESPACE_BEGIN

/**
每个事件里面包含一个 线程池指针，参数管理器指针，将事件放到线程池里，根据策略对事件进行触发
 */
class DEvent : public DEventObject
{
protected:
    /**
     * 事件被触发的时候，执行的函数
     * @param param 是pipeline那一层，注册进来的参数，可以为空
     * @return
     */
    virtual NVoid trigger(DEventParamPtr param) = 0;

    explicit DEvent();

    ~DEvent() override;

    NAO_DECLARE_DPARAM_MANAGER_WRAPPER

private:
    NStatus init() final;

    NStatus destroy() final;

    /**
     * 处理信号事件
     * @param type
     * @param strategy
     * @return
     */
    NStatus process(DEventType type, DEventAsyncStrategy strategy);

    /**
     * 等待并清理所有异步的event逻辑
     * @param strategy
     * @return
     */
    NVoid asyncWait(DEventAsyncStrategy strategy);

private:
    NBool                           is_init_{false};               // 是否初始化
    std::vector<std::future<NVoid>> async_run_finish_futures_{};   // 异步执行的逻辑集合（pipeline run结束的时候）
    std::vector<std::future<NVoid>> async_destroy_futures_{};      // 异步执行的逻辑集合（pipeline destroy 的时候）
    DEventParamPtr                  param_{nullptr};               // 事件参数信息
    std::mutex                      async_run_finished_lock_;
    std::mutex                      async_destroy_lock_;
    friend class DEventManager;
};

using DEventPtr = DEvent*;

NAO_NAMESPACE_END

#endif   // NAO_DEVENT_H
