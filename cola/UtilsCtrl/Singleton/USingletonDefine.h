/**
 * @FilePath     : /cola/src/UtilsCtrl/Singleton/USingletonDefine.h
 * @Description  :
 * @Author       : naonao
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-20 19:54:53
 **/
#ifndef NAO_USINGLETONDEFINE_H
#define NAO_USINGLETONDEFINE_H
#include "../../NBasic/NBasicInclude.h"

NAO_NAMESPACE_BEGIN

enum class USingletonType
{
    LAZY   = 0,
    HUNGRY = 1,
};

NAO_NAMESPACE_END

#endif   // NAO_USINGLETONDEFINE_H
