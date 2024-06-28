/**
 * @FilePath     : /cola/src/Dag/DagDefine.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 11:05:37
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 11:08:01
 **/

#ifndef NAO_DAGDEFINE_H
#define NAO_DAGDEFINE_H

#include "../NBasic/NBasicInclude.h"

NAO_NAMESPACE_BEGIN

static const char* NAO_STR_PIPELINE        = "pipeline";
static const char* NAO_STR_NODE            = "node";
static const char* NAO_STR_CLUSTER         = "cluster";
static const char* NAO_STR_REGION          = "region";
static const char* NAO_STR_CONDITION       = "condition";
static const char* NAO_STR_MULTI_CONDITION = "multi_condition";
static const char* NAO_STR_SOME            = "some";
static const char* NAO_STR_MUTABLE         = "mutable";
static const char* NAO_STR_FUNCTION        = "function";
static const char* NAO_STR_SINGLETON       = "singleton";
static const char* NAO_STR_DAEMON          = "daemon";
static const char* NAO_STR_ASPECT          = "aspect";
static const char* NAO_STR_EVENT           = "event";
static const char* NAO_STR_FENCE           = "fence";
static const char* NAO_STR_COORDINATOR     = "coordinator";

NAO_NAMESPACE_END

#endif   // NAO_DAGDEFINE_H