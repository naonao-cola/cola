/**
 * @FilePath     : /cola/cola/Vision/Math/Calculater/VCalculater.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-15 15:02:27
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-15 16:14:36
 **/
#include "VCalculater.h"

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

double VCalculater::distance_2d_p2p(const cv::Point& a, const cv::Point& b)
{
    return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2));
}
double VCalculater::distance_2d_p2l(cv::Point pointP, cv::Point pointA, cv::Point pointB)
{
    // 求直线方程
    int A = 0, B = 0, C = 0;
    A = pointA.y - pointB.y;
    B = pointB.x - pointA.x;
    C = pointA.x * pointB.y - pointA.y * pointB.x;
    // 代入点到直线距离公式
    float distance = ((float)abs(A * pointP.x + B * pointP.y + C)) / ((float)sqrtf(A * A + B * B));
    return distance;
}

double VCalculater::coordinate_2d_y2x(cv::Point line_p1, cv::Point line_p2, double y)
{
    double x1 = line_p1.x, y1 = line_p1.y, x2 = line_p2.x, y2 = line_p2.y;
    double x = (y - y1) * (x2 - x1) / (y2 - y1) + x1;
    return x;
}

double VCalculater::coordinate_2d_x2y(cv::Point line_p1, cv::Point line_p2, double x)
{
    double x1 = line_p1.x, y1 = line_p1.y, x2 = line_p2.x, y2 = line_p2.y;
    double y = (x - x1) * (y2 - y1) / (x2 - x1) + y1;
    return y;
}

cv::Point2f VCalculater::get_lines_kb(cv::Point line_p1, cv::Point line_p2)
{
    float k = 0;   // 直线斜率
    float b = 0;   // 直线截距

    double x_diff = 0;
    if (abs(line_p1.x - line_p2.x) < 2) {
        x_diff = abs(line_p1.x - line_p2.x);
    }
    k = (double)(line_p1.y - line_p2.y) / (double)(x_diff);   // 求出直线的斜率// -3.1415926/2-----+3.1415926/2
    b = (double)line_p2.y - k * (double)line_p2.x;
    cv::Point2f pt(k, b);
    return pt;
}

cv::Point2d VCalculater::cross_2d_l2l(cv::Point lineOnePt1, cv::Point lineOnePt2, cv::Point lineTwoPt1, cv::Point lineTwoPt2)
{
    double x1 = lineOnePt1.x, y1 = lineOnePt1.y, x2 = lineOnePt2.x, y2 = lineOnePt2.y;
    double a1 = -(y2 - y1), b1 = x2 - x1, c1 = (y2 - y1) * x1 - (x2 - x1) * y1;   // 一般式：a1x+b1y1+c1=0
    double x3 = lineTwoPt1.x, y3 = lineTwoPt1.y, x4 = lineTwoPt2.x, y4 = lineTwoPt2.y;
    double a2 = -(y4 - y3), b2 = x4 - x3, c2 = (y4 - y3) * x3 - (x4 - x3) * y3;   // 一般式：a2x+b2y1+c2=0
    bool   r  = false;                                                            // 判断结果
    double x0 = 0, y0 = 0;                                                        // 交点
    double angle = 0;                                                             // 夹角

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
        double a = sqrt(pow(x4 - x2, 2) + pow(y4 - y2, 2));
        double b = sqrt(pow(x4 - x0, 2) + pow(y4 - y0, 2));
        double c = sqrt(pow(x2 - x0, 2) + pow(y2 - y0, 2));
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
        double a1 = -(line_pt2.y - line_pt1.y);
        double b1 = (line_pt2.x - line_pt1.x);
        double c1 = (line_pt2.y - line_pt1.y) * line_pt1.x - (line_pt2.x - line_pt1.x) * line_pt1.y;

        root_pt.x = (b1 * b1 * src_pt.x - a1 * b1 * src_pt.y - a1 * c1) / (a1 * a1 + b1 * b1);
        root_pt.y = (a1 * a1 * src_pt.y - a1 * b1 * src_pt.x - b1 * c1) / (a1 * a1 + b1 * b1);
    }
    return root_pt;
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
    float k           = (p[1] - p[3]) / (p[0] - p[2]);
    float y_intercept = p[1] - k * p[0];
    float theta;                    /*atan 值域范围[-pi/2,pi/2]; atan2 值域范围[-pi,pi],根据直线斜率与截距 判断角度所在象限*/
    if (k < 0 && y_intercept > 0)   // 第一象限
        theta = atan(-1 / k);
    else if (k > 0 && y_intercept > 0)   // 第二象限，
        theta = CV_PI + atan(-1 / k);
    else if (k < 0 && y_intercept < 0)   // 第三象限
        theta = CV_PI + atan(-1 / k);
    else if (k > 0 && y_intercept < 0)   // 第四象限
        theta = 2 * CV_PI + atan(-1 / k);
    float _cos = cos(theta);
    float _sin = sin(theta);
    float r    = fabs(p[0] * _cos + p[1] * _sin);
    return cv::Vec2d(r, theta);
}

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END