/**
 * @FilePath     : /cola/cola/Vision/Math/Line/Spline/VBandMatrix.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-21 22:06:36
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-08-09 21:52:56
 **/
#ifndef NAONAO_VBANDMATRIX_H
#define NAONAO_VBANDMATRIX_H

#include "../../../VisionObject.h"
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <vector>


NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

// band matrix solver
// 带状矩阵，目前先不看。
class Vband_matrix : public VisionObject
{
private:
    std::vector<std::vector<double>> upper_;   // upper band
    std::vector<std::vector<double>> lower_;   // lower band
public:
    Vband_matrix(){};                          // constructor
    Vband_matrix(int dim, int n_u, int n_l);   // constructor
    ~Vband_matrix() = default;                 // destructor
    void resize(int dim, int n_u, int n_l);    // init with dim,n_u,n_l
    int  dim() const;                          // matrix dimension
    int  num_upper() const { return upper_.size() - 1; }
    int  num_lower() const { return lower_.size() - 1; }
    // access operator
    double& operator()(int i, int j);         // write
    double  operator()(int i, int j) const;   // read
                                              // we can store an additional diogonal (in m_lower)
    double&             saved_diag(int i);
    double              saved_diag(int i) const;
    void                lu_decompose();
    std::vector<double> r_solve(const std::vector<double>& b) const;
    std::vector<double> l_solve(const std::vector<double>& b) const;
    std::vector<double> lu_solve(const std::vector<double>& b, bool is_lu_decomposed = false);
};

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END

#endif   // NAONAO_VBANDMATRIX_H