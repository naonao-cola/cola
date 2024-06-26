/**
 * @FilePath     : /cola/src/Dag/DagElement/DElement.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 11:31:32
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-26 16:28:52
 **/
#ifndef NAO_DELEMENT_H
#define NAO_DELEMENT_H

#include <algorithm>
#include <atomic>
#include <set>
#include <string>
#include <vector>


#include "DElementDefine.h"
#include "DElementObject.h"
#include "DElementRelation.h"

NAO_NAMESPACE_BEGIN

enum class DMultiConditionType;
struct DPerfInfo;

/**
元素依赖 event aspect param,可以先看这几个,顺序，param  event message daemon aspect
 */
class DElement : public DElementObject, public NDescInfo
{
public:
    /**
     * 实现添加切面的逻辑
     * @tparam TAspect
     * @tparam TParam
     * @param param
     * @return
     */
    template<typename TAspect, typename TParam = DAspectDefaultParam, c_enable_if_t<std::is_base_of<DAspect, TAspect>::value, int> = 0,
             c_enable_if_t<std::is_base_of<DAspectParam, TParam>::value, int> = 0>
    DElement* addDAspect(TParam* param = nullptr);

    /**
     * 实现添加模板切面的逻辑
     * @tparam TAspect
     * @tparam Args
     * @return
     */
    template<typename TAspect, typename... Args, c_enable_if_t<std::is_base_of<DTemplateAspect<Args...>, TAspect>::value, int> = 0>
    DElement* addDAspect(Args... args);

    /**
     * 添加当前element内部参数
     * @tparam T
     * @param key
     * @param param
     * @return
     */
    template<typename T, c_enable_if_t<std::is_base_of<DElementParam, T>::value, int> = 0>
    DElement* addEParam(const std::string& key, T* param);

    /**
     * 添加依赖节点信息
     * @param elements
     * @return
     */
    NStatus addDependDElements(const std::set<DElement*>& elements);

    /**
     * 设置name信息
     * @param name
     * @return
     */
    DElement* setName(const std::string& name) override;

    /**
     * 设置循环次数
     * @param loop
     * @return
     */
    DElement* setLoop(NSize loop);

    /**
     * 设置level信息，用于控制init和destroy方法的执行顺序
     * level值越低，函数越先执行
     * @param level
     * @return
     */
    DElement* setLevel(NLevel level);

    /**
     * 设置visible信息。当 visible = false 的时候，算子实际不执行
     * @param visible
     * @return
     */
    DElement* setVisible(NBool visible);

    /**
     * 设定绑定的线程id
     * @param index，需要绑定的 thread id 信息
     * @return
     * @notice 本接口仅保证绑定线程优先调度，但不保证最终一定在绑定线程上执行。若不了解调度机制，不建议使用本接口，否则可能导致运行时阻塞。
     */
    DElement* setBindingIndex(NIndex index);

    /**
     * 设定当前算子的超时时间
     * @param timeout 超时时间
     * @param strategy 当超时的时候，处理的策略
     * @return
     */
    DElement* setTimeout(NMSec timeout, DElementTimeoutStrategy strategy = DElementTimeoutStrategy::AS_ERROR);

    /**
     * 当前element是否是一个 group逻辑
     * @return
     */
    NBool isGroup() const;

    /**
     * 获取当前节点状态信息
     * @return
     */
    DElementState getCurState() const;

    /**
     * 实现连续注册的语法糖，形如：
     *  (*a)-->b&c;
     *  (*b)-->d;
     *  (*c)-->d;
     *  (*b)*2;
     * @return
     */
    DElement& operator--(int) noexcept;
    DElement& operator>(DElement* element);
    DElement& operator&(DElement* element);
    DElement& operator*(NSize loop) noexcept;

protected:
    /**
     * 构造函数
     */
    explicit DElement() = default;

    /**
     * 析构函数
     */
    ~DElement() override;

    /**
     * init()后，第一次执行run之前，会执行的函数
     * @return
     * @notice 主要为了弥补init()方法，不是并发执行的缺陷。也属于单次执行的函数
     */
    virtual NStatus prepareRun();

    /**
     * run() 方法完成之后（包含所有循环次数和 isHold逻辑）的校验函数
     * @return
     * @notice 不建议在异步element中使用
     */
    virtual NStatus checkRunResult();

    /**
     * 是否持续进行
     * 默认为false，表示执行且仅执行一次
     * @return
     */
    virtual NBool isHold();

