/**
 * @FilePath     : /cola/cola/Vision/Enhance/VEnhance.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-26 23:34:15
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-27 22:55:31
 * @Copyright (c) 2024 by G, All Rights Reserved.
 **/

#include <algorithm>
#include <iostream>

#ifdef HAVE_OPENMP
#    include <omp.h>
#endif
#include "VEnhance.h"

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

/**
 * @brief			单通道的ace简化版算法
 * @param src
 * @param dst
 * @param win_size
 * @param maxCg
 * 参考链接 https://blog.csdn.net/EbowTang/article/details/42373081
 */
void __simple_ace(cv::Mat& src, cv::Mat& dst, int win_size = 3, int maxCg = 7.5)
{
    if (CV_8UC1 != src.type()) {
        return;
    }
    int rows = src.rows;
    int cols = src.cols;
    // 图像局部均值
    cv::Mat mean_local;
    // 图像局部方差
    cv::Mat var_local;
    // 全局均值
    cv::Mat mean_global;
    // 全局标准差
    cv::Mat var_global;
    // 获取局部均值
    cv::blur(src.clone(), mean_local, cv::Size(win_size, win_size));
    // 获取局部高频
    cv::Mat high_freq = src - mean_local;
    // 获取方差
    var_local = high_freq.mul(high_freq);
    // 计算局部方差
    cv::blur(var_local, var_local, cv::Size(win_size, win_size));
    var_local.convertTo(var_local, CV_32F);
    for (int i = 0; i < rows; i++) {
        // 获取局部标准差
        for (int j = 0; j < cols; j++) {
            var_local.at<float>(i, j) = (float)sqrt(var_local.at<float>(i, j));
        }
    }
    // 获取全局均值与标准差
    cv::meanStdDev(src, mean_global, var_global);
    // 增益系数矩阵
    cv::Mat gain_Arr = 0.5 * mean_global / var_local;
    for (int i = 0; i < rows; i++) {
        // 增益系数截断
        for (int j = 0; j < cols; j++) {
            if (gain_Arr.at<float>(i, j) > maxCg) {
                gain_Arr.at<float>(i, j) = maxCg;
            }
        }
    }
    gain_Arr.convertTo(gain_Arr, CV_8U);
    gain_Arr = gain_Arr.mul(high_freq);
    dst      = mean_local + gain_Arr;
}

/**
 * @brief				获取图像的局部均值与局部标准差的图
 * @param src			输入图像，为单通道；
 * @param means_dst		计算得到的均值的图，均值的值与输入图像中的点一一对应，为单通道；
 * @param variance_dst	计算得到的标准差的图，标准差的值与输入图像中的点一一对应，为单通道；
 * @param win_size
 * @return
 */
bool __get_variance_mean(cv::Mat& src, cv::Mat& means_dst, cv::Mat& variance_dst, int win_size)
{
    if (!src.data) {
        std::cerr << "获取方差与均值的函数读入图片有误" << std::endl;
        return false;
    }
    if (win_size % 2 == 0) {
        std::cerr << "计算局部均值与标准差的窗口大小应该为单数" << std::endl;
        return false;
    }
    cv::Mat copyBorder_yChannels;
    int     copyBorderSize = (win_size - 1) / 2;
    /*扩充图像边界*/
    cv::copyMakeBorder(src, copyBorder_yChannels, copyBorderSize, copyBorderSize, copyBorderSize, copyBorderSize, cv::BORDER_REFLECT);
    for (int i = (win_size - 1) / 2; i < copyBorder_yChannels.rows - (win_size - 1) / 2; i++) {
        for (int j = (win_size - 1) / 2; j < copyBorder_yChannels.cols - (win_size - 1) / 2; j++) {
            /*截取扩展后的图像中的一个方块*/
            cv::Mat    temp = copyBorder_yChannels(cv::Rect(j - (win_size - 1) / 2, i - (win_size - 1) / 2, win_size, win_size));
            cv::Scalar mean;
            cv::Scalar dev;
            cv::meanStdDev(temp, mean, dev);
            // 一一对应赋值
            variance_dst.at<float>(i - (win_size - 1) / 2, j - (win_size - 1) / 2) = dev.val[0];
            means_dst.at<float>(i - (win_size - 1) / 2, j - (win_size - 1) / 2)    = mean.val[0];
        }
    }
    return true;
}


