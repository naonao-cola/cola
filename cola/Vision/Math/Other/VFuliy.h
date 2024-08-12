/**
 * @FilePath     : /cola/cola/Vision/Math/Other/VFuliy.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-27 22:57:37
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-08-09 21:47:55
 * @Copyright (c) 2024 by G, All Rights Reserved.
 **/
#ifndef NAONAO_VFULIY_H
#define NAONAO_VFULIY_H

// 傅里叶变换
// https://blog.csdn.net/weixin_45875105/article/details/106917609
// https://blog.csdn.net/cyf15238622067/article/details/87917766

#include "../../VisionObject.h"

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

class VFuliy : public VisionObject
{
public:
    cv::Mat FuLiY(const cv::Mat src)
    {
        int     w = cv::getOptimalDFTSize(src.cols);
        int     h = cv::getOptimalDFTSize(src.rows);
        cv::Mat padded;
        cv::copyMakeBorder(src, padded, 0, h - src.rows, 0, w - src.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));
        cv::Mat plane[] = {cv::Mat_<float>(padded), cv::Mat::zeros(padded.size(), CV_32F)};
        cv::Mat complexIm;
        cv::merge(plane, 2, complexIm);   // 为延扩后的图像增添一个初始化为0的通道
        cv::dft(complexIm, complexIm);
        cv::split(complexIm, plane);
        cv::magnitude(plane[0], plane[1], plane[0]);
        cv::Mat magnitudeImage = plane[0];

        magnitudeImage += cv::Scalar::all(1);
        cv::log(magnitudeImage, magnitudeImage);   // 自然对数

        magnitudeImage = magnitudeImage(cv::Rect(0, 0, magnitudeImage.cols & -2, magnitudeImage.rows & -2));
        int cx         = magnitudeImage.cols / 2;
        int cy         = magnitudeImage.rows / 2;

        cv::Mat q0(magnitudeImage, cv::Rect(0, 0, cx, cy));
        cv::Mat q1(magnitudeImage, cv::Rect(cx, 0, cx, cy));
        cv::Mat q2(magnitudeImage, cv::Rect(0, cy, cx, cy));
        cv::Mat q3(magnitudeImage, cv::Rect(cx, cy, cx, cy));


        cv::Mat temp;
        q0.copyTo(temp);
        q3.copyTo(q0);
        temp.copyTo(q3);

        q1.copyTo(temp);
        q2.copyTo(q1);
        temp.copyTo(q2);

        cv::normalize(magnitudeImage, magnitudeImage, 0, 1, cv::NORM_MINMAX);
        return magnitudeImage;
        // dst = magnitudeImage.clone();
    }
};


NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END

#endif