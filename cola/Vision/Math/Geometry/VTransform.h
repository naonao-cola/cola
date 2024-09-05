/**
 * @FilePath     : /cola/cola/Vision/Math/Geometry/VTransform.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-15 15:01:10
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-20 12:17:26
 **/
#ifndef NAO_VTRANSFORM_H
#define NAO_VTRANSFORM_H

#include "../../VisionObject.h"

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

struct TRigidTrans3D;

class VTransform : public VisionObject
{
public:
    /**
     * @brief 点排序，按照顺时针方向排序，适用于4个点，不适用于多点
     * @param src_pt，输入的原点
     * @return 排序后的点,顺时针排序的点，第一个点为左上角的点
     */
    template<typename T>
    std::vector<T> order_pts(const std::vector<T> src_pt);

    /**
     * @brief 4点透视变换矩阵，原点与透视点需要一一对应。从实际图像变换到正视图
     * @param src_points 原点
     * @param dst_points 透视后的点，
     * @param wrap_mat   得到的透视矩阵
     */
    template<typename T>
    cv::Mat get_perspective(const std::vector<T> src_points, const std::vector<T> dst_points);

    /**
     * @brief    4点透视变换，不需要提供目标点，原点必须是矩形的4个角点，从实际图像变换到正视图
     * @param pts 原点
     * @param wrap_mat 透视矩阵
     */
    template<typename T>
    cv::Mat get_perspective(const std::vector<T> pts);
    /**
     * @brief   最小二乘法透视矩阵，不限制点的个数，要求一一对应。
     * @tparam T
     * @param src_points
     * @param dst_points
     * @return
     */
    template<typename T>
    cv::Mat get_perspective_homo(const std::vector<T> src_points, const std::vector<T> dst_points);

    /**
     * @brief 点变换，将实际图像上的点通过透视矩阵，求得到正视图上对应的点
     * @param src_point 原点
     * @param wrap_mat 透视矩阵
     * @param dst_point 输出点
     * @refer https://www.cnblogs.com/yanghailin/p/12510318.html
     */
    template<typename T>
    T perspective_point(const T src_point, const cv::Mat wrap_mat);

    /**
     * @brief 点反变换，根据透视矩阵反变换，将目标图的点反变换到原图
     * @tparam T
     * @param src_point
     * @param wrap_mat
     * @param dst_point
     */
    template<typename T>
    T perspective_inv_point(const T& src_point, const cv::Mat& wrap_mat);


    /*------------------------------------------仿射变换--------------------------------------------------------*/
    /**
     * @brief 图像翻转
     * @param src  原图
     * @param dst  目标图
     * @param flip_type 翻转类型，0，表示上下翻转。1，表示左右翻转。-1表示对角线翻转。
     */
    cv::Mat affine_img_flip(const cv::Mat src, NInt flip_type);
    /**
     * @brief  图像旋转
     * @param src   原图
     * @param dst   目标图像
     * @param angle 逆时针旋转角度
     * @param center 旋转中心
     * @param scale  缩放比例
     */
    template<typename T>
    cv::Mat affine_img_rotate(const cv::Mat src, NDouble angle, T center = T(0, 0), NDouble scale = 1.0);

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
     * 所以矩阵可以写为 cv::Mat m = (cv::Mat_<NDouble>(2,3)<< 1,0,xoffset,0,1,yoffset);
     * 三点透视变换
     */
    template<typename T>
    cv::Mat affine_img_move(const cv::Mat src, NDouble xoffset, NDouble yoffset);

    /**
     * @brief 最原始的仿射变换函数,用此函数进行错切变换
     * @param src 原图
     * @param dst 目标图
     * @param src_point 原图的三个点，float型的数组，数组的个数是3
     * @param dst_points 目标图的三个点，float型的数组，数组的个数是3
     * @refer https://zhuanlan.zhihu.com/p/387408410
     */
    template<typename T>
    cv::Mat affine_img_affine(const cv::Mat src, const T src_points[], const T dst_points[]);


    /*
    向量的旋转平移变化
    */
    cv::Mat     vector_angle_to_M(NDouble x1, NDouble y1, NDouble d1, NDouble x2, NDouble y2, NDouble d2);
    cv::Point2f TransPoint(const cv::Mat& M, const cv::Point2f& point);


    /*------------------------------------------刚体三维变换--------------------------------------------------------*/

    /**
     * @brief:
     * @return
     * @note :
     * @other:
     //旋转矩阵关系  https://blog.csdn.net/changbaolong/article/details/8307052
     //刚体三维旋转变换求解
     //公式推导与python代码 https://blog.csdn.net/u012836279/article/details/80203170
     //c++ 代码  https://blog.csdn.net/kewei9/article/details/74157236
    **/
    cv::Mat get_affine_3d_matrix(const std::vector<cv::Point3f>& src_point_vec, const std::vector<cv::Point3f>& dst_point_vec);

    TRigidTrans3D get_affine_3d_matrix(const std::vector<cv::Point3f>& src_point_vec, const std::vector<cv::Point3f>& dst_point_vec, NInt pointsNum);
};

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END

#include "VTransform.inl"

#endif   // NAO_VTRANSFORM_H