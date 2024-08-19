/**
 * @FilePath     : /cola/cola/Vision/VisionDefine.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-28 15:43:42
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-20 12:13:37
 **/
#ifndef NAO_VISIONDEFINE_H
#define NAO_VISIONDEFINE_H

#include "../NBasic/NBasicInclude.h"
#include "../UtilsCtrl/UtilsInclude.h"
#include "opencv2/opencv.hpp"

NAO_NAMESPACE_BEGIN

#define NAO_VISION_NAMESPACE_BEGIN \
    namespace vision               \
    {
/* end of namespace vision */
#define NAO_VISION_NAMESPACE_END }

NAO_VISION_NAMESPACE_BEGIN

#define pdata(image, x, y, c) image.data[y * image.step + x * image.channels() + c]
#define SWAP(a, b, t) \
    do {              \
        t = a;        \
        a = b;        \
        b = t;        \
    } while (0)
#define CLIP_RANGE(value, min, max) ((value) > (max) ? (max) : (((value) < (min)) ? (min) : (value)))
#define COLOR_RANGE(value) CLIP_RANGE((value), 0, 255)
#define WITHIN(x1, delta, x2) ((delta) > 0) ? ((x1) <= (x2)) : ((x1) >= (x2))
#define EXCEED(x1, delta, x2) ((delta) > 0) ? ((x1) >= (x2)) : ((x1) <= (x2))

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END

#endif   // NAO_VISIONDEFINE_H