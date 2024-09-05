/**
 * @FilePath     : /cola/cola/UtilsCtrl/UAllocator.h
 * @Description  :
 * @Author       : naonao
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-09-05 13:58:36
 * @Copyright    :
 **/
#ifndef NAO_UALLOCATOR_H
#define NAO_UALLOCATOR_H

#include <new>
#include <memory>
#include <mutex>


#include "../NBasic/NBasicInclude.h"

NAO_NAMESPACE_BEGIN

/**
 * 仅用于生成NObject类型的类
 */
class UAllocator : public NObject
{
public:
    /**
     * 生成一个 NObject 对象
     * @tparam T
     * @return T*
     */
    template<typename T, c_enable_if_t<std::is_base_of<NObject, T>::value, int> = 0>
    static T* safeMallocNObject()
    {
        return safeMalloc<T>();
    }

    /**
     * 生成一个 NStruct 的对象
     * @tparam T
     * @return  T*
     */
    template<typename T, c_enable_if_t<std::is_base_of<NStruct, T>::value, int> = 0>
    static T* safeMallocNStruct()
    {
        return safeMalloc<T>();
    }

    /**
     * 生成带参数的普通指针
     * @tparam T
     * @tparam Args
     * @param args
     * @return T*
     */
    template<typename T, typename... Args,
             c_enable_if_t<std::is_base_of<NObject, T>::value, int> = 0>
    static T* safeMallocTemplateNObject(Args... args)
    {
        T* result = nullptr;
        while (!result) {
            result = new(std::nothrow) T(std::forward<Args&&>(args)...);
        }
        return result;
    }

    /**
     * 生成unique智能指针信息
     * @tparam T
     * @return std::unique_ptr<T>
     */
    template<typename T, c_enable_if_t<std::is_base_of<NObject, T>::value, int> = 0>
    static std::unique_ptr<T> makeUniqueNObject()
    {
        return c_make_unique<T>();
    }

private:
    /**
     * 生成T类型的对象
     * @tparam T
     * @return T*
     */
    template<class T> static T* safeMalloc()
    {
        T* ptr = nullptr;
        while (!ptr) {
            ptr = new (std::nothrow) T();
        }
        return ptr;
    }
};


#define NAO_SAFE_MALLOC_NOBJECT(Type)           \
        UAllocator::safeMallocNObject<Type>();  \

#define NAO_MAKE_UNIQUE_NOBJECT(Type)           \
        UAllocator::makeUniqueNObject<Type>();  \

NAO_NAMESPACE_END

#endif   // NAO_UALLOCATOR_H
