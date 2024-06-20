/**
 * @FilePath     : /cola/src/UtilsCtrl/ThreadPool/UThreadObject.h
 * @Description  :
 * @Author       : naonao
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-20 17:22:03
 **/
#ifndef NAO_UTHREADOBJECT_H
#define NAO_UTHREADOBJECT_H

#include "../UtilsObject.h"
#include "UThreadPoolDefine.h"

NAO_NAMESPACE_BEGIN

class UThreadObject : public UtilsObject
{
protected:
    /**
     * 部分thread中的算子，可以不实现run方法
     * @return
     */
    NStatus run() override { NAO_NO_SUPPORT }
};

NAO_NAMESPACE_END

#endif   // CGRAPH_UTHREADOBJECT_H
