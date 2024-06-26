
/**
 * @FilePath     : /cola/src/Dag/DagElement/DGroup/DCondition/DConditionDefine.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-26 15:25:54
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-26 16:24:46
**/

#ifndef NAO_DCONDITIONDEFINE_H
#define NAO_DCONDITIONDEFINE_H

#include "../DGroupDefine.h"

NAO_NAMESPACE_BEGIN

enum class DMultiConditionType {
    SERIAL = 0,        // 串行执行
    PARALLEL = 1,      // 并行执行
};

NAO_NAMESPACE_END

#endif //NAO_DCONDITIONDEFINE_H
