/**
 * @FilePath     : /cola/src/Dag/DagElement/_DOptimizer/DOptimizer.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-26 11:51:25
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-26 12:00:16
 **/
#ifndef NAO_DOPTIMIZER_H
#define NAO_DOPTIMIZER_H

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

    friend class DPerf;
};

NAO_NAMESPACE_END

#endif   // NAO_DOPTIMIZER_H
