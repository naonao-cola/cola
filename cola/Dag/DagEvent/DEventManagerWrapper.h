/**
 * @FilePath     : /cola/src/Dag/DagEvent/DEventManagerWrapper.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 15:16:39
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 15:16:39
 **/
#ifndef NAO_DEVENTMANAGERWRAPPER_H
#define NAO_DEVENTMANAGERWRAPPER_H

#include "DEvent.h"
#include "DEventManager.h"

NAO_NAMESPACE_BEGIN

#define NAO_DECLARE_DEVENT_MANAGER_WRAPPER                                                                                           \
    /**                                                                                                                              \
     * 设置统一管控信号类                                                                                                   \
     * @param em                                                                                                                     \
     * @return                                                                                                                       \
     */                                                                                                                              \
    virtual void* setDEventManager(DEventManagerPtr em)                                                                              \
    {                                                                                                                                \
        this->event_manager_ = em;                                                                                                   \
        return this;                                                                                                                 \
    }                                                                                                                                \
                                                                                                                                     \
    /**                                                                                                                              \
     * 触发一个事件                                                                                                            \
     * @param key                                                                                                                    \
     * @param type                                                                                                                   \
     * @return                                                                                                                       \
     * @notice 返回值仅表示是否触发成功，不表示事件是否执行成功                                              \
     */                                                                                                                              \
    NStatus notify(const std::string& key, DEventType type, DEventAsyncStrategy strategy = DEventAsyncStrategy::PIPELINE_RUN_FINISH) \
    {                                                                                                                                \
        NAO_FUNCTION_BEGIN                                                                                                           \
        NAO_ASSERT_NOT_NULL(this->event_manager_)                                                                                    \
        status = this->event_manager_->trigger(key, type, strategy);                                                                 \
        NAO_FUNCTION_END                                                                                                             \
    }




#define NAO_DECLARE_DEVENT_MANAGER_WRAPPER_WITH_MEMBER \
private:                                               \
    DEventManagerPtr event_manager_ = nullptr;         \
                                                       \
protected:                                             \
    NAO_DECLARE_DEVENT_MANAGER_WRAPPER


NAO_NAMESPACE_END

#endif   // NAO_DEVENTMANAGERWRAPPER_H
