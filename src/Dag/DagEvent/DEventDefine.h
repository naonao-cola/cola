/**
 * @FilePath     : /cola/src/Dag/DagEvent/DEventDefine.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 15:13:59
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 15:13:59
**/
#ifndef NAO_DEVENTDEFINE_H
#define NAO_DEVENTDEFINE_H

#include "../DagDefine.h"

NAO_NAMESPACE_BEGIN
NAO_INTERNAL_NAMESPACE_BEGIN

/**
 * 执行模式
 */
enum class DEVENT_TYPE {
    SYNC = 0,        // 同步模式
    ASYNC = 1,       // 异步模式
};


/**
 * 等待异步执行的策略
 */
enum class DEVENT_ASYNC_STRATEGY {
    PIPELINE_RUN_FINISH = 0,              // 在每次pipeline run结束的时候（默认）
    PIPELINE_DESTROY = 1,                 // 每次pipeline 结束的时候
    NO_WAIT = 2,                          // 不等待
};

NAO_INTERNAL_NAMESPACE_END
NAO_NAMESPACE_END

// 方便外部引用
using DEventType = nao::internal::DEVENT_TYPE;
using DEventAsyncStrategy = nao::internal::DEVENT_ASYNC_STRATEGY;

#endif //NAO_DEVENTDEFINE_H
