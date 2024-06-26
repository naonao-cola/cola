/**
 * @FilePath     : /cola/src/Dag/DagElement/DGroup/DMutable/DMutable.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-26 15:26:03
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-26 16:14:13
 **/
#ifndef NAO_DMUTABLE_H
#define NAO_DMUTABLE_H

#include "../../DElementManager.h"
#include "../DGroup.h"


NAO_NAMESPACE_BEGIN

class DMutable : public DGroup
{
protected:
    /**
     * 重新设定内部数据的结构信息
     * @param elements
     * @return
     */
    virtual NStatus reshape(DElementPtrArr& elements) = 0;

    explicit DMutable();

    ~DMutable() override;

private:
    NStatus init() final;

    NStatus run() final;

    NStatus destroy() final;

    NBool isSerializable() const override;

    /**
     * 将数据进行恢复
     * @return
     */
    NVoid setup();

private:
    DElementManagerPtr manager_ = nullptr;

    friend class DPipeline;
    friend class UAllocator;
};

using DMutablePtr = DMutable*;

NAO_NAMESPACE_END

#endif   // NAO_DMUTABLE_H
