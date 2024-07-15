/**
 * @FilePath     : /cola/cola/Vision/Math/Geometry/VTransform.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-15 15:01:10
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-15 17:21:39
 **/
#ifndef NAO_VTRANSFORM_H
#define NAO_VTRANSFORM_H
#include "../../VisionObject.h"

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

class VTransform : public VisionObject
{
public:
    /**
     * @brief 点排序，按照顺时针方向排序，适用于4个点，不适用于多点
     * @param src_pt，输入的原点
     * @return 排序后的点,顺时针排序的点，第一个点为左上角的点
     */
    template<typename T>
    std::vector<T> order_pts(const std::vector<T> src_pt)
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

    /**
     * @brief 4点透视变换矩阵，原点与透视点需要一一对应。从实际图像变换到正视图
     * @param src_points 原点
     * @param dst_points 透视后的点，
     * @param wrap_mat   得到的透视矩阵
     */
    template<typename T>
    void perspective(const std::vector<T> src_points, const std::vector<T> dst_points, cv::Mat& wrap_mat)
    {
        if (src_points.size() != 4 || dst_points.size() != 4)
            return;
        // 排序赋值
        std::vector<T> src_tmp = order_pts(src_points);
        std::vector<T> dst_tmp = order_pts(dst_points);
        T              src[4], dst[4];
        for (int i = 0; i < src_tmp.size(); i++)
            src[i] = T(src_tmp[i].x, src_tmp[i].y);
        for (int i = 0; i < dst_tmp.size(); i++)
            dst[i] = T(dst_tmp[i].x, dst_tmp[i].y);
        wrap_mat = cv::getPerspectiveTransform(src, dst).clone();
    }
    /**
     * @brief    4点透视变换，不需要提供目标点，原点必须是矩形的4个角点，从实际图像变换到正视图
     * @param pts 原点
     * @param wrap_mat 透视矩阵
     */
    template<typename T>
    void perspective(const std::vector<T> pts, cv::Mat& wrap_mat)
    {
        if (pts.size() != 4)
            return;
        // 排序
        std::vector<T> tmp_pt = order_pts(pts);
        T              src_pts[4], dst_pts[4];
        // 赋值
        for (int i = 0; i < tmp_pt.size(); i++)
            src_pts[i] = T(tmp_pt[i].x, tmp_pt[i].y);
        dst_pts[0]  = T(tmp_pt[0].x, tmp_pt[0].y);
        dst_pts[1]  = T(tmp_pt[2].x, tmp_pt[0].y);
        dst_pts[2]  = T(tmp_pt[2].x, tmp_pt[2].y);
        dst_pts[3]  = T(tmp_pt[0].x, tmp_pt[2].y);
        cv::Mat ret = cv::getPerspectiveTransform(src_pts, dst_pts);
        wrap_mat    = ret.clone();
        // 透视变换函数 cv::warpPerspective(,);
    }
    /**
     * @brief   最小二乘法透视矩阵，不限制点的个数，要求一一对应。
     * @tparam T
     * @param src_points
     * @param dst_points
     * @return
     */
    template<typename T>
    cv::Mat perspective(const std::vector<T> src_points, const std::vector<T> dst_points)
    {
        if (src_points.size() != dst_points.size()) {
            return cv::Mat();
}
        cv::Mat ret = cv::findHomography(src_points, dst_points, 0);
        return ret;
        // 透视变换函数 cv::warpPerspective(,);
    }

    /**
     * @brief 点变换，将实际图像上的点通过透视矩阵，求得到正视图上对应的点
     * @param src_point 原点
     * @param wrap_mat 透视矩阵
     * @param dst_point 输出点
     * @refer https://www.cnblogs.com/yanghailin/p/12510318.html
     */
    template<typename T>
    void point_transform(const T src_point, const cv::Mat wrap_mat, T& dst_point)
    {
        cv::Mat_<double> pt(3, 1);
        pt(0, 0)    = src_point.x;
        pt(0, 1)    = src_point.y;
        pt(0, 2)    = 1;
        cv::Mat ret = wrap_mat * pt;
        double  a1  = ret.at<double>(0, 0);
        double  a2  = ret.at<double>(1, 0);
        double  a3  = ret.at<double>(2, 0);
        dst_point.x = static_cast<double>(a1 / a3);
        dst_point.y = static_cast<double>(a2 / a3);
    }

