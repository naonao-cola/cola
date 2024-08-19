/**
 * @FilePath     : /cola/cola/Vision/Enhance/VEnhance.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-26 23:33:56
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-08-19 15:36:15
 * @Copyright (c) 2024 by G, All Rights Reserved.
 **/
#ifndef NAONAO_VENHANCE_H
#define NAONAO_VENHANCE_H

#include "../VisionObject.h"

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

class VEnhance : public VisionObject
{
public:
    VEnhance();
    ~VEnhance() override;

    /**
     * @brief		图像局部增强ACE算法，单通道耗时大概在3毫秒左右，三通道由于计算是在循环内部，未改进，所以耗时
     * @param scr
     * @param dst
     * @param win_size
     * @param maxCg
     * @return
     * @code
     * example:
        Mat scr = imread("temp1.jpg", IMREAD_COLOR);  //以彩色的方式读入图像
        if (!scr.data) {              //判断图像是否被正确读取；
            return;
        }
        const NInt WINSIZE = 15;      //WINSIZE表示求均值与方差的窗口大小，应该是单数
        const NInt MAXCG = 10;        //设定最大的增强比例
        Mat dst = scr.clone();
        adaptContrastEnhancement(scr, dst, WINSIZE,MAXCG);
     */
    static cv::Mat adapt_contrast_enhancement(cv::Mat scr, NInt win_size = 15, NInt maxCg = 10);


    /**
        * @brief				基本视网膜修复。图像和过滤后的图像将转换为对数域并进行相减。
        * @param img			将在适当的位置增强img。
        * @param sigma			用于滤波的高斯核的标准偏差。
        * @param gain			将图像缩放回可见范围的因子。
        * @param offset		类似于增益的偏移量。
        * https://blog.csdn.net/yl_best/article/details/89048980
        * example:
        *  NInt scales = 3;
        NDouble weight[3] = { 1.0 / 3.0,1.0 / 3.0,1.0 / 3.0 };
        NDouble sigmas[3] = { 1,100,200 };
        nao::img_pre::Enhance en;
        en.retinex(src1, 100);
        en.multi_scale_retinex(src2, scales, weight, sigmas);
        en.multi_scale_retinex_CR(src3, scales, weight, sigmas);
    */
    static cv::Mat retinex(cv::Mat img, NDouble sigma, NInt gain = 128, NInt offset = 128);

    /**
     * @brief				多尺度retinex恢复。图像和一组过滤后的图像被转换为对数域，并从原始图像中减去一些权重。
     *						通常情况下，我们会用三个相等的加权尺度来衡量精细、中等和宽标准偏差。
     * @param img
     * @param scales
     * @param weights
     * @param sigmas
     * @param gain
     * @param offset
     */
    static cv::Mat multi_scale_retinex(cv::Mat img, NInt scales, NDouble* weights, NDouble* sigmas, NInt gain = 128, NInt offset = 128);

    /**
     * @brief				具有颜色恢复的多尺度retinex恢复。图像和一组过滤后的图像被转换为对数域，并从原始图像
     *						中减去一些权重。通常情况下，我们会用三个相等的加权尺度来衡量精细、中等和宽标准偏差。
     *						然后将颜色恢复权重应用于每个颜色通道。
     * @param img
     * @param scales
     * @param weights
     * @param sigmas
     * @param gain
     * @param offset
     * @param restoration_factor
     * @param color_gain
     */
    static cv::Mat multi_scale_retinex_CR(cv::Mat img, NInt scales, NDouble* weights, NDouble* sigmas, NInt gain = 128, NInt offset = 128, NDouble restoration_factor = 6, NDouble color_gain = 2);

    /**
     * @brief				自适应高低阈值，根据中位数求得，可用于candy边缘检测
     * @param img
     * @param min_val
     * @param max_val
     * @param sigma
     */
    static void adapt_min_max_threshold_median(cv::Mat img, NInt& min_val, NInt& max_val, NFloat sigma = 0.3);

    /**
     * @brief				自适应找高低阈值matlab版本，主要是在边缘强度的直方图上，进行截断。
     * @param image			单通道图像
     * @param low
     * @param high
     * @param percent_of_pix_is_not_edges 边缘图在图中所拥有的比例
     * @param aperture_size
     * https://www.zybuluo.com/johntian/note/593072
     */
    static void adapt_find_threshold_matlab(const cv::Mat image, NDouble& low, NDouble& high, NDouble percent_of_pix_is_not_edges = 0.7, NInt aperture_size = 3);

    /**
     * @brief			非极大值抑制函数，可以进一步消除非边缘的噪点，更重要的是，可以细化边缘。
     *					<!--使用之前注意测试一下速度,注意核的大小-->
     * @param src
     * @param dst
     * @param win_size
     */
    static void non_maximum_suppression(cv::Mat& src, cv::Mat& dst, NInt win_size = 3);

    // 分段增强
    static cv::Mat segmented_enhancement(const cv::Mat& img, NDouble r1, NDouble r2, NDouble s1 = 0, NDouble s2 = 255);

    /**
     * spline function
     *参考链接： https://blog.csdn.net/qq_37887537/article/details/78498209
     * @param x [in]  array of x-coordinate of control points
     * @param y [in]  array of y-coordinate of control points
     * @param n [in]  count of control points
     * @param t [in]  array of x-coordinate of output points
     * @param m [in]  count of output points
     * @param z [out] array of y-coordinate of output points
     */
    static NDouble spline(NDouble* x, NDouble* y, NInt n, NDouble* t, NInt m, NDouble* z);

    //= = = = = = = = = = = = = = = = = = = =
    // @FuncName:
    // @Brief:     画一条虚线。
    // @InputParameter:
    // @OutputParameter:
    // @Returns:
    // @Others:
    //= = = = = = = = = = = = = = = = = = = =
    static void dot_line(cv::Mat& mat, const cv::Point& p1, const cv::Point& p2, const cv::Scalar& color, NInt thickness = 1, NInt lineType = 8, NInt line_step = 6, NInt blank_step = 6);

    /**
     * @brief gamma变换
     * @param img
     * @param gamma
     * @param n_c
     * @return
     */
    static cv::Mat gamma_trans(const cv::Mat& img, NDouble gamma, NInt n_c = 1);

    /**
     * @brief
     * @param img
     * @param sin 黑场阈值
     * @param hin 白场阈值
     * @param mt 灰度调节
     * @param sout 输出黑场阈值
     * @param hout 输出白场阈值
     * @return
     * @see https://blog.csdn.net/youcans/article/details/125373625
     */
    static cv::Mat gray_stairs(const cv::Mat& img, NDouble sin = 0.0, NDouble hin = 255.0, NDouble mt = 1.0, NDouble sout = 0.0, NDouble hout = 255.0);

    // 获取凸包
    static cv::Mat get_rect_img(const cv::Mat& img, std::vector<cv::Point2i>& vecPt);


    /**
     * @brief 直方图匹配，正矩形的框进行匹配
     * @param hist_img
     * @param hist_rect
     * @param template_img
     * @param template_rect
     * @return
     */
    cv::Mat get_equal_img(const cv::Mat& hist_img, const cv::Rect& hist_rect, const cv::Mat& template_img, const cv::Rect& template_rect);

    /**
     * @brief 直方图匹配，旋转矩形的四个点进行匹配
     * @param hist_img
     * @param hist_pts
     * @param template_img
     * @param template_pts
     * @return
     */
    cv::Mat get_equal_img_2(const cv::Mat& hist_img, const std::vector<cv::Point2f>& hist_pts, const cv::Mat& template_img, std::vector<cv::Point2f>& template_pts);
};

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END

#endif