cv::Mat VEnhance::adapt_contrast_enhancement(cv::Mat scr, int win_size /*= 15*/, int maxCg /*= 10*/)
{
    cv::Mat dst;
    if (!scr.data) {
        std::cerr << "自适应对比度增强函数读入图片有误";
        return dst;
    }
    if (CV_8UC1 == scr.type()) {
        __simple_ace(scr, dst, win_size, maxCg);
        return dst;
    }
    cv::Mat ycc;
    // 转换空间到YCrCb；
    cv::cvtColor(scr, ycc, cv::COLOR_RGB2YCrCb);
    std::vector<cv::Mat> channels(3);
    // 分离通道；
    cv::split(ycc, channels);
    cv::Mat localMeansMatrix(scr.rows, scr.cols, CV_32FC1);
    cv::Mat localVarianceMatrix(scr.rows, scr.cols, CV_32FC1);
    // 对Y通道进行增强；
    if (!__get_variance_mean(channels[0], localMeansMatrix, localVarianceMatrix, win_size)) {
        std::cerr << "计算图像均值与标准差过程中发生错误";
        return dst;
    }

    cv::Mat    temp = channels[0].clone();
    cv::Scalar mean;
    cv::Scalar dev;
    cv::meanStdDev(temp, mean, dev);

    float   meansGlobal = mean.val[0];
    cv::Mat enhanceMatrix(scr.rows, scr.cols, CV_8UC1);
    // 遍历，对每个点进行自适应调节
    for (int i = 0; i < scr.rows; i++) {
        for (int j = 0; j < scr.cols; j++) {
            if (localVarianceMatrix.at<float>(i, j) >= 0.01) {
                float cg  = 0.2 * meansGlobal / localVarianceMatrix.at<float>(i, j);
                float cgs = cg > maxCg ? maxCg : cg;
                cgs       = cgs < 1 ? 1 : cgs;
                int e     = localMeansMatrix.at<float>(i, j) + cgs * (temp.at<uchar>(i, j) - localMeansMatrix.at<float>(i, j));
                if (e > 255) {
                    e = 255;
                }
                else if (e < 0) {
                    e = 0;
                }
                enhanceMatrix.at<uchar>(i, j) = e;
            }
            else {
                enhanceMatrix.at<uchar>(i, j) = temp.at<uchar>(i, j);
            }
        }
    }
    // 合并通道，转换颜色空间回到RGB
    channels[0] = enhanceMatrix;
    cv::merge(channels, ycc);
    cv::cvtColor(ycc, dst, cv::COLOR_YCrCb2RGB);
    return dst;
}

/**
 * @brief			MSRCR算法部分,一维高斯核函数
 * @param sigma
 * @return			n array of values of length ((6*sigma)/2) * 2 + 1.
 * https://blog.csdn.net/lilingyu520/article/details/46755767
 *  https://blog.csdn.net/smallstones/article/details/41787079
 *  https://blog.csdn.net/onezeros/article/details/6342661
 */
double* __create_kernel(double sigma)
{
    int     i, x, filter_size;
    double* filter;
    double  sum;
    if (sigma > 200)
        sigma = 200;
    filter_size = (int)floor(sigma * 6) / 2;
    filter_size = filter_size * 2 + 1;
    filter      = new double[filter_size];
    sum         = 0;
    for (i = 0; i < filter_size; i++) {
        // x的取值范围从中间到两边
        x = i - (filter_size / 2);
        // 一维高斯核
        filter[i] = exp(-(x * x) / (2 * sigma * sigma));
        sum += filter[i];
    }
    // 归一化
    for (i = 0; i < filter_size; i++)
        filter[i] /= sum;
    return filter;
}

#define INT_PREC 1024.0
#define INT_PREC_BITS 10
inline double int2double(int x)
{
    return static_cast<double>(x) / INT_PREC;
}
inline int double2int(double x)
{
    return static_cast<int>(x * INT_PREC + 0.5);
}
inline int int2smallint(int x)
{
    return (x >> INT_PREC_BITS);
}
inline int int2bigint(int x)
{
    return (x << INT_PREC_BITS);
}
/**
 * @brief			使用近似浮点数的整数创建快速高斯核
 * @param sigma
 * @return
 */
int* __create_fast_kernel(double sigma)
{
    double* fp_kernel;
    int*    kernel;
    int     i;
    int     filter_size;
    if (sigma > 200) {
        sigma = 200;
    }
    filter_size = (int)floor(sigma * 6) / 2;
    filter_size = filter_size * 2 + 1;
    kernel      = new int[filter_size];
    fp_kernel   = __create_kernel(sigma);
    for (i = 0; i < filter_size; i++) {
        kernel[i] = double2int(fp_kernel[i]);
    }
    delete fp_kernel;
    return kernel;
}

/**
 * @brief			对两个方向上相等的sigma值执行高斯卷积
 * @param img
 * @param sigma
 */
void __filter_gaussian(cv::Mat& img, double sigma)
{
    int     i, j, k, source, filter_size;
    int*    kernel;
    cv::Mat temp;
    int     v1, v2, v3;
    if (sigma > 200)
        sigma = 200;
    filter_size = (int)floor(sigma * 6) / 2;
    filter_size = filter_size * 2 + 1;
    kernel      = __create_fast_kernel(sigma);
    temp.create(cv::Size(img.cols, img.rows), img.type());

    // 滤波x方向
    for (j = 0; j < temp.rows; j++) {
        for (i = 0; i < temp.cols; i++) {
            // 内部循环已展开
            v1 = v2 = v3 = 0;
            for (k = 0; k < filter_size; k++) {
                source = i + filter_size / 2 - k;

                if (source < 0) {
                    source *= -1;
                }
                if (source > img.cols - 1) {
                    source = 2 * (img.cols - 1) - source;
                }

                v1 += kernel[k] * (unsigned char)pdata(img, source, j, 0);
                if (img.channels() == 1) {
                    continue;
                }
                v2 += kernel[k] * (unsigned char)pdata(img, source, j, 1);
                v3 += kernel[k] * (unsigned char)pdata(img, source, j, 2);
            }
            pdata(temp, i, j, 0) = (char)int2smallint(v1);
            if (img.channels() == 1) {
                continue;
            }
            pdata(temp, i, j, 1) = (char)int2smallint(v2);
            pdata(temp, i, j, 2) = (char)int2smallint(v3);
        }
    }
    for (j = 0; j < img.rows; j++) {
        for (i = 0; i < img.cols; i++) {
            v1 = v2 = v3 = 0;
            for (k = 0; k < filter_size; k++) {
                source = j + filter_size / 2 - k;

                if (source < 0) {
                    source *= -1;
                }
                if (source > temp.rows - 1) {
                    source = 2 * (temp.rows - 1) - source;
                }

                v1 += kernel[k] * (unsigned char)pdata(temp, i, source, 0);
                if (img.channels() == 1) {
                    continue;
                }
                v2 += kernel[k] * (unsigned char)pdata(temp, i, source, 1);
                v3 += kernel[k] * (unsigned char)pdata(temp, i, source, 2);
            }
            // set value and move on
            pdata(img, i, j, 0) = (char)int2smallint(v1);
            if (img.channels() == 1)
                continue;
            pdata(img, i, j, 1) = (char)int2smallint(v2);
            pdata(img, i, j, 2) = (char)int2smallint(v3);
        }
    }
    delete kernel;
}

