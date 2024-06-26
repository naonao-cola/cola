
/**
 * @FilePath     : /cola/src/Dag/DagElement/DGroup/DCondition/DCondition.h
 * @Description  : 实现在多个element中，选择一个执行的功能
 * @Author       : naonao
 * @Date         : 2024-06-26 15:25:54
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-26 16:27:50
**/
#ifndef NAO_DCONDITION_H
#define NAO_DCONDITION_H

#include "../DGroup.h"

NAO_NAMESPACE_BEGIN

class DCondition : public DGroup {
protected:
    explicit DCondition();

    /**
     * 计算需要返回第n个信息
     * 执行最后一个，返回-1即可。
     * 超出-1和size之间的范围，则不执行
     * @return
     */
    virtual NIndex choose() = 0;

    /**
     * 获取当前condition组内部元素的个数
     * @return
     */
    NSize getRange() const;

private:
    NVoid dump(std::ostream& oss) final;

    NStatus run() override;

    NBool isSeparate(DElementCPtr a, DElementCPtr b) const override;

    friend class DPipeline;
};

using DConditionPtr = DCondition *;

NAO_NAMESPACE_END

#endif //NAO_DCONDITION_H
