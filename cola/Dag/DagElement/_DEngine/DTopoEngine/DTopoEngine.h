/**
 * @FilePath     : /cola/src/Dag/DagElement/_DEngine/DTopoEngine/DTopoEngine.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-26 11:45:52
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-26 11:45:52
 **/
#ifndef NAO_DTOPOENGINE_H
#define NAO_DTOPOENGINE_H

#include <set>
#include <vector>


#include "../DEngine.h"

NAO_NAMESPACE_BEGIN

class DTopoEngine : public DEngine
{
protected:
    explicit DTopoEngine() = default;

    NStatus setup(const DSortedDElementPtrSet& elements) override;

    NStatus run() override;

private:
    DElementPtrArr topo_elements_{};   // 拓扑排序后的

    friend class UAllocator;
    friend class GElementManager;
};

NAO_NAMESPACE_END

#endif   // NAO_DTOPOENGINE_H