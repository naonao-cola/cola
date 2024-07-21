/**
 * @FilePath     : /cola/cola/Vision/Math/Line/Fit/VFit.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-20 23:13:34
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-21 21:34:14
 **/
#ifndef NAONAO_VFIT_H
#define NAONAO_VFIT_H

/*
https://blog.csdn.net/czyt1988/article/details/21743595
https://blog.csdn.net/qq_40969467/article/details/83035561
*/

#include "../../../VisionObject.h"
#include <cstdlib>
#include <ctime>
#include <vector>

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

template<typename T>
class VFit : public VisionObject
{

    std::vector<double> factor_;    /// 拟合后的方程系数
    double              ssr_;       /// 回归平方和
    double              sse_;       /// 剩余平方和
    double              rmse_;      /// RMSE均方根误差
    std::vector<double> fitedYs_;   /// 存放拟合后的y值，在拟合时可设置为不保存节省内存
public:
    VFit()
        : ssr_(0)
        , sse_(0)
        , rmse_(0)
    {
        factor_.resize(2, 0);
    };
    ~VFit() override = default;

    /*--------------------------------------------------------------------------------------------------
    * @FuncName:
    * @Author:    闹闹
    * @Brief:     直线拟合-一元回归,拟合的结果可以使用getFactor获取，或者使用getSlope获取斜率，getIntercept获取截距
    * @Version:   1.0.0.1
    * @Date:	  2021/5/18 15:09
    * @InputParameter: x 观察值的x,y 观察值的y,isSaveFitYs 拟合后的数据是否保存，默认否
    * @OutputParameter:
    * @Returns:
    * @Others:
    --------------------------------------------------------------------------------------------------*/

    bool linearFit(const std::vector<T>& x, const std::vector<T>& y, bool isSaveFitYs = false);
    bool linearFit(const T* x, const T* y, size_t length, bool isSaveFitYs = false);

    /*--------------------------------------------------------------------------------------------------
    * @FuncName:
    * @Author:    闹闹
    * @Brief:     多项式拟合，拟合y=a0+a1*x+a2*x^2+……+apoly_n*x^poly_n
    * @Version:   1.0.0.1
    * @Date:	  2021/5/18 15:14
    * @InputParameter: x 观察值的x,y 观察值的y,poly_n 期望拟合的阶数，若poly_n=2，则y=a0+a1*x+a2*x^2,isSaveFitYs 拟合后的数据是否保存，默认是
    * @OutputParameter:
    * @Returns:
    * @Others:
    --------------------------------------------------------------------------------------------------*/

    void polyfit(const std::vector<T>& x, const std::vector<T>& y, int poly_n, bool isSaveFitYs = true);
    void polyfit(const T* x, const T* y, size_t length, int poly_n, bool isSaveFitYs = true);

    /*--------------------------------------------------------------------------------------------------
    * @FuncName:
    * @Author:    闹闹
    * @Brief:    获取系数
    * @Version:   1.0.0.1
    * @Date:	  2021/5/18 15:17
    * @InputParameter: 存放系数的数组
    * @OutputParameter:
    * @Returns:
    * @Others:
    --------------------------------------------------------------------------------------------------*/
    void getFactor(std::vector<double>& factor) { factor = this->factor_; };

    /*--------------------------------------------------------------------------------------------------
    * @FuncName:
    * @Author:    闹闹
    * @Brief:    获取拟合方程对应的y值
    * @Version:   1.0.0.1
    * @Date:	  2021/5/18 15:18
    * @InputParameter:
    * @OutputParameter:
    * @Returns:
    * @Others:    前提是拟合时设置isSaveFitYs为true
    --------------------------------------------------------------------------------------------------*/
    void getFitedYs(std::vector<double>& fitedYs) { fitedYs = this->fitedYs_; };

