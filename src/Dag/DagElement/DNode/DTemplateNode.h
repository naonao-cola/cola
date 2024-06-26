/**
 * @FilePath     : /cola/src/Dag/DagElement/DNode/DTemplateNode.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-26 12:37:51
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-26 12:37:51
**/
#ifndef NAO_DTEMPLATENODE_H
#define NAO_DTEMPLATENODE_H

#include "DNode.h"

NAO_NAMESPACE_BEGIN

template<typename ...Args>
class DTemplateNode : public DNode {
};

template<typename ...Args>
using DTemplateNodePtr = DTemplateNode<Args ...> *;

NAO_NAMESPACE_END

#endif //NAO_DTEMPLATENODE_H