/**
 * @FilePath     : /cola/cola/Vision/Math/Geometry/VTransform.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-16 09:45:29
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-16 09:59:23
 **/
#include "VTransform.h"

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

void VTransform::flip(const cv::Mat src, cv::Mat& dst, int flip_type)
{
    cv::Mat ret;
    cv::flip(src, ret, flip_type);
    dst = ret.clone();
}

cv::Mat VTransform::cvMat6_to_cvMat9(const cv::Mat& mtx6)
{
    cv::Mat       mtx9(3, 3, CV_64FC1);
    double*       M9 = mtx9.ptr<double>();
    const double* M6 = mtx6.ptr<double>();
    M9[0]            = M6[0];
    M9[1]            = M6[1];
    M9[2]            = M6[2];
    M9[3]            = M6[3];
    M9[4]            = M6[4];
    M9[5]            = M6[5];
    M9[6]            = 0.0;
    M9[7]            = 0.0;
    M9[8]            = 1.0;
    return mtx9;
}

cv::Mat VTransform::d6_to_cvMat(double d0, double d1, double d2, double d3, double d4, double d5)
{
    cv::Mat mtx(3, 3, CV_64FC1);
    double* M = mtx.ptr<double>();
    M[0]      = d0;
    M[1]      = d1;
    M[2]      = d2;
    M[3]      = d3;
    M[4]      = d4;
    M[5]      = d5;
    M[6]      = 0.0;
    M[7]      = 0.0;
    M[8]      = 1.0;
    return mtx;
}


cv::Mat VTransform::vector_angle_to_M(double x1, double y1, double d1, double x2, double y2, double d2)
{
    cv::Point2f center(x1, y1);
    double      angle = d2 - d1;
    cv::Mat     rot_M = cv::getRotationMatrix2D(center, angle, 1.0);
    rot_M             = cvMat6_to_cvMat9(rot_M);

    cv::Mat trans_M = d6_to_cvMat(1, 0, x2 - x1, 0, 1, y2 - y1);
    cv::Mat M       = trans_M * rot_M;   // 先旋转在平移（矩阵乘法相反）
    return M;
}

cv::Point2f VTransform::TransPoint(const cv::Mat& M, const cv::Point2f& point)
{
    std::vector<double> values = {point.x, point.y};
    cv::Mat             mat    = cv::Mat(values).clone();   // 将vector变成单列的mat，这里需要clone(),因为这里的赋值操作是浅拷贝
    cv::Mat             dest   = mat.reshape(1, 1);

    cv::Mat homogeneousPoint = (cv::Mat_<double>(3, 1) << point.x, point.y, 1.0);
    cv::Mat transformed      = M * homogeneousPoint;
    return cv::Point2f(transformed.at<double>(0, 0), transformed.at<double>(0, 1));
}


NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END