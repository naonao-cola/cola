/**
 * @FilePath     : /cola/src/Dag/DagDaemon/DDaemon.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 18:06:37
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 18:09:04
**/
#include "DDaemon.h"

NAO_NAMESPACE_BEGIN

NStatus DDaemon::init() {
    NAO_FUNCTION_BEGIN
    timer_.start(interval_, [this] {
        this->daemonTask(param_);
    });
    NAO_FUNCTION_END
}


NStatus DDaemon::destroy() {
    NAO_FUNCTION_BEGIN
    timer_.stop();
    NAO_FUNCTION_END
}


NMSec DDaemon::getInterval() const {
    return interval_;
}

NAO_NAMESPACE_END