    /**
     * 用于在MultiCondition中被判定，是否可以执行。
     * @return
     * @notice 默认返回false，不执行
     */
    virtual NBool isMatch();

    /**
     * 崩溃流程处理
     * @param ex
     * @return
     * @notice 可以自行覆写crashed方法，但不推荐。如果需要复写的话，返回值需要填写 STATUS_CRASH，否则可能出现执行异常
     */
    virtual NStatus crashed(const NException& ex);

    /**
     * 获取当前element内部参数
     * @tparam T
     * @param key
     * @return
     */
    template<typename T, c_enable_if_t<std::is_base_of<DElementParam, T>::value, int> = 0>
    T* getEParam(const std::string& key);

    /**
     * 获取执行线程对应的信息
     * @return
     * @notice 启动线程返回-1（CGRAPH_MAIN_THREAD_ID），辅助线程返回-2（CGRAPH_SECONDARY_THREAD_COMMON_ID），主线程返回 线程index
     */
    NIndex getThreadIndex();

    /**
     * 判断当前是否超时
     * @return
     */
    NBool isTimeout() const;

    /**
     * 获取绑定线程id信息
     * @return
     * @notice 不同的group类型，获取 binding index 的方式不同
     */
    NIndex getBindingIndex() const;

    /**
     * 获取当前节点的相关关系信息，包含前驱、后继、从属关系
     * @return
     */
    DElementRelation getRelation() const;

    NAO_NO_ALLOWED_COPY(DElement);

    /********************************/
    /** 使用者请勿复写private中的函数 **/
    /********************************/
private:
    /**
     * run方法执行之前的执行函数
     * @return
     */
    NVoid beforeRun();

    /**
     * run方法执行之后的执行函数
     * @return
     */
    NVoid afterRun();

    /**
     * 判定node是否可以和前面节点一起执行
     * @return
     */
    NBool isLinkable() const;

    /**
     * 判定当前的内容，是否需要异步执行
     * @return
     */
    NBool isAsync() const;

    /**
     * 判断当前是否可以在运行时更新图结构逻辑
     * @return
     */
    NBool isMutable() const;

    /**
     * 判断当前element是否已经被注册到特定pipeline中了。避免反复注册的问题
     * @return
     */
    virtual NBool isRegistered() const;

    /**
     * 执行切面逻辑
     * @param aspectType
     * @param curStatus
     * @return
     */
    NStatus doAspect(const DAspectType& aspectType, const NStatus& curStatus = NStatus());

    /**
     * 设置element信息
     * @param dependElements
     * @param name
     * @param loop
     * @return
     */
    virtual NStatus addElementInfo(const std::set<DElement*>& dependElements, const std::string& name, NSize loop);

    /**
     * 设置manager信息
     * @param paramManager
     * @param eventManager
     * @return
     */
    virtual NStatus addManagers(DParamManagerPtr paramManager, DEventManagerPtr eventManager);

    /**
     * 包含切面相关功能的函数，fat取自fatjar的意思
     * @param type
     * @return
     */
    NStatus fatProcessor(const NFunctionType& type);

    /**
     * 设置线程池信息
     * @param ptr
     * @return
     */
    DElement* setThreadPool(UThreadPoolPtr ptr);

    /**
     * graphviz dump 逻辑
     * @param oss
     * @return
     */
    virtual NVoid dump(std::ostream& oss);

    /**
     * graphviz dump 边逻辑
     * @param oss
     * @param src 表示开始的元素
     * @param dst 表示结束的元素
     * @param label
     * @return
     */
    NVoid dumpEdge(std::ostream& oss, DElement* src, DElement* dst, const std::string& label = NAO_EMPTY);

    /**
     * graphviz dump 点逻辑
     * @param oss
     */
    virtual NVoid dumpElement(std::ostream& oss);

    /**
     * dump 当前element的header。主要就是为了代码
     * @param oss
     * @return
     */
    NVoid dumpElementHeader(std::ostream& oss);

    /**
     * graphviz dump perf逻辑
     * @param oss
     * @return
     */
    NVoid dumpPerfInfo(std::ostream& oss);

    /**
     * 判断是否进入 yield状态。如果是的话，则等待恢复。未进入yield状态，则继续运行
     * @return
     */
    inline NVoid checkYield();

    /**
     * 判断当前元素，是否可以线性执行。默认返回true
     * @return
     */
    virtual NBool isSerializable() const;

