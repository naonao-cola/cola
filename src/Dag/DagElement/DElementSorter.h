/**
 * @FilePath     : /cola/src/Dag/DagElement/DElementSorter.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 23:09:40
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 23:09:40
**/
#ifndef NAO_DELEMENTSORTER_H
#define NAO_DELEMENTSORTER_H

#include <set>

#include "DElement.h"

NAO_NAMESPACE_BEGIN

class DElementSorter : public NStruct {
public:
    /**
     * 当level不同的时候，level值小的element优先被init和destroy
     * 默认情况下， init和destroy顺序随机
     * @param a
     * @param b
     * @return
     */
    NBool operator()(DElementPtr a, DElementPtr b) const {
        return (a->level_ == b->level_) ? (a < b) : (a->level_ < b->level_);
    }
};

using DSortedDElementPtrSet = std::set<DElementPtr, DElementSorter>;

NAO_NAMESPACE_END

#endif //NAO_DELEMENTSORTER_H