/**
 * @FilePath     : /cola/src/Dag/DagElement/DGroup/DGroup.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-26 15:11:27
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-26 15:19:28
 **/
#ifndef NAO_DGROUP_H
#define NAO_DGROUP_H

#include <vector>

#include "../DElement.h"
#include "DGroupDefine.h"


NAO_NAMESPACE_BEGIN

/* 所有节点组合的基类，所有节点组合功能，均继承自此类 */
class DGroup : public DElement
{
protected:
    /**
     * 向group中，添加element信息
     * @param element
     * @return
     */
    virtual NStatus addElement(DElementPtr element);

    NBool isSerializable() const override;

private:
    explicit DGroup();

    NStatus addManagers(DParamManagerPtr paramManager, DEventManagerPtr eventManager) override;

    NStatus init() override;

    NStatus destroy() override;

    /**
     * 生成graphviz中 group对应的label 的开头信息
     * @param oss
     * @return
     */
    NVoid dumpGroupLabelBegin(std::ostream& oss);

    /**
     * 生成graphviz中的 group对应的label 的结尾信息
     * @param oss
     * @return
     */
    NVoid dumpGroupLabelEnd(std::ostream& oss);

    /**
     * 判断两个element，是否相互独立
     * @param a
     * @param b
     * @return
     * @notice 如果返回 true，则表示一定有前后依赖关系，不可能并发执行。否则表示不确定，原因是并发情况无法完全确定
     */
    virtual NBool isSeparate(DElementCPtr a, DElementCPtr b) const;

private:
    DElementPtrArr group_elements_arr_;   // 存放 element的数组

    friend class DPipeline;
    friend class DCluster;
    friend class DRegion;
    friend class DCondition;
    friend class DMutable;
    template<DMultiConditionType>
    friend class DMultiCondition;
    template<NInt>
    friend class DSome;
};

using DGroupPtr = DGroup*;

NAO_NAMESPACE_END

#endif   // NAO_DGROUP_H
