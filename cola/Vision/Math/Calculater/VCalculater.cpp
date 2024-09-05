/**
 * @FilePath     : /cola/cola/Vision/Math/Calculater/VCalculater.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-15 15:02:27
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-08-19 15:33:51
 **/
#include "VCalculater.h"

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

NDouble VCalculater::distance_2d_p2p(const cv::Point& a, const cv::Point& b)
{
    return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2));
}

NDouble VCalculater::distance_2d_p2l(cv::Point pointP, cv::Point pointA, cv::Point pointB)
{
    // 求直线方程
    NInt A = 0, B = 0, C = 0;
    A = pointA.y - pointB.y;
    B = pointB.x - pointA.x;
    C = pointA.x * pointB.y - pointA.y * pointB.x;
    // 代入点到直线距离公式
    NFloat distance = ((NFloat)abs(A * pointP.x + B * pointP.y + C)) / ((NFloat)sqrtf(A * A + B * B));
    return distance;
}

NDouble VCalculater::coordinate_2d_y2x(cv::Point line_p1, cv::Point line_p2, NDouble y)
{
    NDouble x1 = line_p1.x, y1 = line_p1.y, x2 = line_p2.x, y2 = line_p2.y;
    NDouble x = (y - y1) * (x2 - x1) / (y2 - y1) + x1;
    return x;
}

NDouble VCalculater::coordinate_2d_x2y(cv::Point line_p1, cv::Point line_p2, NDouble x)
{
    NDouble x1 = line_p1.x, y1 = line_p1.y, x2 = line_p2.x, y2 = line_p2.y;
    NDouble y = (x - x1) * (y2 - y1) / (x2 - x1) + y1;
    return y;
}

cv::Point2f VCalculater::get_lines_kb(cv::Point line_p1, cv::Point line_p2)
{
    NFloat k = 0;   // 直线斜率
    NFloat b = 0;   // 直线截距

    NDouble x_diff = 0;
    if (abs(line_p1.x - line_p2.x) < 2) {
        x_diff = abs(line_p1.x - line_p2.x);
    }
    k = (NDouble)(line_p1.y - line_p2.y) / (NDouble)(x_diff);   // 求出直线的斜率// -3.1415926/2-----+3.1415926/2
    b = (NDouble)line_p2.y - k * (NDouble)line_p2.x;
    cv::Point2f pt(k, b);
    return pt;
}

cv::Point2d VCalculater::cross_2d_l2l(cv::Point lineOnePt1, cv::Point lineOnePt2, cv::Point lineTwoPt1, cv::Point lineTwoPt2)
{
    NDouble x1 = lineOnePt1.x, y1 = lineOnePt1.y, x2 = lineOnePt2.x, y2 = lineOnePt2.y;
    NDouble a1 = -(y2 - y1), b1 = x2 - x1, c1 = (y2 - y1) * x1 - (x2 - x1) * y1;   // 一般式：a1x+b1y1+c1=0
    NDouble x3 = lineTwoPt1.x, y3 = lineTwoPt1.y, x4 = lineTwoPt2.x, y4 = lineTwoPt2.y;
    NDouble a2 = -(y4 - y3), b2 = x4 - x3, c2 = (y4 - y3) * x3 - (x4 - x3) * y3;   // 一般式：a2x+b2y1+c2=0
    bool   r  = false;                                                            // 判断结果
    NDouble x0 = 0, y0 = 0;                                                        // 交点
    NDouble angle = 0;                                                             // 夹角

    cv::Point2d result(-1, -1);
    // 判断相交
    if (b1 == 0 && b2 != 0)   // l1垂直于x轴，l2倾斜于x轴
        r = true;
    else if (b1 != 0 && b2 == 0)   // l1倾斜于x轴，l2垂直于x轴
        r = true;
    else if (b1 != 0 && b2 != 0 && a1 / b1 != a2 / b2)
        r = true;

    if (r) {
        // 计算交点
        x0 = (b1 * c2 - b2 * c1) / (a1 * b2 - a2 * b1);
        y0 = (a1 * c2 - a2 * c1) / (a2 * b1 - a1 * b2);
        // 计算夹角
        NDouble a = sqrt(pow(x4 - x2, 2) + pow(y4 - y2, 2));
        NDouble b = sqrt(pow(x4 - x0, 2) + pow(y4 - y0, 2));
        NDouble c = sqrt(pow(x2 - x0, 2) + pow(y2 - y0, 2));
        angle    = acos((b * b + c * c - a * a) / (2 * b * c)) * 180 / CV_PI;
    }
    result.x = x0;
    result.y = y0;
    return result;
}