    /**
     * 弹出一个最后一个切面
     * @return
     */
    NStatus popLastAspect();

    /**
     * 异步执行
     * @return
     */
    NStatus asyncRun();

    /**
     * 异步获取结果信息
     * @return
     */
    NStatus getAsyncResult();

    /**
     * 判断当前节点，是否符合执行逻辑。主要用于init的早期阶段
     * @return
     */
    virtual NStatus checkSuitable();

    /**
     * 获取链路所有的 belong信息
     * @param reverse
     * @return
     */
    std::vector<DElement*> getDeepPath(NBool reverse) const;

private:
    /** 状态相关信息 */
    NBool                      done_{false};                           // 判定被执行结束
    NBool                      linkable_{false};                       // 判定是否可以连通计算
    NBool                      visible_{true};                         // 判定可见的，如果被删除的话，则认为是不可见的
    NBool                      is_init_{false};                        // 判断是否init
    DElementType               element_type_{DElementType::ELEMENT};   // 用于区分element 内部类型
    std::atomic<DElementState> cur_state_{DElementState::CREATE};      // 当前执行状态

    /** 配置相关信息 */
    NSize                   loop_{NAO_DEFAULT_LOOP_TIMES};               // 元素执行次数
    NLevel                  level_{NAO_DEFAULT_ELEMENT_LEVEL};           // 用于设定init的执行顺序(值小的，优先init，可以为负数)
    NIndex                  binding_index_{NAO_DEFAULT_BINDING_INDEX};   // 用于设定绑定线程id
    NMSec                   timeout_{NAO_DEFAULT_ELEMENT_TIMEOUT};       // 超时时间信息（0表示不计算超时）
    DElementTimeoutStrategy timeout_strategy_{DElementTimeoutStrategy::AS_ERROR};   // 判定超时的情况下，是否返回错误

    /** 执行期间相关信息 */
    DElementParamMap  local_params_;              // 用于记录当前element的内部参数
    DAspectManagerPtr aspect_manager_{nullptr};   // 整体流程的切面管理类
    UThreadPoolPtr    thread_pool_{nullptr};      // 用于执行的线程池信息
    DPerfInfo*        perf_info_{nullptr};        // 用于perf的信息
    NBool             is_prepared_{false};        // 判断是否已经执行过 prepareRun() 方法

    /** 图相关信息 */
    std::atomic<NSize>  left_depend_{0};    // 当 left_depend_ 值为0的时候，即可以执行该element信息
    std::set<DElement*> run_before_;        // 被依赖的节点（后继）
    std::set<DElement*> dependence_;        // 依赖的节点信息（前驱）
    DElement*           belong_{nullptr};   // 从属的element 信息，如为nullptr，则表示从属于 pipeline

    /** 异步执行相关信息 */
    std::future<NStatus>    async_result_;   // 用于记录当前节点的异步执行情况
    std::mutex              yield_mutex_;    // 控制停止执行的锁
    std::condition_variable yield_cv_;       // 控制停止执行的条件变量

    friend class DNode;
    friend class DGroup;
    friend class DCluster;
    friend class DRegion;
    friend class DCondition;
    friend class DMutable;
    template<NInt>
    friend class DSome;
    template<DMultiConditionType>
    friend class DMultiCondition;
    friend class DPipeline;
    friend class DElementManager;
    friend class DElementSorter;
    friend class DAdapter;
    friend class DFunction;
    friend class DFence;
    template<NInt>
    friend class DCoordinator;
    template<typename T>
    friend class DSingleton;
    friend class DEngine;
    friend class DDynamicEngine;
    friend class DTopoEngine;
    friend class DAspectObject;
    friend class DOptimizer;
    friend class DMaxParaOptimizer;
    friend class DSeparateOptimizer;
    friend class DElementRepository;
    friend class DPerf;

    NAO_DECLARE_DPARAM_MANAGER_WRAPPER_WITH_MEMBER
    NAO_DECLARE_DEVENT_MANAGER_WRAPPER_WITH_MEMBER
};

using DElementRef    = DElement&;
using DElementPtr    = DElement*;
using DElementCPtr   = const DElement*;
using DElementPPtr   = DElementPtr*;
using DElementPtrArr = std::vector<DElementPtr>;
using DElementPtrSet = std::set<DElementPtr>;
NAO_NAMESPACE_END

#endif   // NAO_DELEMENT_H