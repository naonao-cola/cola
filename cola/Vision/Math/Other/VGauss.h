/**
 * @FilePath     : /cola/cola/Vision/Math/Other/VGauss.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-23 18:36:16
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-23 18:36:17
 * @Copyright (c) 2024 by G, All Rights Reserved.
 **/
#ifndef NAONAO_VGAUSS_H
#define NAONAO_VGAUSS_H

#include "../../VisionObject.h"
#include "opencv2/imgproc/imgproc.hpp"
#include <Eigen/Dense>

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

class VGauss : public VisionObject
{
public:
    /**
     * @brief 高斯拟合求光斑,求光斑的中心，测试函数
     * @param src
     */
    static std::vector<cv::Point2f> gauss_spot_center(const cv::Mat& src)
    {
        std::vector<cv::Point2f> dst_pt;
        cv::Mat                  img = src.clone();
        cv::Mat                  dis_img;
        cv::cvtColor(img, dis_img, cv::COLOR_GRAY2BGR);

        cv::Mat thresh_img;

        // 二值化
        double dCurdwTh = 200;
        cv::threshold(img, thresh_img, dCurdwTh, 255, cv::THRESH_BINARY);
        // cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));;//X腐蚀
        // cv::Mat elementP = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));//膨胀
        // cv::erode(thresh_img, thresh_img, element);//腐蚀操作
        // cv::dilate(thresh_img, thresh_img, elementP);//膨胀操作

        std::vector<std::vector<cv::Point>> contours;
        std::vector<cv::Vec4i>              hierarchy;

        cv::findContours(thresh_img, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_NONE);
        std::vector<cv::Point2f> pt;
        for (int i = 0; i < contours.size(); i++) {
            cv::Rect rect = cv::boundingRect(contours.at(i));
            if (rect.width < 4 || rect.width > 65 || rect.height < 4)
                continue;

            cv::Mat tmp = img(cv::Rect(rect.x - 5, rect.y - 5, rect.width + 10, rect.height + 10)).clone();
            tmp.convertTo(tmp, CV_32F);
            Eigen::VectorXf vector_A;
            Eigen::MatrixXf matrix_B;

            initData(tmp, vector_A, matrix_B);
            float x0 = 0;
            float y0 = 0;
            getCentrePoint(x0, y0, vector_A, matrix_B);
            cv::Point2f p;

            if (x0 != 0 && y0 != 0) {
                p.x = x0 + rect.tl().x - 5;
                p.y = y0 + rect.tl().y - 5;
                pt.emplace_back(p);
                cv::circle(dis_img, p, 1, cv::Scalar(0, 0, 255), -1);
            }
        }
        for (int i = 0; i < pt.size(); i++) {
            dst_pt.emplace_back(pt[i]);
        }
        // cv::waitKey(0);
        return dst_pt;
    }

private:
    /**
     * @brief 初始化数据矩阵
     * @param img      单通道float型数据
     * @param vector_A 向量A
     * @param matrix_B 矩阵B
     * @return
     * @参考链接 https://blog.csdn.net/houjixin/article/details/8490653
     */
    static bool initData(cv::Mat img, Eigen::VectorXf& vector_A, Eigen::MatrixXf& matrix_B)
    {
        if (img.empty()) {
            return false;
        }
        int             length = img.cols * img.rows;
        Eigen::VectorXf tmp_A(length);
        Eigen::MatrixXf tmp_B(length, 5);
        int             i    = 0;
        int             j    = 0;
        int             iPos = 0;

        while (i < img.cols) {
            j = 0;
            while (j < img.rows) {
                float value = *img.ptr<float>(i, j);
                if (value <= 0.5) {
                    value = 1;
                }
                tmp_A(iPos)    = value * log(value);
                tmp_B(iPos, 0) = value;
                tmp_B(iPos, 1) = value * i;
                tmp_B(iPos, 2) = value * j;
                tmp_B(iPos, 3) = value * i * i;
                tmp_B(iPos, 4) = value * j * j;
                ++iPos;
                ++j;
            }
            ++i;
        }
        vector_A = tmp_A;
        matrix_B = tmp_B;
    }

    /**
     * @brief 获取光斑中心
     * @param x0
     * @param y0
     * @param vector_A
     * @param matrix_B
     * @return
     */
    static bool getCentrePoint(float& x0, float& y0, const Eigen::VectorXf vector_A, const Eigen::MatrixXf matrix_B)
    {
        // QR分解
        Eigen::HouseholderQR<Eigen::MatrixXf> qr;
        qr.compute(matrix_B);
        Eigen::MatrixXf R = qr.matrixQR().triangularView<Eigen::Upper>();
        Eigen::MatrixXf Q = qr.householderQ();

        // 块操作，获取向量或矩阵的局部
        Eigen::VectorXf S;
        S = (Q.transpose() * vector_A).head(5);
        Eigen::MatrixXf R1;
        R1 = R.block(0, 0, 5, 5);

        Eigen::VectorXf C;
        C = R1.inverse() * S;

        x0 = -0.5 * C[1] / C[3];
        y0 = -0.5 * C[2] / C[4];
        return true;
    }
};

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END

#endif   // NAONAO_VGAUSS_H