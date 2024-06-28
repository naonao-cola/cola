/**
 * @FilePath     : /cola/src/Dag/DagAspect/DAspect.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 19:48:10
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 19:48:10
 **/
#include "DAspect.h"

NAO_NAMESPACE_BEGIN

NStatus DAspect::beginInit(){NAO_EMPTY_FUNCTION}


NVoid DAspect::finishInit(const NStatus& curStatus)
{}


NStatus DAspect::beginRun(){NAO_EMPTY_FUNCTION}


NVoid DAspect::finishRun(const NStatus& curStatus)
{}


NStatus DAspect::beginDestroy(){NAO_EMPTY_FUNCTION}


NVoid DAspect::finishDestroy(const NStatus& curStatus)
{}


NVoid DAspect::enterCrashed() {}

NAO_NAMESPACE_END