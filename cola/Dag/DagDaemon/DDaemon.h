/**
 * @FilePath     : /cola/src/Dag/DagDaemon/DDaemon.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 18:06:19
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 09:39:27
 **/
#ifndef NAO_DDAEMON_H
#define NAO_DDAEMON_H

#include <set>

#include "../DagParam/DParamInclude.h"
#include "DDaemonObject.h"


NAO_NAMESPACE_BEGIN

class DDaemon : public DDaemonObject
{
protected:
    /**
     * 后台执行函数，间隔interval时间后，执行一次
     * @param param
     * @return
     */
    virtual NVoid daemonTask(DDaemonParamPtr param) = 0;

    /**
     * 获取设置的延时信息
     * @return
     */
    NMSec getInterval() const;

private:
    NStatus init() final;
    NStatus destroy() final;

    friend class DDaemonManager;
    friend class DPipeline;

private:
    UTimer timer_;   // 计时器
};

using DDaemonPtr = DDaemon*;
using DDaemonSet = std::set<DDaemonPtr>;

NAO_NAMESPACE_END

#include "DDaemonObject.inl"

#endif   // CGRAPH_GDAEMON_H
