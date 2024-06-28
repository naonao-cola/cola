/**
 * @FilePath     : /cola/src/Dag/DagPipeline/DPipelineFactory.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-28 10:37:07
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 11:49:44
 **/
#include "DPipelineFactory.h"

NAO_NAMESPACE_BEGIN

DPipelinePtrList DPipelineFactory::s_pipeline_list_;
std::mutex       DPipelineFactory::s_lock_;

DPipelinePtr DPipelineFactory::create()
{
    NAO_FUNCTION_BEGIN
    NAO_LOCK_GUARD lock(s_lock_);

    auto pipeline = NAO_SAFE_MALLOC_NOBJECT(DPipeline)
    s_pipeline_list_.emplace_back(pipeline);
    return pipeline;
}


NStatus DPipelineFactory::remove(DPipelinePtr pipeline)
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_NOT_NULL(pipeline)

    NAO_LOCK_GUARD lock(s_lock_);
    s_pipeline_list_.remove(pipeline);
    NAO_DELETE_PTR(pipeline)

    NAO_FUNCTION_END
}


NStatus DPipelineFactory::clear()
{
    NAO_FUNCTION_BEGIN
    NAO_LOCK_GUARD lock(s_lock_);

    for (DPipelinePtr pipeline : DPipelineFactory::s_pipeline_list_) {
        NAO_DELETE_PTR(pipeline)
    }

    s_pipeline_list_.clear();
    NAO_FUNCTION_END
}

NAO_NAMESPACE_END
