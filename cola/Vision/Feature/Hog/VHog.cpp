/**
 * @FilePath     : /cola/cola/Vision/Feature/Hog/VHog.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-18 15:51:54
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-18 16:00:32
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
        // cv::Mat hog_descriptors;
        cv::resize(gray_img, gray_img, train_size_);
        cv::Mat roi_img;
        // vl_hog_put_image expects a float* (values 0.0f-255.0f)
        gray_img.convertTo(roi_img, CV_32FC1);
        VlHog* hog = vl_hog_new(VlHogVariant::VlHogVariantUoctti, num_bins_, VL_FALSE);
        // (the '1' is numChannels)
        vl_hog_put_image(hog, (float*)roi_img.data, roi_img.cols, roi_img.rows, 1, cell_size_);
        int     ww = static_cast<int>(vl_hog_get_width(hog));   // assert ww == hh == numCells
        int     hh = static_cast<int>(vl_hog_get_height(hog));
        int     dd = static_cast<int>(vl_hog_get_dimension(hog));   // assert ww=hogDim1, hh=hogDim2, dd=hogDim3
        cv::Mat hogArray(1, ww * hh * dd, CV_32FC1);                // safer & same result. Don't use C-style memory management.
        vl_hog_extract(hog, hogArray.ptr<float>(0));
        vl_hog_delete(hog);
        // 高，宽
        cv::Mat hogDescriptor(hh * ww * dd, 1, CV_32FC1);
        // Stack the third dimensions of the HOG descriptor of this patch one after each other in a column-vector:
        for (int j = 0; j < dd; ++j) {
            // Creates the same array as in Matlab. I might have to check this again if hh!=ww (non-square)
            cv::Mat hogFeatures(hh, ww, CV_32FC1, hogArray.ptr<float>(0) + j * ww * hh);
            // necessary because the Matlab reshape() takes column-wise from the matrix while the OpenCV reshape() takes row-wise.
            hogFeatures = hogFeatures.t();
            // make it to a column-vector
            hogFeatures              = hogFeatures.reshape(0, hh * ww);
            cv::Mat currentDimSubMat = hogDescriptor.rowRange(j * ww * hh, j * ww * hh + ww * hh);
            hogFeatures.copyTo(currentDimSubMat);
        }
        hogDescriptor = hogDescriptor.t();   // now a row-vector
        hog_descriptors_.push_back(hogDescriptor);
    }
    // concatenate all the descriptors for this sample vertically (into a row-vector):
    //_hog_descriptors = _hog_descriptors.reshape(0, _hog_descriptors.cols).t();
    hog_descriptors_ = hog_descriptors_.reshape(0, hog_descriptors_.cols);
    hog_descriptors_ = hog_descriptors_.t();
    return hog_descriptors_.clone();
};

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END