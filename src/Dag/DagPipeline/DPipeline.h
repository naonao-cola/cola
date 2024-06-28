/**
 * @FilePath     : /cola/src/Dag/DagPipeline/DPipeline.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-28 10:35:36
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 11:17:21
 **/
#ifndef NAO_DPIPELINE_H
#define NAO_DPIPELINE_H

#include <list>
#include <memory>
#include <sstream>
#include <thread>
#include <vector>


#include "../DagDaemon/DDaemonInclude.h"
#include "../DagElement/DElementInclude.h"
#include "../DagEvent/DEventInclude.h"
#include "DPipelineObject.h"
#include "_DSchedule/DScheduleInclude.h"


NAO_NAMESPACE_BEGIN

class DPipeline : public DPipelineObject, public NDescInfo
{
public:
    /**
     * 初始化pipeline信息
     * @return
     */
    NStatus init() override;

    /**
     * 执行pipeline信息
     * @return
     */
    NStatus run() override;

    /**
     * 逆初始化pipeline信息
     * @return
     */
    NStatus destroy() override;

    /**
     * 一次性执行完成初始化，执行runTimes次，和销毁的过程
     * @param runTimes
     * @return
     */
    NStatus process(NSize runTimes = NAO_DEFAULT_LOOP_TIMES);

    /**
     * 异步执行pipeline的run流程
     * @return
     */
    std::future<NStatus> asyncRun();

    /**
     * 异步执行pipeline的全部流程
     * @param runTimes
     * @return
     */
    std::future<NStatus> asyncProcess(NSize runTimes = NAO_DEFAULT_LOOP_TIMES);

    /**
     * 停止执行流程，多用于异步执行流程中
     * @return
     */
    NStatus cancel();

    /**
     * 暂停当前pipeline的执行，多用于异步执行流程中
     * @return
     */
    NStatus yield();

    /**
     * 恢复当前pipeline的执行，多用于异步执行流程中
     * @return
     */
    NStatus resume();

    /**
     * 生成图可视化 graphviz 信息
     * @param oss
     * @return
     * @notice 将输出的内容，复制到 https://dreampuf.github.io/GraphvizOnline/ 中查看效果
     */
    NStatus dump(std::ostream& oss = std::cout);

    /**
     * 查看性能分析
     * @return
     * @notice 将输出的内容，复制到 https://dreampuf.github.io/GraphvizOnline/ 中查看效果，字段解释如下：
     * start : 当前element第一次被执行的时间
     * finish : 当前element最后一次被执行结束的时间
     * per_cost : 当前element单次执行耗时
     * total_cost : 当前element执行的总耗时（仅在element多次执行时展示）
     */
    NStatus perf(std::ostream& oss = std::cout);

    /**
     * 根据传入的info信息，创建node节点
     * @tparam T
     * @tparam Args
     * @param info
     * @param args
     * @return
     */
    template<typename T, typename... Args, c_enable_if_t<std::is_base_of<DNode, T>::value, int> = 0>
    DNodePtr createDNode(const DNodeInfo& info, Args&&... args);

    /**
     * 根据传入的信息，创建node节点
     * @tparam T
     * @tparam Args
     * @param name
     * @param loop
     * @param dependence
     * @param args
     * @return
     */
    template<typename T, typename... Args, c_enable_if_t<std::is_base_of<DNode, T>::value, int> = 0>
    DNodePtr createDNode(const DElementPtrSet& dependence = std::initializer_list<DElementPtr>(), const std::string& name = NAO_EMPTY,
                         NSize loop = NAO_DEFAULT_LOOP_TIMES, Args&&... args);

    /**
     * 根据传入的信息，创建Group信息
     * @tparam T
     * @param elements
     * @param dependElements
     * @param name
     * @param loop
     * @return
     */
    template<typename T, c_enable_if_t<std::is_base_of<DGroup, T>::value, int> = 0>
    DGroupPtr createDGroup(const DElementPtrArr& elements, const DElementPtrSet& dependElements = std::initializer_list<DElementPtr>(),
                           const std::string& name = NAO_EMPTY, NSize loop = NAO_DEFAULT_LOOP_TIMES);

