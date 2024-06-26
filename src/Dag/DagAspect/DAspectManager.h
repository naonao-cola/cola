/**
 * @FilePath     : /cola/src/Dag/DagAspect/DAspectManager.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 20:02:33
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 20:02:33
 **/
#ifndef NAO_DASPECTMANAGER_H
#define NAO_DASPECTMANAGER_H

#include "../DagManager.h"
#include "DAspect.h"
#include "DAspectObject.h"


NAO_NAMESPACE_BEGIN

class DAspectManager : public DAspectObject, public DagManager<DAspect>
{
protected:
    explicit DAspectManager();

    ~DAspectManager() override;

    /**
     * 执行切面逻辑
     * @param type
     * @param curStatus
     * @return
     */
    NStatus reflect(const DAspectType& type, const NStatus& curStatus = NStatus());

    NStatus add(DAspectPtr aspect) final;

    NSize getSize() const final;

    NStatus clear() final;

    /**
     * 弹出去最后一个
     * @return
     */
    NStatus popLast();

    NVoidPtr setDParamManager(DParamManagerPtr pm) final;

    NVoidPtr setDEventManager(DEventManagerPtr em) final;

    NAO_NO_ALLOWED_COPY(DAspectManager)

private:
    DAspectPtrArr aspect_arr_;   // 存储aspect的容器

    friend class DElement;
    friend class UAllocator;
};

using DAspectManagerPtr = DAspectManager*;

NAO_NAMESPACE_END

#endif   // NAO_DASPECTMANAGER_H
