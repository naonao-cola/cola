/**
 * @FilePath     : /cola/cola/Vision/Math/Line/LeastSquares/VLineEstimator.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-20 23:50:57
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-08-19 15:10:15
 **/
#ifndef NAONAO_VLINEESTIMATOR_H
#define NAONAO_VLINEESTIMATOR_H

/*
矩阵法拟合曲线
https://zhuanlan.zhihu.com/p/268884807
*/

#include "../../../VisionObject.h"
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <Eigen/Geometry>
#include <vector>

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

template<typename T>
class VLineParamEstimator : public VisionObject
{
public:
    typedef Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> MatrixX;   // 动态矩阵
    typedef Eigen::Matrix<T, Eigen::Dynamic, 1>              VectorX;   // 动态列向量
    VLineParamEstimator(NDouble error)
        : error_(error)
        , mean_(0)
        , sse_(0)
        , ssr_(0)
        , rmse_(0){};
    ~VLineParamEstimator() override = default;

    /*--------------------------------------------------------------------------------------------------
    * @FuncName:
    * @Author:    闹闹
    * @Brief:    使用矩阵拟合的最小二乘法。
    * @Version:   1.0.0.1
    * @Date:	  2021/5/18 17:11
    * @InputParameter:
    * @OutputParameter:
    * @Returns:
    * @Others:
    --------------------------------------------------------------------------------------------------*/
    Eigen::VectorXd LeastSquareMethod(std::vector<T> X, std::vector<T> Y, uint8_t orders);

    /*--------------------------------------------------------------------------------------------------
    * @FuncName:
    * @Author:    闹闹
    * @Brief:     使用均方根误差 计算误差
    * @Version:   1.0.0.1
    * @Date:	  2021/5/18 17:21
    * @InputParameter:
    * @OutputParameter:
    * @Returns:
    * @Others:
    --------------------------------------------------------------------------------------------------*/
    void calcError(std::vector<T> X, std::vector<T> Y);

    /*--------------------------------------------------------------------------------------------------
    * @FuncName:
    * @Author:    闹闹
    * @Brief:     计算点到曲线的距离，取点比较，然后维护一个最大值。复杂度略。
    * @Version:   1.0.0.1
    * @Date:	  2021/6/18 16:35
    * @InputParameter:
    * @OutputParameter:
    * @Returns:
    * @Others:    只取 x 前后各100个点的位置
    --------------------------------------------------------------------------------------------------*/
    bool agree(T x, T y);

    /*--------------------------------------------------------------------------------------------------
    * @FuncName:
    * @Author:    闹闹
    * @Brief:     计算均值
    * @Version:   1.0.0.1
    * @Date:	  2021/5/19 9:42
    * @InputParameter:
    * @OutputParameter:
    * @Returns:
    * @Others:
    --------------------------------------------------------------------------------------------------*/
    NDouble mean(std::vector<T> Y);

private:
    Eigen::VectorXd mat_;
    NInt             order_;
    NDouble          error_;   // 错误阈值
    NDouble          mean_;
    NDouble          ssr_;
    NDouble          sse_;
    NDouble          rmse_;
};

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END

#include "VLineEstimator.inl"

#endif   // NAONAO_VLINEESTIMATOR_H