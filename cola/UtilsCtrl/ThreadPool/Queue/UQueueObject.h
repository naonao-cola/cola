/*
 * @FilePath     : /cola/src/UtilsCtrl/ThreadPool/Queue/UQueueObject
 * @Description  :
 * @Author       : naonao
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-20 17:56:59
 */

#ifndef NAO_UQUEUEOBJECT_H
#define NAO_UQUEUEOBJECT_H

#include <mutex>

#include "../UThreadObject.h"
#include "UQueueDefine.h"

NAO_NAMESPACE_BEGIN

class UQueueObject : public UThreadObject
{
protected:
    std::mutex              mutex_;
    std::condition_variable cv_;
};

NAO_NAMESPACE_END

#endif   // NAO_UQUEUEOBJECT_H