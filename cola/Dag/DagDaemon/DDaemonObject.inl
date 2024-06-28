/**
 * @FilePath     : /cola/src/Dag/DagDaemon/DDaemonObject.inl
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 18:08:46
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 18:09:20
 */
#ifndef NAO_DDAEMONOBJECT_INL
#define NAO_DDAEMONOBJECT_INL

#include "DDaemonObject.h"

NAO_NAMESPACE_BEGIN

template<typename DParam, c_enable_if_t<std::is_base_of<DDaemonParam, DParam>::value, int>>
DDaemonObjectPtr DDaemonObject::setDParam(DParam* param)
{
    if (param) {
        NAO_DELETE_PTR(param_)
        param_ = NAO_SAFE_MALLOC_NOBJECT(DParam)   // 确保param是最新的
                 param_->clone(static_cast<DParam*>(param));
    }

    return this;
}

NAO_NAMESPACE_END

#endif   // NAO_DDAEMONOBJECT_INL