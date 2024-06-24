/**
 * @FilePath     : /cola/src/Dag/DagParam/DParamManger.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 11:51:40
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 13:47:53
 **/
#include "DParamManager.h"

NAO_NAMESPACE_BEGIN

DParamManager::DParamManager()
{
    clear();
}
DParamManager::~DParamManager()
{
    clear();
}

NStatus DParamManager::init()
{
    NAO_FUNCTION_BEGIN
    for (auto& param : params_map_) {
        /**
         * 一般情况下，是不需要init的。需要注意init 和 setup的执行阶段的信息
         * init() 是在pipeline init的时候被执行的
         * setup() 是在pipeline run开始的时候被执行的
         */
        NAO_ASSERT_NOT_NULL(param.second);
        status += param.second->init();
    }
    NAO_FUNCTION_END
}


NStatus DParamManager::destroy()
{
    NAO_FUNCTION_BEGIN
    for (auto& param : params_map_) {
        NAO_ASSERT_NOT_NULL(param.second);
        status += param.second->destroy();
    }
    NAO_FUNCTION_END
}


NStatus DParamManager::clear()
{
    NAO_FUNCTION_BEGIN
    NAO_LOCK_GUARD lock(this->mutex_);
    for (auto& param : params_map_) {
        NAO_DELETE_PTR(param.second)
    }

    params_map_.clear();
    NAO_FUNCTION_END;
}

NVoid DParamManager::resetWithStatus(const NStatus& curStatus)
{
    for (auto& param : params_map_) {
        param.second->reset(curStatus);
    }
}


NStatus DParamManager::setup()
{
    NAO_FUNCTION_BEGIN
    for (auto& param : params_map_) {
        // 这里不需要判断非空，因为在init的时候，已经判断过了
        status += param.second->setup();
    }
    NAO_FUNCTION_END
}

NStatus DParamManager::removeByKey(const std::string& key)
{
    NAO_FUNCTION_BEGIN
    NAO_LOCK_GUARD lock(this->mutex_);   // 创建和销毁的时候，加锁2
    auto           param = params_map_.find(key);
    if (param == params_map_.end()) {
        NAO_RETURN_ERROR_STATUS("param [" + key + "] no find")
    }

    NAO_DELETE_PTR(param->second)
    params_map_.erase(param);
    NAO_FUNCTION_END
}


std::vector<std::string> DParamManager::getKeys()
{
    std::vector<std::string> keys;
    NAO_LOCK_GUARD           lock(this->mutex_);
    for (const auto& iter : params_map_) {
        keys.emplace_back(iter.first);
    }

    return keys;
}

NAO_NAMESPACE_END