/**
 * @brief			通过使用图像金字塔和分离滤波器，可以快速执行任意大小的高斯卷积。使用递归。
 * @param img
 * @param sigma
 */
void __fast_filter(cv::Mat& img, double sigma)
{
    int filter_size;
    if (sigma > 200) {
        sigma = 200;
    }
    filter_size = (int)floor(sigma * 6) / 2;
    filter_size = filter_size * 2 + 1;
    if (filter_size < 3) {
        return;
    }

    // 过滤，或降采样和递归
    if (filter_size < 10) {
#ifdef USE_EXACT_SIGMA
        __filter_gaussian(img, sigma)
#else
        cv::GaussianBlur(img, img, cv::Size(filter_size, filter_size), 0);
#endif
    }
    else {
        if (img.cols < 2 || img.rows < 2)
            return;
        cv::Mat sub_img(cv::Size(img.cols / 2, img.rows / 2), img.type());
        cv::pyrDown(img, sub_img);
        __fast_filter(sub_img, sigma / 2.0);
        cv::resize(sub_img, img, cv::Size(img.cols, img.rows), cv::INTER_NEAREST);
    }
}

cv::Mat VEnhance::retinex(cv::Mat img, double sigma, int gain, int offset)
{
    cv::Mat   A;
    cv::Mat   fA;
    cv::Mat   fB;
    cv::Mat   fC;
    cv::Mat   dst;
    const int ch = img.channels();
    fA.create(cv::Size(img.cols, img.rows), CV_32FC(ch));
    fB.create(cv::Size(img.cols, img.rows), CV_32FC(ch));
    fC.create(cv::Size(img.cols, img.rows), CV_32FC(ch));

    // 计算log
    img.convertTo(fA, CV_32F);
    cv::log(fA, fB);
    // Compute log of blured image
    A = img.clone();
    __fast_filter(A, sigma);
    A.convertTo(fA, CV_32F);
    cv::log(fA, fC);

    // Compute difference
    cv::subtract(fB, fC, fA);
    // Restore
    // cv::convertScaleAbs(fA, img, gain, offset);
    // fA.convertTo(img, img.type(), gain, offset);
    fA.convertTo(dst, img.type(), gain, offset);
    return dst;
}

cv::Mat VEnhance::multi_scale_retinex(cv::Mat img, int scales, double* weights, double* sigmas, int gain, int offset)
{
    int       i;
    double    weight;
    const int ch = img.channels();
    cv::Mat   A;
    cv::Mat   fA;
    cv::Mat   fB;
    cv::Mat   fC;
    cv::Mat   dst;
    fA.create(cv::Size(img.cols, img.rows), CV_32FC(ch));
    fB.create(cv::Size(img.cols, img.rows), CV_32FC(ch));
    fC.create(cv::Size(img.cols, img.rows), CV_32FC(ch));

    // 计算log
    img.convertTo(fA, CV_32F);
    cv::log(fA, fB);
    // Normalize according to given weights
    for (i = 0, weight = 0; i < scales; i++) {
        weight += weights[i];
    }

    if (weight != 1.0) {
        fB.convertTo(fB, fB.type(), weight);   // cv::convertScaleAbs(fB, fB, weight);
    }

    // Filter at each scale
    for (i = 0; i < scales; i++) {
        A = img.clone();
        __fast_filter(A, sigmas[i]);

        A.convertTo(fA, CV_32F);
        cv::log(fA, fC);

        // Compute weighted difference
        // cvScale(fC, fC, weights[i]);
        // cv::convertScaleAbs(fC, fC, weights[i]);
        fC.convertTo(fC, fC.type(), weights[i]);
        // cv::scaleAdd(fC, fC, weights[i]);
        // cvSub(fB, fC, fB);
        cv::subtract(fB, fC, fB);
    }
    // cv::convertScaleAbs(fB, img, gain, offset);
    // fB.convertTo(img, img.type(), gain, offset);
    fB.convertTo(dst, img.type(), gain, offset);
    return dst;
}

