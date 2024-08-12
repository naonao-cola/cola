/**
 * @FilePath     : /cola/cola/Dag/DagEvent/DEventManager.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 15:14:58
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-08-12 10:29:18
 **/
#include "DEventManager.h"

NAO_NAMESPACE_BEGIN

NStatus DEventManager::init()
{
    NAO_FUNCTION_BEGIN
    for (auto& iter : events_map_) {
        status += (iter.second)->init();
    }
    NAO_FUNCTION_END
}


NStatus DEventManager::destroy()
{
    NAO_FUNCTION_BEGIN
    for (auto& iter : events_map_) {
        status += (iter.second)->destroy();
    }
    NAO_FUNCTION_END
}


NStatus DEventManager::clear()
{
    NAO_FUNCTION_BEGIN
    for (auto& iter : events_map_) {
        NAO_DELETE_PTR(iter.second)
    }

    events_map_.clear();
    NAO_FUNCTION_END
}


NStatus DEventManager::trigger(const std::string& key, DEventType type, DEventAsyncStrategy strategy)
{
    NAO_FUNCTION_BEGIN
    auto result = events_map_.find(key);
    NAO_RETURN_ERROR_STATUS_BY_CONDITION(events_map_.end() == result,
                                            "event key [" + key + "] no find")

    auto event = result->second;
    NAO_ASSERT_NOT_NULL(event)
    status = event->process(type, strategy);
    NAO_FUNCTION_END
}

std::shared_future<NVoid> DEventManager::asyncTrigger(const std::string &key, DEventAsyncStrategy strategy) {
    auto result = events_map_.find(key);
    NAO_THROW_EXCEPTION_BY_CONDITION(events_map_.end() == result || !result->second,
                                        "event key [" + key + "] no find");

    auto event = result->second;
    return event->asyncProcess(strategy);
}

DEventObjectPtr DEventManager::setThreadPool(UThreadPoolPtr ptr)
{
    NAO_ASSERT_NOT_NULL_THROW_ERROR(ptr)
    for (auto& iter : events_map_) {
        NAO_ASSERT_NOT_NULL_THROW_ERROR(iter.second)
        (iter.second)->setThreadPool(ptr);
    }

    return this;
}


DEventManager::~DEventManager()
{
    clear();
}


NStatus DEventManager::reset()
{
    NAO_FUNCTION_BEGIN
    for (auto& iter : events_map_) {
        iter.second->asyncWait(DEventAsyncStrategy::PIPELINE_RUN_FINISH);
    }
    NAO_FUNCTION_END
}

NAO_NAMESPACE_END