/**
 * @FilePath     : /cola/src/DomainCtrl/Ann/DAnnNode.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 22:41:16
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 10:12:21
 **/
#ifndef NAO_DANNNODE_H
#define NAO_DANNNODE_H

#include "DAnnDefine.h"
#include "DAnnObject.h"

NAO_NAMESPACE_BEGIN

class DAnnNode : public DAnnObject, public DNode
{
protected:
    /**
     * 预处理参数信息，并且返回需要执行的函数信息
     * @return
     */
    virtual DAnnFuncType prepareParam() = 0;

    /**
     * 训练
     * @return
     */
    virtual NStatus train();

    /**
     * 查询
     * @return
     */
    virtual NStatus search();

    /**
     * 插入
     * @return
     */
    virtual NStatus insert();

    /**
     * 修改
     * @return
     */
    virtual NStatus update();

    /**
     * 删除
     * @return
     */
    virtual NStatus remove();

    /**
     * 加载模型信息
     * @return
     */
    virtual NStatus loadModel();

    /**
     * 保存模型信息
     * @return
     */
    virtual NStatus saveModel();

    /**
     * 其他方法
     */
    virtual NStatus edition();

    /**
     * 将参数更新，作用于所有功能函数之后
     * @return
     */
    virtual NStatus refreshParam();

protected:
    /**
     * 构造函数信息
     */
    explicit DAnnNode();

    NAO_NO_ALLOWED_COPY(DAnnNode)

    /**
     * 分拆执行函数
     * @return
     */
    NStatus run() override;

private:
    typedef NStatus (DAnnNode::*DAnnNodeFuncPtr)();
    DAnnNodeFuncPtr ann_func_arr_[static_cast<NUint>(DAnnFuncType::ANN_MAX_SIZE)]{};   // ann函数映射关系
};

NAO_NAMESPACE_END

#endif   // NAO_DANNNODE_H
