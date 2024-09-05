/**
 * @FilePath     : /cola/cola/Vision/Math/Line/Spline/VBandMatrix.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-21 22:06:36
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-08-19 15:24:07
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
    std::vector<std::vector<NDouble>> upper_;   // upper band
    std::vector<std::vector<NDouble>> lower_;   // lower band
public:
    Vband_matrix(){};                                // constructor
    Vband_matrix(NInt dim, NInt n_u, NInt n_l);   // constructor
    ~Vband_matrix() = default;                       // destructor
    void  resize(NInt dim, NInt n_u, NInt n_l);   // init with dim,n_u,n_l
    NInt dim() const;                               // matrix dimension
    NInt num_upper() const { return upper_.size() - 1; }
    NInt num_lower() const { return lower_.size() - 1; }
    // access operator
    NDouble& operator()(NInt i, NInt j);         // write
    NDouble  operator()(NInt i, NInt j) const;   // read
                                                   // we can store an additional diogonal (in m_lower)
    NDouble&             saved_diag(NInt i);
    NDouble              saved_diag(NInt i) const;
    void                 lu_decompose();
    std::vector<NDouble> r_solve(const std::vector<NDouble>& b) const;
    std::vector<NDouble> l_solve(const std::vector<NDouble>& b) const;
    std::vector<NDouble> lu_solve(const std::vector<NDouble>& b, bool is_lu_decomposed = false);
};

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END

#endif   // NAONAO_VBANDMATRIX_H