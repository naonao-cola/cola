/**
 * @FilePath     : /cola/cola/Dag/DagElement/DElement.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 11:32:29
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-09-05 14:42:56
 **/
#include <algorithm>

#include "../DagPipeline/_DPerf/DPerfInclude.h"
#include "DElement.h"


NAO_NAMESPACE_BEGIN

DElement::~DElement()
{
    NAO_DELETE_PTR(perf_info_)
    NAO_DELETE_PTR(aspect_manager_)
    for (auto& param : local_params_) {
        NAO_DELETE_PTR(param.second)   // 依次删除本地的参数信息
    }
}


NVoid DElement::beforeRun()
{
    this->done_ = false;
    this->left_depend_.store(dependence_.size(), std::memory_order_release);
}



#ifdef _WIN32
NVoidPtr DElement::setName(const std::string& name)
{
#else
DElementPtr DElement::setName(const std::string& name)
{
#endif
    NAO_ASSERT_INIT_THROW_ERROR(false)
    this->name_ = name.empty() ? this->session_ : name;
    return this;
}


DElementPtr DElement::setLoop(NSize loop)
{
    // 由于运行机制问题，loop执行的element，不支持异步执行
    NAO_ASSERT_MUTABLE_INIT_THROW_ERROR(false)
    NAO_THROW_EXCEPTION_BY_CONDITION((timeout_ > NAO_DEFAULT_ELEMENT_TIMEOUT && loop != NAO_DEFAULT_LOOP_TIMES), "cannot set loop value when timeout is bigger than 0")

    this->loop_ = loop;
    return this;
}


DElementPtr DElement::setLevel(NLevel level)
{
    NAO_ASSERT_INIT_THROW_ERROR(false)

    this->level_ = level;
    return this;
}


DElementPtr DElement::setVisible(NBool visible)
{
    NAO_ASSERT_MUTABLE_INIT_THROW_ERROR(false)

    this->visible_ = visible;
    return this;
}


DElementPtr DElement::setBindingIndex(NIndex index)
{
    NAO_ASSERT_INIT_THROW_ERROR(false)
    /**
     * 由于内部有调度机制，不保证绑定线程后，一定在固定线程上执行。
     * 仅保证优先考虑使用绑定线程执行
     */
    this->binding_index_ = index;
    return this;
}


DElementPtr DElement::setTimeout(NMSec timeout, DElementTimeoutStrategy strategy)
{
    NAO_ASSERT_INIT_THROW_ERROR(false)
    NAO_THROW_EXCEPTION_BY_CONDITION((timeout < NAO_DEFAULT_ELEMENT_TIMEOUT), "timeout value cannot smaller than 0")
    NAO_THROW_EXCEPTION_BY_CONDITION((loop_ > NAO_DEFAULT_LOOP_TIMES && NAO_DEFAULT_ELEMENT_TIMEOUT != timeout), "cannot set timeout value when loop bigger than 1")

    this->timeout_          = timeout;
    this->timeout_strategy_ = strategy;
    return this;
}

DElementPtr DElement::setMacro(NBool macro)
{
    NAO_ASSERT_INIT_THROW_ERROR(false)
    // 目前仅针对非group逻辑生效
    NAO_THROW_EXCEPTION_BY_CONDITION(isDGroup(), "cannot set group as macro")
    is_marco_ = macro;
    return this;
}

DElementRef DElement::operator--(int) noexcept
{
    try {
        this->setVisible(true);
    }
    catch (const NException&) {
        NAO_ECHO("[warning] default set visible failed.");
    }

    return (*this);
}


DElementRef DElement::operator>(DElementPtr element)
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_NOT_NULL_THROW_ERROR(element)
    NAO_ASSERT_MUTABLE_INIT_THROW_ERROR(false)
    NAO_THROW_EXCEPTION_BY_STATUS(element->addDependDElements({this}))

    // 默认通过 这种方式注入的内容，都设置成 visible 的状态
    this->setVisible(true);
    element->setVisible(true);
    return (*this);
}


DElementRef DElement::operator&(DElementPtr element)
{
    return operator>(element);
}


DElement& DElement::operator*(NSize loop) noexcept
{
    try {
        this->setLoop(loop);
    }
    catch (const NException&) {
        NAO_ECHO("[warning] default set loop failed.");
    }

    return (*this);
}

NBool DElement::isAsync() const
{
    // 如果timeout != 0, 则异步执行
    return this->timeout_ != NAO_DEFAULT_ELEMENT_TIMEOUT;
}


NBool DElement::isRegistered() const
{
    return (nullptr != param_manager_) && (nullptr != event_manager_);
}


