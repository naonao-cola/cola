/**
 * @FilePath     : /cola/src/Dag/DagElement/DElement.inl
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 11:32:12
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 11:32:12
 */
#ifndef NAO_DELEMENT_INL
#define NAO_DELEMENT_INL

#include "DElement.h"

NAO_NAMESPACE_BEGIN

template<typename TAspect, typename TParam, c_enable_if_t<std::is_base_of<DAspect, TAspect>::value, int>,
         c_enable_if_t<std::is_base_of<DAspectParam, TParam>::value, int>>
DElementPtr DElement::addDAspect(TParam* param)
{
    if (!aspect_manager_) {
        /** 采用懒加载的方式执行，这里不会有并发问题，故不需要采用单例模式了 */
        aspect_manager_ = NAO_SAFE_MALLOC_NOBJECT(DAspectManager)
    }

    DAspectPtr aspect = NAO_SAFE_MALLOC_NOBJECT(TAspect) aspect->setAParam<TParam>(param);
    aspect->setBelong(this);
    aspect_manager_->add(aspect);
    return this;
}


template<typename TAspect, typename... Args, c_enable_if_t<std::is_base_of<DTemplateAspect<Args...>, TAspect>::value, int>>
DElementPtr DElement::addDAspect(Args... args)
{
    if (!aspect_manager_) {
        aspect_manager_ = NAO_SAFE_MALLOC_NOBJECT(DAspectManager)
    }

    auto aspect = UAllocator::safeMallocTemplateNObject<TAspect>(std::forward<Args>(args)...);
    aspect->setBelong(this);
    aspect_manager_->add(aspect);
    return this;
}


template<typename T, c_enable_if_t<std::is_base_of<DElementParam, T>::value, int>>
DElementPtr DElement::addEParam(const std::string& key, T* param)
{
    NAO_ASSERT_INIT_THROW_ERROR(false)
    NAO_ASSERT_NOT_NULL_THROW_ERROR(param)
    if (local_params_.end() != local_params_.find(key)) {
        // 如果重复同名key信息，则删除原先的内容
        NAO_DELETE_PTR(local_params_[key]);
    }
    T* cur = NAO_SAFE_MALLOC_NOBJECT(T);
    cur->clone(param);

    local_params_[key] = cur;   // 写入其中
    return this;
}


template<typename T, c_enable_if_t<std::is_base_of<DElementParam, T>::value, int>>
T* DElement::getEParam(const std::string& key)
{
    auto iter = local_params_.find(key);
    return dynamic_cast<T*>((iter != local_params_.end()) ? iter->second : nullptr);
}

NAO_NAMESPACE_END

#endif   // NAO_DELEMENT_INL
