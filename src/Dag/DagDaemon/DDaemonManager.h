/**
 * @FilePath     : /cola/src/Dag/DagDaemon/DDaemonManager.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 18:07:38
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 18:07:38
**/
#ifndef NAO_DDAEMONMANAGER_H
#define NAO_DDAEMONMANAGER_H

#include "DDaemon.h"
#include "../DagManager.h"

NAO_NAMESPACE_BEGIN

class DDaemonManager : public DDaemonObject,
                       public DagManager<DDaemon> {
protected:
    explicit DDaemonManager() = default;

    ~DDaemonManager() override;

    NStatus init() final;

    NStatus destroy() final;

    NStatus add(DDaemonPtr daemon) final;

    NStatus remove(DDaemonPtr daemon) final;

    NStatus clear() final;

    NSize getSize() const final;

    friend class GPipeline;
    friend class UAllocator;
    NAO_NO_ALLOWED_COPY(DDaemonManager)

private:
    DDaemonSet daemons_ {};                    // daemon信息集合
};

using DDaemonManagerPtr = DDaemonManager *;

NAO_NAMESPACE_END

#endif //NAO_DDAEMONMANAGER_H
