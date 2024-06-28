/**
 * @FilePath     : /cola/src/Dag/DagPipeline/DPipelineManager.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-28 10:38:02
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 10:38:02
 **/
#ifndef NAO_DPIPELINEMANAGER_H
#define NAO_DPIPELINEMANAGER_H

#include <list>
#include <mutex>

#include "DPipeline.h"
#include "DPipelineObject.h"


NAO_NAMESPACE_BEGIN

class DPipelineManager : public DPipelineObject, public DagManager<DPipeline>
{
public:
    NStatus init() final;

    NStatus run() final;

    NStatus destroy() final;

    NStatus add(DPipelinePtr ptr) final;

    NStatus clear() final;

    NBool find(DPipelinePtr ptr) const final;

    NStatus remove(DPipelinePtr ptr) final;

    NSize getSize() const final;

    ~DPipelineManager() override;

    /**
     * 获取一个可以运行的pipeline信息
     * 没有可用的话，则返回 nullptr
     * @return
     */
    DPipelinePtr fetch();

    /**
     * 释放一个pipeline信息，重新放回 free list 中去
     * @param ptr
     * @return
     */
    NStatus release(DPipelinePtr ptr);

private:
    DPipelinePtrList free_list_;   // 空闲的 pipeline信息
    DPipelinePtrList used_list_;   // 被用的 pipeline信息
    std::mutex       mutex_;       // 控制 free 和 used 之间转移的互斥量
};

NAO_NAMESPACE_END

#endif   // NAO_DPIPELINEMANAGER_H
