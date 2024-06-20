/**
 * @FilePath     : /cola/src/UtilsCtrl/UtilsObject.h
 * @Description  :
 * @Author       : naonao
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-20 19:47:39
 * @Copyright    :
 **/
#ifndef NAO_UTILSOBJECT_H
#define NAO_UTILSOBJECT_H

#include "UtilsDefine.h"

NAO_NAMESPACE_BEGIN

class UtilsObject : public NObject
{
protected:
    NStatus run() override { NAO_NO_SUPPORT }
};

NAO_NAMESPACE_END

#endif   // NAO_UTILSOBJECT_H