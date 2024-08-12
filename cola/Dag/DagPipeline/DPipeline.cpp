/**
 * @FilePath     : /cola/cola/Dag/DagPipeline/DPipeline.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-28 10:35:54
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-08-12 15:09:35
 **/
#include "DPipeline.h"
#include "../DagElement/_DOptimizer/DOptimizerInclude.h"
#include "_DPerf/DPerfInclude.h"


NAO_NAMESPACE_BEGIN

DPipeline::DPipeline()
{
    session_         = URandom<>::generateSession(NAO_STR_PIPELINE);
    element_manager_ = NAO_SAFE_MALLOC_NOBJECT(DElementManager)
    param_manager_   = NAO_SAFE_MALLOC_NOBJECT(DParamManager)
    daemon_manager_  = NAO_SAFE_MALLOC_NOBJECT(DDaemonManager)
    event_manager_   = NAO_SAFE_MALLOC_NOBJECT(DEventManager)
}


DPipeline::~DPipeline()
{// 结束的时候，会自动释放所有的element信息
    NAO_DELETE_PTR(daemon_manager_)
    NAO_DELETE_PTR(element_manager_)
    NAO_DELETE_PTR(param_manager_)
    NAO_DELETE_PTR(event_manager_)
}


NStatus DPipeline::init()
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_INIT(false)   // 必须是非初始化的状态下，才可以初始化。反之同理
    NAO_ASSERT_NOT_NULL(element_manager_, param_manager_, daemon_manager_, event_manager_)

    status += initEnv();
    NAO_FUNCTION_CHECK_STATUS

    status += param_manager_->init();
    status += event_manager_->init();
    status += element_manager_->init();
    status += daemon_manager_->init();   // daemon的初始化，需要晚于所有element的初始化
    NAO_FUNCTION_CHECK_STATUS

    is_init_ = true;
    NAO_FUNCTION_END
}


NStatus DPipeline::run()
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_INIT(true)

    /**
     * 1. 将所有 GElement 的状态设定为 NORMAL
     * 2. 将所有的 GParam 设置为初始值
     * 3. 执行dag逻辑
     * 4. 将所有 GElement 的状态恢复为 NORMAL
     * 5. 等到所有异步事件执行结束
     * 6. 将所有的 GParam 复原
     */
    status += repository_.setup();
    status += param_manager_->setup();
    NAO_FUNCTION_CHECK_STATUS

    status = element_manager_->run();
    status += repository_.reset();       // 等异步的逻辑执行，并且获取异步执行的结果信息
    status += event_manager_->reset();   // 所有的事件（特指异步事件）执行结束
    param_manager_->resetWithStatus(status);
    NAO_FUNCTION_END
}


NStatus DPipeline::destroy()
{
    NAO_FUNCTION_BEGIN

    NAO_ASSERT_INIT(true)
    NAO_ASSERT_NOT_NULL(element_manager_, param_manager_, daemon_manager_, event_manager_)

    /**
     * 需要先将线程池相关内容析构，然后再做destroy。
     * 否则的话，在错误status累积的情况下，在windows平台上，会产生崩溃
     */
    status += repository_.destroy();
    status += schedule_.destroy();
    NAO_FUNCTION_CHECK_STATUS

    status += event_manager_->destroy();
    status += daemon_manager_->destroy();
    status += element_manager_->destroy();
    status += param_manager_->destroy();
    NAO_FUNCTION_CHECK_STATUS

    is_init_ = false;
    NAO_FUNCTION_END
}


NStatus DPipeline::process(NSize runTimes)
{
    NAO_FUNCTION_BEGIN
    status = init();
    NAO_FUNCTION_CHECK_STATUS

    while (runTimes-- > 0 && !status.isErr() && !repository_.isCancelState()) {
        /**
         * 1. 执行轮数（runTimes）没有结束
         * 2. 执行结果正常
         * 3. 没有进入取消状态
         */
        status += run();
    }

    status += destroy();
    NAO_FUNCTION_END
}


