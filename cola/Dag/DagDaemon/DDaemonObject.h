/**
 * @FilePath     : /cola/src/Dag/DagDaemon/DDaemonObject.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 18:08:30
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 09:40:02
 **/
#ifndef NAO_DDAEMONOBJECT_H
#define NAO_DDAEMONOBJECT_H

#include "../DagEvent/DEventInclude.h"
#include "../DagObject.h"
#include "../DagParam/DParamInclude.h"


NAO_NAMESPACE_BEGIN

class DDaemonObject : public DagObject, public NDescInfo
{
protected:
    explicit DDaemonObject() { session_ = URandom<>::generateSession(NAO_STR_DAEMON); }

    ~DDaemonObject() override { NAO_DELETE_PTR(param_) }

    /**
     * 设置休眠时间信息，单位ms
     * @return
     */
    virtual DDaemonObject* setInterval(NMSec interval)
    {
        if (0 == interval) {
            return this;
        }

        interval_ = interval;
        return this;
    }

    /**
     * 设置daemon中参数，类型为DDaemonParam (即：DPassedParam)
     * @tparam T
     * @param param
     * @return
     */
    template<typename DParam, c_enable_if_t<std::is_base_of<DDaemonParam, DParam>::value, int> = 0>
    DDaemonObject* setDParam(DParam* param);

    NAO_DECLARE_DPARAM_MANAGER_WRAPPER

    NAO_DECLARE_DEVENT_MANAGER_WRAPPER

private:
    /**
     * 所有Daemon均不执行run方法
     * @return
     */
    NStatus run() final{NAO_NO_SUPPORT}

    NAO_NO_ALLOWED_COPY(DDaemonObject)

        friend class DDaemon;
    friend class DDaemonManager;
    friend class DPipeline;

private:
    DParamManagerPtr param_manager_ = nullptr;   // GParam参数管理类
    DEventManagerPtr event_manager_ = nullptr;   // 事件管理类
    DDaemonParamPtr  param_         = nullptr;   // 用于存储daemon对象
    NMSec            interval_      = 0;         // 执行间隔时间
};

using DDaemonObjectPtr = DDaemonObject*;

NAO_NAMESPACE_END

#include "DDaemonObject.inl"

#endif   // NAO_DDAEMONOBJECT_H