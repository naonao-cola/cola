/**
 * @FilePath     : /cola/cola/Vision/Feature/Hog/VHog.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-18 15:51:35
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-08-19 15:57:46
 **/
#ifndef NAONAO_VHOG_H
#define NAONAO_VHOG_H

#include "../../VisionObject.h"

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

class VHog : public VisionObject
{

public:
    /**
     * @brief 使用给定的图像和参数构造hog变换。
     * @param images 图像用于训练或测试的图像矢量。
     * @param vlhog_variant  ，默认 VlHogVariant::VlHogVariantUoctti
     * @param num_cells num_cells应围绕每个地标构建的HoG单元数。
     * @param cell_size 单元大小一个HoG单元的宽度，以像素为单位。
     * @param num_bins  num_bins表示hog的方向数。
     *
     * 当使用模型对新图像进行预测时，图像只能由1个图像组成。
     */
    VHog(std::vector<cv::Mat> images, NInt num_cells, NInt cell_size, NInt num_bins, cv::Size train_size, NInt vlhog_variant = 1)
        : images_(std::move(images))
        , num_cells_(num_cells)
        , cell_size_(cell_size)
        , num_bins_(num_bins)
        , train_size_(train_size)
        , vlhog_variant_(vlhog_variant){};

    /**
     * @brief 特征提取
     * @return 返回特征向量
     */
    cv::Mat operator()();

    // 图像类型
    enum class IMG_TYPE
    {
        GRAY = 0,
        RGB  = 1,
        HSV  = 2,
    };
    IMG_TYPE img_type_ = IMG_TYPE::GRAY;

private:
    std::vector<cv::Mat> images_;
    NInt                  vlhog_variant_;
    cv::Size             train_size_;
    cv::Mat              hog_descriptors_;
    NInt                  num_cells_;
    NInt                  cell_size_;
    NInt                  num_bins_;

    cv::Mat extract_hog(const cv::Mat& img);
};

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END

#endif   // NAONAO_VHOG_H