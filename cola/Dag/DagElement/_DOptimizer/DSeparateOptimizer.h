/**
 * @FilePath     : /cola/src/Dag/DagElement/_DOptimizer/DSeparateOptimizer.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-26 11:51:53
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-26 11:51:53
 **/
#ifndef NAO_DSEPARATEOPTIMIZER_H
#define NAO_DSEPARATEOPTIMIZER_H

#include <algorithm>

#include "DOptimizer.h"

NAO_NAMESPACE_BEGIN

class DSeparateOptimizer : public DOptimizer
{
protected:
    /**
     * 判断两个 element 是否是独立运行的
     * @param elements
     * @param fst
     * @param snd
     * @return
     */
    static NBool checkSeparate(const DSortedDElementPtrSet& elements, DElementCPtr fst, DElementCPtr snd)
    {
        NBool       result = false;
        const auto& paths  = DOptimizer::collectPaths(elements);
        for (const auto& path : paths) {
            if (std::find(path.begin(), path.end(), fst) != path.end() && std::find(path.begin(), path.end(), snd) != path.end()) {
                // 如果 fst 和 snd 在同一条 path中，则表示不可能同时运行
                result = true;
                break;
            }
        }
        return result;
    }

    /**
     * 获取最近的从属节点信息
     * @param fst
     * @param snd
     * @param fstPatch
     * @param sndPatch
     * @return
     * @notice 如果没有最近从属节点，即不在同一个 group中，则返回 nullptr
     */
    static DElementPtr getNearestAncestor(DElementCPtr fst, DElementCPtr snd, DElementPPtr fstPatch, DElementPPtr sndPatch)
    {
        DElementPtr ancestor = nullptr;
        const auto& fstPath  = fst->getDeepPath(false);
        const auto& sndPath  = snd->getDeepPath(false);
        NSize       minSize  = std::min(fstPath.size(), sndPath.size());
        for (int i = 0; i < minSize && (fstPath[i] == sndPath[i]); i++) {
            ancestor = fstPath[i];   // 最后一个一致的
        }

        (*fstPatch) = getPatch(fstPath, ancestor, fst);
        (*sndPatch) = getPatch(sndPath, ancestor, snd);
        return ancestor;
    }

    /**
     * 根据 ancestor信息，获取对等同级的element信息
     * @param path
     * @param ancestor
     * @param element
     * @return
     */
    static DElementPtr getPatch(const DElementPtrArr& path, DElementCPtr ancestor, DElementCPtr element)
    {
        DElementPtr patch = nullptr;
        for (int i = (int)path.size() - 1; i >= 0 && (ancestor != element); i--) {
            if (path[i] == ancestor) {
                break;
            }
            patch = path[i];
        }
        return patch;
    }

    friend class DPipeline;
    friend class DRegion;
};

NAO_NAMESPACE_END

#endif   // NAO_DSEPARATEOPTIMIZER_H
