/**
 * @FilePath     : /cola/src/Dag/DagElement/DAdapter/DFence/DFence.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-26 14:07:43
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-26 14:07:44
**/
#ifndef NAO_DFENCE_H
#define NAO_DFENCE_H

#include <set>

#include "../DAdapter.h"

NAO_NAMESPACE_BEGIN

class DFence : public DAdapter {
public:
    /**
     * 添加需要等待的element（必须是异步的）
     * @param element
     * @return
     * @notice 对外仅可以添加异步节点（设置了timeout的节点）
     */
    DFence* waitGElement(DElementPtr element);

    /**
     * 添加需要等待的一组element
     * @param elements
     * @return
     * @notice 对外仅可以添加异步节点（设置了timeout的节点）
     */
    DFence* waitGElements(const std::set<DElementPtr>& elements);

    /**
     * 清空所有的elements信息
     * @return
     */
    NVoid clear();

protected:
    explicit DFence();

    NStatus checkSuitable() final;

    NVoid dumpElement(std::ostream& oss) final;

    NAO_DECLARE_DPARAM_MANAGER_WRAPPER

    NAO_DECLARE_DEVENT_MANAGER_WRAPPER

    NStatus run() final;

private:
    std::set<DElementPtr> fence_elements_;                           // 用于存放需要等待的值

    friend class DPipeline;
};

using DFencePtr = DFence *;
using DFencePPtr = DFencePtr *;

NAO_NAMESPACE_END

#endif //NAO_DFENCE_H
