/**
 * @FilePath     : /cola/src/Dag/DagElement/DNode/DNode.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-26 12:36:30
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-26 13:37:59
 **/
#include "DNode.h"

NAO_NAMESPACE_BEGIN

DNode::DNode()
    : DElement()
{
    element_type_ = DElementType::NODE;
    session_      = URandom<>::generateSession(NAO_STR_NODE);
}


DNodePtr DNode::setType(const DNodeType& type)
{
    node_type_ = type;
    return this;
}


NStatus DNode::spawn(const UTaskGroup& tasks, NMSec ttl)
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_INIT(true)
    NAO_ASSERT_NOT_NULL(thread_pool_)

    status = thread_pool_->submit(tasks, ttl);
    NAO_FUNCTION_END
}

NAO_NAMESPACE_END