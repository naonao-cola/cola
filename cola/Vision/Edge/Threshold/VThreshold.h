/**
 * @FilePath     : /cola/cola/Vision/Edge/Threshold/VThreshold.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-15 17:27:46
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-20 12:16:07
 **/
#ifndef NAONAO_VTHRESHOLD_H
#define NAONAO_VTHRESHOLD_H

#include "../../VisionObject.h"

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

enum class THRESHOLD_TYPE
{
    DEFAULT       = 0,
    HUANG         = 1,
    HUANG2        = 2,
    INTERMODES    = 3,
    ISO_DATA      = 4,
    LI            = 5,
    MAX_ENTROPY   = 6,
    MEAN          = 7,
    MIN_ERROR     = 8,
    MINIMUM       = 9,
    MOMENTS       = 10,
    OTSU          = 11,
    PERCENTILE    = 12,
    RENYI_ENTROPY = 13,
    SHANBHAG      = 14,
    TRIANGLE      = 15,
    YEN           = 16,
    JUBU          = 17,
    SAUVOLA       = 18
};

enum class METHOD
{
    MEAN,
    GAUSS,
    MEDIAN
};

class VThreshold : public VisionObject
{

public:
    /**
     * @brief 获取直方图
     * @param src
     * @param dst
     */
    static NVoid  get_histogram(const cv::Mat& src, int* dst);

    /**
     * @brief
     * @param data
     * @return
     * @details 该过程通过采用初始阈值将图像分为对象和背景，然后计算等于或低于阈值的像素和高于阈值的像素的平均值。
     * 计算这两个值的平均值，增加阈值并重复该过程，直到阈值大于复合平均值。与iso_data类似
     */
    NInt threshold_default(std::vector<int> data);

    /**
     * @brief
     * @param data
     * @return
     * @details 实现Huang的模糊阈值方法,使用香农熵函数（也可以使用Yager熵函数）
     */
    NInt huang(std::vector<int> data);

    /**
     * @brief
     * @param data
     * @return
     * @details 实现Huang的模糊阈值方法,使用香农熵函数（也可以使用Yager熵函数）
     */
    NInt huang2(std::vector<int> data);

    /**
     * @brief  中间模
     * @param data
     * @return
     * @details 采用双峰直方图。直方图需要平滑（使用迭代地运行大小为3的平均值），直到只有两个局部极大值。j和k阈值t为（j+k）/2。
     * 直方图具有极不相等的峰值或宽而平坦不适合这种方法。
     */
    NInt intermodes(std::vector<int> data);

    /**
     * @brief
     * @param data
     * @return
     * @details 该过程通过采用初始阈值将图像分为对象和背景，然后计算等于或低于阈值的像素和高于阈值的像素的平均值。
     * 计算这两个值的平均值，增加阈值并重复该过程，直到阈值大于复合平均值。与threshold_default类似
     */
    NInt iso_data(std::vector<int> data);

    /**
     * @brief
     * @param data
     * @return
     * @details 实现了李的最小交叉熵阈值方法,该实现基于算法的迭代版本
     */
    NInt li(std::vector<int> data);

    /**
     * @brief
     * @param data
     * @return
     * @details 一种利用直方图熵进行灰度图像阈值化的新方法,实现 Kapur-Sahoo-Wong（最大熵）阈值方法。
     */
    NInt max_entropy(std::vector<int> data);

    /**
     * @brief
     * @param data
     * @return
     * @details 阈值是灰度数据的平均值
     */
    NInt mean(std::vector<int> data);

    /**
     * @brief
     * @param data
     * @return
     * @details Kittler 和 Illingworth 的最小误差阈值的迭代实现。此实现似乎比原始实现更频繁地收敛。然而，有时该算法不会收敛到一个解决方案。
       在这种情况下，将向日志窗口报告警告，结果默认为使用均值方法计算的阈值的初始估计值。
    */
    NInt min_errorI(std::vector<int> data);

