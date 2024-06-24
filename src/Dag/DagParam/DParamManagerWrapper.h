/**
 * @FilePath     : /cola/src/Dag/DagParam/DParamManagerWrapper.h
 * @Description  : 本类的子类型，有获取GParam的功能
 * @Author       : naonao
 * @Date         : 2024-06-24 11:53:49
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 14:28:35
**/
#ifndef NAO_DPARAMMANAGERWRAPPER_H
#define NAO_DPARAMMANAGERWRAPPER_H

#include <string>
#include <vector>

#include "DParam.h"
#include "DParamManager.h"

NAO_NAMESPACE_BEGIN

#define NAO_DECLARE_DPARAM_MANAGER_WRAPPER                                           \
    /**
     * 创建param信息，如果创建成功，则直接返回ok
     * @tparam TGParam
     * @param key
     * @param backtrace
     * @return
     */                                                                                 \
    template<typename TGParam,                                                          \
             c_enable_if_t<std::is_base_of<DParam, TGParam>::value, int> = 0>           \
    NStatus createDParam(const std::string& key, NBool backtrace = false) {             \
        NAO_ASSERT_NOT_NULL(param_manager_)                                             \
        return param_manager_->create<TGParam>(key, backtrace);                         \
    }                                                                                   \
                                                                                        \
    /**
     * 获取参数信息，如果未找到，则返回nullptr
     * @tparam TGParam
     * @param key
     * @return
     */                                                                                 \
    template<typename TGParam,                                                          \
             c_enable_if_t<std::is_base_of<DParam, TGParam>::value, int> = 0>           \
    TGParam* getDParam(const std::string& key) {                                        \
        NAO_ASSERT_NOT_NULL_THROW_ERROR(param_manager_)                                 \
        auto param = param_manager_->get<TGParam>(key);                                 \
        if (nullptr != param) {                                                         \
            concerned_params_.insert(param);                                            \
            param->addBacktrace(name_.empty() ? session_ : name_);                      \
        }                                                                               \
        return param;                                                                   \
    }                                                                                   \
                                                                                        \
    /**
     * 获取参数信息，如果未找到，则返回nullptr
     * @tparam TGParam
     * @param key
     * @return
     */                                                                                 \
    template<typename TGParam,                                                          \
             c_enable_if_t<std::is_base_of<DParam, TGParam>::value, int> = 0>           \
    TGParam* getDParamWithNoEmpty(const std::string& key) {                             \
        auto* param = getDParam<TGParam>(key);                                          \
        if (nullptr == param) {                                                         \
            NAO_THROW_EXCEPTION("param [" + key + "] is null")                          \
        }                                                                               \
        return param;                                                                   \
    }                                                                                   \
                                                                                        \
    /**
     * 删除param信息
     * @param key
     * @return
     */                                                                                 \
    NStatus removeDParam(const std::string& key) {                                      \
        NAO_ASSERT_NOT_NULL(param_manager_)                                             \
        return param_manager_->removeByKey(key);                                        \
    }                                                                                   \
                                                                                        \
    /**
     * 获取所有的keys信息
     * @param keys
     * @return
     */                                                                                 \
    std::vector<std::string> getDParamKeys() {                                          \
        NAO_ASSERT_NOT_NULL_THROW_ERROR(param_manager_)                                 \
        return param_manager_->getKeys();                                               \
    }                                                                                   \
                                                                                        \
     /**
     * 获取所有的keys信息
     * @param keys
     * @return
     */                                                                                 \
    std::vector<std::string> getConcernedDParamKeys() const {                           \
        std::vector<std::string> keys;                                                  \
        for (auto param : concerned_params_) {                                          \
            keys.emplace_back(param->getKey());                                         \
        }                                                                               \
        return keys;                                                                    \
    }                                                                                   \
                                                                                        \
private:                                                                                \
    /**
     * 设置统一管控参数类
     * @param pm
     * @return
     */                                                                                 \
    virtual void* setDParamManager(DParamManagerPtr pm) {                               \
        this->param_manager_ = pm;                                                      \
        return this;                                                                    \
    }                                                                                   \
                                                                                        \
    DParamPtrSet concerned_params_;    /* 记录链路上使用过GParam信息 */                   \
                                                                                        \
protected:                                                                              \
                                                                                        \

#define NAO_DECLARE_DPARAM_MANAGER_WRAPPER_WITH_MEMBER                                 \
private:                                                                                \
    DParamManagerPtr param_manager_ = nullptr;                                          \
protected:                                                                              \
    NAO_DECLARE_DPARAM_MANAGER_WRAPPER                                               \
                                                                                        \

NAO_NAMESPACE_END

#endif //NAO_DPARAMMANAGERWRAPPER_H
