/*
 * @FilePath     : /cola/src/Dag/DagAspect/DAspectObject.inl
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 20:31:06
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 21:55:51
 */
#ifndef NAO_DASPECTOBJECT_INL
#define NAO_DASPECTOBJECT_INL

#include "DAspectObject.h"

NAO_NAMESPACE_BEGIN

template<typename T, c_enable_if_t<std::is_base_of<DAspectParam, T>::value, int>>
DAspectObjectPtr DAspectObject::setAParam(T* param)
{
    /** 传入的param可以为空 */
    if (param) {
        NAO_DELETE_PTR(param_)
        param_ = NAO_SAFE_MALLOC_NOBJECT(T)   // 确保param是最新的
                 param_->clone(static_cast<T*>(param));
    }

    return this;
}


template<typename T, c_enable_if_t<std::is_base_of<DAspectParam, T>::value, int>>
T* DAspectObject::getAParam()
{
    NAO_ASSERT_NOT_NULL_THROW_ERROR(param_)

    T* param = nullptr;
    if ((typeid(*param_).name() == typeid(T).name())) {
        // 如果类型相同才可以获取成功，否则直接返回nullptr
        param = dynamic_cast<T*>(this->param_);
    }
    return param;
}

NAO_NAMESPACE_END

#endif   // NAO_DASPECTOBJECT_INL
