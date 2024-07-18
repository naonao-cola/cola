/**
 * @FilePath     : /cola/cola/Vision/Feature/LBP/VLbp.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-18 16:29:50
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-18 16:39:46
 **/
#ifndef NAONAO_VLBP_H
#define NAONAO_VLBP_H


#include <utility>

#include "../../VisionObject.h"

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

class VLbp : public VisionObject
{
public:
    /**
     * @brief 提取LBP特征
     * @param images 图像的数组
     * @param train_size 训练图像的大小，建议不小于20，最低是15
     */
    VLbp(std::vector<cv::Mat> images, cv::Size train_size)
        : images_(std::move(images))
        , train_size_(train_size){};
    ~VLbp() override = default;
    cv::Mat operator()();

private:
    std::vector<cv::Mat> images_;
    cv::Size             train_size_;
    cv::Mat              lbp_descriptors_;
};

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END

#endif   // NAONAO_VLBP_H