NStatus DElement::addDependDElements(const DElementPtrSet& elements)
{
    NAO_FUNCTION_BEGIN
    if (!isMutable()) {
        // 如果是 mutable的逻辑，则可以在 init之后，修改依赖关系
        NAO_ASSERT_INIT(false)
    }

    for (DElementPtr element : elements) {
        NAO_ASSERT_NOT_NULL(element)
        NAO_RETURN_ERROR_STATUS_BY_CONDITION((element->belong_ != this->belong_), element->getName() + " cannot depend because not same belong info")
        if (this == element) {
            continue;
        }

        element->run_before_.insert(this);
        this->dependence_.insert(element);
    }

    this->left_depend_.store(this->dependence_.size(), std::memory_order_release);
    NAO_FUNCTION_END
}


NStatus DElement::addElementInfo(const DElementPtrSet& dependElements, const std::string& name, NSize loop)
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_INIT(false)

    // 添加依赖的时候，可能会出现异常情况。故在这里提前添加 && 做判定
    status = this->addDependDElements(dependElements);
    NAO_FUNCTION_CHECK_STATUS
    this->setLoop(loop);
    this->setName(name);
    NAO_FUNCTION_END
}


NStatus DElement::addManagers(DParamManagerPtr paramManager, DEventManagerPtr eventManager)
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_INIT(false)
    NAO_ASSERT_NOT_NULL(paramManager, eventManager)

    this->setDParamManager(paramManager);
    this->setDEventManager(eventManager);
    if (aspect_manager_) {
        aspect_manager_->setDParamManager(paramManager);
        aspect_manager_->setDEventManager(eventManager);
    }

    NAO_FUNCTION_END
}


NStatus DElement::doAspect(const internal::DAspectType& aspectType, const NStatus& curStatus)
{
    NAO_FUNCTION_BEGIN

    // 如果切面管理类为空，或者未添加切面，直接返回
    if (this->aspect_manager_ && 0 != this->aspect_manager_->getSize()) {
        status = aspect_manager_->reflect(aspectType, curStatus);
    }

    NAO_FUNCTION_END
}


NStatus DElement::fatProcessor(const NFunctionType& type)
{
    NAO_FUNCTION_BEGIN

    if (unlikely(!visible_)) {
        /**
         * 如果当前的 element 因为被remove等原因，变成 不可见的状态
         * 则不运行。但不是实际删除当前节点信息
         */
        NAO_FUNCTION_END
    }

    try {
        switch (type) {
        case NFunctionType::RUN:
        {
            if (!is_prepared_) {
                /** 第一次执行的时候，预先执行一下 prepareRun方法 */
                status = prepareRun();
                NAO_FUNCTION_CHECK_STATUS
                is_prepared_ = true;
            }

            for (NSize i = 0; i < this->loop_ && status.isOK() && DElementState::NORMAL == this->getCurState(); i++) {
                /** 执行带切面的run方法 */
                status += doAspect(internal::DAspectType::BEGIN_RUN);
                NAO_FUNCTION_CHECK_STATUS
                do {
                    status += isAsync() ? asyncRun() : run();
                    /**
                     * 在实际run结束之后，首先需要判断一下是否进入yield状态了。
                     * 接下来，如果状态是ok的，并且被条件hold住，则循环执行
                     * 默认所有element的isHold条件均为false，即不hold，即执行一次
                     * 可以根据需求，对任意element类型，添加特定的isHold条件
                     * */
                } while (checkYield(), this->isHold() && status.isOK());
                doAspect(internal::DAspectType::FINISH_RUN, status);
            }

            NAO_THROW_EXCEPTION_BY_STATUS(checkRunResult())
            break;
        }
        case NFunctionType::INIT:
        {
            concerned_params_.clear();   // 仅需要记录这一轮使用到的 GParam 信息
            is_prepared_ = false;
            status       = doAspect(internal::DAspectType::BEGIN_INIT);
            NAO_FUNCTION_CHECK_STATUS
            status = init();
            doAspect(internal::DAspectType::FINISH_INIT, status);
            break;
        }
        case NFunctionType::DESTROY:
        {
            status = doAspect(internal::DAspectType::BEGIN_DESTROY);
            NAO_FUNCTION_CHECK_STATUS
            status = destroy();
            doAspect(internal::DAspectType::FINISH_DESTROY, status);
            break;
        }
        default:
            NAO_RETURN_ERROR_STATUS("get function type error")
        }
    }
    catch (const NException& ex) {
        doAspect(internal::DAspectType::ENTER_CRASHED);
        status = crashed(ex);
    }

    NAO_FUNCTION_END
}


