/*
 * @FilePath     : /cola/cola/Vision/Math/Geometry/VTransform.inl
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-16 09:47:07
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-20 12:17:51
 */
#ifndef NAONAO_VTRANSFORM_INL
#define NAONAO_VTRANSFORM_INL

#include "VTransform.h"

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

template<typename T>
std::vector<T> VTransform::order_pts(const std::vector<T> src_pt)
{
    std::vector<T> tmp, dst;
    if (src_pt.size() != 4) {
        std::cerr << "输入的原点个数错误" << std::endl;
        return dst;
    }
    tmp.assign(src_pt.begin(), src_pt.end());
    // 按照x值大小升序排列，x值小的两个点位左侧的两个点
    std::sort(tmp.begin(), tmp.end(), [=](const T pt1, const T pt2) { return pt1.x < pt2.x; });
    if (tmp[0].y > tmp[1].y) {
        if (tmp[2].y > tmp[3].y) {
            dst.emplace_back(tmp[1]);
            dst.emplace_back(tmp[3]);
            dst.emplace_back(tmp[2]);
            dst.emplace_back(tmp[0]);
        }
        else {
            dst.emplace_back(tmp[1]);
            dst.emplace_back(tmp[2]);
            dst.emplace_back(tmp[3]);
            dst.emplace_back(tmp[0]);
        }
    }
    else {
        if (tmp[2].y > tmp[3].y) {
            dst.emplace_back(tmp[0]);
            dst.emplace_back(tmp[3]);
            dst.emplace_back(tmp[2]);
            dst.emplace_back(tmp[1]);
        }
        else {
            dst.emplace_back(tmp[0]);
            dst.emplace_back(tmp[2]);
            dst.emplace_back(tmp[3]);
            dst.emplace_back(tmp[1]);
        }
    }
    return dst;
}

template<typename T>
cv::Mat VTransform::get_perspective(const std::vector<T> src_points, const std::vector<T> dst_points)
{
    if (src_points.size() != 4 || dst_points.size() != 4) {
        return cv::Mat();
    }
    // 排序赋值
    std::vector<T> src_tmp = order_pts(src_points);
    std::vector<T> dst_tmp = order_pts(dst_points);
    T              src[4];
    T              dst[4];
    for (int i = 0; i < src_tmp.size(); i++) {
        src[i] = T(src_tmp[i].x, src_tmp[i].y);
    }
    for (int i = 0; i < dst_tmp.size(); i++) {
        dst[i] = T(dst_tmp[i].x, dst_tmp[i].y);
    }
    return cv::getPerspectiveTransform(src, dst).clone();
}


template<typename T>
cv::Mat VTransform::get_perspective(const std::vector<T> pts)
{
    if (pts.size() != 4) {
        return cv::Mat();
    }
    // 排序
    std::vector<T> tmp_pt = order_pts(pts);
    T              src_pts[4], dst_pts[4];
    // 赋值
    for (int i = 0; i < tmp_pt.size(); i++)
        src_pts[i] = T(tmp_pt[i].x, tmp_pt[i].y);
    dst_pts[0] = T(tmp_pt[0].x, tmp_pt[0].y);
    dst_pts[1] = T(tmp_pt[2].x, tmp_pt[0].y);
    dst_pts[2] = T(tmp_pt[2].x, tmp_pt[2].y);
    dst_pts[3] = T(tmp_pt[0].x, tmp_pt[2].y);
    return cv::getPerspectiveTransform(src_pts, dst_pts);

    // 透视变换函数 cv::warpPerspective(,);
}

template<typename T>
cv::Mat VTransform::get_perspective_homo(const std::vector<T> src_points, const std::vector<T> dst_points)
{
    if (src_points.size() != dst_points.size()) {
        return cv::Mat();
    }
    return cv::findHomography(src_points, dst_points, 0);

    // 透视变换函数 cv::warpPerspective(,);
}

template<typename T>
T VTransform::perspective_point(const T src_point, const cv::Mat wrap_mat)
{
    cv::Mat_<double> pt(3, 1);
    pt(0, 0)    = src_point.x;
    pt(0, 1)    = src_point.y;
    pt(0, 2)    = 1;
    cv::Mat ret = wrap_mat * pt;
    double  a1  = ret.at<double>(0, 0);
    double  a2  = ret.at<double>(1, 0);
    double  a3  = ret.at<double>(2, 0);
    T       dst_point;
    dst_point.x = static_cast<double>(a1 / a3);
    dst_point.y = static_cast<double>(a2 / a3);
    return dst_point;
}

template<typename T>
T VTransform::perspective_inv_point(const T& src_point, const cv::Mat& wrap_mat)
{
    cv::Mat M_inv;
    cv::invert(wrap_mat, M_inv, cv::DECOMP_SVD);
    cv::Mat_<double> pt(3, 1);
    pt(0, 0)    = src_point.x;
    pt(0, 1)    = src_point.y;
    pt(0, 2)    = 1;
    cv::Mat ret = M_inv * pt;
    double  a1  = ret.at<double>(0, 0);
    double  a2  = ret.at<double>(1, 0);
    double  a3  = ret.at<double>(2, 0);
    T       dst_point;
    dst_point.x = static_cast<double>(a1 / a3);
    dst_point.y = static_cast<double>(a2 / a3);
    return dst_point;
}


template<typename T>
cv::Mat VTransform::affine_img_rotate(const cv::Mat src, double angle, T center, double scale)
{
    cv::Mat ret;
    cv::Mat m = cv::getRotationMatrix2D(center, angle, scale);
    cv::warpAffine(src, ret, m, src.size(), cv::INTER_LINEAR + cv::WARP_FILL_OUTLIERS);
    return ret;
}


template<typename T>
cv::Mat VTransform::affine_img_move(const cv::Mat src, double xoffset, double yoffset)
{
    T pt1[3], pt2[3];
    // 平移前的位置
    pt1[0] = T(0, 0);
    pt1[1] = T(xoffset, 0);
    pt1[2] = T(0, yoffset);
    // 平移后的位置
    pt2[0] = T(xoffset, yoffset);
    pt2[1] = T(2 * xoffset, yoffset);
    pt2[2] = T(xoffset, 2 * yoffset);
    // 平移矩阵
    cv::Mat m, ret;
    m = cv::getAffineTransform(pt1, pt2);
    cv::warpAffine(src, ret, m, cv::Size(src.cols + xoffset, src.rows + yoffset));
    return ret;
}

template<typename T>
cv::Mat VTransform::affine_img_affine(const cv::Mat src, const T src_points[], const T dst_points[])
{
    cv::Mat m = cv::getAffineTransform(src_points, dst_points);
    cv::Mat ret;
    cv::warpAffine(src, ret, m, src.size());
    return ret;
}

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END

#endif