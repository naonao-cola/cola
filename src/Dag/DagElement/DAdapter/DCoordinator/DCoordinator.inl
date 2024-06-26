/**
 * @FilePath     : /cola/src/Dag/DagElement/DAdapter/DCoordinator/DCoordinator.cpp
 * @Description  :D
 * @Author       : naonao
 * @Date         : 2024-06-26 14:24:02
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-26 14:24:03
**/
#ifndef NAO_DCOORDINATOR_INL
#define NAO_DCOORDINATOR_INL

#include "DCoordinator.h"

NAO_NAMESPACE_BEGIN

template<NInt SIZE>
DCoordinator<SIZE>::DCoordinator() {
    element_type_ = DElementType::COORDINATOR;
    session_ = URandom<>::generateSession(NAO_STR_COORDINATOR);
}


template<NInt SIZE>
NStatus DCoordinator<SIZE>::run() {
    NAO_FUNCTION_BEGIN
    status = (SIZE >= 0)
             ? thread_pool_->createSecondaryThread(SIZE)
             : thread_pool_->releaseSecondaryThread((-1) * SIZE);

    NAO_FUNCTION_END
}


template<NInt SIZE>
NVoid DCoordinator<SIZE>::dumpElement(std::ostream& oss) {
    dumpElementHeader(oss);
    dumpPerfInfo(oss);
    oss << "\", shape=diamond]\n";
}


template<NInt SIZE>
NStatus DCoordinator<SIZE>::checkSuitable() {
    NAO_FUNCTION_BEGIN
    NAO_RETURN_ERROR_STATUS_BY_CONDITION(loop_ > 1,    \
    "cannot set GCoordinator loop > 1")

    NAO_FUNCTION_END
}

NAO_NAMESPACE_END

#endif //NAO_DCOORDINATOR_INL