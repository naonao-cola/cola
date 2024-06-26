/**
 * @FilePath     : /cola/src/Dag/DagElement/DGroup/DRegion/DRegion.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-26 15:26:11
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-26 15:44:42
**/
#ifndef NAO_DREGION_H
#define NAO_DREGION_H

#include "../DGroup.h"
#include "../../DElementManager.h"

NAO_NAMESPACE_BEGIN

class DRegion : public DGroup {
public:
    /**
     * 设置EngineType信息
     * @param type
     * @return
     */
    DRegion* setDEngineType(DEngineType type);

protected:
    explicit DRegion();
    ~DRegion() override;

    NStatus init() final;
    NStatus run() final;
    NStatus destroy() final;

private:
    NStatus addElement(DElementPtr element) final;

    NVoid dump(std::ostream& oss) final;

    NBool isSerializable() const final;

    NStatus addManagers(DParamManagerPtr paramManager,
                        DEventManagerPtr eventManager) final;

    NBool isSeparate(DElementCPtr a, DElementCPtr b) const final;

private:
    DElementManagerPtr manager_ = nullptr;    // region 内部通过 manager来管理其中的 element 信息

    NAO_NO_ALLOWED_COPY(DRegion)

    friend class DPipeline;
    friend class UAllocator;
};

using DRegionPtr = DRegion *;

NAO_NAMESPACE_END

#endif //NAO_DREGION_H
