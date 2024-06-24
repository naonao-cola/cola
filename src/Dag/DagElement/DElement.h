/**
 * @FilePath     : /cola/src/Dag/DagElement/DElement.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 11:31:32
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 18:04:52
 **/
#ifndef NAO_DELEMENT_H
#define NAO_DELEMENT_H

#include <algorithm>
#include <atomic>
#include <set>
#include <string>
#include <vector>


#include "DElementDefine.h"
#include "DElementObject.h"
#include "DElementRelation.h"

NAO_NAMESPACE_BEGIN

enum class GMultiConditionType;
struct DPerfInfo;

/**
元素依赖 event aspect param,可以先看这几个,顺序，param  event message daemon aspect
 */
class DElement : public DElementObject, public NDescInfo
{
public:
};

using DElementRef = DElement &;
using DElementPtr = DElement *;
using DElementCPtr = const DElement *;
using DElementPPtr = DElementPtr *;
using DElementPtrArr = std::vector<DElementPtr>;
using DElementPtrSet = std::set<DElementPtr>;
NAO_NAMESPACE_END

#endif   // NAO_DELEMENT_H