    /**
     * 在图中注册一个Element信息
     * 如果注册的是GNode信息，则内部自动生成
     * 如果注册的是GGroup信息，则需外部提前生成，然后注册进来
     * @tparam T
     * @param elementRef
     * @param dependElements
     * @param name
     * @param loop
     * @return
     */
    template<typename T, c_enable_if_t<std::is_base_of<DElement, T>::value, int> = 0>
    NStatus registerDElement(DElementPPtr elementRef, const DElementPtrSet& dependElements = std::initializer_list<DElementPtr>(),
                             const std::string& name = NAO_EMPTY, NSize loop = NAO_DEFAULT_LOOP_TIMES);

    /**
     * 注册function类型的内容，模板特化
     * @tparam GFunction
     * @param functionRef
     * @param dependElements
     * @param name
     * @param loop
     * @return
     */
    template<typename DFunction>
    NStatus registerDElement(DFunctionPPtr functionRef, const DElementPtrSet& dependElements = std::initializer_list<DElementPtr>(),
                             const std::string& name = NAO_EMPTY, NSize loop = NAO_DEFAULT_LOOP_TIMES);

    /**
     * 注册fence类型的内容，模板特化
     * @tparam GFence
     * @param fenceRef
     * @param dependElements
     * @param name
     * @param loop
     * @return
     */
    template<typename DFence>
    NStatus registerDElement(DFencePPtr fenceRef, const DElementPtrSet& dependElements = std::initializer_list<DElementPtr>(),
                             const std::string& name = NAO_EMPTY, NSize loop = NAO_DEFAULT_LOOP_TIMES);

    /**
     * 注册GCoordinator类型的内容，模板特化
     * @tparam GCoordinator
     * @tparam SIZE
     * @param coordinatorRef
     * @param dependElements
     * @param name
     * @param loop
     * @return
     */
    template<typename DCoordinator, NInt SIZE>
    NStatus registerDElement(DCoordinatorPPtr<SIZE> coordinatorRef, const DElementPtrSet& dependElements = std::initializer_list<DElementPtr>(),
                             const std::string& name = NAO_EMPTY, NSize loop = NAO_DEFAULT_LOOP_TIMES);

    /**
     * 在图中注册一个模板Element信息
     * @tparam TNode
     * @tparam Args
     * @param elementRef
     * @param dependElements
     * @return
     */
    template<typename DNode, typename... Args, c_enable_if_t<std::is_base_of<DTemplateNode<Args...>, DNode>::value, int> = 0>
    NStatus registerDElement(DTemplateNodePtr<Args...>* elementRef, const DElementPtrSet& dependElements, Args... args);

    /**
     * 注册一个节点信息
     * @param nodeRef
     * @param dependElements
     * @param name
     * @param loop
     * @return
     */
    NStatus registerDNode(DElementPPtr nodeRef, const DElementPtrSet& dependElements = std::initializer_list<DElementPtr>(),
                          const std::string& name = NAO_EMPTY, NSize loop = NAO_DEFAULT_LOOP_TIMES);

    /**
     * 注册一个组信息（推荐使用）
     * @param groupRef
     * @param dependElements
     * @param name
     * @param loop
     * @return
     */
    NStatus registerDGroup(DElementPPtr groupRef, const DElementPtrSet& dependElements = std::initializer_list<DElementPtr>(),
                           const std::string& name = NAO_EMPTY, NSize loop = NAO_DEFAULT_LOOP_TIMES);

    /**
     * 添加切面
     * @tparam TAspect
     * @tparam TParam
     * @param elements
     * @param param
     * @return
     */
    template<typename TAspect, typename TParam = DAspectDefaultParam, c_enable_if_t<std::is_base_of<DAspect, TAspect>::value, int> = 0,
             c_enable_if_t<std::is_base_of<DAspectParam, TParam>::value, int> = 0>
    DPipeline* addDAspect(const DElementPtrSet& elements = std::initializer_list<DElementPtr>(), TParam* param = nullptr);

    /**
     * 添加守护
     * @tparam TDaemon
     * @tparam TParam
     * @param ms
     * @param param
     * @return
     */
    template<typename TDaemon, typename TParam = DDaemonDefaultParam, c_enable_if_t<std::is_base_of<DDaemon, TDaemon>::value, int> = 0,
             c_enable_if_t<std::is_base_of<DDaemonParam, TParam>::value, int> = 0>
    DPipeline* addDDaemon(NMSec ms, TParam* param = nullptr);

