/**
 * @FilePath     : /cola/src/Dag/DagAspect/DAspectObject.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 20:03:13
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 20:31:44
 **/
#ifndef NAO_DASPECTOBJECT_H
#define NAO_DASPECTOBJECT_H

#include <string>

#include "../DagEvent/DEventInclude.h"
#include "../DagObject.h"
#include "../DagParam/DParamInclude.h"
#include "DAspectDefine.h"


NAO_NAMESPACE_BEGIN

class DElement;

class DAspectObject : public DagObject, public NDescInfo
{
protected:
    explicit DAspectObject();

    ~DAspectObject() override;

    /**
     * 获取切面参数内容
     * @return
     */
    template<typename T, c_enable_if_t<std::is_base_of<DAspectParam, T>::value, int> = 0>
    T* getAParam();

    /**
     * 设置切面参数内容
     * @param param
     */
    template<typename T, c_enable_if_t<std::is_base_of<DAspectParam, T>::value, int> = 0>
    DAspectObject* setAParam(T* param);

    /**
     * 设置从属的 element信息
     * @param belong
     * @return
     */
    DAspectObject* setBelong(DElement* belong);

    const std::string& getName() const override;

    NAO_NO_ALLOWED_COPY(DAspectObject)

    NAO_DECLARE_DPARAM_MANAGER_WRAPPER

    NAO_DECLARE_DEVENT_MANAGER_WRAPPER

private:
    /**
     * GAspect 相关内容，不需要执行run方法
     * @return
     */
    NStatus run() final { NAO_NO_SUPPORT }

private:
    DAspectParamPtr  param_{nullptr};           // 参数信息
    DParamManagerPtr param_manager_{nullptr};   // GParam参数管理类
    DEventManagerPtr event_manager_{nullptr};   // 事件管理类
    DElement*        belong_{nullptr};          // 从属的 element信息

    friend class DAspectManager;
    friend class DAspect;
    friend class DElement;
};

using DAspectObjectPtr = DAspectObject*;

NAO_NAMESPACE_END

#include "DAspectObject.inl"

#endif   // NAO_DASPECTOBJECT_H
