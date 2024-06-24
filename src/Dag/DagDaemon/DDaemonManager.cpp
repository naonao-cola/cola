/**
 * @FilePath     : /cola/src/Dag/DagDaemon/DDaemonManager.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 18:08:11
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 18:08:11
**/
#include "DDaemonManager.h"

NAO_NAMESPACE_BEGIN

DDaemonManager::~DDaemonManager() {
    clear();
}


NStatus DDaemonManager::init() {
    NAO_FUNCTION_BEGIN
    for (auto daemon: daemons_) {
        NAO_ASSERT_NOT_NULL(daemon)
        status += daemon->init();
    }

    NAO_FUNCTION_END
}


NStatus DDaemonManager::destroy() {
    NAO_FUNCTION_BEGIN
    for (auto daemon: daemons_) {
        NAO_ASSERT_NOT_NULL(daemon)
        status += daemon->destroy();
    }

    NAO_FUNCTION_END
}


NStatus DDaemonManager::add(DDaemonPtr daemon) {
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_NOT_NULL(daemon)

    daemons_.insert(daemon);
    NAO_FUNCTION_END
}


NStatus DDaemonManager::remove(DDaemonPtr daemon) {
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_NOT_NULL(daemon)

    daemons_.erase(daemon);
    NAO_DELETE_PTR(daemon)
    NAO_FUNCTION_END
}


NStatus DDaemonManager::clear() {
    NAO_FUNCTION_BEGIN
    for (auto daemon: daemons_) {
        NAO_DELETE_PTR(daemon)
    }

    daemons_.clear();
    NAO_FUNCTION_END
}


NSize DDaemonManager::getSize() const {
    return daemons_.size();
}

NAO_NAMESPACE_END
