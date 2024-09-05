/**
 * @FilePath     : /cola/cola/Dag/DagElement/_DOptimizer/DMaxParaOptimizer.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-26 11:51:08
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-09-05 14:23:45
 **/
#ifndef NAO_DMAXPARAOPTIMIZER_H
#define NAO_DMAXPARAOPTIMIZER_H

#include <algorithm>
#include <vector>

#include "DOptimizer.h"

NAO_NAMESPACE_BEGIN

class DMaxParaOptimizer : public DOptimizer
{
protected:
    /**
     * 判定是否match计算条件
     * @param elements
     * @return
     */
    static NBool match(const DSortedDElementPtrSet& elements)
    {
        return !std::any_of(elements.begin(), elements.end(), [](DElementCPtr ptr) { return ptr->isDGroup(); });
    }

    /**
     * 计算当前dag的最大并发度
     * @param elements
     * @return
     */
    static NSize getMaxParaSize(const DSortedDElementPtrSet& elements)
    {
        /**
         * 计算思路如下：
         * 1. 列出来所有的可行路径(paths)
         * 2. 根据路径，生成一张全连通图，然后将图取反，得到对应的补图(reGraph)
         * 3. 计算补图的最大团中元素个数(maxCliqueSize)，即为当前dag的最大并行度
         */
        const auto& paths   = collectPaths(elements);
        const auto& reGraph = buildGraph(elements, paths, 0, 0, 1);

        NSize maxCliqueSize = calcMaxCliqueSize(reGraph);   // 计算最大团信息
        return maxCliqueSize;
    }

    /**
     * 计算当前最大团的大小
     * @param graph
     * @return
     */
    static NSize calcMaxCliqueSize(const std::vector<std::vector<int>>& graph)
    {
        NSize              eleSize       = graph.size();
        NSize              maxCliqueSize = 0;   // 最大团size
        std::vector<NSize> curClique;           // 当前团

        std::function<void(NSize, NSize)> backtrace = [&](NSize start, NSize depth) {
            for (NSize i = start; i < eleSize; i++) {
                if (depth + eleSize - i <= maxCliqueSize) {
                    return;
                }   // 剪枝策略：剩余的元素数量，已经不足以超过 max 值了

                if (std::all_of(curClique.begin(), curClique.end(), [&](const int j) {
                        // 如果跟当前团的node，均为连接状态，则标记为可以进入当前团中
                        return 1 == graph[j][i];
                    })) {
                    curClique.push_back(i);
                    backtrace(i + 1, depth + 1);   // depth 表示，团里已有元素的个数
                    curClique.pop_back();
                }
            }

            maxCliqueSize = std::max(curClique.size(), maxCliqueSize);
        };

        backtrace(0, 0);
        return maxCliqueSize;
    }

    friend class DElementManager;
};

NAO_NAMESPACE_END

#endif   // NAO_DMAXPARAOPTIMIZER_H
