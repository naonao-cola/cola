/*
 * @FilePath     : /cola/cola/Vision/Math/Line/Fit/VFit.inl
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-20 23:13:57
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-08-19 15:07:22
 */
#ifndef NAONAO_VFIT_INL
#define NAONAO_VFIT_INL

#include "VFit.h"

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

template<typename T>
NBool VFit<T>::linearFit(const std::vector<T>& x, const std::vector<T>& y, NBool isSaveFitYs)
{
    return linearFit(&x[0], &y[0], getSeriesLength(x, y), isSaveFitYs);
};

template<typename T>
NBool VFit<T>::linearFit(const T* x, const T* y, NSize length, NBool isSaveFitYs)
{
    factor_.resize(2, 0);
    T t1 = 0;
    T t2 = 0;
    T t3 = 0;
    T t4 = 0;
    for (NInt i = 0; i < length; ++i) {
        t1 += x[i] * x[i];
        t2 += x[i];
        t3 += x[i] * y[i];
        t4 += y[i];
    }
    factor_[1] = (t3 * length - t2 * t4) / (t1 * length - t2 * t2);
    factor_[0] = (t1 * t4 - t2 * t3) / (t1 * length - t2 * t2);
    calcError(x, y, length, this->ssr_, this->sse_, this->rmse_, isSaveFitYs);   // 计算误差
    return true;
};

template<typename T>
void VFit<T>::polyfit(const std::vector<T>& x, const std::vector<T>& y, NInt poly_n, NBool isSaveFitYs)
{
    polyfit(&x[0], &y[0], getSeriesLength(x, y), poly_n, isSaveFitYs);
};

template<typename T>
void VFit<T>::polyfit(const T* x, const T* y, NSize length, NInt poly_n, NBool isSaveFitYs)
{
    factor_.resize(poly_n + 1, 0);
    NInt i;
    NInt j;
    // NDouble *tempx,*tempy,*sumxx,*sumxy,*ata;
    std::vector<NDouble> tempx(length, 1.0);
    std::vector<NDouble> tempy(y, y + length);

    std::vector<NDouble> sumxx(poly_n * 2 + 1);
    std::vector<NDouble> ata((poly_n + 1) * (poly_n + 1));
    std::vector<NDouble> sumxy(poly_n + 1);
    for (i = 0; i < 2 * poly_n + 1; i++) {
        for (sumxx[i] = 0, j = 0; j < length; j++) {
            sumxx[i] += tempx[j];
            tempx[j] *= x[j];
        }
    }
    for (i = 0; i < poly_n + 1; i++) {
        for (sumxy[i] = 0, j = 0; j < length; j++) {
            sumxy[i] += tempy[j];
            tempy[j] *= x[j];
        }
    }
    for (i = 0; i < poly_n + 1; i++) {
        for (j = 0; j < poly_n + 1; j++) {
            ata[i * (poly_n + 1) + j] = sumxx[i + j];
        }
    }
    gauss_solve(poly_n + 1, ata, factor_, sumxy);

    fitedYs_.reserve(length);   // 计算拟合后的数据并计算误差
    calcError(&x[0], &y[0], length, this->ssr_, this->sse_, this->rmse_, isSaveFitYs);
};

template<typename T>
NDouble VFit<T>::getY(const T x) const
{
    NDouble ans(0);
    for (NSize i = 0; i < factor_.size(); ++i) {
        ans += factor_[i] * pow((NDouble)x, static_cast<NInt>(i));
    }
    return ans;
};

template<typename T>
NSize VFit<T>::getSeriesLength(const std::vector<T>& x, const std::vector<T>& y)
{
    return (x.size() > y.size() ? y.size() : x.size());
};

template<typename T>
T VFit<T>::Mean(const std::vector<T>& v)
{
    return Mean(&v[0], v.size());
};
template<typename T>
T VFit<T>::Mean(const T* v, NSize length)
{
    T total(0);
    for (NSize i = 0; i < length; ++i) {
        total += v[i];
    }
    return (total / length);
};

template<typename T>
void VFit<T>::calcError(const T* x, const T* y, NSize length, NDouble& r_ssr, NDouble& r_sse, NDouble& r_rmse, NBool isSaveFitYs)
{
    T mean_y = Mean(y, length);
    T yi(0);
    fitedYs_.reserve(length);
    for (NInt i = 0; i < length; ++i) {
        yi = getY(x[i]);
        r_ssr += ((yi - mean_y) * (yi - mean_y));   // 计算回归平方和
        r_sse += ((yi - y[i]) * (yi - y[i]));       // 残差平方和
        if (isSaveFitYs) {
            fitedYs_.push_back(NDouble(yi));
        }
    }
    r_rmse = sqrt(r_sse / (static_cast<NDouble>(length)));
}

template<typename T>
void VFit<T>::gauss_solve(NInt n, std::vector<T>& A, std::vector<T>& x, std::vector<T>& b)
{
    gauss_solve(n, &A[0], &x[0], &b[0]);
}

template<typename T>
void VFit<T>::gauss_solve(NInt n, T* A, T* x, T* b)
{
    NInt    i;
    NInt    j;
    NInt    k;
    NInt    r;
    NDouble max;
    for (k = 0; k < n - 1; k++) {
        max = fabs(A[k * n + k]); /*find maxmum*/
        r   = k;
        for (i = k + 1; i < n - 1; i++) {
            if (max < fabs(A[i * n + i])) {
                max = fabs(A[i * n + i]);
                r   = i;
            }
        }
        if (r != k) {
            for (i = 0; i < n; i++) { /*change array:A[k]&A[r] */
                max          = A[k * n + i];
                A[k * n + i] = A[r * n + i];
                A[r * n + i] = max;
            }
        }
        max  = b[k]; /*change array:b[k]&b[r]     */
        b[k] = b[r];
        b[r] = max;
        for (i = k + 1; i < n; i++) {
            for (j = k + 1; j < n; j++) {
                A[i * n + j] -= A[i * n + k] * A[k * n + j] / A[k * n + k];
            }
            b[i] -= A[i * n + k] * b[k] / A[k * n + k];
        }
    }

    for (i = n - 1; i >= 0; x[i] /= A[i * n + i], i--) {
        for (j = i + 1, x[i] = b[i]; j < n; j++) {
            x[i] -= A[i * n + j] * x[j];
        }
    }
}

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END

#endif