    /**
     * @brief 点反变换，根据透视矩阵反变换，将目标图的点反变换到原图
     * @tparam T
     * @param src_point
     * @param wrap_mat
     * @param dst_point
     */
    template<typename T>
    void point_inv_transform(const T& src_point, const cv::Mat& wrap_mat, T& dst_point)
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
        dst_point.x = static_cast<double>(a1 / a3);
        dst_point.y = static_cast<double>(a2 / a3);
    }


    /*------------------------------------------仿射变换--------------------------------------------------------*/
    /**
     * @brief 图像翻转
     * @param src  原图
     * @param dst  目标图
     * @param flip_type 翻转类型，0，表示上下翻转。1，表示左右翻转。-1表示对角线翻转。
     */
    void flip(const cv::Mat src, cv::Mat& dst, int flip_type)
    {
        cv::Mat ret;
        cv::flip(src, ret, flip_type);
        dst = ret.clone();
    }
    /**
     * @brief  图像旋转
     * @param src   原图
     * @param dst   目标图像
     * @param angle 逆时针旋转角度
     * @param center 旋转中心
     * @param scale  缩放比例
     */
    template<typename T>
    void rotate(const cv::Mat src, cv::Mat& dst, double angle, T center = T(0, 0), double scale = 1.0)
    {
        cv::Mat ret;
        cv::Mat m = cv::getRotationMatrix2D(center, angle, scale);
        cv::warpAffine(src, ret, m, src.size(), cv::INTER_LINEAR + cv::WARP_FILL_OUTLIERS);
        dst = ret.clone();
    }

    /**
     * @brief 平移变换
     * @param src     原图
     * @param dst     目标图
     * @param xoffset x轴平移量
     * @param yoffset y轴平移量
     * @refer https://blog.csdn.net/southfor/article/details/109393028
     * @other 另一种写法，平移矩阵为,2行3列矩阵
     * 1,0，xoffset
     * 0,1，yoffset
     * 所以矩阵可以写为 cv::Mat m = (cv::Mat_<double>(2,3)<< 1,0,xoffset,0,1,yoffset);
     * 三点透视变换
     */
    template<typename T>
    void move(const cv::Mat src, cv::Mat& dst, double xoffset, double yoffset)
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
        dst = ret.clone();
    }
    /**
     * @brief 最原始的仿射变换函数,用此函数进行错切变换
     * @param src 原图
     * @param dst 目标图
     * @param src_point 原图的三个点，float型的数组，数组的个数是3
     * @param dst_points 目标图的三个点，float型的数组，数组的个数是3
     * @refer https://zhuanlan.zhihu.com/p/387408410
     */
    template<typename T>
    void affine(const cv::Mat src, cv::Mat& dst, const T src_points[], const T dst_points[])
    {
        cv::Mat m = cv::getAffineTransform(src_points, dst_points);
        cv::Mat ret;
        cv::warpAffine(src, ret, m, src.size());
        dst = ret.clone();
    }


    cv::Mat cvMat6_to_cvMat9(const cv::Mat& mtx6)
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

    cv::Mat d6_to_cvMat(double d0, double d1, double d2, double d3, double d4, double d5)
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

    /*
        向量的旋转平移变化
    */
    cv::Mat vector_angle_to_M(double x1, double y1, double d1, double x2, double y2, double d2)
    {
        cv::Point2f center(x1, y1);
        double      angle = d2 - d1;
        cv::Mat     rot_M = cv::getRotationMatrix2D(center, angle, 1.0);
        rot_M             = cvMat6_to_cvMat9(rot_M);

        cv::Mat trans_M = d6_to_cvMat(1, 0, x2 - x1, 0, 1, y2 - y1);
        cv::Mat M       = trans_M * rot_M;   // 先旋转在平移（矩阵乘法相反）
        return M;
    }

    cv::Point2f TransPoint(const cv::Mat& M, const cv::Point2f& point)
    {
        std::vector<double> values = {point.x, point.y};
        cv::Mat             mat    = cv::Mat(values).clone();   // 将vector变成单列的mat，这里需要clone(),因为这里的赋值操作是浅拷贝
        cv::Mat             dest   = mat.reshape(1, 1);

        cv::Mat homogeneousPoint = (cv::Mat_<double>(3, 1) << point.x, point.y, 1.0);
        cv::Mat transformed      = M * homogeneousPoint;
        return cv::Point2f(transformed.at<double>(0, 0), transformed.at<double>(0, 1));
    }
};


NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END
#endif   // NAO_VTRANSFORM_H