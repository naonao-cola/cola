/**
 * @FilePath     : /cola/src/Dag/DagElement/DGroup/DRegion/DRegion.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-26 15:26:11
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 09:49:28
**/

#include <functional>

#include "DRegion.h"
// #include "DagCtrl/DagElement/_DOptimizer/DOptimizerInclude.h"
#include "../../_DOptimizer/DOptimizerInclude.h"
NAO_NAMESPACE_BEGIN

DRegion::DRegion()
    : DGroup()
{
    is_init_ = false;
    manager_ = NAO_SAFE_MALLOC_NOBJECT(DElementManager) element_type_ = DElementType::REGION;
    session_                                                          = URandom<>::generateSession(NAO_STR_REGION);
}


DRegion::~DRegion(){NAO_DELETE_PTR(manager_)}


NStatus DRegion::init()
{
    NAO_FUNCTION_BEGIN
    // 在这里将初始化所有的节点信息，并且实现分析，联通等功能
    NAO_ASSERT_NOT_NULL(thread_pool_, manager_)

    // 在region中，需要专门的调度逻辑
    this->manager_->setThreadPool(thread_pool_);
    status = this->manager_->init();
    NAO_FUNCTION_CHECK_STATUS
    // 设置调度类型，需要在引擎初始化完成之后
    this->manager_->setScheduleStrategy(NAO_POOL_TASK_STRATEGY);

    is_init_ = true;
    NAO_FUNCTION_END
}


NStatus DRegion::destroy()
{
    NAO_FUNCTION_BEGIN
    status = manager_->destroy();
    NAO_FUNCTION_CHECK_STATUS

    is_init_ = false;
    NAO_FUNCTION_END
}


NStatus DRegion::run()
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_INIT(true)
    NAO_ASSERT_NOT_NULL(manager_)

    status = manager_->run();
    NAO_FUNCTION_END
}


NStatus DRegion::addElement(DElementPtr element)
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_NOT_NULL(element, manager_)

    manager_->manager_elements_.emplace(element);
    NAO_FUNCTION_END
}


DRegionPtr DRegion::setDEngineType(DEngineType type)
{
    NAO_ASSERT_INIT_THROW_ERROR(false)
    NAO_ASSERT_NOT_NULL_THROW_ERROR(manager_)

    this->manager_->setEngineType(type);
    return this;
}


NVoid DRegion::dump(std::ostream& oss)
{
    dumpElement(oss);
    dumpGroupLabelBegin(oss);
    oss << 'p' << this << "[shape=point height=0];\n";
    oss << "color=blue;\n";

    for (const auto& element : manager_->manager_elements_) {
        element->dump(oss);
    }

    dumpGroupLabelEnd(oss);

    for (const auto& element : run_before_) {
        dumpEdge(oss, this, element);
    }
}


NBool DRegion::isSerializable() const
{
    if (nullptr == manager_) {
        return false;
    }

    return manager_->checkSerializable();
}


NStatus DRegion::addManagers(DParamManagerPtr paramManager, DEventManagerPtr eventManager)
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_INIT(false)
    NAO_ASSERT_NOT_NULL(paramManager, eventManager)

    this->setDParamManager(paramManager);
    this->setDEventManager(eventManager);
    for (auto* cur : manager_->manager_elements_) {
        NAO_ASSERT_NOT_NULL(cur)
        status += cur->addManagers(paramManager, eventManager);
    }

    NAO_FUNCTION_END
}


NBool DRegion::isSeparate(DElementCPtr a, DElementCPtr b) const
{
    return DSeparateOptimizer::checkSeparate(manager_->manager_elements_, a, b);
}

NAO_NAMESPACE_END