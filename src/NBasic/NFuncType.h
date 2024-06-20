/**
 * @FilePath     : /cola/src/NBasic/NFuncType.h
 * @Description  :
 * @Author       : naonao
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-20 19:48:36
 * @Copyright    :
 **/
#ifndef NAO_FUNCTYPE_H
#define NAO_FUNCTYPE_H

#include "NStrDefine.h"
#include "NValType.h"
#include <functional>


NAO_NAMESPACE_BEGIN

using NAO_DEFAULT_FUNCTION            = std::function<void()>;
using NAO_DEFAULT_CONST_FUNCTION_REF  = const std::function<void()>&;
using NAO_NSTATUS_FUNCTION            = std::function<NStatus()>;
using NAO_NSTATUS_CONST_FUNCTION_REF  = const std::function<NStatus()>&;
using NAO_CALLBACK_FUNCTION           = std::function<void(NStatus)>;
using NAO_CALLBACK_CONST_FUNCTION_REF = const std::function<void(NStatus)>&;

/**
 * 描述函数类型
 */
enum class NFunctionType
{
    INIT    = 1, /** 初始化函数 */
    RUN     = 2, /** 执行函数 */
    DESTROY = 3  /** 释放函数 */
};

/** 开启函数流程 */
#define NAO_FUNCTION_BEGIN NStatus status;

/** 结束函数流程 */
#define NAO_FUNCTION_END return status;

/** 无任何功能函数 */
#define NAO_EMPTY_FUNCTION return NStatus();


/** 获取当前代码所在的位置信息 */
#define NAO_GET_LOCATE                                                           \
    (std::string(__FILE__) + " | " + std::string(__FUNCTION__) + " | line = [" + \
     ::std::to_string(__LINE__) + "]")


/** 生成一个包含异常位置的 NStatus
 * 这里这样实现，是为了符合 NStatus 类似写法
 * */
#define NErrStatus(info) NStatus(info, NAO_GET_LOCATE)

/** 返回异常信息和状态 */
#define NAO_RETURN_ERROR_STATUS(info) return NErrStatus(info);

/** 根据条件判断是否返回错误状态 */
#define NAO_RETURN_ERROR_STATUS_BY_CONDITION(cond, info) \
    if (unlikely(cond)) {                                \
        NAO_RETURN_ERROR_STATUS(info);                   \
    }

/** 不支持当前功能 */
#define NAO_NO_SUPPORT return NErrStatus(NAO_FUNCTION_NO_SUPPORT);

/** 定义为不能赋值和拷贝的对象类型 */
#define NAO_NO_ALLOWED_COPY(CType)                 \
    CType(const CType&)                  = delete; \
    const CType& operator=(const CType&) = delete;

/** 抛出异常 */
#define NAO_THROW_EXCEPTION(info) throw NException(info, NAO_GET_LOCATE);

/** 在异常状态的情况下，抛出异常 */
#define NAO_THROW_EXCEPTION_BY_STATUS(status)    \
    if (unlikely((status).isErr())) {            \
        NAO_THROW_EXCEPTION((status).getInfo()); \
    }

/** 根据条件判断是否抛出异常 */
#define NAO_THROW_EXCEPTION_BY_CONDITION(cond, info) \
    if (unlikely(cond)) {                            \
        NAO_THROW_EXCEPTION(info);                   \
    }

NAO_NAMESPACE_END
#endif   // NAO_FUNCTYPE_H
