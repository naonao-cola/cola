/**
 * @FilePath     : /cola/src/Dag/DagElement/DAdapter/DFence/DFence.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-26 14:08:02
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 09:46:15
 **/
#include "DFence.h"

NAO_NAMESPACE_BEGIN

DFence::DFence()
{
    element_type_ = DElementType::FENCE;
    session_      = URandom<>::generateSession(NAO_STR_FENCE);
}


DFencePtr DFence::waitGElement(DElementPtr element)
{
    NAO_ASSERT_INIT_THROW_ERROR(false)
    NAO_ASSERT_NOT_NULL_THROW_ERROR(element)
    NAO_THROW_EXCEPTION_BY_CONDITION(!element->isAsync(),
                                     "fence can add async element only. please set timeout value for [" + element->getName() + "] if you need.")

    fence_elements_.insert(element);
    return this;
}


DFencePtr DFence::waitGElements(const std::set<DElementPtr>& elements)
{
    NAO_ASSERT_INIT_THROW_ERROR(false)
    for (auto* element : elements) {
        waitGElement(element);
    }
    return this;
}


NVoid DFence::clear()
{
    NAO_ASSERT_INIT_THROW_ERROR(false)
    fence_elements_.clear();
}


NStatus DFence::checkSuitable()
{
    NAO_FUNCTION_BEGIN
    for (auto* element : fence_elements_) {
        NAO_ASSERT_NOT_NULL(element)
        NAO_RETURN_ERROR_STATUS_BY_CONDITION(!element->isAsync(),
                                             "[" + element->name_ + "] is not async node. please set timeout value if you want to fence it")
    }

    NAO_FUNCTION_END
}


NVoid DFence::dumpElement(std::ostream& oss)
{
    dumpElementHeader(oss);
    dumpPerfInfo(oss);
    oss << "\", shape=box]\n";
}


NStatus DFence::run()
{
    NAO_FUNCTION_BEGIN

    for (auto* element : fence_elements_) {
        // 等待全部到达之后，判断错误信息
        status += element->getAsyncResult();
    }

    NAO_FUNCTION_END
}

NAO_NAMESPACE_END