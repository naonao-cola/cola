/**
 * @FilePath     : /cola/src/Dag/DagElement/DElementRelation.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 11:26:14
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 11:26:14
 **/
#ifndef NAO_DELEMENTRELATION_H
#define NAO_DELEMENTRELATION_H

#include <set>

#include "DElementObject.h"

NAO_NAMESPACE_BEGIN

class DElement;

struct DElementRelation : public NStruct
{
public:
    std::set<DElement*> predecessors_;       // 前驱信息
    std::set<DElement*> successors_;         // 后继信息
    DElement*           belong_ = nullptr;   // 从属信息。如果从属于group，则为该group的值；如果从属于pipeline，则为nullptr
};

NAO_NAMESPACE_END

#endif   // NAO_DELEMENTRELATION_H
