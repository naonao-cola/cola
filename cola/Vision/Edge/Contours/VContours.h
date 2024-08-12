/**
 * @FilePath     : /cola/cola/Vision/Edge/Contours/VContours.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-08-09 11:59:03
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-08-12 10:08:16
 * @Copyright (c) 2024 by G, All Rights Reserved.
 **/
#ifndef NAONAO_VCONTOURS_H
#define NAONAO_VCONTOURS_H

#include "../../VisionObject.h"
#include "Clipper/clipper.h"
NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

class VContours : public VisionObject
{
public:
    VContours()=default;
    ~VContours() override=default;

    // 寻找轮廓
    static std::vector<std::vector<cv::Point>> get_contours(const cv::Mat& src);

    /**
     * @brief: 筛选轮廓
     * @return
     * @note :
     * @code
        cv::Mat in_img = cv::imread(input_path, 0);
        std::vector<std::vector<cv::Point>> contours, out_contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours(in_img, contours, hierarchy, cv::RETR_EXTERNAL,
                        cv::CHAIN_APPROX_NONE);
        std::function func = [&](const cv::Rect &rect,
                                const cv::RotatedRect &rotated_rect,
                                const std::vector<cv::Point> &cur_contours) {
        std::cout << rect.width << rect.height << std::endl;
        if (rect.height > 1000) {
            out_contours.push_back(contours[0]);
        }
        };
        cv::Mat out_img;
        select_shape(in_img, out_img, contours, out_contours, func);
    **/
    template<typename callback>
    void select_shape(const cv::Mat& in_img, cv::Mat& out_img, const std::vector<std::vector<cv::Point>>& in_contours, std::vector<std::vector<cv::Point>>& out_contours, callback callback_func)
    {
        for (size_t i = 0; i < in_contours.size(); ++i) {
            cv::RotatedRect rot_rect = cv::minAreaRect(in_contours[i]);
            cv::Rect        rect     = cv::boundingRect(in_contours[i]);
            callback_func(rect, rot_rect, in_contours[i]);
        }
    }

    /**
     * @brief:  轮廓交集
     * @return
     * @note :
     **/
    static std::vector<cv::Point> contours_and(const std::vector<cv::Point>& lcontour, const std::vector<cv::Point>& rcontour);

    /**
     * @brief: 轮廓并集
     * @return
     * @note :
     **/
    static std::vector<cv::Point> contours_or(const std::vector<cv::Point>& lcontour, const std::vector<cv::Point>& rcontour);

    /**
     * @brief: 轮廓差集
     * @return
     * @note :
     **/
    static std::vector<cv::Point> contours_not(const std::vector<cv::Point>& lcontour, const std::vector<cv::Point>& rcontour);

    /**
     * @brief: 轮廓异或，获取两个区域互不重复的区域
     * @return
     * @note :
     **/
    static std::vector<cv::Point> contours_xor(const std::vector<cv::Point>& lcontour, const std::vector<cv::Point>& rcontour);

    static std::vector<cv::Point> contours_offset(const std::vector<cv::Point>& contour, double off_value);

    static int PointInPolygon(const std::vector<cv::Point>& contour, const cv::Point& point);

    static double contours_area(const std::vector<cv::Point>& contour);

    static ClipperLib::Path                    contour2path(const std::vector<cv::Point>& contour);
    static ClipperLib::Paths                   contours2paths(const std::vector<std::vector<cv::Point>>& contours);
    static std::vector<cv::Point>              path2contour(const ClipperLib::Path& path);
    static std::vector<std::vector<cv::Point>> path2contours(const ClipperLib::Paths& paths);
};

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END

#endif   // NAONAO_VCONTOURS_H