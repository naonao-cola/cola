/**
 * @FilePath     : /cola/cola/Dag/DagEvent/DEvent.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 15:13:34
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-08-12 10:24:55
 **/
#include "DEvent.h"

NAO_NAMESPACE_BEGIN


DEvent::DEvent()
{
    session_ = URandom<>::generateSession(NAO_STR_EVENT);
}


DEvent::~DEvent(){NAO_DELETE_PTR(param_)}


NStatus DEvent::init()
{
    NAO_FUNCTION_BEGIN
    async_run_finish_futures_.clear();
    async_destroy_futures_.clear();
    NAO_FUNCTION_END
}

NStatus DEvent::destroy()
{
    NAO_FUNCTION_BEGIN
    asyncWait(DEventAsyncStrategy::PIPELINE_DESTROY);
    async_run_finish_futures_.clear();
    async_destroy_futures_.clear();
    NAO_FUNCTION_END
}



NStatus DEvent::process(DEventType type, DEventAsyncStrategy strategy)
{
    NAO_FUNCTION_BEGIN
    switch (type) {
    case DEventType::SYNC:   // 同步触发
        this->trigger(this->param_);
        break;
    case DEventType::ASYNC:   // 异步触发
        NAO_ASSERT_NOT_NULL(this->thread_pool_)
        asyncProcess(strategy);
        break;
    default: NAO_RETURN_ERROR_STATUS("unknown event type")
    }

    NAO_FUNCTION_END
}

std::shared_future<NVoid> DEvent::asyncProcess(DEventAsyncStrategy strategy) {
    NAO_ASSERT_NOT_NULL_THROW_ERROR(thread_pool_)
    std::shared_future<NVoid> future = thread_pool_->commit([this] {
        this->trigger(this->param_);
    }, NAO_POOL_TASK_STRATEGY);

    /**
     * 根据具体策略，将 future信息放到对应的容器中
     * 在特定的时间点，等待执行结束
     */
    if (DEventAsyncStrategy::PIPELINE_RUN_FINISH == strategy) {
        NAO_LOCK_GUARD lock(async_run_finished_lock_);
        async_run_finish_futures_.emplace_back(future);
    } else if (DEventAsyncStrategy::PIPELINE_DESTROY == strategy) {
        NAO_LOCK_GUARD lock(async_destroy_lock_);
        async_destroy_futures_.emplace_back(future);
    }
    return future;
}


NVoid DEvent::asyncWait(DEventAsyncStrategy strategy)
{
    switch (strategy) {
    case DEventAsyncStrategy::PIPELINE_RUN_FINISH:
    {
        NAO_LOCK_GUARD lock(async_run_finished_lock_);
        for (auto& cur : async_run_finish_futures_) {
            cur.valid() ? cur.wait() : void();
        }
        async_run_finish_futures_.clear();
        break;
    }
    case DEventAsyncStrategy::PIPELINE_DESTROY:
    {
        NAO_LOCK_GUARD lock(async_destroy_lock_);
        for (auto& cur : async_destroy_futures_) {
            cur.valid() ? cur.wait() : void();
        }
        async_destroy_futures_.clear();
        break;
    }
    default: NAO_THROW_EXCEPTION("unknown event async strategy type")
    }
}

NAO_NAMESPACE_END