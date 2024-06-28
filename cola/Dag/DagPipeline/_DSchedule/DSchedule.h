/**
 * @FilePath     : /cola/src/Dag/DagPipeline/_DSchedule/DSchedule.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-28 10:22:32
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 10:22:33
 **/
#ifndef NAO_DSCHEDULE_H
#define NAO_DSCHEDULE_H

#include "../../DagObject.h"
#include "DScheduleDefine.h"

NAO_NAMESPACE_BEGIN

class DSchedule : public DagObject
{
private:
    /**
     * 设置调度资源模式
     * @param tp
     * @return
     */
    NStatus makeType(UThreadPoolPtr tp);

    /**
     * 获取线程池信息
     * @return
     */
    UThreadPoolPtr getThreadPool();

    NStatus init() final;

    NStatus run() final;

    NStatus destroy() final;

    explicit DSchedule() = default;

    ~DSchedule() override;

private:
    UThreadPoolPtr          unique_tp_ = nullptr;                      // 内部独占的线程池
    UThreadPoolPtr          shared_tp_ = nullptr;                      // 外部输入的线程池
    UThreadPoolConfig       config_;                                   // 线程池的配置信息
    internal::DScheduleType type_ = internal::DScheduleType::UNIQUE;   // 调度类型

    friend class DPipeline;
};

NAO_NAMESPACE_END

#endif   // NAO_DSCHEDULE_H
