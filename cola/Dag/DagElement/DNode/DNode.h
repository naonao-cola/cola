/**
 * @FilePath     : /cola/src/Dag/DagElement/DNode/DNode.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-26 12:36:21
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-26 12:45:59
 **/
#ifndef NAO_DNODE_H
#define NAO_DNODE_H

#include "DNodeDefine.h"

NAO_NAMESPACE_BEGIN

class DNode : public DElement
{
protected:
    /**
     * 构造函数
     */
    explicit DNode();

    /**
     * 设置节点类型
     * @param type
     * @return
     */
    DNode* setType(const DNodeType& type);

    /**
     * 并发执行所有的task信息
     * @param tasks
     * @param ttl
     * @return
     */
    NStatus spawn(const UTaskGroup& tasks, NMSec ttl = NAO_MAX_BLOCK_TTL);

private:
    DNodeType node_type_{DNodeType::BASIC};   // 节点类型
};

using DNodePtr    = DNode*;
using DNodePtrArr = std::vector<DNodePtr>;

NAO_NAMESPACE_END

#endif   // NAO_DNODE_H