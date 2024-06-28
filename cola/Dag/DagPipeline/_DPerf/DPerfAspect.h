/**
 * @FilePath     : /cola/src/Dag/DagPipeline/_DPerf/DPerfAspect.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-28 10:21:07
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 10:21:07
**/
#ifndef NAO_DPERFASPECT_H
#define NAO_DPERFASPECT_H

#include "../../DagAspect/DAspectInclude.h"
#include "DPerfDefine.h"

NAO_NAMESPACE_BEGIN

template <typename ...Args>
class DPerfAspect : public DTemplateAspect<Args ...> {
protected:
    explicit DPerfAspect(NFMSec startTs, DPerfInfoPtr perfInfo) {
        NAO_ASSERT_NOT_NULL_THROW_ERROR(perfInfo)
        pipeline_start_ts_ = startTs;
        perf_info_ = perfInfo;
    }

    NStatus beginRun() final {
        NAO_FUNCTION_BEGIN

        cur_start_ts_ = NAO_GET_CURRENT_ACCURATE_MS();
        if (0 == perf_info_->first_start_ts_) {
            // 记录开始的时间信息，仅记录第一次运行到这个node的时间信息
            perf_info_->first_start_ts_ = (cur_start_ts_ - pipeline_start_ts_);
        }
        NAO_FUNCTION_END
    }

    NVoid finishRun(const NStatus& curStatus) final {
        auto cur = NAO_GET_CURRENT_ACCURATE_MS();
        perf_info_->last_finish_ts_ = cur - pipeline_start_ts_;
        perf_info_->loop_++;
        perf_info_->accu_cost_ts_ += (cur - cur_start_ts_);
    }

private:
    NFMSec pipeline_start_ts_ = 0.0;                  // 流水线开启的时间
    NFMSec cur_start_ts_ = 0.0;                       // 当前element开始运行的时间
    DPerfInfoPtr perf_info_ = nullptr;                // 具体赋值的对象

    friend class UAllocator;
};

NAO_NAMESPACE_END

#endif //NAO_DPERFASPECT_H