    /**
     * 添加模板类型守护
     * @tparam TAspect
     * @tparam Args
     * @param ms
     * @param args
     * @return
     */
    template<typename TDaemon, typename... Args, c_enable_if_t<std::is_base_of<DTemplateDaemon<Args...>, TDaemon>::value, int> = 0>
    DPipeline* addDDaemon(NMSec ms, Args&&... args);

    /**
     * 添加一个事件
     * @tparam TEvent
     * @tparam TParam
     * @param key
     * @param param
     * @return
     */
    template<typename TEvent, typename TParam = DEventDefaultParam, c_enable_if_t<std::is_base_of<DEvent, TEvent>::value, int> = 0,
             c_enable_if_t<std::is_base_of<DEventParam, TParam>::value, int> = 0>
    DPipeline* addDEvent(const std::string& key, TParam* param = nullptr);

    /**
     * 设置引擎策略
     * @param type
     * @return
     */
    DPipeline* setDEngineType(DEngineType type);

    /**
     * 设置本pipeline内部线程池相关信息
     * @param config
     * @return
     */
    DPipeline* setUniqueThreadPoolConfig(const UThreadPoolConfig& config);

    /**
     * 设置共享的线程池
     * @param ptr
     * @return
     * @notice 如果传入 nullptr 的话，则默认使用本地独占线程池
     */
    DPipeline* setSharedThreadPool(UThreadPoolPtr ptr);

    /**
     * 设置在执行完成后，是否校验整体执行逻辑
     * @param enable
     * @return
     * @notice 默认校验。如果确定流程正常，可以考虑取消校验流程，从而降低整体耗时
     */
    DPipeline* setAutoCheck(NBool enable);

    /**
     * 获取最大并发度
     * @return
     * @notice 暂时仅支持dag中所有内容均为 node的情况下计算。返回的值，是理论最大线程数，不是最优值
     */
    NSize getMaxPara();

    /**
     * 将符合串行执行条件的pipeline，设定为串行执行的模式。可以大幅度提升运行性能。
     * @return
     * @notice 如果pipeline不可以设定的话，通过返回值提示对应信息
     */
    NStatus makeSerial();

    /**
     * 获取当前的执行状态
     * @return
     */
    DPipelineState getCurState() const;

    /**
     * 判断两个element，是否有依赖关系
     * @param fst
     * @param snd
     * @return
     * @notice 如果返回 true，则表示不可能并发执行；否则表示可能并发执行
     */
    NBool checkSeparate(DElementPtr fst, DElementPtr snd) const;

    /**
     * 注册GParam 交互类集合
     * @return
     */
    NAO_DECLARE_DPARAM_MANAGER_WRAPPER

protected:
    explicit DPipeline();
    ~DPipeline() override;

private:
    /**
     * 初始化环境信息，包括线程池 等
     * @return
     */
    NStatus initEnv();

    /**
     * 内部真实一个 element 信息
     * @param element
     * @param dependElements
     * @param name
     * @param loop
     * @return
     */
    NStatus innerRegister(DElementPtr element, const DElementPtrSet& dependElements, const std::string& name, NSize loop);

    /** 不允许外部赋值和构造 */
    NAO_NO_ALLOWED_COPY(DPipeline)

private:
    DElementManagerPtr element_manager_ = nullptr;   // 节点管理类（管理所有注册过的element信息）
    DParamManagerPtr   param_manager_   = nullptr;   // 参数管理类
    DDaemonManagerPtr  daemon_manager_  = nullptr;   // 守护管理类
    DEventManagerPtr   event_manager_   = nullptr;   // 事件管理类

    DSchedule          schedule_;     // 调度管理类
    DElementRepository repository_;   // 记录创建的所有element的仓库

    friend class DPipelineFactory;
    friend class UAllocator;
    friend class DPerf;
};

using DPipelinePtr     = DPipeline*;
using DPipelinePtrList = std::list<DPipelinePtr>;

NAO_NAMESPACE_END

#include "DPipeline.inl"

#endif   // NAO_DPIPELINE_H
