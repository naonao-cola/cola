/**
 * @FilePath     : /cola/src/Dag/DagAspect/DAspectDefine.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 19:48:43
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 19:48:43
**/
#ifndef NAO_DASPECTDEFINE_H
#define NAO_DASPECTDEFINE_H

#include "../DagObject.h"

NAO_NAMESPACE_BEGIN

enum class DAspectType {
    BEGIN_INIT = 0,
    FINISH_INIT = 1,
    BEGIN_RUN = 2,
    FINISH_RUN = 3,
    BEGIN_DESTROY = 4,
    FINISH_DESTROY = 5,
    ENTER_CRASHED = 99,
};

NAO_NAMESPACE_END

#endif //NAO_DASPECTDEFINE_H
