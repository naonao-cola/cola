/**
 * @FilePath     : /cola/cola/Vision/Filter/VFliter.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-23 17:57:29
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-23 17:57:29
 * @Copyright (c) 2024 by G, All Rights Reserved.
 **/
#ifndef NAONAO_VFILTER_H
#define NAONAO_VFILTER_H

#include "../VisionObject.h"

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

class VFilter : public VisionObject
{
public:
    /**
     * @brief		理想低通滤波器
     * @param src
     * @param sigma
     * @return
     * 半径D0越小，模糊越大；半径D0越大，模糊越小
     */
    static cv::Mat ideal_low_pass_filter(cv::Mat& src, float sigma);


    /**
     * @brief		理想高通滤波
     * @param src
     * @param sigma
     * @return
     */
    static cv::Mat ideal_high_pass_filter(cv::Mat& src, float sigma);

    /**
     * @brief		巴特沃斯低通滤波器
     * @param src
     * @param d0
     * @param n
     * @return
     *  H = 1 / (1+(D/D0)^2n)    n表示巴特沃斯滤波器的次数
     * 阶数n=1 无振铃和负值    阶数n=2 轻微振铃和负值  阶数n=5 明显振铃和负值   阶数n=20 与ILPF相似
     * 半径D0越小，模糊越大；半径D0越大，模糊越小
     */
    static cv::Mat butterworth_low_paass_filter(cv::Mat& src, float d0, int n);

    /**
     * @brief		巴特沃斯高通滤波器
     * @param src
     * @param d0
     * @param n
     * @return
     */
    static cv::Mat butterworth_high_paass_filter(cv::Mat& src, float d0, int n);

    /**
     * @brief			 高斯低通滤波器
     * @param src
     * @param d0
     * @return
     *  高斯函数参数，越小，频率高斯滤波器越窄，滤除高频成分越多，图像就越平滑
     */
    static cv::Mat gaussian_low_pass_filter(cv::Mat& src, float d0);


    /**
     * @brief			高斯高通滤波器
     * @param src
     * @param d0
     * @return
     */
    static cv::Mat gaussian_high_pass_filter(cv::Mat& src, float d0);

    /**
     * @brief			 级联滤波器，用于边缘检测（中值滤波与双边滤波结合）
     * @param src
     * @param K
     * @param ksize
     * @return
     * 可以用峰值信噪比比较滤波器的去燥效果，公式 PSNR = 10* log(255 * 255 / MES)
     */
    static cv::Mat cascade_filter(const cv::Mat& src, int K, int ksize = 3);

    /**
     * @brief			计算信噪比，用于图像质量评价
     * @param Mat1
     * @param Mat2
     * @return
     * 参考链接 https://www.jianshu.com/p/08b1b5014ce7
     */
    static double compute_PSNR(const cv::Mat& Mat1, const cv::Mat& Mat2);

    /**
     * @brief			SSIM是一个广泛使用的图像质量评价指标，
     * @param mat1
     * @param mat2
     * @return
     * 参考链接    https://zhuanlan.zhihu.com/p/150865007
     * 原理参考    https://blog.csdn.net/qq_35860352/article/details/84026507
     * 代码参考    https://blog.csdn.net/chaipp0607/article/details/70160307
     */
    static cv::Scalar compute_MSSIM(const cv::Mat& mat1, const cv::Mat& mat2);

    /**
     * @brief			拉普拉斯检测图像是否模糊
     * @param src
     * @return
     * 参考链接  https://zhuanlan.zhihu.com/p/248275907
     */
    static double variance_laplacian(const cv::Mat& src);
};

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END

#endif   // NAONAO_VFILTER_H