    /*--------------------------------------------------------------------------------------------------
    * @FuncName:
    * @Author:    闹闹
    * @Brief:    根据x获取拟合方程的y值
    * @Version:   1.0.0.1
    * @Date:	  2021/5/18 15:18
    * @InputParameter:
    * @OutputParameter:
    * @Returns:   返回x对应的y值
    * @Others:
    --------------------------------------------------------------------------------------------------*/

    double getY(const T x) const;

    /*--------------------------------------------------------------------------------------------------
    * @FuncName:
    * @Author:    闹闹
    * @Brief:     获取斜率
    * @Version:   1.0.0.1
    * @Date:	  2021/5/18 15:20
    * @InputParameter:
    * @OutputParameter:
    * @Returns:    斜率值
    * @Others:
    --------------------------------------------------------------------------------------------------*/
    double getSlope() { return factor_[1]; };

    /*--------------------------------------------------------------------------------------------------
    * @FuncName:
    * @Author:    闹闹
    * @Brief:    获取截距
    * @Version:   1.0.0.1
    * @Date:	  2021/5/18 15:21
    * @InputParameter:
    * @OutputParameter:
    * @Returns:   截距值
    * @Others:
    --------------------------------------------------------------------------------------------------*/
    double getIntercept() { return factor_[0]; };

    /*--------------------------------------------------------------------------------------------------
    * @FuncName:
    * @Author:    闹闹
    * @Brief:     剩余平方和,回归平方和,均方根误差 。确定系数，系数是0~1之间的数，是数理上判定拟合优度的一个量。
    * @Version:   1.0.0.1
    * @Date:	  2021/5/18 15:24
    * @InputParameter:
    * @OutputParameter:
    * @Returns:
    * @Others:
    --------------------------------------------------------------------------------------------------*/
    double getSSE() { return sse_; };
    double getSSR() { return ssr_; };
    double getRMSE() { return rmse_; };
    double getR_square() { return 1 - (sse_ / (ssr_ + sse_)); };

    /*--------------------------------------------------------------------------------------------------
    * @FuncName:
    * @Author:    闹闹
    * @Brief:    获取两个vector的安全size,
    * @Version:   1.0.0.1
    * @Date:	  2021/5/18 15:26
    * @InputParameter:
    * @OutputParameter:
    * @Returns:   最小的一个长度
    * @Others:
    --------------------------------------------------------------------------------------------------*/
    size_t getSeriesLength(const std::vector<T>& x, const std::vector<T>& y);

    /*--------------------------------------------------------------------------------------------------
    * @FuncName:
    * @Author:    闹闹
    * @Brief:     计算均值
    * @Version:   1.0.0.1
    * @Date:	  2021/5/18 15:27
    * @InputParameter:
    * @OutputParameter:
    * @Returns:
    * @Others:
    --------------------------------------------------------------------------------------------------*/

    static T Mean(const std::vector<T>& v);
    static T Mean(const T* v, size_t length);

    /*--------------------------------------------------------------------------------------------------
    * @FuncName:
    * @Author:    闹闹
    * @Brief:    获取拟合方程系数的个数。 根据阶次获取拟合方程的系数，如getFactor(2),就是获取y=a0+a1*x+a2*x^2+……+apoly_n*x^poly_n中a2的值。
    * @Version:   1.0.0.1
    * @Date:	  2021/5/18 15:28
    * @InputParameter:
    * @OutputParameter:
    * @Returns:
    * @Others:
    --------------------------------------------------------------------------------------------------*/
    size_t getFactorSize() { return factor_.size(); };
    double getFactor(size_t i) { return factor_.at(i); };
private:
    void calcError(const T* x, const T* y, size_t length, double& r_ssr, double& r_sse, double& r_rmse, bool isSaveFitYs = true);
    void gauss_solve(int n, std::vector<T>& A, std::vector<T>& x, std::vector<T>& b);
    void gauss_solve(int n, T* A, T* x, T* b);
};

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END

#include "VFit.inl"

#endif   // NAONAO_VFIT_H