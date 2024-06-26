/**
 * @FilePath     : /cola/src/Dag/DagAspect/DTemplateAspect.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 20:31:25
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 22:38:53
 **/
#ifndef NAO_DTEMPLATEASPECT_H
#define NAO_DTEMPLATEASPECT_H

#include "DAspect.h"

NAO_NAMESPACE_BEGIN

template<class... Args>
class DTemplateAspect : public DAspect
{};

NAO_NAMESPACE_END

#endif   // NAO_DTEMPLATEASPECT_H