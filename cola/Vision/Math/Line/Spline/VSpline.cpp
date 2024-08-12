/**
 * @FilePath     : /cola/cola/Vision/Math/Line/Spline/VSpline.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-21 22:18:15
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-08-09 21:53:14
 **/
#include "VSpline.h"
#include "VBandMatrix.h"
NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

// VSpline implementation
// -----------------------

void VSpline::set_boundary(VSpline::bd_type left, double left_value, VSpline::bd_type right, double right_value, bool force_linear_extrapolation)
{
    assert(m_x.size() == 0);   // set_points() must not have happened yet
    m_left                       = left;
    m_right                      = right;
    m_left_value                 = left_value;
    m_right_value                = right_value;
    m_force_linear_extrapolation = force_linear_extrapolation;
}

void VSpline::set_points(const std::vector<double>& x, const std::vector<double>& y, bool cubic_spline)
{
    assert(x.size() == y.size());
    assert(x.size() > 2);
    m_x   = x;
    m_y   = y;
    int n = x.size();
    // TODO: maybe sort x and y, rather than returning an error
    for (int i = 0; i < n - 1; i++) {
        assert(m_x[i] < m_x[i + 1]);
    }

    if (cubic_spline == true) {   // cubic spline interpolation
                                  // setting up the matrix and right hand side of the equation system
                                  // for the parameters b[]
        Vband_matrix        A(n, 1, 1);
        std::vector<double> rhs(n);
        for (int i = 1; i < n - 1; i++) {
            A(i, i - 1) = 1.0 / 3.0 * (x[i] - x[i - 1]);
            A(i, i)     = 2.0 / 3.0 * (x[i + 1] - x[i - 1]);
            A(i, i + 1) = 1.0 / 3.0 * (x[i + 1] - x[i]);
            rhs[i]      = (y[i + 1] - y[i]) / (x[i + 1] - x[i]) - (y[i] - y[i - 1]) / (x[i] - x[i - 1]);
        }
        // boundary conditions
        if (m_left == VSpline::second_deriv) {
            // 2*b[0] = f''
            A(0, 0) = 2.0;
            A(0, 1) = 0.0;
            rhs[0]  = m_left_value;
        }
        else if (m_left == VSpline::first_deriv) {
            // c[0] = f', needs to be re-expressed in terms of b:
            // (2b[0]+b[1])(x[1]-x[0]) = 3 ((y[1]-y[0])/(x[1]-x[0]) - f')
            A(0, 0) = 2.0 * (x[1] - x[0]);
            A(0, 1) = 1.0 * (x[1] - x[0]);
            rhs[0]  = 3.0 * ((y[1] - y[0]) / (x[1] - x[0]) - m_left_value);
        }
        else {
            assert(false);
        }
        if (m_right == VSpline::second_deriv) {
            // 2*b[n-1] = f''
            A(n - 1, n - 1) = 2.0;
            A(n - 1, n - 2) = 0.0;
            rhs[n - 1]      = m_right_value;
        }
        else if (m_right == VSpline::first_deriv) {
            // c[n-1] = f', needs to be re-expressed in terms of b:
            // (b[n-2]+2b[n-1])(x[n-1]-x[n-2])
            // = 3 (f' - (y[n-1]-y[n-2])/(x[n-1]-x[n-2]))
            A(n - 1, n - 1) = 2.0 * (x[n - 1] - x[n - 2]);
            A(n - 1, n - 2) = 1.0 * (x[n - 1] - x[n - 2]);
            rhs[n - 1]      = 3.0 * (m_right_value - (y[n - 1] - y[n - 2]) / (x[n - 1] - x[n - 2]));
        }
        else {
            assert(false);
        }

        // solve the equation system to obtain the parameters b[]
        m_b = A.lu_solve(rhs);

        // calculate parameters a[] and c[] based on b[]
        m_a.resize(n);
        m_c.resize(n);
        for (int i = 0; i < n - 1; i++) {
            m_a[i] = 1.0 / 3.0 * (m_b[i + 1] - m_b[i]) / (x[i + 1] - x[i]);
            m_c[i] = (y[i + 1] - y[i]) / (x[i + 1] - x[i]) - 1.0 / 3.0 * (2.0 * m_b[i] + m_b[i + 1]) * (x[i + 1] - x[i]);
        }
    }
    else {   // linear interpolation
        m_a.resize(n);
        m_b.resize(n);
        m_c.resize(n);
        for (int i = 0; i < n - 1; i++) {
            m_a[i] = 0.0;
            m_b[i] = 0.0;
            m_c[i] = (m_y[i + 1] - m_y[i]) / (m_x[i + 1] - m_x[i]);
        }
    }

    // for left extrapolation coefficients
    m_b0 = (m_force_linear_extrapolation == false) ? m_b[0] : 0.0;
    m_c0 = m_c[0];

    // for the right extrapolation coefficients
    // f_{n-1}(x) = b*(x-x_{n-1})^2 + c*(x-x_{n-1}) + y_{n-1}
    double h = x[n - 1] - x[n - 2];
    // m_b[n-1] is determined by the boundary condition
    m_a[n - 1] = 0.0;
    m_c[n - 1] = 3.0 * m_a[n - 2] * h * h + 2.0 * m_b[n - 2] * h + m_c[n - 2];   // = f'_{n-2}(x_{n-1})
    if (m_force_linear_extrapolation == true)
        m_b[n - 1] = 0.0;
}

