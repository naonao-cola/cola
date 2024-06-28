/**
 * @FilePath     : /cola/src/Dag/DagPipeline/_DSchedule/DScheduleDefine.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-28 10:22:58
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 10:22:58
 **/
#ifndef NAO_DSCHEDULEDEFINE_H
#define NAO_DSCHEDULEDEFINE_H

#include "../../DagDefine.h"

NAO_NAMESPACE_BEGIN
NAO_INTERNAL_NAMESPACE_BEGIN

enum class DScheduleType
{
    UNIQUE = 0,   // 独占式，一个pipeline 独占一个 thread pool，默认的类型
    SHARED = 1,   // 共享式，n个pipeline 共享m个 thread pool
};

NAO_INTERNAL_NAMESPACE_END
NAO_NAMESPACE_END

#endif   // NAO_DSCHEDULEDEFINE_H