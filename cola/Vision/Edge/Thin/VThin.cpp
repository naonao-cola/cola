/**
 * @FilePath     : /cola/cola/Vision/Edge/Thin/VThin.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-23 18:21:39
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-23 18:26:54
 * @Copyright (c) 2024 by G, All Rights Reserved.
 **/
#include "VThin.h"

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

cv::Mat VThin::thin_line(cv::Mat& src, int intera /*= 2*/)
{
    cv::Mat dst;
    if (src.type() != CV_8UC1) {
        std::cerr << "输入图像类型不是单通道" << std::endl;
        return dst;
    }

    // 非原地操作
    if (dst.data != src.data) {
        src.copyTo(dst);
    }
    int i;
    int j;
    int n;
    int width;
    int height;
    // 减一是为了方便处理8邻域，防止越界
    width        = src.cols - 1;
    height       = src.rows - 1;
    int     step = (int)src.step;
    int     p2;
    int     p3;
    int     p4;
    int     p5;
    int     p6;
    int     p7;
    int     p8;
    int     p9;
    uchar*  img;
    bool    ifEnd;
    int     Al;
    cv::Mat tmp_img;
    // n表示迭代次数
    for (n = 0; n < intera; n++) {
        dst.copyTo(tmp_img);
        ifEnd = false;
        img   = tmp_img.data;
        for (i = 1; i < height; i++) {
            img += step;
            for (j = 1; j < width; j++) {
                uchar* p = img + j;
                Al       = 0;
                if (p[0]) {
                    // p2,p3 01模式
                    if (p[-step] == 0 && p[-step + 1] > 0) {
                        Al++;
                    }
                    // p3,p4 01模式
                    if (p[-step + 1] == 0 && p[1] > 0) {
                        Al++;
                    }
                    // p4,p5 01模式
                    if (p[1] == 0 && p[step + 1] > 0) {
                        Al++;
                    }
                    // p5,p6 01模式
                    if (p[step + 1] == 0 && p[step] > 0) {
                        Al++;
                    }
                    // p6,p7 01模式
                    if (p[step] == 0 && p[step - 1] > 0) {
                        Al++;
                    }
                    // p7,p8 01模式
                    if (p[step - 1] == 0 && p[-1] > 0) {
                        Al++;
                    }
                    // p8,p9 01模式
                    if (p[-1] == 0 && p[-step - 1] > 0) {
                        Al++;
                    }
                    // p9,p2 01模式
                    if (p[-step - 1] == 0 && p[-step] > 0) {
                        Al++;
                    }
                    p2 = p[-step] > 0 ? 1 : 0;
                    p3 = p[-step + 1] > 0 ? 1 : 0;
                    p4 = p[1] > 0 ? 1 : 0;
                    p5 = p[step + 1] > 0 ? 1 : 0;
                    p6 = p[step] > 0 ? 1 : 0;
                    p7 = p[step - 1] > 0 ? 1 : 0;
                    p8 = p[-1] > 0 ? 1 : 0;
                    p9 = p[-step - 1] > 0 ? 1 : 0;

                    if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) > 1 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) < 7 && Al == 1) {
                        // p2*p4*p6=0 && p4*p6*p8==0
                        if ((p2 == 0 || p4 == 0 || p6 == 0) && (p4 == 0 || p6 == 0 || p8 == 0)) {
                            // 满足删除条件，设置当前像素为0
                            dst.at<uchar>(i, j) = 0;
                            ifEnd               = true;
                        }
                    }
                }
            }
        }
        dst.copyTo(tmp_img);
        img = tmp_img.data;
        for (i = 1; i < height; i++) {
            img += step;
            for (j = 1; j < width; j++) {
                Al       = 0;
                uchar* p = img + j;
                if (p[0] > 0) {
                    // p2,p3 01模式
                    if (p[-step] == 0 && p[-step + 1] > 0) {
                        Al++;
                    }
                    // p3,p4 01模式
                    if (p[-step + 1] == 0 && p[1] > 0) {
                        Al++;
                    }
                    // p4,p5 01模式
                    if (p[1] == 0 && p[step + 1] > 0) {
                        Al++;
                    }
                    // p5,p6 01模式
                    if (p[step + 1] == 0 && p[step] > 0) {
                        Al++;
                    }
                    // p6,p7 01模式
                    if (p[step] == 0 && p[step - 1] > 0) {
                        Al++;
                    }
                    // p7,p8 01模式
                    if (p[step - 1] == 0 && p[-1] > 0) {
                        Al++;
                    }
                    // p8,p9 01模式
                    if (p[-1] == 0 && p[-step - 1] > 0) {
                        Al++;
                    }
                    // p9,p2 01模式
                    if (p[-step - 1] == 0 && p[-step] > 0) {
                        Al++;
                    }
                    p2 = p[-step] > 0 ? 1 : 0;
                    p3 = p[-step + 1] > 0 ? 1 : 0;
                    p4 = p[1] > 0 ? 1 : 0;
                    p5 = p[step + 1] > 0 ? 1 : 0;
                    p6 = p[step] > 0 ? 1 : 0;
                    p7 = p[step - 1] > 0 ? 1 : 0;
                    p8 = p[-1] > 0 ? 1 : 0;
                    p9 = p[-step - 1] > 0 ? 1 : 0;
                    if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) > 1 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) < 7 && Al == 1) {
                        // p2*p4*p8=0 && p2*p6*p8==0
                        if ((p2 == 0 || p4 == 0 || p8 == 0) && (p2 == 0 || p6 == 0 || p8 == 0)) {
                            // 满足删除条件，设置当前像素为0
                            dst.at<uchar>(i, j) = 0;
                            ifEnd               = true;
                        }
                    }
                }
            }
        }
        // 如果两个子迭代已经没有可以细化的像素了，则退出迭代
        if (!ifEnd) {
            break;
        }
    }
    return dst;
}

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END