double VSpline::operator()(double x) const
{
    size_t n = m_x.size();
    // find the closest point m_x[idx] < x, idx=0 even if x<m_x[0]
    std::vector<double>::const_iterator it;
    it      = std::lower_bound(m_x.begin(), m_x.end(), x);
    int idx = std::max(int(it - m_x.begin()) - 1, 0);

    double h = x - m_x[idx];
    double interpol;
    if (x < m_x[0]) {
        // extrapolation to the left
        interpol = (m_b0 * h + m_c0) * h + m_y[0];
    }
    else if (x > m_x[n - 1]) {
        // extrapolation to the right
        interpol = (m_b[n - 1] * h + m_c[n - 1]) * h + m_y[n - 1];
    }
    else {
        // interpolation
        interpol = ((m_a[idx] * h + m_b[idx]) * h + m_c[idx]) * h + m_y[idx];
    }
    return interpol;
}

double VSpline::deriv(int order, double x) const
{
    assert(order > 0);

    size_t n = m_x.size();
    // find the closest point m_x[idx] < x, idx=0 even if x<m_x[0]
    std::vector<double>::const_iterator it;
    it      = std::lower_bound(m_x.begin(), m_x.end(), x);
    int idx = std::max(int(it - m_x.begin()) - 1, 0);

    double h = x - m_x[idx];
    double interpol;
    if (x < m_x[0]) {
        // extrapolation to the left
        switch (order) {
        case 1:
            interpol = 2.0 * m_b0 * h + m_c0;
            break;
        case 2:
            interpol = 2.0 * m_b0 * h;
            break;
        default:
            interpol = 0.0;
            break;
        }
    }
    else if (x > m_x[n - 1]) {
        // extrapolation to the right
        switch (order) {
        case 1:
            interpol = 2.0 * m_b[n - 1] * h + m_c[n - 1];
            break;
        case 2:
            interpol = 2.0 * m_b[n - 1];
            break;
        default:
            interpol = 0.0;
            break;
        }
    }
    else {
        // interpolation
        switch (order) {
        case 1:
            interpol = (3.0 * m_a[idx] * h + 2.0 * m_b[idx]) * h + m_c[idx];
            break;
        case 2:
            interpol = 6.0 * m_a[idx] * h + 2.0 * m_b[idx];
            break;
        case 3:
            interpol = 6.0 * m_a[idx];
            break;
        default:
            interpol = 0.0;
            break;
        }
    }
    return interpol;
}

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END