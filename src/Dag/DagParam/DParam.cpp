/**
 * @FilePath     : /cola/src/Dag/DagParam/DParam.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 11:50:54
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 11:50:54
**/
#include "DParam.h"

NAO_NAMESPACE_BEGIN

std::vector<std::string> DParam::getBacktrace() {
    NAO_THROW_EXCEPTION_BY_CONDITION(!backtrace_enable_,    \
                                        "no enable backtrace for [" + key_ + "] param")

    std::vector<std::string> traces;
    backtrace_lock_.lock();
    backtrace_.getUniqueArray(traces);
    backtrace_lock_.unlock();

    return traces;
}

NStatus DParam::addBacktrace(const std::string& trace) {
    NAO_FUNCTION_BEGIN
    NAO_RETURN_ERROR_STATUS_BY_CONDITION(!backtrace_enable_,    \
                                            "no enable backtrace for [" + key_ + "] param")

    // 如果name不为空，则添加name信息。如果name为空，则添加session信息
    backtrace_lock_.lock();
    backtrace_.uniqueAdd(trace);
    backtrace_lock_.unlock();

    NAO_FUNCTION_END
}

NVoid DParam::cleanBacktrace() {
    if (!backtrace_enable_) {
        return;
    }

    backtrace_lock_.lock();
    backtrace_.clear();
    backtrace_lock_.unlock();
}


std::string DParam::getKey() const {
    return key_;
}


NStatus DParam::setup() {
    NAO_EMPTY_FUNCTION
}


NVoid DParam::reset(const NStatus& curStatus) {
}

NAO_NAMESPACE_END