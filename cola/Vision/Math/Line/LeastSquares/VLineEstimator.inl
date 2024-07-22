/*
 * @FilePath     : /cola/cola/Vision/Math/Line/LeastSquares/VLineEstimator.inl
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-20 23:51:14
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-21 00:00:31
 */
#ifndef NAONAO_VLINEESTIMATOR_INL
#define NAONAO_VLINEESTIMATOR_INL

#include "VLineEstimator.h"

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

template<typename T>
Eigen::VectorXd VLineParamEstimator<T>::LeastSquareMethod(std::vector<T> X, std::vector<T> Y, uint8_t orders)
{
    if (X.size() < 2 || Y.size() < 2 || X.size() != Y.size() || orders < 1)
        exit(EXIT_FAILURE);
    order_ = orders;
    Eigen::Map<VectorX> sampleX(X.data(), X.size());   // 从STL数组转为EIGEN数组
    Eigen::Map<VectorX> sampleY(Y.data(), Y.size());

    MatrixX mtxVandermonde(X.size(), orders + 1);   // Vandermonde matrix of X-axis coordinate vector of sample data
    MatrixX colVandermonde = sampleX;               // Vandermonde column

    for (size_t i = 0; i < orders + 1; ++i) {   // construct Vandermonde matrix column by column
        if (0 == i) {
            mtxVandermonde.col(0) = VectorX::Constant(X.size(), 1, 1);   // 最后一列赋值为1
            continue;
        }
        if (1 == i) {
            mtxVandermonde.col(1) = colVandermonde;   // 倒数第二列赋值为X本身
            continue;
        }
        colVandermonde        = colVandermonde.array() * sampleX.array();
        mtxVandermonde.col(i) = colVandermonde;
    }
    // calculate coefficients vector of fitted polynomial
    mat_ = (mtxVandermonde.transpose() * mtxVandermonde).inverse() * (mtxVandermonde.transpose()) * sampleY;
    return mat_;
};

template<typename T>
void VLineParamEstimator<T>::calcError(std::vector<T> X, std::vector<T> Y)
{
    if (X.size() < 2 || Y.size() < 2 || X.size() != Y.size()) {
        exit(EXIT_FAILURE);
    }
    mean_ = mean<T>(Y);
    double yi;   // 用拟合后的曲线计算得到的yi
    for (int i = 0; i < X.size(); i++) {
        yi = 0;
        for (int j = 0; j < order_; j++)
            yi += pow(X[i], j) * mat_[order_ - j];   // 用拟合后的曲线计算得到的yi
        ssr_ += ((yi - mean_) * (yi - mean_));       // 计算回归平方和
        sse_ += ((yi - Y[i]) * (yi - Y[i]));         // 残差平方和
    }
    rmse_ = sqrt(sse_ / (double(X.size())));   // 剩余平方和
};

template<typename T>
bool VLineParamEstimator<T>::agree(T x, T y)
{
    double dis = 0;
    double yi  = 0;
    for (int i = x - 200; i < x + 200; i++) {
        for (int j = 0; j < order_; j++)
            yi = pow(i, j) * mat_[order_ - j];
        double tmp = (x - i) * (x - i) + (y - yi) * (y - yi);
        tmp        = pow(tmp, 0.5);
        if (tmp > dis) {
            dis = tmp;
        }
    }
    return (dis <= error_);
};

template<typename T>
double VLineParamEstimator<T>::mean(std::vector<T> Y)
{
    if (Y.size() < 2)
        exit(EXIT_FAILURE);
    double sum = 0.0;
    for (size_t i = 0; i < Y.size(); i++)
        sum += Y[i];
    sum = (sum / (Y.size() * 1.0));
    return sum;
};

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END

#endif