cv::Mat VEnhance::multi_scale_retinex_CR(cv::Mat img, int scales, double* weights, double* sigmas, int gain, int offset, double restoration_factor, double color_gain)
{
    int     i;
    double  weight;
    cv::Mat A;
    cv::Mat B;
    cv::Mat C;
    cv::Mat fA;
    cv::Mat fB;
    cv::Mat fC;
    cv::Mat fsA;
    cv::Mat fsB;
    cv::Mat fsC;
    cv::Mat fsD;
    cv::Mat fsE;
    cv::Mat fsF;
    cv::Mat dst;

    // Initialize temp images
    const int ch = img.channels();
    fA.create(cv::Size(img.cols, img.rows), CV_32FC(ch));
    fB.create(cv::Size(img.cols, img.rows), CV_32FC(ch));
    fC.create(cv::Size(img.cols, img.rows), CV_32FC(ch));

    fsA.create(cv::Size(img.cols, img.rows), CV_32FC1);
    fsB.create(cv::Size(img.cols, img.rows), CV_32FC1);
    fsC.create(cv::Size(img.cols, img.rows), CV_32FC1);
    fsD.create(cv::Size(img.cols, img.rows), CV_32FC1);
    fsE.create(cv::Size(img.cols, img.rows), CV_32FC1);
    fsF.create(cv::Size(img.cols, img.rows), CV_32FC1);

    // Compute log image
    img.convertTo(fB, CV_32F);
    cv::log(fB, fA);

    for (i = 0, weight = 0; i < scales; i++)
        weight += weights[i];

    if (weight != 1.0)
        fA.convertTo(fA, fA.type(), weight);   // cv::convertScaleAbs(fA, fA, weight);

    // Filter at each scale
    for (i = 0; i < scales; i++) {
        // TODO
        // https://blog.csdn.net/smallstones/article/details/41787079
        A = img.clone();
        __fast_filter(A, sigmas[i]);

        A.convertTo(fB, CV_32F);
        cv::log(fB, fC);

        // Compute weighted difference
        // cv::convertScaleAbs(fC, fC, weights[i]);
        fC.convertTo(fC, fC.type(), weights[i]);
        cv::subtract(fA, fC, fA);
    }
    // Color restoration
    if (img.channels() > 1) {
        A.create(cv::Size(img.cols, img.rows), CV_8UC1);
        B.create(cv::Size(img.cols, img.rows), CV_8UC1);
        C.create(cv::Size(img.cols, img.rows), CV_8UC1);

        // Divide image into channels, convert and store sum
        std::vector<cv::Mat> channels;
        channels.push_back(A);
        channels.push_back(B);
        channels.push_back(C);
        cv::split(img, channels);

        A.convertTo(fsA, CV_32FC1);
        B.convertTo(fsB, CV_32FC1);
        C.convertTo(fsC, CV_32FC1);

        // Sum components
        cv::add(fsA, fsB, fsD);
        cv::add(fsD, fsC, fsD);

        // Normalize weights
        cv::divide(fsA, fsD, fsA, restoration_factor);
        cv::divide(fsB, fsD, fsB, restoration_factor);
        cv::divide(fsC, fsD, fsC, restoration_factor);

        /*cv::convertScaleAbs(fsA, fsA, 1, 1);
        cv::convertScaleAbs(fsB, fsB, 1, 1);
        cv::convertScaleAbs(fsC, fsC, 1, 1);*/

        fsA.convertTo(fsA, fsA.type(), 1, 1);
        fsB.convertTo(fsB, fsB.type(), 1, 1);
        fsC.convertTo(fsC, fsC.type(), 1, 1);

        // Log weights
        cv::log(fsA, fsA);
        cv::log(fsB, fsB);
        cv::log(fsC, fsC);

        // Divide retinex image, weight accordingly and recombine
        // cvCvtPixToPlane(fA, fsD, fsE, fsF, NULL);
        std::vector<cv::Mat> channels_temp;
        channels_temp.push_back(fsD);
        channels_temp.push_back(fsE);
        channels_temp.push_back(fsF);
        cv::split(fA, channels_temp);

        /*cvMul(fsD, fsA, fsD, color_gain);
        cvMul(fsE, fsB, fsE, color_gain);
        cvMul(fsF, fsC, fsF, color_gain);*/
        cv::multiply(fsD, fsA, fsD, color_gain);
        cv::multiply(fsE, fsB, fsE, color_gain);
        cv::multiply(fsF, fsC, fsF, color_gain);

        // cvCvtPlaneToPix(fsD, fsE, fsF, NULL, fA);
        cv::merge(channels_temp, fA);
    }
    // cv::convertScaleAbs(fA, img, gain, offset);
    // fA.convertTo(img, img.type(), gain, offset);
    fA.convertTo(dst, img.type(), gain, offset);
    return dst;
}

/**
 * @brief			求图像的中位数
 * @param src		输入单通道图像
 * @return
 */
int __get_mat__midval(cv::Mat& src)
{
    if (src.channels() > 1) {
        return 0;
    }
    int rows = src.rows;
    int cols = src.cols;
    /*定义数组*/
    float mat_hists[256] = {0};
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            int val = src.at<uchar>(row, col);
            mat_hists[val]++;
        }
    }
    int calc_val = rows * cols / 2;
    int tmp_sum  = 0;
    for (int i = 0; i < 255; i++) {
        tmp_sum += mat_hists[i];
        if (tmp_sum > calc_val) {
            return i;
        }
    }
    return 0;
}

void VEnhance::adapt_min_max_threshold_median(cv::Mat img, int& min_val, int& max_val, float sigma /*= 0.3*/)
{
    int mid_val = __get_mat__midval(img);
    if (mid_val == 0)
        return;
    /*opencv自带的防止颜色溢出的函数*/
    min_val = cv::saturate_cast<uchar>((1.0 - sigma) * mid_val);
    max_val = cv::saturate_cast<uchar>((1.0 + sigma) * mid_val);
}

/**
 * @brief			仿照matlab，自适应求高低两个门限
 * @param dx
 * @param dy
 * @param low
 * @param high
 * @param percent_of_pix_is_not_edges
 */