cv::Point2d VCalculater::cross_2d_p2l(cv::Point line_pt1, cv::Point line_pt2, cv::Point src_pt)
{
    cv::Point2f root_pt(0, 0);
    if (line_pt1.x == line_pt2.x) {
        // 线与x轴垂直
        root_pt.x = line_pt1.x;
        root_pt.y = src_pt.y;
    }
    else if (line_pt1.y == line_pt2.y) {
        // 线与Y轴垂直
        root_pt.x = src_pt.x;
        root_pt.y = line_pt1.y;
    }
    else {
        // 线与 x轴 y轴 都不垂直
        NDouble a1 = -(line_pt2.y - line_pt1.y);
        NDouble b1 = (line_pt2.x - line_pt1.x);
        NDouble c1 = (line_pt2.y - line_pt1.y) * line_pt1.x - (line_pt2.x - line_pt1.x) * line_pt1.y;

        root_pt.x = (b1 * b1 * src_pt.x - a1 * b1 * src_pt.y - a1 * c1) / (a1 * a1 + b1 * b1);
        root_pt.y = (a1 * a1 * src_pt.y - a1 * b1 * src_pt.x - b1 * c1) / (a1 * a1 + b1 * b1);
    }
    return root_pt;
}

/*
行列式求解
std::vector<cv::Point2d> pts;
pts.emplace_back(0   ,4079);
pts.emplace_back(351 , 4079);
pts.emplace_back(0 , 3125);
pts.emplace_back(306.509827, 4028.913330);


area(pts[0], pts[1],pts[2]);
area(pts[0], pts[1], pts[3]);
area(pts[0], pts[2], pts[3]);
area(pts[3], pts[1], pts[2]);

cv::Mat m1 = (cv::Mat_<NDouble>(3, 3) << 0.000000, 4079.000000,1, 351.000000, 4079.000000, 1,0.000000, 3125.000000,1);
cv::Mat m2 = (cv::Mat_<NDouble>(3, 3) << 0.000000, 4079.000000, 1,351.000000, 4079.000000, 1,306.509827, 4028.913330,1);
cv::Mat m3 = (cv::Mat_<NDouble>(3, 3) << 0.000000, 4079.000000,1, 0.000000, 3125.000000,  1, 306.509827, 4028.913330,1);
cv::Mat m4 = (cv::Mat_<NDouble>(3, 3) << 351.000000, 4079.000000,1, 0.000000, 3125.000000,1, 306.509827, 4028.913330,1);

NDouble s1 = 0.5 * std::abs(cv::determinant(m1));
NDouble s2 = 0.5 * std::abs(cv::determinant(m2));
NDouble s3 = 0.5 * std::abs(cv::determinant(m3));
NDouble s4 = 0.5 * std::abs(cv::determinant(m4));
*/
NDouble VCalculater::area_triangle(cv::Point2d p1, cv::Point2d p2, cv::Point2d p3)
{
    return std::abs((p1.x * p2.y + p2.x * p3.y + p3.x * p1.y - p1.x * p3.y - p2.x * p1.y - p3.x * p2.y) / 2);
}

