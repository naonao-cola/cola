/**
 * @FilePath     : /cola/src/Dag/DagPipeline/_DPerf/DPerf.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-28 10:20:51
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 14:04:59
 **/
#include "DPerf.h"
#include "../DPipeline.h"
#include "DPerfDefine.h"
#include "../../DagElement/_DOptimizer/DOptimizerInclude.h"


NAO_NAMESPACE_BEGIN

NStatus DPerf::perf(DPipelinePtr pipeline, std::ostream& oss)
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_NOT_NULL(pipeline)

    status = inject(pipeline);
    NAO_FUNCTION_CHECK_STATUS

    status = pipeline->process();
    NAO_FUNCTION_CHECK_STATUS

    status = markLongestPath(pipeline);
    NAO_FUNCTION_CHECK_STATUS

    status = pipeline->dump(oss);
    NAO_FUNCTION_CHECK_STATUS

    status = recover(pipeline);
    NAO_FUNCTION_CHECK_STATUS

    NAO_FUNCTION_END
}


NStatus DPerf::inject(DPipelinePtr pipeline)
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_NOT_NULL(pipeline)

    const NFMSec now = NAO_GET_CURRENT_ACCURATE_MS();
    for (auto* cur : pipeline->repository_.elements_) {
        /**
         * 给其中的每个element，都添加这个切面信息
         * 这里是不需要考虑 delete perf_info_的，因为在 element结束的时候，会自动释放
         */
        cur->perf_info_ = UAllocator::safeMallocNStruct<DPerfInfo>();
        cur->addDAspect<DPerfAspect<NFMSec, DPerfInfoPtr>>(now, cur->perf_info_);
    }
    NAO_FUNCTION_END
}


NStatus DPerf::markLongestPath(DPipelinePtr pipeline)
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_NOT_NULL(pipeline, pipeline->element_manager_)
    DElementPtrArr longestPath;
    NFMSec         maxTs = 0.0;
    auto           paths = DOptimizer::collectPaths(pipeline->element_manager_->manager_elements_);
    for (const auto& path : paths) {
        NFMSec curTs = 0.0;
        for (auto* element : path) {
            NAO_ASSERT_NOT_NULL(element)
            curTs += element->perf_info_->accu_cost_ts_;
        }
        if (curTs >= maxTs) {
            maxTs       = curTs;
            longestPath = path;
        }
    }

    for (auto* element : longestPath) {
        NAO_ASSERT_NOT_NULL(element->perf_info_)
        element->perf_info_->in_longest_path_ = true;
    }
    NAO_FUNCTION_END
}


NStatus DPerf::recover(DPipelinePtr pipeline)
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_NOT_NULL(pipeline)
    for (auto* cur : pipeline->repository_.elements_) {
        status += cur->popLastAspect();
        NAO_DELETE_PTR(cur->perf_info_)
    }
    NAO_FUNCTION_END
}

NAO_NAMESPACE_END
