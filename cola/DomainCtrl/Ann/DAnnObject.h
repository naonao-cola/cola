/**
 * @FilePath     : /cola/src/DomainCtrl/Ann/DAnnObject.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 22:42:21
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 22:42:21
 **/
#ifndef NAO_DANNOBJECT_H
#define NAO_DANNOBJECT_H

#include "../DomainObject.h"

NAO_NAMESPACE_BEGIN

class DAnnObject : public DomainObject
{
protected:
    NStatus run() override { NAO_NO_SUPPORT }
};

NAO_NAMESPACE_END

#endif   // NAO_DANNOBJECT_H