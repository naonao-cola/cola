/**
 * @FilePath     : /cola/cola/Vision/Math/Calculater/VCalculater.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-15 15:02:06
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-15 16:29:59
 **/
#ifndef NAO_VCALCULATER_H
#define NAO_VCALCULATER_H
#include "../../VisionObject.h"

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

class VCalculater : public VisionObject
{
public:
    /**
     * @brief: 点到点的距离
     * @param a
     * @param b
     * @return
     * @note :
     **/
    static double distance_2d_p2p(const cv::Point& a, const cv::Point& b);

    /**
     * @brief: 点到直线的距离
     * @param pointP
     * @param pointA
     * @param pointB
     * @return
     * @note :
     **/
    static double distance_2d_p2l(cv::Point pointP, cv::Point pointA, cv::Point pointB);

    /**
     * @brief: 已知一条直线，与一点的y，求x
     * @param line_p1
     * @param line_p2
     * @param y
     * @return
     * @note :
     **/
    static double coordinate_2d_y2x(cv::Point line_p1, cv::Point line_p2, double y);

    /**
     * @brief: 已知一条直线，与一点的x，求y
     * @param line_p1
     * @param line_p2
     * @param x
     * @return
     * @note :
     **/
    static double coordinate_2d_x2y(cv::Point line_p1, cv::Point line_p2, double x);

    /**
     * @brief:  获取直线的 斜率与截距
     * @param ll
     * @return
     * @note :
     **/
    static cv::Point2f get_lines_kb(cv::Point line_p1, cv::Point line_p2);
    /**
     * @brief:  线与线的交点
     * @param lineOnePt1
     * @param lineOnePt2
     * @param lineTwoPt1
     * @param lineTwoPt2
     * @return
     * @note :
     **/
    static cv::Point2d cross_2d_l2l(cv::Point lineOnePt1, cv::Point lineOnePt2, cv::Point lineTwoPt1, cv::Point lineTwoPt2);

    /**
     * @brief: 点到线的垂足
     * @param line_pt1
     * @param line_pt2
     * @param src_pt
     * @return
     * @note :
     **/
    static cv::Point2d cross_2d_p2l(cv::Point line_pt1, cv::Point line_pt2, cv::Point src_pt);

    /**
     * @brief:  将线段转化为极坐标
     * @param p
     * @return
     * @note : cv::Vec2d polarline = getPolarLine(cv::Vec4d(x1 - width / 2.0, height / 2.0 - y1, x2 - width / 2.0, height / 2.0 - y2));
     **/
    static cv::Vec2d get_polar_line(cv::Vec4d p);
};
NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END
#endif   // NAO_VCALCULATER_H