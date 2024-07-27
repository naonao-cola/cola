/**
 * @FilePath     : /cola/cola/Vision/Feature/Hog/VHog.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-18 15:51:54
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-20 11:06:14
 **/
#include "VHog.h"

extern "C" {
#include "hog.h"   // From the VLFeat C library
}
NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

cv::Mat VHog::operator()(/*int training_index = 0 */)
{
    cv::Mat gray_img;
    for (int training_index = 0; training_index < images_.size(); training_index++) {
        if (images_[training_index].channels() == 3) {
            cv::cvtColor(images_[training_index], gray_img, cv::COLOR_RGB2GRAY);
        }
        else {
            gray_img = images_[training_index];
        }
        cv::resize(gray_img, gray_img, train_size_);
        cv::Mat roi_img;
        gray_img.convertTo(roi_img, CV_32FC1);
        VlHog* hog = vl_hog_new(VlHogVariant::VlHogVariantUoctti, num_bins_, VL_FALSE);
        vl_hog_set_use_bilinear_orientation_assignments(hog, VL_TRUE);
        vl_hog_put_image(hog, reinterpret_cast<float*>(roi_img.data), roi_img.cols, roi_img.rows, 1, cell_size_);
        int     ww = static_cast<int>(vl_hog_get_width(hog));   // assert ww == hh == numCells
        int     hh = static_cast<int>(vl_hog_get_height(hog));
        int     dd = static_cast<int>(vl_hog_get_dimension(hog));   // assert ww=hogDim1, hh=hogDim2, dd=hogDim3
        cv::Mat hogArray(1, ww * hh * dd, CV_32FC1);                // safer & same result. Don't use C-style memory management.
        vl_hog_extract(hog, hogArray.ptr<float>(0));
        vl_hog_delete(hog);
        hog_descriptors_.push_back(hogArray);
    }
    return hog_descriptors_.clone();
};

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END