cv::Vec2d VCalculater::get_polar_line(cv::Vec4d p)
{
    if (fabs(p[0] - p[2]) < 1e-5) {   // 垂直直线
        if (p[0] > 0)
            return cv::Vec2d(fabs(p[0]), 0);
        else
            return cv::Vec2d(fabs(p[0]), CV_PI);
    }
    if (fabs(p[1] - p[3]) < 1e-5) {   // 水平直线
        if (p[1] > 1e-5)
            return cv::Vec2d(fabs(p[1]), CV_PI / 2);
        else
            return cv::Vec2d(fabs(p[1]), 3 * CV_PI / 2);
    }
    NFloat k           = (p[1] - p[3]) / (p[0] - p[2]);
    NFloat y_intercept = p[1] - k * p[0];
    NFloat theta;                    /*atan 值域范围[-pi/2,pi/2]; atan2 值域范围[-pi,pi],根据直线斜率与截距 判断角度所在象限*/
    if (k < 0 && y_intercept > 0)   // 第一象限
        theta = atan(-1 / k);
    else if (k > 0 && y_intercept > 0)   // 第二象限，
        theta = CV_PI + atan(-1 / k);
    else if (k < 0 && y_intercept < 0)   // 第三象限
        theta = CV_PI + atan(-1 / k);
    else if (k > 0 && y_intercept < 0)   // 第四象限
        theta = 2 * CV_PI + atan(-1 / k);
    NFloat _cos = cos(theta);
    NFloat _sin = sin(theta);
    NFloat r    = fabs(p[0] * _cos + p[1] * _sin);
    return cv::Vec2d(r, theta);
}

NDouble VCalculater::angle_2d_l2l(cv::Point p1, cv::Point p2, cv::Point p3, cv::Point p4)
{
    NDouble dx1           = p1.x - p2.x;
    NDouble dy1           = p1.y - p2.y;
    NDouble dx2           = p3.x - p4.x;
    NDouble dy2           = p3.y - p4.y;
    NDouble m1            = dy1 / dx1;
    NDouble m2            = dy2 / dx2;
    NDouble in_side_angle = std::atan(std::abs((m2 - m1) / (1 + (m1 * m2))));
    NDouble angle         = in_side_angle / CV_PI * 180;
    if (angle > -370 and angle < 370) {
        angle = NInt(angle);
    }
    return angle;
}

/**
 * @brief 判断p3是否在 p1,p2所组成的线上。
 * @param p1
 * @param p2
 * @param p3
 * @return 在线上返回1，在线外返回0
 */
NInt VCalculater::on_line(const cv::Point p1, const cv::Point p2, const cv::Point p3)
{
    if ((p3.x - p1.x) * (p2.y - p1.y) == (p2.y - p1.x) * (p3.y - p1.y) && std::min(p1.x, p2.x) <= p3.x && p3.x <= std::max(p1.x, p2.x) && std::min(p1.y, p2.y) <= p3.y &&
        p3.y <= std::max(p1.y, p2.y)) {
        return 1;
    }
    else {
        return 0;
    }
}

NInt VCalculater::in_polygon(NInt pointCoint, std::vector<cv::Point> ptVec, const cv::Point p)
{
    // 首先判断是否在多边形外部，左上，左下，右上，右下
    std::vector<NDouble> x_value, y_value;
    for (NInt i = 0; i < ptVec.size(); i++) {
        x_value.emplace_back(ptVec[i].x);
        y_value.emplace_back(ptVec[i].y);
    }
    auto   pair_x = std::minmax_element(x_value.begin(), x_value.end());
    auto   pair_y = std::minmax_element(y_value.begin(), y_value.end());
    NDouble min_x  = *pair_x.first;
    NDouble max_x  = *pair_x.second;
    NDouble min_y  = *pair_y.first;
    NDouble max_y  = *pair_y.second;
    if (p.x < min_x || p.x > max_x || p.y > max_y || p.y < min_y) {
        return 0;
    }
    //
    NInt i;
    NInt j;
    NInt c = 0;
    for (NInt i = 0, j = pointCoint - 1; i < pointCoint; j = i++) {
        // 判断是否在线段上
        if (on_line(ptVec[i], ptVec[j], p))
            return 1;
        if (((ptVec[i].y > p.y) != (ptVec[j].y > p.y)) && (p.x < (ptVec[j].x - ptVec[i].x) * (p.y - ptVec[i].y) / (ptVec[j].y - ptVec[i].y + ptVec[i].x))) {
            c = !c;
        }
    }
    return c;
}

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END