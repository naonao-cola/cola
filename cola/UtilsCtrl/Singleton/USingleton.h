/**
 * @FilePath     : /cola/src/UtilsCtrl/Singleton/USingleton.h
 * @Description  :
 * @Author       : naonao
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-21 18:45:28
 **/
#ifndef NAO_USINGLETON_H
#define NAO_USINGLETON_H

#include "../UtilsObject.h"
#include "USingletonDefine.h"
NAO_NAMESPACE_BEGIN

template<typename T, USingletonType type = USingletonType::HUNGRY, NBool autoInit = false>
class USingleton : public UtilsObject
{
public:
    explicit USingleton() noexcept
    {
        if (USingletonType::HUNGRY == type || autoInit) {
            /* 如果是饥汉模式，则直接构造 */
            create();
        }

        if (autoInit) {
            init();
        }
    }

    ~USingleton() override { clear(); }

    /**
     * 获取singleton句柄信息
     * @return
     */
    T* get()
    {
        if (USingletonType::LAZY == type) {
            create();
        }

        T* handle = handle_;
        return handle;
    }

protected:
    NStatus init() override
    {
        NAO_FUNCTION_BEGIN
        // 如果传入的是NObject类型的对象的话，则调用其init接口
        if (std::is_base_of<NObject, T>::value) {
            status = this->get()->init();
        }
        NAO_FUNCTION_END
    }

    NStatus destroy() override
    {
        NAO_FUNCTION_BEGIN
        if (std::is_base_of<NObject, T>::value) {
            status = this->get()->destroy();
        }
        NAO_FUNCTION_END
    }

    /**
     * 单例的创建一个句柄
     * @return
     */
    NVoid create()
    {
        if (unlikely(nullptr == handle_)) {
            NAO_LOCK_GUARD lock(lock_);
            if (nullptr == handle_) {
                handle_ = NAO_SAFE_MALLOC_NOBJECT(T)
            }
        }
    }

    /**
     * 销毁单例句柄
     * @return
     */
    NStatus clear()
    {
        NAO_FUNCTION_BEGIN
        NAO_LOCK_GUARD lock(lock_);
        NAO_DELETE_PTR(handle_)
        NAO_FUNCTION_END
    }

    NAO_NO_ALLOWED_COPY(USingleton);

private:
    T*         handle_{nullptr};   // 对应的单例句柄信息
    std::mutex lock_;
};

NAO_NAMESPACE_END

#endif   // NAO_USINGLETON_H