NStatus DElement::prepareRun(){NAO_EMPTY_FUNCTION}


NStatus DElement::checkRunResult(){NAO_EMPTY_FUNCTION}


NBool DElement::isHold()
{
    /**
     * 默认仅返回false
     * 可以根据自己逻辑，来实现"持续循环执行，直到特定条件出现的时候停止"的逻辑
     */
    return false;
}


NBool DElement::isMatch()
{
    /**
     * 默认仅返回false
     * 主要面对写入 MultiCondition 的时候，做判断当前element是否被执行
     */
    return false;
}


NBool DElement::isTimeout() const
{
    /**
     * 判断的标准是：
     * 1. 如果当前节点超时，则认定为超时
     * 2. 如果当前节点所在的group超时，则也认定为超时
     */
    NBool       result = (DElementState::TIMEOUT == cur_state_.load(std::memory_order_acquire));
    DElementPtr belong = this->belong_;
    while (!result && belong) {
        result = (DElementState::TIMEOUT == belong->cur_state_.load(std::memory_order_acquire));
        belong = belong->belong_;
    }

    return result;
}


NBool DElement::isMutable() const
{
    // 写入 GMutable的 element，属于 mutable，可以在运行时，修改依赖关系
    return belong_ && DElementType::MUTABLE == belong_->element_type_;
}


NBool DElement::isMacro() const
{
    return is_marco_;
}

NStatus DElement::crashed(const NException& ex)
{
    (void)(this);
    return NStatus(internal::STATUS_CRASH, ex.what());
}


NIndex DElement::getThreadIndex()
{
    NAO_THROW_EXCEPTION_BY_CONDITION((nullptr == thread_pool_), this->getName() + " getThreadIndex with no thread pool")   // 理论不可能出现的情况

    auto tid = (NSize)std::hash<std::thread::id>{}(std::this_thread::get_id());
    return thread_pool_->getThreadIndex(tid);
}


DElementPtr DElement::setThreadPool(UThreadPoolPtr ptr)
{
    NAO_ASSERT_NOT_NULL_THROW_ERROR(ptr)
    NAO_ASSERT_INIT_THROW_ERROR(false)
    this->thread_pool_ = ptr;
    return this;
}


NVoid DElement::dump(std::ostream& oss)
{
    dumpElement(oss);

    for (const auto& node : run_before_) {
        dumpEdge(oss, this, node);
    }
}


NVoid DElement::dumpEdge(std::ostream& oss, DElementPtr src, DElementPtr dst, const std::string& label)
{
    (void)(this);
    if (src->isDGroup() && dst->isDGroup()) {
        // 在group的逻辑中，添加 cluster_ 的信息
        oss << 'p' << src << " -> p" << dst << label << "[ltail=cluster_p" << src << " lhead=cluster_p" << dst << "]";
    }
    else if (src->isDGroup() && !dst->isDGroup()) {
        oss << 'p' << src << " -> p" << dst << label << "[ltail=cluster_p" << src << "]";
    }
    else if (!src->isDGroup() && dst->isDGroup()) {
        oss << 'p' << src << " -> p" << dst << label << "[lhead=cluster_p" << dst << "]";
    }
    else {
        oss << 'p' << src << " -> p" << dst << label;
    }

    if (src->perf_info_ && src->perf_info_->in_longest_path_ && dst->perf_info_ && dst->perf_info_->in_longest_path_) {
        oss << "[color=red]";
    }
    oss << ";\n";
}


NVoid DElement::dumpElement(std::ostream& oss)
{
    dumpElementHeader(oss);
    dumpPerfInfo(oss);

    oss << "\"];\n";
    if (this->loop_ > 1 && !this->isDGroup()) {
        oss << 'p' << this << " -> p" << this << "[label=\"" << this->loop_ << "\"]" << ";\n";
    }
}


NVoid DElement::dumpElementHeader(std::ostream& oss)
{
    oss << 'p' << this << "[label=\"";
    if (this->name_.empty()) {
        oss << 'p' << this;   // 如果没有名字，则通过当前指针位置来代替
    }
    else {
        oss << this->name_;
    }
}


NVoid DElement::dumpPerfInfo(std::ostream& oss)
{
    if (perf_info_ && perf_info_->loop_ > 0) {
        // 包含 perf信息的情况
        oss << "\n";
        oss << "[start " << perf_info_->first_start_ts_;
        oss << "ms, finish " << perf_info_->last_finish_ts_ << "ms,\n";
        oss << "per_cost " << (perf_info_->accu_cost_ts_ / perf_info_->loop_);
        if (perf_info_->loop_ > 1) {
            oss << "ms, total_cost " << perf_info_->accu_cost_ts_;
        }
        oss << "ms]";
    }
}


