
/**
 * @FilePath     : /cola/src/Dag/DagElement/DGroup/DCluster/DCluster.h
 * @Description  : 实现多个element，线性执行的功能
 * @Author       : naonao
 * @Date         : 2024-06-26 15:25:46
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-26 17:11:41
**/
#ifndef NAO_DCLUSTER_H
#define NAO_DCLUSTER_H

#include <vector>

#include "../DGroup.h"

NAO_NAMESPACE_BEGIN

class DCluster : public DGroup {
protected:
    explicit DCluster();

private:
    NStatus run() final;

    NVoid dump(std::ostream& oss) final;

    NBool isSeparate(DElementCPtr a, DElementCPtr b) const final;

    friend class DPipeline;
    friend class DDynamicEngine;
    friend class UAllocator;
};

NAO_NAMESPACE_END

#endif //NAO_DCLUSTER_H
