/**
 * @FilePath     : /cola/src/Dag/DagMessage/DMessageObject.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 16:58:12
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 09:43:36
 **/
#ifndef NAO_DMESSAGEOBJECT_H
#define NAO_DMESSAGEOBJECT_H

#include "../DagObject.h"
#include "../DagParam/DParamInclude.h"

NAO_NAMESPACE_BEGIN

class DMessageObject : public DagObject
{
public:
    NStatus run() final { NAO_NO_SUPPORT }
};

NAO_NAMESPACE_END

#endif   // NAO_DMESSAGEOBJECT_H