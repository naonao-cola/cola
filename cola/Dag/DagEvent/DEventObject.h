/**
 * @FilePath     : /cola/src/Dag/DagEvent/DEventObject.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 15:15:39
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 15:26:39
 **/
#ifndef NAO_DEVENTOBJECT_H
#define NAO_DEVENTOBJECT_H

#include "../DagObject.h"
#include "../DagParam/DParamInclude.h"

NAO_NAMESPACE_BEGIN

/**
事件里面包含线程池，管理事件
 */
class DEventObject : public DagObject, public NDescInfo
{
protected:
    NStatus run() final { NAO_NO_SUPPORT }

    virtual DEventObject* setThreadPool(UThreadPoolPtr ptr)
    {
        thread_pool_ = ptr;
        return this;
    }

protected:
    UThreadPoolPtr   thread_pool_   = nullptr;   // 线程池类
    DParamManagerPtr param_manager_ = nullptr;   // DParam参数管理类
};

using DEventObjectPtr = DEventObject*;

NAO_NAMESPACE_END

#endif   // NAO_DEVENTOBJECT_H