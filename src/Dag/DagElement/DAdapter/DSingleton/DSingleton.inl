/**
 * @FilePath     : /cola/src/Dag/DagElement/DAdapter/DSingleton/DSingleton.inl
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-26 13:51:34
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-26 13:54:49
 */
#ifndef NAO_DSINGLETON_INL
#define NAO_DSINGLETON_INL

#include "DSingleton.h"

NAO_NAMESPACE_BEGIN

template<typename T>
USingleton<T> DSingleton<T>::s_singleton_;

template<typename T>
std::atomic<NBool> DSingleton<T>::s_is_init_(false);


template<typename T>
DSingleton<T>::DSingleton()
{
    this->element_type_ = DElementType::SINGLETON;
    session_            = URandom<>::generateSession(NAO_STR_SINGLETON);
}


template<typename T>
NStatus DSingleton<T>::init()
{
    NAO_FUNCTION_BEGIN
    /* 确保仅 GSingletonNode 类型内容，init一次 */
    if (s_is_init_) {
        NAO_FUNCTION_END
    }

    /* 因为采取的是饥汉模式，不需要判断ptr是否为空了 */
    auto element = dynamic_cast<T*>(s_singleton_.get());
    status       = element->init();
    if (status.isOK()) {
        s_is_init_ = true;
    }

    NAO_FUNCTION_END
}


template<typename T>
NStatus DSingleton<T>::run()
{
    NAO_FUNCTION_BEGIN

    auto element = dynamic_cast<T*>(s_singleton_.get());
    status       = element->run();
    NAO_FUNCTION_END
}


template<typename T>
NStatus DSingleton<T>::destroy()
{
    NAO_FUNCTION_BEGIN
    if (!s_is_init_) {
        NAO_FUNCTION_END
    }

    auto element = dynamic_cast<T*>(s_singleton_.get());
    status       = element->destroy();
    if (status.isOK()) {
        s_is_init_ = false;
    }

    NAO_FUNCTION_END
}


template<typename T>
NStatus DSingleton<T>::addElementInfo(const std::set<DElementPtr>& dependElements, const std::string& name, NSize loop)
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_INIT(false)

    status = this->addDependDElements(dependElements);
    NAO_FUNCTION_CHECK_STATUS

    // 这里，内部和外部均需要设定name信息
    this->setName(name)->setLoop(loop);
    // 获取单例信息，然后将信息node中信息
    auto element   = dynamic_cast<T*>(s_singleton_.get());
    element->name_ = name;
    NAO_FUNCTION_END
}


template<typename T>
NStatus DSingleton<T>::addManagers(DParamManagerPtr paramManager, DEventManagerPtr eventManager)
{
    NAO_FUNCTION_BEGIN

    NAO_ASSERT_INIT(false)
    NAO_ASSERT_NOT_NULL(paramManager, eventManager)
    auto element = dynamic_cast<T*>(s_singleton_.get());
    status       = element->addManagers(paramManager, eventManager);

    NAO_FUNCTION_END
}


template<typename T>
NBool DSingleton<T>::isHold()
{
    auto element = dynamic_cast<T*>(s_singleton_.get());
    return element->isHold();
}


template<typename T>
NBool DSingleton<T>::isMatch()
{
    auto element = dynamic_cast<T*>(s_singleton_.get());
    return element->isMatch();
}


template<typename T>
NBool DSingleton<T>::isRegistered() const
{
    auto element = dynamic_cast<T*>(s_singleton_.get());
    return element->isRegistered();
}


template<typename T>
const std::string& DSingleton<T>::getName() const
{
    auto element = dynamic_cast<T*>(s_singleton_.get());
    return element->getName();
}

NAO_NAMESPACE_END

#endif   // NAO_DSINGLETON_INL