NStatus DPipeline::registerDNode(DElementPPtr nodeRef, const DElementPtrSet& dependElements, const std::string& name, NSize loop)
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_INIT(false)
    NAO_ASSERT_NOT_NULL(*nodeRef)

    auto node = dynamic_cast<DNodePtr>(*nodeRef);
    NAO_RETURN_ERROR_STATUS_BY_CONDITION(nullptr == node, "[" + (*nodeRef)->getName() + "] is not based on GNode")
    NAO_RETURN_ERROR_STATUS_BY_CONDITION(
        nullptr != node->belong_, "[" + node->getName() + "] can not register to pipeline for its belong to [" + node->belong_->getName() + "]")
    NAO_RETURN_ERROR_STATUS_BY_CONDITION(node->isRegistered(), "[" + node->getName() + "] register duplicate")

    status = innerRegister(node, dependElements, name, loop);
    NAO_FUNCTION_END
}


NStatus DPipeline::registerDGroup(DElementPPtr groupRef, const DElementPtrSet& dependElements, const std::string& name, NSize loop)
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_INIT(false)
    NAO_ASSERT_NOT_NULL(*groupRef)

    auto group = dynamic_cast<DGroupPtr>(*groupRef);
    NAO_RETURN_ERROR_STATUS_BY_CONDITION(nullptr == group, "[" + (*groupRef)->getName() + "] is not based on GGroup")
    NAO_RETURN_ERROR_STATUS_BY_CONDITION(
        nullptr != group->belong_, "[" + group->getName() + "] can not register to pipeline for its belong to [" + group->belong_->getName() + "]")
    NAO_RETURN_ERROR_STATUS_BY_CONDITION(group->isRegistered(), "[" + group->getName() + "] register duplicate")

    status = innerRegister(group, dependElements, name, loop);
    NAO_FUNCTION_END
}


std::future<NStatus> DPipeline::asyncRun()
{
    /**
     * 1. 确认当前pipeline已经初始化完毕
     * 2. 异步的执行 run() 方法，并且返回执行结果的 future 信息
     */
    NAO_ASSERT_INIT_THROW_ERROR(true)

    return std::async(std::launch::async, [this] { return run(); });
}


std::future<NStatus> DPipeline::asyncProcess(NSize runTimes)
{
    NAO_ASSERT_INIT_THROW_ERROR(false)

    return std::async(std::launch::async, [runTimes, this] { return process(runTimes); });
}


NStatus DPipeline::cancel()
{
    NAO_ASSERT_INIT(true)
    return repository_.pushAllState(DElementState::CANCEL);
}


NStatus DPipeline::yield()
{
    NAO_ASSERT_INIT(true)
    return repository_.pushAllState(DElementState::YIELD);
}


NStatus DPipeline::resume()
{
    // 直接恢复正常状态好了
    NAO_ASSERT_INIT(true)
    return repository_.pushAllState(DElementState::NORMAL);
}


NStatus DPipeline::dump(std::ostream& oss)
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_NOT_NULL(element_manager_)
    oss << std::fixed << std::setprecision(2);   // 小数点最多展示2位数字

    oss << "\ndigraph CGraph {\n";
    oss << "compound=true;\n";

    for (const auto& element : element_manager_->manager_elements_) {
        NAO_ASSERT_NOT_NULL(element)
        element->dump(oss);
    }

    oss << "}\n\n";
    std::cout.unsetf(std::ios::fixed);
    NAO_FUNCTION_END
}


NStatus DPipeline::perf(std::ostream& oss)
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_INIT_THROW_ERROR(false)

    status = DPerf::perf(this, oss);
    NAO_FUNCTION_END
}


DPipelinePtr DPipeline::setDEngineType(DEngineType type)
{
    NAO_ASSERT_INIT_THROW_ERROR(false)
    NAO_ASSERT_NOT_NULL_THROW_ERROR(element_manager_)

    element_manager_->setEngineType(type);
    return this;
}


DPipelinePtr DPipeline::setUniqueThreadPoolConfig(const UThreadPoolConfig& config)
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_INIT_THROW_ERROR(false)
    NAO_THROW_EXCEPTION_BY_STATUS(config.check())

    /**
     * 实际是将信息传递给 schedule中，如果是unique的话，就使用这个参数
     * 如果是 shared的话，其实配置是无效的
     */
    schedule_.config_ = config;
    return this;
}


