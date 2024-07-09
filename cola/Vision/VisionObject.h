/**
 * @FilePath     : /cola/cola/Vision/VisionObject.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-08 14:45:34
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-08 14:50:11
 **/

#ifndef NAO_VISIONOBJECT_H
#define NAO_VISIONOBJECT_H

#include "VisionDefine.h"


NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

class VisionObject : public NObject
{
protected:
    NStatus run() override{NAO_NO_SUPPORT};
};
NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END

#endif