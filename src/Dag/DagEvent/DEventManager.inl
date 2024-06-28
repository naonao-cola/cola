/**
 * @FilePath     : /cola/src/Dag/DagEvent/DEventManager.inl
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 15:16:08
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 15:16:09
 */
#ifndef NAO_DEVENTMANAGER_INL
#define NAO_DEVENTMANAGER_INL

#include "DEventManager.h"

NAO_NAMESPACE_BEGIN

template<typename TEvent, typename TEParam, c_enable_if_t<std::is_base_of<DEvent, TEvent>::value, int>,
         c_enable_if_t<std::is_base_of<DEventParam, TEParam>::value, int>>
NStatus DEventManager::createWithParam(const std::string& key, TEParam* param)
{
    NAO_FUNCTION_BEGIN
    auto result = events_map_.find(key);
    if (events_map_.end() != result) {
        // 如果注册过了，则直接返回注册失败
        NAO_RETURN_ERROR_STATUS("event [" + key + "] has been register")
    }

    DEventPtr event = NAO_SAFE_MALLOC_NOBJECT(TEvent) event->setDParamManager(this->param_manager_);

    // 每次创建，都是重新生成参数的一个过程
    NAO_DELETE_PTR(event->param_)
    if (nullptr != param) {
        event->param_ = NAO_SAFE_MALLOC_NOBJECT(TEParam);
        event->param_->clone(param);
    }
    events_map_[key] = event;
    NAO_FUNCTION_END
}

NAO_NAMESPACE_END

#endif   // NAO_DEVENTMANAGER_INL