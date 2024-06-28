/**
 * @FilePath     : /cola/src/Dag/DagPipeline/DPipelineFactory.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-28 10:36:59
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 10:36:59
**/
#ifndef  NAO_DPIPELINEFACTORY_H
#define  NAO_DPIPELINEFACTORY_H

#include <list>
#include <mutex>

#include "DPipeline.h"
#include "DPipelineObject.h"

NAO_NAMESPACE_BEGIN

class DPipelineFactory : public DPipelineObject {
public:
    /**
     * 创建一个pipeline信息
     * @return
     */
    static DPipelinePtr create();

    /**
     * 销毁一个pipeline信息
     * @param pipeline
     * @return
     */
    static NStatus remove(DPipelinePtr pipeline);

    /**
     * 清空所有的pipeline信息
     */
    static NStatus clear();

private:
    static DPipelinePtrList s_pipeline_list_;    // 记录所有的
    static std::mutex s_lock_;
};

NAO_NAMESPACE_END

#endif //NAO_DPIPELINEFACTORY_H
