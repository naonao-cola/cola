/**
 * @FilePath     : /cola/src/Dag/DagPipeline/_DPerf/DPerf.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-28 10:20:36
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 10:20:36
 **/
#ifndef NAO_DPERF_H
#define NAO_DPERF_H

#include "../../DagObject.h"
#include "DPerfAspect.h"
#include "DPerfDefine.h"

NAO_NAMESPACE_BEGIN

class DPipeline;

class DPerf : public DagObject
{
protected:
    /**
     * 针对pipeline进行性能分析
     * @param pipeline
     * @return
     */
    static NStatus perf(DPipeline* pipeline, std::ostream& oss);

    /**
     * 注入对应的切面信息
     * @param pipeline
     * @return
     */
    static NStatus inject(DPipeline* pipeline);

    /**
     * 查询最长链路信息
     * @param pipeline
     * @return
     */
    static NStatus markLongestPath(DPipeline* pipeline);

    /**
     * 恢复原来的pipeline信息
     * @param pipeline
     * @return
     */
    static NStatus recover(DPipeline* pipeline);

    friend class DPipeline;
};

NAO_NAMESPACE_END

#endif   // NAO_DPERF_H