void __adaptive_find_threshold(cv::Mat dx, cv::Mat dy, double& low, double& high, double percent_of_pix_is_not_edges = 0.7)
{
    int      i;
    int      j;
    cv::Size size = dx.size();
    cv::Mat  imge = cv::Mat::zeros(size, CV_32FC1);
    float    maxv = 0.0;
    float    data = 0.0;
    for (i = 0; i < size.height; i++) {
        // 计算边缘强度
        for (j = 0; j < size.width; j++) {
            data                 = (float)(abs(dx.at<uchar>(i, j)) + abs(dy.at<uchar>(i, j)));
            imge.at<float>(i, j) = data;
            maxv                 = maxv < data ? data : maxv;
        }
    }
    if (maxv == 0) {
        low  = 0;
        high = 0;
        return;
    }
    // 计算直方图
    int hist_size = maxv;
    // hist_size                               = hist_size > (int)maxv ? (int)maxv : hist_size;
    float        range_0[2]  = {0, (float)maxv};
    const float* ranges[1]   = {range_0};
    const int    channels[1] = {0};
    cv::Mat      hist;
    // 注意const类型，避免参数类型不符合
    cv::calcHist(&imge, 1, channels, cv::Mat(), hist, 1, &hist_size, ranges);
    int        total  = (int)(size.height * size.width * percent_of_pix_is_not_edges);
    float      sum    = 0;
    int        icount = hist.rows;
    cv::Scalar mean   = cv::mean(hist);
    cv::Scalar mean1  = cv::mean(imge);
    for (i = 0; i < icount; i++) {
        sum += hist.at<float>(i, 0);
        if (sum > total) {
            break;
        }
    }
    // Canny 推荐的 高:低 阈值比在 2:1 到3:1之间。2~3 --> 2.5
    high = (double)((i + 1) * maxv / hist_size);
    if (high >= 255)
        high = 255;
    if (0.0 == high) {
        low = 0;
    }
    else {
        low = high * 0.4;
    }
}


void VEnhance::adapt_find_threshold_matlab(const cv::Mat image, double& low, double& high, double percent_of_pix_is_not_edges /*= 0.7*/, int aperture_size /*= 3*/)
{
    const int cn = image.channels();
    cv::Mat   dx(image.rows, image.cols, CV_16SC(cn));
    cv::Mat   dy(image.rows, image.cols, CV_16SC(cn));
    cv::Sobel(image, dx, CV_16S, 1, 0, aperture_size, 1, 0, cv::BORDER_REPLICATE);
    cv::Sobel(image, dy, CV_16S, 0, 1, aperture_size, 1, 0, cv::BORDER_REPLICATE);
    cv::Mat _dx = dx.clone();
    cv::Mat _dy = dy.clone();
    __adaptive_find_threshold(_dx, _dy, low, high);
}

void VEnhance::non_maximum_suppression(cv::Mat& src, cv::Mat& dst, int win_size /*= 15*/)
{
    int     border_size = (win_size - 1) / 2;
    cv::Mat src_diff, temp;
    cv::convertScaleAbs(src, src_diff);
    dst = cv::Mat::zeros(src_diff.size(), src_diff.type());
    for (int i = border_size; i < src.rows - border_size; i++) {
        for (int j = border_size; j < src.cols - border_size; j++) {
            temp      = src_diff(cv::Rect(j - border_size, i - border_size, win_size, win_size));
            uchar max = *std::max_element(temp.begin<uchar>(), temp.end<uchar>());
            for (int m = 0; m < temp.rows; m++) {
                uchar* every_data_row = temp.ptr<uchar>(m);
                for (int n = 0; n < temp.cols; n++) {
                    uchar every_data = every_data_row[n];
                    if (every_data >= max) {
                        uchar* dst_row               = dst.ptr<uchar>(m + i - border_size);
                        dst_row[n + j - border_size] = every_data;
                    }
                }
            }
        }
    }
}


cv::Mat VEnhance::segmented_enhancement(const cv::Mat& img, double r1, double r2, double s1, double s2)
{
    double k1 = s1 / r1;
    double k2 = (s2 - s1) / (r2 - r1);
    double k3 = (255 - s2) / (255 - r2);
    uchar  Lutfirst[256];
    for (int i = 0; i < 256; i++) {
        if (i <= r2 && i >= r1) {
            Lutfirst[i] = k2 * (i - r1);
        }
        if (i < r1) {
            Lutfirst[i] = k1 * i;
        }
        if (i > r2) {
            Lutfirst[i] = k3 * (i - r2);
        }
    }
    cv::Mat lut(1, 256, CV_8U, Lutfirst);
    cv::Mat dst;
    cv::LUT(img, lut, dst);
    return dst;
}

/**
 * @brief			将rgb转为YCrCb
 * @param input
 * @param chls
 */
void __color_transfer_with_spilt(cv::Mat& input, std::vector<cv::Mat>& chls)
{
    cv::cvtColor(input, input, cv::COLOR_BGR2YCrCb);
    cv::split(input, chls);
}

/**
 * @brief			将YCrCb转为rgb
 * @param output
 * @param chls
 */
void __color_retransfer_with_merge(cv::Mat& output, std::vector<cv::Mat>& chls)
{
    cv::merge(chls, output);
    cv::cvtColor(output, output, cv::COLOR_YCrCb2BGR);
}

/**
 * @brief			 求直方图
 * @param src
 * @param dst
 */
void __get_histogram(const cv::Mat& src, int* dst)
{
    cv::Mat      hist;
    int          channels[1] = {0};
    int          histSize[1] = {256};
    float        hranges[2]  = {0, 256.0};
    const float* ranges[1]   = {hranges};
    cv::calcHist(&src, 1, channels, cv::Mat(), hist, 1, histSize, ranges);
    for (int i = 0; i < 256; i++) {
        float binVal = hist.at<float>(i);
        dst[i]       = int(binVal);
    }
}

