/**
 * @FilePath     : /cola/src/Dag/DagElement/DAdapter/DCoordinator/DCoordinator.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-26 14:23:41
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 09:46:48
 **/
#ifndef NAO_DCOORDINATOR_H
#define NAO_DCOORDINATOR_H

#include "../DAdapter.h"

NAO_NAMESPACE_BEGIN

template<NInt SIZE>
class DCoordinator : public DAdapter
{
protected:
    explicit DCoordinator();

    NStatus run() final;

    NVoid dumpElement(std::ostream& oss) final;

    NStatus checkSuitable() final;

    friend class GPipeline;
};

template<NInt SIZE>
using DCoordinatorPtr = DCoordinator<SIZE>*;

template<NInt SIZE>
using DCoordinatorPPtr = DCoordinatorPtr<SIZE>*;

NAO_NAMESPACE_END

#include "DCoordinator.inl"

#endif   // NAO_DCOORDINATOR_H