DPipelinePtr DPipeline::setSharedThreadPool(UThreadPoolPtr ptr)
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_INIT_THROW_ERROR(false)
    NAO_THROW_EXCEPTION_BY_STATUS(schedule_.makeType(ptr))
    return this;
}


DPipelinePtr DPipeline::setAutoCheck(NBool enable)
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_INIT_THROW_ERROR(false)
    NAO_ASSERT_NOT_NULL_THROW_ERROR(element_manager_)

    element_manager_->auto_check_enable_ = enable;
    return this;
}


NSize DPipeline::getMaxPara()
{
    NAO_ASSERT_NOT_NULL_THROW_ERROR(element_manager_)
    return element_manager_->calcMaxParaSize();
}


NStatus DPipeline::makeSerial()
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_INIT(false)
    NAO_ASSERT_NOT_NULL(element_manager_)

    NAO_RETURN_ERROR_STATUS_BY_CONDITION((schedule_.type_ != internal::DScheduleType::UNIQUE), "cannot set serial config without UNIQUE schedule")

    NAO_RETURN_ERROR_STATUS_BY_CONDITION(!element_manager_->checkSerializable(), "cannot set serial config for this pipeline")

    UThreadPoolConfig config;
    config.default_thread_size_   = 0;   // 设置之后，不再开辟线程池，直接通过主线程执行pipeline的逻辑
    config.secondary_thread_size_ = 0;
    config.max_thread_size_       = 0;
    config.monitor_enable_        = false;
    config.batch_task_enable_     = false;
    schedule_.config_             = config;
    NAO_FUNCTION_END
}


DPipelineState DPipeline::getCurState() const
{
    return repository_.cur_state_;
}


NBool DPipeline::checkSeparate(DElementPtr fst, DElementPtr snd) const
{
    NBool result = false;
    if ((fst == snd) || (!repository_.find(fst)) || (!repository_.find(snd))) {
        return result;
    }

    DElementPtr fstPatch, sndPatch = nullptr;
    DElementPtr ancestor = DSeparateOptimizer::getNearestAncestor(fst, snd, &fstPatch, &sndPatch);
    if (ancestor == fst || ancestor == snd) {
        // 其中一个是另外一个的祖先，则直接认为是分离的
        result = true;
    }
    else if (ancestor) {
        // 两个同属于一个 group 的情况，则根据 group 的属性来决定
        NAO_THROW_EXCEPTION_BY_CONDITION(!ancestor->isDGroup(), "calculate ancestor failed, not a group.")
        result = (dynamic_cast<DGroupPtr>(ancestor))->isSeparate(fstPatch, sndPatch);
    }
    else {
        // ancestor == nullptr，则认定fst 和 snd 的 ancestor 是pipeline
        result = DSeparateOptimizer::checkSeparate(element_manager_->manager_elements_, fstPatch, sndPatch);
    }

    return result;
}


NStatus DPipeline::initEnv()
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_NOT_NULL(event_manager_, element_manager_)

    status = schedule_.init();
    NAO_FUNCTION_CHECK_STATUS

    auto tp = schedule_.getThreadPool();
    event_manager_->setThreadPool(tp);
    // 这里设置线程池，是为了将tp信息，传递给内部的engine类
    element_manager_->setThreadPool(tp);

    // 设置所有的element 中的thread_pool
    repository_.setThreadPool(tp);
    status += repository_.init();
    NAO_FUNCTION_END
}


NStatus DPipeline::innerRegister(DElementPtr element, const DElementPtrSet& dependElements, const std::string& name, NSize loop)
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_NOT_NULL(element)
    NAO_ASSERT_INIT(false)

    const std::string& curName = name.empty() ? element->getName() : name;
    status                     = element->addElementInfo(dependElements, curName, loop);
    NAO_FUNCTION_CHECK_STATUS

    status = element->addManagers(param_manager_, event_manager_);
    NAO_FUNCTION_CHECK_STATUS
    status = element_manager_->add(element);
    NAO_FUNCTION_CHECK_STATUS
    repository_.insert(element);

    NAO_FUNCTION_END
}

NAO_NAMESPACE_END
