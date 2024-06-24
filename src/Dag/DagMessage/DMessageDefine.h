/**
 * @FilePath     : /cola/src/Dag/DagMessage/DMessage.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 16:58:33
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 16:58:33
 **/

#ifndef NAO_DMESSAGEDEFINE_H
#define NAO_DMESSAGEDEFINE_H

#include "DMessageObject.h"

NAO_NAMESPACE_BEGIN

using DMessagePushStrategy = URingBufferPushStrategy;

NAO_INTERNAL_NAMESPACE_BEGIN
static const char* PUB_SUB_PREFIX   = "PS_PRE_";
static const char* SEND_RECV_PREFIX = "SR_PRE_";
NAO_INTERNAL_NAMESPACE_END

NAO_NAMESPACE_END

#endif   // NAO_DMESSAGEDEFINE_H