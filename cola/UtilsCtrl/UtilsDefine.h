/**
 * @FilePath     : /cola/src/UtilsCtrl/UtilsDefine.h
 * @Description  :
 * @Author       : naonao
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-21 10:49:53
 * @Copyright    :
 **/
#ifndef NAO_UTILSDEFINE_H
#define NAO_UTILSDEFINE_H

#include <iostream>
#include <string>

#if __cplusplus >= 201703L
#    include <shared_mutex>
#else
#    include <mutex>
#endif

#include "../NBasic/NBasicInclude.h"
#include "UAllocator.h"
#include "UtilsFunction.h"

NAO_NAMESPACE_BEGIN

#ifdef _ENABLE_LIKELY_
#    define likely(x) __builtin_expect(!!(x), 1)
#    define unlikely(x) __builtin_expect(!!(x), 0)
#else
#    define likely
#    define unlikely
#endif

using NAO_LOCK_GUARD  = std::lock_guard<std::mutex>;
using NAO_UNIQUE_LOCK = std::unique_lock<std::mutex>;

/* 判断函数流程是否可以继续 */
NAO_INTERNAL_NAMESPACE_BEGIN
static std::mutex g_check_status_mtx;
static std::mutex g_echo_mtx;
NAO_INTERNAL_NAMESPACE_END

#if __cplusplus >= 201703L
using NAO_READ_LOCK  = std::shared_lock<std::shared_mutex>;
using NAO_WRITE_LOCK = std::unique_lock<std::shared_mutex>;
#else
using NAO_READ_LOCK  = NAO_LOCK_GUARD;   // C++14不支持读写锁，使用mutex替代
using NAO_WRITE_LOCK = NAO_LOCK_GUARD;
#endif


template<typename T>
NStatus __ASSERT_NOT_NULL(T t)
{
    return (unlikely(nullptr == t)) ? NErrStatus(NAO_INPUT_IS_NULL) : NStatus();
}

template<typename T, typename... Args>
NStatus __ASSERT_NOT_NULL(T t, Args... args)
{
    if (unlikely(t == nullptr)) {
        return __ASSERT_NOT_NULL(t);
    }

    return __ASSERT_NOT_NULL(args...);
}

template<typename T>
NVoid __ASSERT_NOT_NULL_THROW_EXCEPTION(T t)
{
    if (unlikely(nullptr == t)) {
        NAO_THROW_EXCEPTION("[CException] " + std::string(NAO_INPUT_IS_NULL))
    }
}

template<typename T, typename... Args>
NVoid __ASSERT_NOT_NULL_THROW_EXCEPTION(T t, Args... args)
{
    if (unlikely(nullptr == t)) {
        __ASSERT_NOT_NULL_THROW_EXCEPTION(t);
    }

    __ASSERT_NOT_NULL_THROW_EXCEPTION(args...);
}


/** 判断传入的多个指针信息，是否为空 */
#define NAO_ASSERT_NOT_NULL(ptr, ...)                                          \
    {                                                                          \
        const NStatus& __cur_status__ = __ASSERT_NOT_NULL(ptr, ##__VA_ARGS__); \
        if (unlikely(__cur_status__.isErr())) {                                \
            return __cur_status__;                                             \
        }                                                                      \
    }


/** 判断传入的多个指针，是否为空。如果为空，则抛出异常信息 */
#define NAO_ASSERT_NOT_NULL_THROW_ERROR(ptr, ...) __ASSERT_NOT_NULL_THROW_EXCEPTION(ptr, ##__VA_ARGS__);

/* 删除资源信息 */
#define NAO_DELETE_PTR(ptr)           \
    if (unlikely((ptr) != nullptr)) { \
        delete (ptr);                 \
        (ptr) = nullptr;              \
    }                                 \

#define NAO_ASSERT_INIT(isInit)                                \
    if (unlikely((isInit) != is_init_)) {                      \
        NAO_RETURN_ERROR_STATUS("init status is not suitable") \
    }                                                          \

#define NAO_ASSERT_INIT_THROW_ERROR(isInit)                             \
    if (unlikely((isInit) != is_init_)) {                               \
        NAO_THROW_EXCEPTION("[NException] init status is not suitable") \
    }                                                                   \

#define NAO_ASSERT_MUTABLE_INIT_THROW_ERROR(isInit)                             \
    if (unlikely((isInit) != is_init_) && !isMutable()) {                       \
        NAO_THROW_EXCEPTION("[NException] mutable init status is not suitable") \
    }                                                                           \

#define NAO_SLEEP_SECOND(s)                              \
    std::this_thread::sleep_for(std::chrono::seconds(s));\

#define NAO_SLEEP_MILLISECOND(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms));

#define NAO_FUNCTION_CHECK_STATUS                                                                                                    \
    if (unlikely(status.isErr())) {                                                                                                  \
        if (status.isCrash()) {                                                                                                      \
            throw NException(status.getInfo());                                                                                      \
        }                                                                                                                            \
        NAO_LOCK_GUARD lock{internal::g_check_status_mtx};                                                                           \
        NAO_ECHO("%s, errorCode = [%d], errorInfo = [%s].", status.getLocate().c_str(), status.getCode(), status.getInfo().c_str()); \
        return status;                                                                                                               \
    }                                                                                                                                \

/**
 * 定制化输出
 * @param cmd
 * @param ...
 * 注：内部包含全局锁，不建议正式上线的时候使用
 */
inline NVoid NAO_ECHO(const char* cmd, ...)
{
#ifdef _NAO_SILENCE_
    return;
#endif

    std::lock_guard<std::mutex> lock{internal::g_echo_mtx};
    auto                        now  = std::chrono::system_clock::now();
    auto                        time = std::chrono::system_clock::to_time_t(now);
    auto                        ms   = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;
    std::cout << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S.") << std::setfill('0') << std::setw(3) << ms << "] ";

    va_list args;
    va_start(args, cmd);
    vprintf(cmd, args);
    va_end(args);
    std::cout << "\n";
}

NAO_NAMESPACE_END

#endif   // NAO_UTILSDEFINE_H
