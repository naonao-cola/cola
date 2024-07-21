/**
 * @FilePath     : /cola/cola/Vision/Math/Geometry/VTransform.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-16 09:45:29
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-20 12:17:18
 **/
#include "VTransform.h"

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

struct TRigidTrans3D
{
    cv::Mat matR;

    float X;
    float Y;
    float Z;
};

cv::Mat VTransform::affine_img_flip(const cv::Mat src, int flip_type)
{
    cv::Mat ret;
    cv::flip(src, ret, flip_type);
    return ret;
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


cv::Mat VTransform::vector_angle_to_M(double x1, double y1, double d1, double x2, double y2, double d2)
{
    cv::Point2f center(x1, y1);
    double      angle = d2 - d1;
    cv::Mat     rot_M = cv::getRotationMatrix2D(center, angle, 1.0);
    rot_M             = cvMat6_to_cvMat9(rot_M);
    cv::Mat trans_M   = d6_to_cvMat(1, 0, x2 - x1, 0, 1, y2 - y1);
    cv::Mat M         = trans_M * rot_M;   // 先旋转在平移（矩阵乘法相反）
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

cv::Mat VTransform::get_affine_3d_matrix(const std::vector<cv::Point3f>& src_point_vec, const std::vector<cv::Point3f>& dst_point_vec)
{

    cv::Mat            aff(3, 4, CV_64F);
    std::vector<uchar> inliers;
    cv::estimateAffine3D(src_point_vec, dst_point_vec, aff, inliers);
    return aff;
}

TRigidTrans3D VTransform::get_affine_3d_matrix(const std::vector<cv::Point3f>& src_point_vec, const std::vector<cv::Point3f>& dst_point_vec, int pointsNum)
{

    cv::Mat       src_avg, dst_avg, src_rep, dst_rep, srcMat, dstMat;
    TRigidTrans3D transform;
    cv::Mat       src_mat = cv::Mat(src_point_vec, true).reshape(1, pointsNum);
    cv::Mat       dst_mat = cv::Mat(dst_point_vec, true).reshape(1, pointsNum);
    cv::reduce(src_mat, src_avg, 0, cv::REDUCE_AVG);
    cv::reduce(dst_mat, dst_avg, 0, cv::REDUCE_AVG);
    cv::repeat(src_avg, pointsNum, 1, src_rep);
    cv::repeat(dst_avg, pointsNum, 1, dst_rep);
    srcMat = (src_mat - src_rep).t();
    dstMat = (dst_mat - dst_rep).t();

    cv::Mat matS = srcMat * dstMat.t();
    cv::Mat matU, matW, matV;
    cv::SVDecomp(matS, matW, matU, matV);

    cv::Mat matTemp = matU * matV;
    float   det     = cv::determinant(matTemp);   // 计算矩阵的行列式

    float   datM[] = {1, 0, 0, 0, 1, 0, 0, 0, det};
    cv::Mat matM(3, 3, CV_32FC1, datM);
    cv::Mat matR = matV.t() * matM * matU.t();

    transform.matR = matR.clone();
    float* datR    = reinterpret_cast<float*>(matR.data);
    transform.X    = dst_avg.at<float>(0, 0) - (src_avg.at<float>(0, 0) * datR[0] + src_avg.at<float>(0, 1) * datR[1] + src_avg.at<float>(0, 2) * datR[2]);
    transform.Y    = dst_avg.at<float>(0, 1) - (src_avg.at<float>(0, 0) * datR[3] + src_avg.at<float>(0, 1) * datR[4] + src_avg.at<float>(0, 2) * datR[5]);
    transform.Z    = dst_avg.at<float>(0, 2) - (src_avg.at<float>(0, 0) * datR[6] + src_avg.at<float>(0, 1) * datR[7] + src_avg.at<float>(0, 2) * datR[8]);
    return transform;
}

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END