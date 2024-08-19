/**
 * @FilePath     : /cola/cola/Vision/Feature/Hog/VHog.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-18 15:51:54
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-08-19 15:58:10
 **/
#include "VHog.h"

extern "C" {
#include "hog.h"   // From the VLFeat C library
}
NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

cv::Mat VHog::operator()(/*NInt training_index = 0 */)
{
    for (NInt training_index = 0; training_index < images_.size(); training_index++) {
        cv::Mat src = images_[training_index];
        cv::Mat dst;
        cv::Mat hogArray;
        if (img_type_ == IMG_TYPE::GRAY) {
            if (src.channels() == 3) {
                cv::cvtColor(src, dst, cv::COLOR_RGB2GRAY);
            }
            else {
                dst = src;
            }
            hogArray = extract_hog(dst);
        }
        if (img_type_ == IMG_TYPE::RGB) {
            if (src.channels() == 3) {
                dst = src;
            }
            else {
                cv::cvtColor(src, dst, cv::COLOR_GRAY2RGB);
            }
            std::vector<cv::Mat> img_v1;
            cv::split(dst, img_v1);
            cv::Mat hogArray_0 = extract_hog(img_v1[0]);
            cv::Mat hogArray_1 = extract_hog(img_v1[1]);
            cv::Mat hogArray_2 = extract_hog(img_v1[2]);

            std::vector<cv::Mat> rows;
            rows.push_back(hogArray_0);
            rows.push_back(hogArray_1);
            rows.push_back(hogArray_2);
            cv::hconcat(rows, hogArray);
        }
        if (img_type_ == IMG_TYPE::HSV) {
            if (src.channels() == 3) {
                cv::cvtColor(src, dst, cv::COLOR_BGR2HSV);
            }
            else {
                cv::cvtColor(src, dst, cv::COLOR_BGR2HSV);
                cv::cvtColor(dst, dst, cv::COLOR_BGR2HSV);
            }
            std::vector<cv::Mat> img_v1;
            cv::split(dst, img_v1);
            cv::Mat hogArray_0 = extract_hog(img_v1[0]);
            cv::Mat hogArray_1 = extract_hog(img_v1[1]);
            cv::Mat hogArray_2 = extract_hog(img_v1[2]);

            std::vector<cv::Mat> rows;
            rows.push_back(hogArray_0);
            rows.push_back(hogArray_1);
            rows.push_back(hogArray_2);
            cv::hconcat(rows, hogArray);
        }
        hog_descriptors_.push_back(hogArray);
    }
    return hog_descriptors_.clone();
};

cv::Mat VHog::extract_hog(const cv::Mat& img)
{
    cv::Mat roi_img;
    cv::Mat size_img;
    cv::resize(img, size_img, train_size_);
    size_img.convertTo(roi_img, CV_32FC1);
    VlHog* hog = vl_hog_new(VlHogVariant::VlHogVariantUoctti, num_bins_, VL_FALSE);
    vl_hog_set_use_bilinear_orientation_assignments(hog, VL_TRUE);
    vl_hog_put_image(hog, reinterpret_cast<NFloat*>(roi_img.data), roi_img.cols, roi_img.rows, 1, cell_size_);
    NInt     ww = static_cast<int>(vl_hog_get_width(hog));   // assert ww == hh == numCells
    NInt     hh = static_cast<int>(vl_hog_get_height(hog));
    NInt     dd = static_cast<int>(vl_hog_get_dimension(hog));   // assert ww=hogDim1, hh=hogDim2, dd=hogDim3
    cv::Mat hogArray(1, ww * hh * dd, CV_32FC1);                // safer & same result. Don't use C-style memory management.
    vl_hog_extract(hog, hogArray.ptr<NFloat>(0));
    vl_hog_delete(hog);
    return hogArray;
}

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END