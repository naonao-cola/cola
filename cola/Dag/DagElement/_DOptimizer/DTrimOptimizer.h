/**
 * @FilePath     : /cola/cola/Dag/DagElement/_DOptimizer/DTrimOptimizer.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-09-05 14:18:33
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-09-05 14:25:41
 * @Copyright (c) 2024 by G, All Rights Reserved.
 **/
#ifndef NAO_GTRIMOPTIMIZER_H
#define NAO_GTRIMOPTIMIZER_H

#include <algorithm>
#include <iterator>

#include "DOptimizer.h"
#include "../DGroup/DGroupInclude.h"
NAO_NAMESPACE_BEGIN

class DTrimOptimizer : public DOptimizer
{
    /**
     * 针对图结构，做剪裁
     * @param elements
     * @return
     */
    static NSize trim(DSortedDElementPtrSet& elements)
    {
        NSize       trimNum = 0;
        const auto& paths   = collectPaths(elements);
        auto        graph   = buildGraph(elements, paths, 1, 0, 0);

        for (auto* cur : elements) {
            NSize            idx = std::distance(elements.begin(), elements.find(cur));
            DElementPtrArr candidates;
            for (NSize i = 0; i < cur->dependence_.size(); i++) {
                NSize x = std::distance(elements.begin(), elements.find(cur->dependence_[i]));
                for (NSize j = i; j < cur->dependence_.size(); j++) {
                    // 这里必须是 n^2 的循环
                    NSize y = std::distance(elements.begin(), elements.find(cur->dependence_[j]));
                    if (1 == graph[x][y]) {
                        graph[x][idx] = 0;
                        candidates.push_back(cur->dependence_[i]);
                    }
                }
            }

            for (auto* candidate : candidates) {
                if (cur->removeDepend(candidate)) {
                    trimNum++;
                }
            }
            if (DElementType::REGION == cur->element_type_) {
                trimNum += ((DRegionPtr)cur)->trim();
            }
        }

        return trimNum;
    }

    friend class DElementManager;
    friend class DRegion;
};

NAO_NAMESPACE_END

#endif   // NAO_GTRIMOPTIMIZER_H
