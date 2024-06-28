/**
 * @FilePath     : /cola/src/Dag/DagPipeline/_DPerf/DPerfDefine.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-28 10:21:25
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 10:21:25
 **/
#ifndef NAO_DPERFDEFINE_H
#define NAO_DPERFDEFINE_H

#include "../../DagDefine.h"

NAO_NAMESPACE_BEGIN

struct DPerfInfo : public NStruct
{
    NUint  loop_            = 0;       // 循环数
    NFMSec first_start_ts_  = 0.0;     // 开始的时间戳
    NFMSec last_finish_ts_  = 0.0;     // 最后一次结束的时间（需要考虑多次执行，或者多次被循环执行的情况）
    NFMSec accu_cost_ts_    = 0.0;     // 总体的耗时信息(累计值)
    NBool  in_longest_path_ = false;   // 是否在最长耗时链路上
};

using DPerfInfoPtr = DPerfInfo*;

NAO_NAMESPACE_END

#endif   // NAO_DPERFDEFINE_H