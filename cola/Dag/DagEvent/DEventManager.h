/**
 * @FilePath     : /cola/src/Dag/DagEvent/DEventManager.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 15:14:40
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 15:14:40
 **/
#ifndef NAO_DEVENTMANAGER_H
#define NAO_DEVENTMANAGER_H

#include <string>
#include <unordered_map>
#include <future>

#include "../DagManager.h"
#include "DEvent.h"


NAO_NAMESPACE_BEGIN

/*
事件管理类，包含事件的创建，触发等，参数验证
*/
class DEventManager : public DEventObject, public DagManager<DEvent>
{
public:
    /**
     * 创建一个特定类型的事件，并且通过参数赋值
     * @tparam TEvent
     * @tparam TEParam
     * @param key
     * @param param
     * @return
     */
    template<typename TEvent, typename TEParam = DEventDefaultParam, c_enable_if_t<std::is_base_of<DEvent, TEvent>::value, int> = 0,
             c_enable_if_t<std::is_base_of<DEventParam, TEParam>::value, int> = 0>
    NStatus createWithParam(const std::string& key, TEParam* param = nullptr);

    /**
     * 触发事件
     * @param key
     * @param type
     * @param strategy, 仅当异步的时候生效
     * @return
     */
    NStatus trigger(const std::string& key, DEventType type, DEventAsyncStrategy strategy);

     /**
     * 异步事件触发
     * @param key
     * @param strategy
     * @return
     */
    std::shared_future<NVoid> asyncTrigger(const std::string &key, DEventAsyncStrategy strategy);

protected:
    NStatus init() final;

    NStatus destroy() final;

    NStatus clear() final;

    DEventObjectPtr setThreadPool(UThreadPoolPtr ptr) final;

    explicit DEventManager() = default;

    ~DEventManager() override;

    NStatus reset() final;

    NAO_NO_ALLOWED_COPY(DEventManager)

private:
    std::unordered_map<std::string, DEventPtr> events_map_;   // event 管理类

    friend class UAllocator;
    friend class DPipeline;
};

using DEventManagerPtr = DEventManager*;

NAO_NAMESPACE_END

#include "DEventManager.inl"

#endif   // NAO_DEVENTMANAGER_H
