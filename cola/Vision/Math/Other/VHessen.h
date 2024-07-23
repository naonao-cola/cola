/**
 * @FilePath     : /cola/cola/Vision/Math/Other/VHessen.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-23 18:29:49
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-23 18:29:49
 * @Copyright (c) 2024 by G, All Rights Reserved.
 **/
#ifndef NAONAO_VHSSEN_H
#define NAONAO_VHSSEN_H

/**
 * @brief https://blog.csdn.net/dangkie/article/details/78996761
 */

#include "../../VisionObject.h"

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

class VHessen : public VisionObject
{
public:
    static std::vector<cv::Point2f> StegerLine(const cv::Mat& src)
    {
        std::vector<cv::Point2f> dst_pt;
        // cv::Mat src = cv::imread("1650.bmp", 0);
        cv::Mat img = src.clone();
        // 高斯滤波
        img.convertTo(img, CV_32FC1);
        cv::GaussianBlur(img, img, cv::Size(0, 0), 2.5, 2.5);
        cv::Mat dx;
        cv::Mat dy;
        cv::Mat dxx;
        cv::Mat dyy;
        cv::Mat dxy;
        // 一阶偏导数
        cv::Mat mDx, mDy;
        // 二阶偏导数
        cv::Mat mDxx, mDyy, mDxy;

        mDx  = (cv::Mat_<float>(1, 2) << 1, -1);          // x偏导
        mDy  = (cv::Mat_<float>(2, 1) << 1, -1);          // y偏导
        mDxx = (cv::Mat_<float>(1, 3) << 1, -2, 1);       // 二阶x偏导
        mDyy = (cv::Mat_<float>(3, 1) << 1, -2, 1);       // 二阶y偏导
        mDxy = (cv::Mat_<float>(2, 2) << 1, -1, -1, 1);   // 二阶xy偏导

        cv::filter2D(img, dx, CV_32FC1, mDx);
        cv::filter2D(img, dy, CV_32FC1, mDy);
        cv::filter2D(img, dxx, CV_32FC1, mDxx);
        cv::filter2D(img, dyy, CV_32FC1, mDyy);
        cv::filter2D(img, dxy, CV_32FC1, mDxy);

        // hessian矩阵
        int                      cols = src.cols;
        int                      rows = src.rows;
        std::vector<cv::Point2f> pts;

        for (int col = 0; col < cols; ++col) {
            for (int row = rows - 1; row != -1; --row) {
                if (src.at<uchar>(row, col) < 210) {
                    continue;
                }

                cv::Mat hessian(2, 2, CV_32FC1);
                hessian.at<float>(0, 0) = dxx.at<float>(row, col);
                hessian.at<float>(0, 1) = dxy.at<float>(row, col);
                hessian.at<float>(1, 0) = dxy.at<float>(row, col);
                hessian.at<float>(1, 1) = dyy.at<float>(row, col);
                cv::Mat eValue;
                cv::Mat eVectors;
                cv::eigen(hessian, eValue, eVectors);
                double nx, ny;
                double fmaxD = 0;

                if (std::fabs(eValue.at<float>(0, 0)) >= std::fabs(eValue.at<float>(1, 0)))   // 求特征值最大时对应的特征向量
                {
                    nx    = eVectors.at<float>(0, 0);
                    ny    = eVectors.at<float>(0, 1);
                    fmaxD = eValue.at<float>(0, 0);
                }
                else {
                    nx    = eVectors.at<float>(1, 0);
                    ny    = eVectors.at<float>(1, 1);
                    fmaxD = eValue.at<float>(1, 0);
                }

                float t =
                    -(nx * dx.at<float>(row, col) + ny * dy.at<float>(row, col)) / (nx * nx * dxx.at<float>(row, col) + 2 * nx * ny * dxy.at<float>(row, col) + ny * ny * dyy.at<float>(row, col));
                float tnx = t * nx;
                float tny = t * ny;

                if (std::fabs(tnx) <= 0.25 && std::fabs(tny) <= 0.25) {
                    float x = col + /*.5*/ tnx;
                    float y = row + /*.5*/ tny;
                    pts.push_back({x, y});
                    break;
                }
            }
        }
        // cv::Mat display;
        // cv::cvtColor(src, display, cv::COLOR_GRAY2BGR);
        // for (int k = 0; k < pts.size(); k++) {
        //     cv::Point rpt;
        //     rpt.x = pts[k].x;
        //     rpt.y = pts[k].y;
        //     cv::circle(display, rpt, 1, cv::Scalar(0, 0, 255));
        //     dst_pt.emplace_back(rpt);
        // }
        // cv::imshow("result", display);
        // cv::waitKey(0);
        return dst_pt;
    }
};

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END

#endif //NAONAO_VHSSEN_H