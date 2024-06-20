/**
 * @FilePath     : /cola/src/UtilsCtrl/ThreadPool/Queue/UQueueDefine.h
 * @Description  :
 * @Author       : naonao
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-20 19:52:03
 **/
#ifndef NAO_UQUEUEDEFINE_H
#define NAO_UQUEUEDEFINE_H
#include "../../UtilsDefine.h"

NAO_NAMESPACE_BEGIN

/** 当环形队列满的时候，写入信息时候的策略 */
enum class URingBufferPushStrategy
{
    WAIT    = 1,   // 等待有数据被消费后，再写入
    REPLACE = 2,   // 替换未被消费的最早进入的内容
    DROP    = 3,   // 丢弃当前信息
};

NAO_NAMESPACE_END

#endif   // NAO_UQUEUEDEFINE_H
