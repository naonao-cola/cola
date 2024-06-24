/**
 * @FilePath     : /cola/src/Dag/DagElement/DElement.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 11:31:32
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 11:43:51
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
struct GPerfInfo;

/**
元素依赖 event aspect param,可以先看这几个
 */
class GElement : public GElementObject, public NDescInfo
{
public:
};

NAO_NAMESPACE_END

#endif   // NAO_DELEMENT_H