/**
 * @FilePath     : /cola/cola/Vision/Math/Line/Spline/VBandMatrix.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-21 22:09:20
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-21 22:11:46
 **/
#include "VBandMatrix.h"
NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

// ---------------------------------------------------------------------
// implementation part, which could be separated into a cpp file
// ---------------------------------------------------------------------
// Vband_matrix implementation
// -------------------------

Vband_matrix::Vband_matrix(int dim, int n_u, int n_l)
{
    resize(dim, n_u, n_l);
}

void Vband_matrix::resize(int dim, int n_u, int n_l)
{
    assert(dim > 0);
    assert(n_u >= 0);
    assert(n_l >= 0);
    upper_.resize(n_u + 1);
    lower_.resize(n_l + 1);
    for (size_t i = 0; i < upper_.size(); i++) {
        upper_[i].resize(dim);
    }
    for (size_t i = 0; i < lower_.size(); i++) {
        lower_[i].resize(dim);
    }
}

int Vband_matrix::dim() const
{
    if (upper_.size() > 0) {
        return upper_[0].size();
    }
    return 0;
}

// defines the new operator (), so that we can access the elements
// by A(i,j), index going from i=0,...,dim()-1
double& Vband_matrix::operator()(int i, int j)
{
    int k = j - i;   // what band is the entry
    assert((i >= 0) && (i < dim()) && (j >= 0) && (j < dim()));
    assert((-num_lower() <= k) && (k <= num_upper()));
    // k=0 -> diogonal, k<0 lower left part, k>0 upper right part
    if (k >= 0) {
        return upper_[k][i];
    }
    else {
        return lower_[-k][i];
    }
}

double Vband_matrix::operator()(int i, int j) const
{
    int k = j - i;   // what band is the entry
    assert((i >= 0) && (i < dim()) && (j >= 0) && (j < dim()));
    assert((-num_lower() <= k) && (k <= num_upper()));
    // k=0 -> diogonal, k<0 lower left part, k>0 upper right part
    if (k >= 0) {
        return upper_[k][i];
    }
    else {
        return lower_[-k][i];
    }
}

// second diag (used in LU decomposition), saved in m_lower
double Vband_matrix::saved_diag(int i) const
{
    assert((i >= 0) && (i < dim()));
    return lower_[0][i];
}

double& Vband_matrix::saved_diag(int i)
{
    assert((i >= 0) && (i < dim()));
    return lower_[0][i];
}

// LR-Decomposition of a band matrix
void Vband_matrix::lu_decompose()
{
    int    i_max, j_max;
    int    j_min;
    double x;

    // preconditioning
    // normalize column i so that a_ii=1
    for (int i = 0; i < this->dim(); i++) {
        assert(this->operator()(i, i) != 0.0);
        this->saved_diag(i) = 1.0 / this->operator()(i, i);
        j_min               = std::max(0, i - this->num_lower());
        j_max               = std::min(this->dim() - 1, i + this->num_upper());
        for (int j = j_min; j <= j_max; j++) {
            this->operator()(i, j) *= this->saved_diag(i);
        }
        this->operator()(i, i) = 1.0;   // prevents rounding errors
    }

    // Gauss LR-Decomposition
    for (int k = 0; k < this->dim(); k++) {
        i_max = std::min(this->dim() - 1, k + this->num_lower());   // num_lower not a mistake!
        for (int i = k + 1; i <= i_max; i++) {
            assert(this->operator()(k, k) != 0.0);
            x                      = -this->operator()(i, k) / this->operator()(k, k);
            this->operator()(i, k) = -x;   // assembly part of L
            j_max                  = std::min(this->dim() - 1, k + this->num_upper());
            for (int j = k + 1; j <= j_max; j++) {
                // assembly part of R
                this->operator()(i, j) = this->operator()(i, j) + x * this->operator()(k, j);
            }
        }
    }
}

// solves Ly=b
std::vector<double> Vband_matrix::l_solve(const std::vector<double>& b) const
{
    assert(this->dim() == (int)b.size());
    std::vector<double> x(this->dim());
    int                 j_start;
    double              sum;
    for (int i = 0; i < this->dim(); i++) {
        sum     = 0;
        j_start = std::max(0, i - this->num_lower());
        for (int j = j_start; j < i; j++)
            sum += this->operator()(i, j) * x[j];
        x[i] = (b[i] * this->saved_diag(i)) - sum;
    }
    return x;
}

// solves Rx=y
std::vector<double> Vband_matrix::r_solve(const std::vector<double>& b) const
{
    assert(this->dim() == (int)b.size());
    std::vector<double> x(this->dim());
    int                 j_stop;
    double              sum;
    for (int i = this->dim() - 1; i >= 0; i--) {
        sum    = 0;
        j_stop = std::min(this->dim() - 1, i + this->num_upper());
        for (int j = i + 1; j <= j_stop; j++)
            sum += this->operator()(i, j) * x[j];
        x[i] = (b[i] - sum) / this->operator()(i, i);
    }
    return x;
}

std::vector<double> Vband_matrix::lu_solve(const std::vector<double>& b, bool is_lu_decomposed)
{
    assert(this->dim() == (int)b.size());
    std::vector<double> x, y;
    if (is_lu_decomposed == false) {
        this->lu_decompose();
    }
    y = this->l_solve(b);
    x = this->r_solve(y);
    return x;
}

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END