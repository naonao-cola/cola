/*
 * @FilePath     : /cola/src/Dag/DagPipeline/DPipeline.inl
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-28 10:36:10
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 11:21:45
 */
#ifndef NAO_DPIPELINE_INL
#define NAO_DPIPELINE_INL

#include <algorithm>

#include "DPipeline.h"

NAO_NAMESPACE_BEGIN

template<typename T, c_enable_if_t<std::is_base_of<DElement, T>::value, int>>
NStatus DPipeline::registerDElement(DElementPPtr elementRef, const DElementPtrSet& dependElements, const std::string& name, NSize loop)
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_INIT(false)

    if (std::is_base_of<DGroup, T>::value) {
        /**
         * 如果是GGroup类型的信息，则：
         * 1，必须外部创建
         * 2，未被注册到其他的pipeline中
         */
        if ((*elementRef) != nullptr && ((*elementRef)->isRegistered())) {
            NAO_RETURN_ERROR_STATUS("this group register duplicate")
        }
    }
    else if (std::is_base_of<DNode, T>::value || std::is_base_of<DAdapter, T>::value) {
        /**
         * 如果不是group信息的话，且属于element（包含node和adapter）
         * 则直接内部创建该信息
         */
        (*elementRef) = new (std::nothrow) T();
    }

    status = innerRegister(*elementRef, dependElements, name, loop);
    NAO_FUNCTION_END
}


template<typename DFunction>
NStatus DPipeline::registerDElement(DFunctionPPtr functionRef, const DElementPtrSet& dependElements, const std::string& name, NSize loop)
{
    // 通过模板特化的方式，简化 DFunction 的注册方式
    return this->registerDElement<DFunction>((DElementPtr*)(functionRef), dependElements, name, loop);
}


template<typename DFence>
NStatus DPipeline::registerDElement(DFencePPtr fenceRef, const DElementPtrSet& dependElements, const std::string& name, NSize loop)
{
    return this->registerDElement<DFence>((DElementPtr*)(fenceRef), dependElements, name, loop);
}


template<typename DCoordinator, NInt SIZE>
NStatus DPipeline::registerDElement(DCoordinatorPPtr<SIZE> coordinatorRef, const DElementPtrSet& dependElements, const std::string& name, NSize loop)
{
    return this->registerDElement<DCoordinator, SIZE>((DElementPtr*)(coordinatorRef), dependElements, name, loop);
}


template<typename TNode, typename... Args, c_enable_if_t<std::is_base_of<DTemplateNode<Args...>, TNode>::value, int>>
NStatus DPipeline::registerDElement(DTemplateNodePtr<Args...>* elementRef, const DElementPtrSet& dependElements, Args... args)
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_INIT(false)

    // 构建模板node信息
    (*elementRef) = new (std::nothrow) TNode(std::forward<Args&&>(args)...);
    NAO_ASSERT_NOT_NULL(*elementRef)

    status = innerRegister(*elementRef, dependElements, NAO_EMPTY, NAO_DEFAULT_LOOP_TIMES);
    NAO_FUNCTION_END
}


template<typename T, typename... Args, c_enable_if_t<std::is_base_of<DNode, T>::value, int>>
DNodePtr DPipeline::createDNode(const DNodeInfo& info, Args&&... args)
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_INIT_THROW_ERROR(false)

    DNodePtr node = new (std::nothrow) T(std::forward<Args&&>(args)...);
    NAO_ASSERT_NOT_NULL_THROW_ERROR(node)
    status = node->addElementInfo(info.dependence_, info.name_, info.loop_);
    NAO_THROW_EXCEPTION_BY_STATUS(status)

    repository_.insert(node);
    return node;
}


template<typename T, typename... Args, c_enable_if_t<std::is_base_of<DNode, T>::value, int>>
DNodePtr DPipeline::createDNode(const DElementPtrSet& dependence, const std::string& name, NSize loop, Args&&... args)
{
    const DNodeInfo& info = DNodeInfo(dependence, name, loop);
    return createDNode<T>(info, std::forward<Args&&>(args)...);
}


