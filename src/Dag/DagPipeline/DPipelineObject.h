/**
 * @FilePath     : /cola/src/Dag/DagPipeline/DPipelineObject.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-28 10:38:34
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 12:07:56
**/
#ifndef NAO_DPIPELINEOBJECT_H
#define NAO_DPIPELINEOBJECT_H

#include "../DagObject.h"

NAO_NAMESPACE_BEGIN

class DPipelineObject : public DagObject {
protected:
    NBool is_init_ = false;                          // 判断是否init
};

NAO_NAMESPACE_END

#endif //NAO_DPIPELINEOBJECT_H