/**
 * @FilePath     : /cola/cola/Vision/Math/Line/Spline/VSpline.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-21 22:00:45
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-21 22:22:47
 **/
#ifndef NAONAO_VSPLINE_H
#define NAONAO_VSPLINE_H

/*
https://blog.csdn.net/qq_37887537/article/details/78498209
*/

#include "../../../VisionObject.h"
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <vector>
NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

// spline interpolation
class VSpline : public VisionObject
{
public:
    enum bd_type
    {
        first_deriv  = 1,
        second_deriv = 2
    };

private:
    std::vector<NDouble> m_x, m_y;        // x,y coordinates of points
                                         // interpolation parameters
                                         // f(x) = a*(x-x_i)^3 + b*(x-x_i)^2 + c*(x-x_i) + y_i
    std::vector<NDouble> m_a, m_b, m_c;   // spline coefficients
    NDouble              m_b0, m_c0;      // for left extrapol
    bd_type             m_left, m_right;
    NDouble              m_left_value, m_right_value;
    bool                m_force_linear_extrapolation;

public:
    // set default boundary condition to be zero curvature at both ends
    VSpline()
        : m_left(second_deriv)
        , m_right(second_deriv)
        , m_left_value(0.0)
        , m_right_value(0.0)
        , m_force_linear_extrapolation(false)
    {
        ;
    }

    // optional, but if called it has to come be before set_points()
    void   set_boundary(bd_type left, NDouble left_value, bd_type right, NDouble right_value, bool force_linear_extrapolation = false);
    void   set_points(const std::vector<NDouble>& x, const std::vector<NDouble>& y, bool cubic_spline = true);
    NDouble operator()(NDouble x) const;
    NDouble deriv(NInt order, NDouble x) const;
};

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END

#endif   // NAONAO_VSPLINE_H