double VEnhance::spline(double* x, double* y, int n, double* t, int m, double* z)
{
    double* dy = new double[n];
    std::memset(dy, 0, sizeof(double) * n);
    dy[0]       = -0.5;
    double* ddy = new double[n];
    std::memset(ddy, 0, sizeof(double) * n);

    double  h1;
    double* s  = new double[n];
    double  h0 = x[1] - x[0];
    s[0]       = 3.0 * (y[1] - y[0]) / (2.0 * h0) - ddy[0] * h0 / 4.0;

    for (int j = 1; j <= n - 2; ++j) {
        h1           = x[j + 1] - x[j];
        double alpha = h0 / (h0 + h1);
        double beta  = (1.0 - alpha) * (y[j] - y[j - 1]) / h0;
        beta         = 3.0 * (beta + alpha * (y[j + 1] - y[j]) / h1);
        dy[j]        = -alpha / (2.0 + (1.0 - alpha) * dy[j - 1]);
        s[j]         = (beta - (1.0 - alpha) * s[j - 1]);
        s[j]         = s[j] / (2.0 + (1.0 - alpha) * dy[j - 1]);
        h0           = h1;
    }

    dy[n - 1] = (3.0 * (y[n - 1] - y[n - 2]) / h1 + ddy[n - 1] * h1 / 2.0 - s[n - 2]) / (2.0 + dy[n - 2]);

    for (int j = n - 2; j >= 0; --j) {
        dy[j] = dy[j] * dy[j + 1] + s[j];
    }
    for (int j = 0; j <= n - 2; ++j) {
        s[j] = x[j + 1] - x[j];
    }

    for (int j = 0; j <= n - 2; ++j) {
        h1     = s[j] * s[j];
        ddy[j] = 6.0 * (y[j + 1] - y[j]) / h1 - 2.0 * (2.0 * dy[j] + dy[j + 1]) / s[j];
    }

    h1         = s[n - 2] * s[n - 2];
    ddy[n - 1] = 6.0 * (y[n - 2] - y[n - 1]) / h1 + 2.0 * (2.0 * dy[n - 1] + dy[n - 2]) / s[n - 2];
    double g   = 0.0;
    for (int i = 0; i <= n - 2; i++) {
        h1 = 0.5 * s[i] * (y[i] + y[i + 1]);
        h1 = h1 - s[i] * s[i] * s[i] * (ddy[i] + ddy[i + 1]) / 24.0;
        g  = g + h1;
    }

    for (int j = 0; j <= m - 1; j++) {
        int i;
        if (t[j] >= x[n - 1]) {
            i = n - 2;
        }
        else {
            i = 0;
            while (t[j] > x[i + 1]) {
                i = i + 1;
            }
        }
        h1   = (x[i + 1] - t[j]) / s[i];
        h0   = h1 * h1;
        z[j] = (3.0 * h0 - 2.0 * h0 * h1) * y[i];
        z[j] = z[j] + s[i] * (h0 - h0 * h1) * dy[i];
        h1   = (t[j] - x[i]) / s[i];
        h0   = h1 * h1;
        z[j] = z[j] + (3.0 * h0 - 2.0 * h0 * h1) * y[i + 1];
        z[j] = z[j] - s[i] * (h0 - h0 * h1) * dy[i + 1];
    }
    delete[] s;
    delete[] dy;
    delete[] ddy;
    return (g);
}

void VEnhance::dot_line(cv::Mat& mat, const cv::Point& p1, const cv::Point& p2, const cv::Scalar& color, int thickness /*= 1*/, int lineType /*= 8*/, int line_step /*= 6*/, int blank_step /*= 6*/)
{
    if (p1 == p2)
        return;
    // validate line_step
    line_step = ::abs(line_step);
    if (line_step == 0)
        line_step = 1;

    // validate blank_step
    blank_step = ::abs(blank_step);
    if (blank_step == 0)
        blank_step = 1;

    // dot_ratio = blank_step / line_step;
    double dot_ratio = blank_step * 1.0 / line_step;

    // calculat step_x, step_y
    double len, step_x, step_y;
    len    = sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
    step_x = (p2.x - p1.x) / len * line_step;
    step_y = (p2.y - p1.y) / len * line_step;

    double x1, y1, x2, y2;
    x1 = p1.x;
    y1 = p1.y;   // start from Point p1

    // draw line step by step, until meet Point p2
    if (::abs(p1.x - p2.x) > ::abs(p1.y - p2.y)) {
        // step in direction of x-coordination
        while (WITHIN(x1, step_x, p2.x)) {
            if (EXCEED(x1 + step_x * (1 + dot_ratio), step_x, p2.x)) {
                x2 = p2.x;
                y2 = p2.y;
            }
            else if (EXCEED(x1 + step_x, step_x, p2.x)) {
                x2 = p2.x;
                y2 = p2.y;
            }
            else {
                x2 = x1 + step_x;
                y2 = y1 + step_y;
            }
            cv::line(mat, cv::Point(static_cast<int>(x1), static_cast<int>(y1)), cv::Point(static_cast<int>(x2), static_cast<int>(y2)), color, thickness, lineType);
            // step
            x1 = x2 + step_x * dot_ratio;
            y1 = y2 + step_y * dot_ratio;
        }
    }
    else {
        // step in direction of y-coordination
        while (WITHIN(y1, step_y, p2.y)) {
            if (EXCEED(y1 + step_y * (1 + dot_ratio), step_y, p2.y)) {
                x2 = p2.x;
                y2 = p2.y;
            }
            else if (EXCEED(y1 + step_y, step_y, p2.y)) {
                x2 = p2.x;
                y2 = p2.y;
            }
            else {
                x2 = x1 + step_x;
                y2 = y1 + step_y;
            }
            cv::line(mat, cv::Point(static_cast<int>(x1), static_cast<int>(y1)), cv::Point(static_cast<int>(x2), static_cast<int>(y2)), color, thickness, lineType);
            // step
            x1 = x2 + step_x * dot_ratio;
            y1 = y2 + step_y * dot_ratio;
        }
    }
}


