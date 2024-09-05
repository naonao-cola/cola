/**
 * @FilePath     : /cola/cola/Dag/DagElement/_DOptimizer/DOptimizer.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-26 11:51:25
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-09-05 14:22:04
 **/
#ifndef NAO_DOPTIMIZER_H
#define NAO_DOPTIMIZER_H

#include <vector>

#include "../DElement.h"
#include "../DElementObject.h"
#include "../DElementSorter.h"


NAO_NAMESPACE_BEGIN

class DOptimizer : public DElementObject
{
protected:
    /**
     * 记录 path 信息
     * @param element
     * @param curPath
     * @param paths
     * @return
     */
    static NVoid collect(DElementPtr element, std::vector<DElementPtr>& curPath, std::vector<std::vector<DElementPtr>>& paths)
    {
        curPath.emplace_back(element);
        if (element->run_before_.empty()) {
            // 如果是最后一个信息了，则记录下来
            paths.emplace_back(curPath);
        }
        else {
            for (auto& cur : element->run_before_) {
                collect(cur, curPath, paths);
            }
        }
        curPath.pop_back();
    }

    /**
     * 记录所有的path信息
     * @param elements
     * @return
     */
    static std::vector<std::vector<DElementPtr>> collectPaths(const DSortedDElementPtrSet& elements)
    {
        std::vector<std::vector<DElementPtr>> paths;
        for (const auto& element : elements) {
            std::vector<DElementPtr> curPath;
            if (element->dependence_.empty()) {
                collect(element, curPath, paths);
            }
        }

        return paths;
    }

     /**
     * 构造对应的二维矩阵图
     * @param elements
     * @param paths
     * @param father
     * @param son
     * @param unlink
     * @return
     */
    static std::vector<std::vector<int>> buildGraph(const DSortedDElementPtrSet& elements,
                            const std::vector<std::vector<DElementPtr>>& paths,
                            int father, int son, int unlink) {
        const NSize size = elements.size();
        std::vector<std::vector<int>> graph(size, std::vector<int>(size, unlink));
        for (auto& path : paths) {
            for (int i = 0; i < path.size() - 1; i++) {
                // 这里的 find是一定能找到的。因为path的数据，是从elements中记录的
                int height = (int)std::distance(elements.begin(), elements.find(path[i]));
                for (int j = i + 1; j < path.size(); j++) {
                    int column = (int)std::distance(elements.begin(), elements.find(path[j]));
                    graph[height][column] = father;
                    graph[column][height] = son;
                }
            }
        }
        return graph;
    }

    friend class DPerf;
};

NAO_NAMESPACE_END

#endif   // NAO_DOPTIMIZER_H