template<typename T, c_enable_if_t<std::is_base_of<DGroup, T>::value, int>>
DGroupPtr DPipeline::createDGroup(const DElementPtrArr& elements, const DElementPtrSet& dependElements, const std::string& name, NSize loop)
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_INIT_THROW_ERROR(false)

    // 如果不是所有的都非空，则创建失败
    NAO_THROW_EXCEPTION_BY_CONDITION(std::any_of(elements.begin(), elements.end(), [](DElementPtr element) { return (nullptr == element); }),
                                     "createGGroup elements have nullptr.")
    NAO_THROW_EXCEPTION_BY_CONDITION(
        std::any_of(dependElements.begin(), dependElements.end(), [](DElementPtr element) { return (nullptr == element); }),
        "createGGroup dependElements have nullptr.")

    DGroupPtr group = NAO_SAFE_MALLOC_NOBJECT(T) for (DElementPtr element : elements)
    {
        status += group->addElement(element);
        element->belong_ = group;   // 从属于这个group的信息
    }
    NAO_THROW_EXCEPTION_BY_STATUS(status)

    // 加入group的时候，是不设定manager信息的
    status = group->addElementInfo(dependElements, name, loop);
    NAO_THROW_EXCEPTION_BY_STATUS(status)

    this->repository_.insert(group);
    return group;
}


template<typename TAspect, typename TParam, c_enable_if_t<std::is_base_of<DAspect, TAspect>::value, int>,
         c_enable_if_t<std::is_base_of<DAspectParam, TParam>::value, int>>
DPipelinePtr DPipeline::addDAspect(const DElementPtrSet& elements, TParam* param)
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_INIT_THROW_ERROR(false)

    // 如果传入的是空的话，则默认将所有的element添加aspect信息
    auto curElements = elements.empty() ? repository_.elements_ : elements;
    for (DElementPtr element : curElements) {
        if (repository_.find(element)) {
            element->addDAspect<TAspect, TParam>(param);
        }
    }

    return this;
}


template<typename TDaemon, typename TParam, c_enable_if_t<std::is_base_of<DDaemon, TDaemon>::value, int>,
         c_enable_if_t<std::is_base_of<DDaemonParam, TParam>::value, int>>
DPipelinePtr DPipeline::addDDaemon(NMSec ms, TParam* param)
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_INIT_THROW_ERROR(false)
    NAO_ASSERT_NOT_NULL_THROW_ERROR(param_manager_, daemon_manager_)

    DDaemonPtr daemon = NAO_SAFE_MALLOC_NOBJECT(TDaemon) daemon->setDParam<TParam>(param)->setInterval(ms);
    daemon->setDParamManager(this->param_manager_);
    daemon->setDEventManager(this->event_manager_);
    status = daemon_manager_->add(daemon);
    NAO_THROW_EXCEPTION_BY_STATUS(status)

    return this;
}


template<typename TDaemon, typename... Args, c_enable_if_t<std::is_base_of<DTemplateDaemon<Args...>, TDaemon>::value, int>>
DPipelinePtr DPipeline::addDDaemon(NMSec ms, Args&&... args)
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_INIT_THROW_ERROR(false)
    NAO_ASSERT_NOT_NULL_THROW_ERROR(param_manager_, daemon_manager_)

    auto daemon = UAllocator::safeMallocTemplateNObject<TDaemon>(std::forward<Args&&>(args)...);
    daemon->setInterval(ms);
    daemon->setGParamManager(this->param_manager_);
    daemon->setGEventManager(this->event_manager_);
    status = daemon_manager_->add(daemon);
    NAO_THROW_EXCEPTION_BY_STATUS(status)

    return this;
}


template<typename TEvent, typename TParam, c_enable_if_t<std::is_base_of<DEvent, TEvent>::value, int>,
         c_enable_if_t<std::is_base_of<DEventParam, TParam>::value, int>>
DPipelinePtr DPipeline::addDEvent(const std::string& key, TParam* param)
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_INIT_THROW_ERROR(false)
    NAO_ASSERT_NOT_NULL_THROW_ERROR(param_manager_, event_manager_)

    event_manager_->param_manager_ = this->param_manager_;
    status                         = event_manager_->createWithParam<TEvent, TParam>(key, param);
    NAO_THROW_EXCEPTION_BY_STATUS(status)

    return this;
}

NAO_NAMESPACE_END

#endif   // NAO_DPIPELINE_INL
