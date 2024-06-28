/**
 * @FilePath     : /cola/src/Dag/DagElement/DGroup/DCondition/DMultiCondition.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-26 15:25:54
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 09:48:57
 **/
#ifndef NAO_DMULTICONDITION_H
#define NAO_DMULTICONDITION_H

#include "DCondition.h"
#include "DConditionDefine.h"

NAO_NAMESPACE_BEGIN

template<DMultiConditionType type>
class DMultiCondition : public DCondition
{
private:
    explicit DMultiCondition();

    NStatus run() final;

    /**
     * 串行执行
     * @return
     */
    NStatus serialRun();

    /**
     * 并行执行
     * @return
     */
    NStatus parallelRun();

    NIndex choose() final;

    NBool isSerializable() const override;

    NBool isSeparate(DElementCPtr a, DElementCPtr b) const final;

    friend class DPipeline;
    friend class UAllocator;
};

NAO_NAMESPACE_END

#include "DMultiCondition.inl"

#endif   // NAO_DMULTICONDITION_H