    /**
     * @brief
     * @param data
     * @return
     * @details 与 Intermodes 方法类似，这假设一个双峰直方图。
     * 直方图使用大小为 3 的运行平均值迭代平滑，直到只有两个局部最大值。阈值 t 使得 yt-1 > yt <= yt+1。
     * 直方图具有极不相等的峰或宽而平坦的谷的图像不适合这种方法。
     */
    NInt minimum(std::vector<int> data);

    /**
     * @brief
     * @param data
     * @return
     * @details  Tsai的方法试图在阈值化结果中保留原始图像的矩。“矩保持阈值：一种新方法”。
     */
    NInt moments(std::vector<int> data);

    /**
     * @brief 大津阈值法
     * @param data
     * @return
     */
    NInt otsu(std::vector<int> data);
    /**
     * @brief 百分比阈值法，默认50%
     * @param data
     * @param ptile
     * @return
     */
    NInt percentile(std::vector<int> data, NDouble ptile = 0.5);

    /**
     * @brief
     * @param data
     * @return
     * @details 一种利用直方图熵进行灰度图像阈值化的新方法,类似于MaxEntropy方法，但使用 Renyi 的熵。
     */
    NInt renyi_entropy(std::vector<int> data);

    /**
     * @brief
     * @param data
     * @return
     * @details “利用信息度量作为图像阈值化“图形模型和图像处理”
     */
    NInt shanbhag(std::vector<int> data);

    /**
     * @brief
     * @param data
     * @return
     * @details 三角形算法是一种几何方法，它无法判断数据是否偏向一侧或另一侧，但会假定直方图一端附近的最大峰值（众数）
     并向另一端搜索。这会在缺少要处理的图像类型信息的情况下，或者当最大值不在直方图极值之一附近（导致在该最大值和极值
     之间产生两个可能的阈值区域）时出现问题。在这里，该算法被扩展为查找数据在最大峰值的哪一侧走得最远，并在该最大范围
     内搜索阈值。
    */
    NInt triangle(std::vector<int> data);

    /**
     * @brief
     * @param data
     * @return
     * @details 实现日元阈值方法
     */
    NInt yen(std::vector<int> data);

    /**
     * @brief
     * @param data
     * @return
     * @details 局部自适应阈值
     */
    NInt jubu(cv::Mat& src, METHOD type = METHOD::GAUSS, NInt radius = 3, NFloat ratio = 0.15);


    /**
     * @brief
     * @param src 单通道灰度图
     * @param dst 单通道处理后的图
     * @param k   threshold = mean*(1 + k*((std / 128) - 1))
     * @param wnd_size 处理区域宽高, 一定是奇数
     * @return
     * @note
     * https://blog.csdn.net/wfh2015/article/details/80418336?spm=1001.2101.3001.6650.3&utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7Edefault-3-80418336-blog-86595319.pc_relevant_multi_platform_whitelistv1_exp2&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7Edefault-3-80418336-blog-86595319.pc_relevant_multi_platform_whitelistv1_exp2&utm_relevant_index=6
     */
    NInt sauvola(const cv::Mat& src, cv::Mat& dst, const double& k = 0.1, const int& wnd_size = 7);

    /**
     * @brief 执行阈值化
     * @param src  原图
     * @param type 阈值化类型
     * @param doIblack 忽略黑色的像素值大小的范围，在此范围下，不进行统计。默认-1，建议 20 到40左右
     * @param doIwhite 忽略白色的像素值大小的范围，在此范围上，不进行统计，默认-1, 建议 220到255左右
     * @param reset    是否进行阈值化
     * @return         返回阈值
     */
    NInt exec_threshold(cv::Mat& src, THRESHOLD_TYPE type, NInt doIblack = -1, NInt doIwhite = -1, bool reset = false);

    /**
     * @brief 选择阈值化类型
     * @param src
     * @return
     */
    NInt select(cv::Mat& src, NInt type = -1);

    template<typename T>
    static NDouble partialSum(std::vector<T> y, NInt j)
    {
        NDouble x = 0;
        for (NInt i = 0; i <= j; i++) {
            x += y[i];
        }
        return x;
    }
};

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END

#endif   // NAONAO_VTHRESHOLD_H