cv::Mat VEnhance::gamma_trans(const cv::Mat& img, double gamma, int n_c)
{
    cv::Mat img_gamma(img.size(), CV_32FC1);
    for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {
            img_gamma.at<float>(i, j) = n_c * pow(img.at<uchar>(i, j), gamma);
        }
    }
    cv::normalize(img_gamma, img_gamma, 0, 255, cv::NormTypes::NORM_MINMAX);
    cv::convertScaleAbs(img_gamma, img_gamma);   // 将CV_32F的图像转成CV_8U的
    return img_gamma;
}

cv::Mat VEnhance::gray_stairs(const cv::Mat& img, double sin /*= 0.0*/, double hin /*= 255.0*/, double mt /*= 1.0*/, double sout /*= 0.0*/, double hout /*= 255.0*/)
{
    double Sin    = std::min(std::max(sin, 0.0), hin - 2);     // Sin, 黑场阈值, 0 <= Sin < Hin
    double Hin    = std::min(hin, 255.0);                      // Hin, 白场阈值, Sin<Hin<=255
    double Mt     = std::min(std::max(mt, 0.01), 9.99);        // Mt, 灰场调节值, 0.01~9.99
    double Sout   = std::min(std::max(sout, 0.0), hout - 2);   // Sout, 输出黑场阈值, 0<=Sout<Hout
    double Hout   = std::min(hout, 255.0);                     // Hout, 输出白场阈值, Sout<Hout<=255
    double difin  = Hin - Sin;
    double difout = Hout - Sout;
    uchar  lutData[256];
    for (int i = 0; i < 256; i++) {
        double v1  = std::min(std::max(255 * (i - Sin) / difin, 0.0), 255.0);         // 输入动态线性拉伸
        double v2  = 255 * std::pow(v1 / 255.0, 1.0 / Mt);                            // 灰场伽马调节
        lutData[i] = (int)std::min(std::max(Sout + difout * v2 / 255, 0.0), 255.0);   // # 输出线性拉伸
    }
    cv::Mat lut(1, 256, CV_8UC1, lutData);
    cv::Mat dst;
    cv::LUT(img, lut, dst);
    return dst;
}

cv::Mat VEnhance::get_rect_img(const cv::Mat& img, std::vector<cv::Point2i>& vecPt)
{
    cv::Mat grayimg;
    if (vecPt.size() < 3) {
        grayimg = img.clone();
        return grayimg;
    }
    // 构建凸包
    std::vector<cv::Point2i> hull;
    cv::convexHull(vecPt, hull);
    // 构建mask
    cv::Mat                               mask = cv::Mat::zeros(cv::Size(img.cols, img.rows), img.type());
    std::vector<std::vector<cv::Point2i>> contour;
    contour.emplace_back(hull);
    cv::drawContours(mask, contour, 0, cv::Scalar(255, 255, 255), cv::FILLED);
    // 输出
    cv::Mat ret;
    img.copyTo(ret, mask);
    grayimg = ret.clone();

    return grayimg;
}


cv::Mat VEnhance::get_equal_img(const cv::Mat& hist_img, const cv::Rect& hist_rect, const cv::Mat& template_img, const cv::Rect& template_rect)
{
    cv::Mat      hist1;
    cv::Mat      hist2;
    const int    channels[1] = {0};
    float        inRanges[2] = {0, 255};
    const float* ranges[1]   = {inRanges};
    const int    bins[1]     = {256};
    // 保留ROI 区域的直方图
    cv::Mat img_1 = hist_img(hist_rect).clone();
    cv::Mat img_2 = template_img(template_rect).clone();
    cv::calcHist(&img_1, 1, channels, cv::Mat(), hist1, 1, bins, ranges, true, false);
    cv::calcHist(&img_2, 1, channels, cv::Mat(), hist2, 1, bins, ranges, true, false);
    float hist1_cdf[256] = {hist1.at<float>(0)};
    float hist2_cdf[256] = {hist2.at<float>(0)};
    for (int i = 1; i < 256; ++i) {
        hist1_cdf[i] = hist1_cdf[i - 1] + hist1.at<float>(i);
        hist2_cdf[i] = hist2_cdf[i - 1] + hist2.at<float>(i);
    }
    // 归一化，两幅图像大小可能不一致
    for (int i = 0; i < 256; i++) {
        hist1_cdf[i] = hist1_cdf[i] / (img_1.rows * img_1.cols);
        hist2_cdf[i] = hist2_cdf[i] / (img_1.rows * img_1.cols);
    }
    float diff_cdf[256][256];
    for (int i = 0; i < 256; ++i) {
        for (int j = 0; j < 256; ++j) {
            diff_cdf[i][j] = fabs(hist1_cdf[i] - hist2_cdf[j]);
        }
    }
    cv::Mat lut(1, 256, CV_8U);
    for (int i = 0; i < 256; ++i) {
        float min   = diff_cdf[i][0];
        int   index = 0;
        for (int j = 1; j < 256; ++j) {
            if (min > diff_cdf[i][j]) {
                min   = diff_cdf[i][j];
                index = j;
            }
        }
        lut.at<uchar>(i) = (uchar)index;
    }
    cv::Mat result;
    cv::Mat hist3;
    cv::LUT(hist_img, lut, result);
    return result;
}


