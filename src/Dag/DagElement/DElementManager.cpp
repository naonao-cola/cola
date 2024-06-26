/**
 * @FilePath     : /cola/src/Dag/DagElement/DElementManager.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 23:07:46
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-26 11:08:35
**/
#include "DElementManager.h"
#include "_DOptimizer/DOptimizerInclude.h"

NAO_NAMESPACE_BEGIN

DElementManager::~DElementManager() {
    /**
     * manager中的节点，在析构的时候不需要释放。
     * 所有的节点信息在GPipeLine类中统一申请和释放
     */
    NAO_DELETE_PTR(engine_)
}


NStatus DElementManager::init() {
    NAO_FUNCTION_BEGIN

    /** 首先判定，注册的element全部不为空 */
    for (auto element : manager_elements_) {
        NAO_ASSERT_NOT_NULL(element)
    }

    status = initEngine();
    NAO_FUNCTION_CHECK_STATUS

    for (DElementPtr element : manager_elements_) {
        status = element->fatProcessor(NFunctionType::INIT);
        NAO_FUNCTION_CHECK_STATUS
        element->is_init_ = true;
    }

    NAO_FUNCTION_END
}


NStatus DElementManager::destroy() {
    NAO_FUNCTION_BEGIN

    for (GElementPtr element : manager_elements_) {
        status = element->fatProcessor(CFunctionType::DESTROY);
        NAO_FUNCTION_CHECK_STATUS
        element->is_init_ = false;
    }

    NAO_DELETE_PTR(engine_)
    NAO_FUNCTION_END
}


NStatus DElementManager::run() {
    NAO_FUNCTION_BEGIN

    status = engine_->run();    // 通过引擎来执行全部的逻辑
    NAO_FUNCTION_CHECK_STATUS

    if (auto_check_enable_) {
        // 默认是需要check一下执行结果的。如果为了增加一点效率，也可以通过外部设置不检查
        status = engine_->afterRunCheck();
    }
    NAO_FUNCTION_END
}


NStatus DElementManager::add(DElementPtr element) {
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_NOT_NULL(element)

    this->manager_elements_.emplace(element);
    NAO_FUNCTION_END
}


NBool DElementManager::find(DElementPtr element) const {
    if (nullptr == element) {
        return false;
    }

    return manager_elements_.end() != manager_elements_.find(element);
}


NStatus DElementManager::remove(DElementPtr element) {
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_NOT_NULL(element)

    manager_elements_.erase(element);
    NAO_FUNCTION_END
}


NStatus DElementManager::clear() {
    NAO_FUNCTION_BEGIN
    for (auto* element : manager_elements_) {
        NAO_DELETE_PTR(element)
    }
    manager_elements_.clear();
    NAO_FUNCTION_END
}


DElementManagerPtr DElementManager::setScheduleStrategy(int strategy) {
    NAO_ASSERT_NOT_NULL_THROW_ERROR(engine_)
    /**
     * 如果是 region中的 GElementManager，需要在init完成后，进行这一步赋值
     * 否则会因为 engine_ 为空，而导致崩溃
     */
    engine_->schedule_strategy_ = strategy;
    return this;
}


DElementManagerPtr DElementManager::setEngineType(DEngineType engineType) {
    engine_type_ = engineType;
    return this;
}


NStatus DElementManager::initEngine() {
    NAO_FUNCTION_BEGIN
    NAO_DELETE_PTR(engine_)

    switch (engine_type_) {
        case DEngineType::DYNAMIC : engine_ = NAO_SAFE_MALLOC_COBJECT(DDynamicEngine) break;
        case DEngineType::TOPO: engine_ = NAO_SAFE_MALLOC_COBJECT(DTopoEngine) break;
        default: NAO_RETURN_ERROR_STATUS("unknown engine type")
    }

    engine_->thread_pool_ = thread_pool_;
    status = engine_->setup(manager_elements_);
    NAO_FUNCTION_END
}


DElementManagerPtr DElementManager::setThreadPool(UThreadPoolPtr ptr) {
    NAO_ASSERT_NOT_NULL_THROW_ERROR(ptr)
    this->thread_pool_ = ptr;
    return this;
}


NSize DElementManager::calcMaxParaSize() {
    NAO_THROW_EXCEPTION_BY_CONDITION(!DMaxParaOptimizer::match(manager_elements_),
                                        "cannot calculate max parallel size within groups")
    return DMaxParaOptimizer::getMaxParaSize(manager_elements_);
}


NBool DElementManager::checkSerializable() {
    if (engine_type_ != DEngineType::DYNAMIC) {
        return false;    // 目前仅支持动态引擎的执行方式
    }

    /**
     * 判定思路：
     * 1. 内部的element，均为可串行执行的
     * 2. 当前element，不超过1个前驱或者后继
     * 3. 有且仅有一个起点，一个终点
     * 4. 有超时逻辑
     */
    int frontSize = 0, tailSize = 0;
    for (auto& cur : manager_elements_) {
        if (!cur->isSerializable()
            || cur->run_before_.size() > 1
            || cur->dependence_.size() > 1
            || cur->isAsync()) {
            return false;
        }

        if (cur->dependence_.empty()) {
            frontSize++;
        }
        if (cur->run_before_.empty()) {
            tailSize++;
        }
    }

    return (1 == frontSize) && (1 == tailSize);
}


NStatus DElementManager::process(const DSortedDElementPtrSet& elements) {
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_NOT_NULL(engine_)

    // 主要是给 mutable 使用
    status += engine_->setup(elements);
    status += run();
    NAO_FUNCTION_END
}

NAO_NAMESPACE_END