NVoid DElement::checkYield()
{
    std::unique_lock<std::mutex> lk(yield_mutex_);
    this->yield_cv_.wait(lk, [this] { return DElementState::YIELD != cur_state_.load(std::memory_order_acquire); });
}


NBool DElement::isDGroup() const
{
    // 按位与 GROUP有值，表示是 GROUP的逻辑
    return (long(element_type_) & long(DElementType::GROUP)) > 0;
}

NBool DElement::isGAdaptor() const
{
    return (long(element_type_) & long(DElementType::ADAPTER)) > 0;
}


NBool DElement::isGNode() const
{
    return DElementType::NODE == element_type_;
}

DElementState DElement::getCurState() const
{
    /**
     * 如果有超时逻辑的话，优先判断
     * 否则就是当前的状态
     */
    auto state = this->isTimeout() ? DElementState::TIMEOUT : cur_state_.load(std::memory_order_acquire);
    return state;
}


NIndex DElement::getBindingIndex() const
{
    return this->binding_index_;
}


DElementRelation DElement::getRelation() const
{
    DElementRelation relation;
    relation.predecessors_ = this->dependence_.asVector();   // 前驱
    relation.successors_   = this->run_before_.asVector();   // 后继
    relation.belong_       = this->belong_;                  // 从属信息

    return relation;
}


NBool DElement::isSerializable() const
{
    return true;
}


NStatus DElement::popLastAspect()
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_INIT(false)
    NAO_ASSERT_NOT_NULL(aspect_manager_)

    status = aspect_manager_->popLast();
    NAO_FUNCTION_CHECK_STATUS

    if (0 == aspect_manager_->getSize()) {
        NAO_DELETE_PTR(aspect_manager_)
    }

    NAO_FUNCTION_END
}


NStatus DElement::asyncRun()
{
    NAO_FUNCTION_BEGIN
    NAO_RETURN_ERROR_STATUS_BY_CONDITION(!isAsync(), "[" + name_ + "] cannot async run.")

    async_result_ = thread_pool_->commit([this] { return run(); }, NAO_POOL_TASK_STRATEGY);

    auto futStatus = async_result_.wait_for(std::chrono::milliseconds(timeout_));
    if (std::future_status::ready == futStatus) {
        status = getAsyncResult();
    }
    else {
        NAO_RETURN_ERROR_STATUS_BY_CONDITION(DElementTimeoutStrategy::AS_ERROR == timeout_strategy_, "[" + name_ + "] running time more than [" + std::to_string(timeout_) + "]ms")
        cur_state_.store(DElementState::TIMEOUT, std::memory_order_release);
    }

    NAO_FUNCTION_END
}


NStatus DElement::getAsyncResult()
{
    NAO_FUNCTION_BEGIN
    if (async_result_.valid()) {
        status = async_result_.get();   // 这里的get和valid方法，都是线程安全的
    }

    NAO_FUNCTION_END
}


NStatus DElement::checkSuitable()
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_NOT_NULL(thread_pool_)

    // 包含异步执行的逻辑，不可以loop超过1次
    NAO_RETURN_ERROR_STATUS_BY_CONDITION(loop_ > NAO_DEFAULT_LOOP_TIMES && this->isAsync(), "[" + this->getName() + "] can set loop <= 1 only for the reason of async run")
    if (!this->isRegistered()) {
        NAO_ECHO("[notice] [%s] is created but not registered into pipeline, so it will not work.", this->getName().c_str());
    }

    NAO_FUNCTION_END
}


DElementPtrArr DElement::getDeepPath(NBool reverse) const
{
    DElementPtrArr path;
    auto*          cur = const_cast<DElementPtr>(this);
    while (cur) {
        path.push_back(cur);
        cur = cur->belong_;
    }

    if (!reverse) {
        // 如果 reverse=false，则 pipeline(nullptr)->a->b->this
        std::reverse(path.begin(), path.end());
    }
    return path;
}

NBool DElement::isDefaultBinding() const
{
    return NAO_DEFAULT_BINDING_INDEX == binding_index_;
}

NBool DElement::removeDepend(DElementPtr element) {
    NAO_ASSERT_NOT_NULL_THROW_ERROR(element)
    NAO_ASSERT_INIT_THROW_ERROR(false)
    if (!dependence_.hasValue(element)) {
        return false;
    }

    dependence_.remove(element);
    element->run_before_.remove(this);
    left_depend_.store(dependence_.size(), std::memory_order_release);
    return true;
}

NAO_NAMESPACE_END