cv::Mat GetRotateCropImage(const cv::Mat& srcimage, std::vector<std::vector<int>> box)
{
    cv::Mat image;
    srcimage.copyTo(image);
    std::vector<std::vector<int>> points       = box;
    int                           x_collect[4] = {box[0][0], box[1][0], box[2][0], box[3][0]};
    int                           y_collect[4] = {box[0][1], box[1][1], box[2][1], box[3][1]};
    int                           left         = int(*std::min_element(x_collect, x_collect + 4));
    int                           right        = int(*std::max_element(x_collect, x_collect + 4));
    int                           top          = int(*std::min_element(y_collect, y_collect + 4));
    int                           bottom       = int(*std::max_element(y_collect, y_collect + 4));
    cv::Mat                       img_crop;
    image(cv::Rect(left, top, right - left, bottom - top)).copyTo(img_crop);

    for (int i = 0; i < points.size(); i++) {
        points[i][0] -= left;
        points[i][1] -= top;
    }
    int         img_crop_width  = int(sqrt(pow(points[0][0] - points[1][0], 2) + pow(points[0][1] - points[1][1], 2)));
    int         img_crop_height = int(sqrt(pow(points[0][0] - points[3][0], 2) + pow(points[0][1] - points[3][1], 2)));
    cv::Point2f pts_std[4], pointsf[4];
    pts_std[0] = cv::Point2f(0., 0.);
    pts_std[1] = cv::Point2f(img_crop_width, 0.);
    pts_std[2] = cv::Point2f(img_crop_width, img_crop_height);
    pts_std[3] = cv::Point2f(0.f, img_crop_height);
    pointsf[0] = cv::Point2f(points[0][0], points[0][1]);
    pointsf[1] = cv::Point2f(points[1][0], points[1][1]);
    pointsf[2] = cv::Point2f(points[2][0], points[2][1]);
    pointsf[3] = cv::Point2f(points[3][0], points[3][1]);
    cv::Mat M  = cv::getPerspectiveTransform(pointsf, pts_std);
    cv::Mat dst_img;
    cv::warpPerspective(img_crop, dst_img, M, cv::Size(img_crop_width, img_crop_height), cv::BORDER_REPLICATE);
    return dst_img;
}

cv::Mat VEnhance::get_equal_img_2(const cv::Mat& hist_img, const std::vector<cv::Point2f>& hist_pts, const cv::Mat& template_img, std::vector<cv::Point2f>& template_pts)
{
    std::vector<std::vector<int>> hist_box{
        {int(hist_pts[0].x), int(hist_pts[0].y)}, {int(hist_pts[1].x), int(hist_pts[1].y)}, {int(hist_pts[2].x), int(hist_pts[2].y)}, {int(hist_pts[3].x), int(hist_pts[3].y)}};
    std::vector<std::vector<int>> temp_box{{int(template_pts[0].x), int(template_pts[0].y)},
                                           {int(template_pts[1].x), int(template_pts[1].y)},
                                           {int(template_pts[2].x), int(template_pts[2].y)},
                                           {int(template_pts[3].x), int(template_pts[3].y)}};
    cv::Mat                       hist1, hist2;
    const int                     channels[1] = {0};
    float                         inRanges[2] = {0, 255};
    const float*                  ranges[1]   = {inRanges};
    const int                     bins[1]     = {256};
    // 保留ROI 区域的直方图
    cv::Mat img_1 = GetRotateCropImage(hist_img, hist_box);
    cv::Mat img_2 = GetRotateCropImage(template_img, temp_box);
    cv::calcHist(&img_1, 1, channels, cv::Mat(), hist1, 1, bins, ranges, true, false);
    cv::calcHist(&img_2, 1, channels, cv::Mat(), hist2, 1, bins, ranges, true, false);

    float hist1_cdf[256] = {hist1.at<float>(0)};
    float hist2_cdf[256] = {hist2.at<float>(0)};
    for (int i = 1; i < 256; ++i) {
        hist1_cdf[i] = hist1_cdf[i - 1] + hist1.at<float>(i);
        hist2_cdf[i] = hist2_cdf[i - 1] + hist2.at<float>(i);
    }
    // 归一化，两幅图像大小可能不一致
    for (int i = 0; i < 256; i++) {
        hist1_cdf[i] = hist1_cdf[i] / (img_1.rows * img_1.cols);
        hist2_cdf[i] = hist2_cdf[i] / (img_1.rows * img_1.cols);
    }
    float diff_cdf[256][256];
    for (int i = 0; i < 256; ++i) {
        for (int j = 0; j < 256; ++j) {
            diff_cdf[i][j] = fabs(hist1_cdf[i] - hist2_cdf[j]);
        }
    }
    cv::Mat lut(1, 256, CV_8U);
    for (int i = 0; i < 256; ++i) {
        float min   = diff_cdf[i][0];
        int   index = 0;
        for (int j = 1; j < 256; ++j) {
            if (min > diff_cdf[i][j]) {
                min   = diff_cdf[i][j];
                index = j;
            }
        }
        lut.at<uchar>(i) = (uchar)index;
    }
    cv::Mat result, hist3;
    cv::LUT(hist_img, lut, result);
    return result;
}

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END