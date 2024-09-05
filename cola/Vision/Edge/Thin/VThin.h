/**
 * @FilePath     : /cola/cola/Vision/Edge/Thin/VThin.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-23 18:18:56
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-23 18:28:27
 * @Copyright (c) 2024 by G, All Rights Reserved.
 **/

#ifndef NAONAO_VTHIN_H
#define NAONAO_VTHIN_H

#include "../../VisionObject.h"

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

class VThin : public VisionObject
{
public:
    /**
     * @brief			细化图像或者曲线，只处理二值图或者灰度图,
     * @param src
     * @param dst
     * @param intera	迭代次数
     * 参考链接 https://www.cnblogs.com/mikewolf2002/p/3321732.html
     */
    static cv::Mat thin_line(cv::Mat& src, NInt intera = 2);
};

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END

#endif   // NAONAO_VTHIN_H