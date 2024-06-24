/**
 * @FilePath     : /cola/src/Dag/DagParam/DParam.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 11:50:35
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 12:29:59
 **/
#ifndef NAO_DPARAM_H
#define NAO_DPARAM_H

#include "DParamObject.h"
#include <set>

NAO_NAMESPACE_BEGIN

/**

参数信息，主要包含 字符串名称信息， 参数调用流程的堆栈
 */
class DParam : public DParamObject
{
public:
#if __cplusplus >= 201703L
    std::shared_mutex _param_shared_lock_;   // 用于参数互斥的锁信息
#else
    std::mutex _param_shared_lock_;
#endif

    /**
     * 获取参数的调用栈信息
     * @return
     */
    std::vector<std::string> getBacktrace();

    /**
     * 添加trace信息
     * @param trace
     * @return
     */
    NStatus addBacktrace(const std::string& trace);

    /**
     * 清空trace信息
     * @return
     */
    NVoid cleanBacktrace();

    /**
     * 获取key信息
     * @return
     */
    std::string getKey() const;

protected:
    /**
     * 每次pipeline执行前，会调用一次setup，可以不实现
     * @return
     */
    virtual NStatus setup();

    /**
     * 每次pipeline执行结束，会调用一次reset
     * @param curStatus 本次pipeline执行的状态
     * @return
     * @notice 为丰富功能和简化操作，v2.4.0 版本中添加了curStatus作为入参，且取消纯虚函数设定。
     */
    virtual NVoid reset(const NStatus& curStatus);


private:
    NBool       backtrace_enable_ = false;   // 是否使能backtrace功能
    std::string key_;                        // 名称信息
    USerialUniqueArray<std::string> backtrace_;   // 记录参数的调用栈信息，仅记录get 此参数的地方。不包括 create和remove的地方。
    USpinLock backtrace_lock_;                    // 针对backtrace的自旋锁

    friend class DParamManager;
};

using DParamPtr    = DParam*;
using DParamPtrSet = std::set<DParamPtr>;

NAO_NAMESPACE_END

#endif