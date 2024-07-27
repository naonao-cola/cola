/**
 * @FilePath     : /cola/cola/Vision/Enhance/VEnhanceV2.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-27 22:36:49
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-27 22:51:10
 * @Copyright (c) 2024 by G, All Rights Reserved.
 **/

#ifdef HAVE_OPENMP
#    include <omp.h>
#endif

#include "VEnhance.h"
#include "VEnhanceV2.h"

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

Curve::Curve()
{
    _color         = cv::Scalar(0, 0, 0);
    _back_color    = cv::Scalar(255, 255, 255);
    _tolerance     = 3;
    _is_mouse_down = false;
    _points.push_back(cv::Point(0, 0));
    _points.push_back(cv::Point(255, 255));
    _current = _points.end();
}

Curve::~Curve() = default;

std::vector<cv::Point>::iterator Curve::find(int x)
{
    std::vector<cv::Point>::iterator iter;
    for (iter = _points.begin(); iter != _points.end(); ++iter) {
        if (::abs(iter->x - x) <= _tolerance) {
            return iter;
        }
    }
    return _points.end();
}

std::vector<cv::Point>::iterator Curve::find(int x, int y)
{
    std::vector<cv::Point>::iterator iter;
    for (iter = _points.begin(); iter != _points.end(); ++iter) {
        if (::abs(iter->x - x) <= _tolerance && ::abs(iter->y - y) <= _tolerance) {
            return iter;
        }
    }
    return _points.end();
}

std::vector<cv::Point>::iterator Curve::add(int x, int y)
{
    std::vector<cv::Point>::iterator it = find(x);
    if (it == _points.end()) {
        cv::Point                        p(x, y);
        std::vector<cv::Point>::iterator iter;
        for (iter = _points.begin(); iter != _points.end(); ++iter) {
            if (iter == _points.begin() && iter->x > p.x) {
                // DEBUG_PRINT("points insert at beginning");
                return _points.insert(iter, p);
            }

            if (iter->x < x && (iter + 1) != _points.end() && (iter + 1)->x > p.x) {
                // DEBUG_PRINT("points insert");
                return _points.insert(iter + 1, p);
            }
        }
        // DEBUG_PRINT("points append");
        return _points.insert(_points.end(), p);
    }
    return it;
}

int Curve::calcCurve(double* output_y)
{
    // if count of control points is less than 2, return linear output
    if (_points.size() < 2) {
        for (int i = 0; i < 256; ++i) {
            output_y[i] = 255 - i;
        }
        return 0;
    }

    // if count of control points is 2, return linear output
    if (_points.size() == 2) {
        std::vector<cv::Point>::iterator point1 = _points.begin();
        std::vector<cv::Point>::iterator point2 = point1 + 1;

        double delta_y = 0;
        if (point2->x != point1->x) {
            delta_y = (point2->y - point1->y) * 1.0 / (point2->x - point1->x);
        }

        // create output
        for (int i = 0; i < 256; ++i) {
            if (i < point1->x) {
                output_y[i] = point1->y;
            }
            else if (i >= point1->x && i < point2->x) {
                output_y[i] = COLOR_RANGE(point1->y + delta_y * (i - point1->x));
            }
            else {
                output_y[i] = point2->y;
            }
        }
        return 0;
    }

    // the count of control points is greater than 2,  create spline line
    int n = static_cast<int>(_points.size());   // count of points

    // create array of x-coordinate and y-coordinate of control points
    double*                          x           = new double[n];
    double*                          y           = new double[n];
    std::vector<cv::Point>::iterator start_point = _points.end();
    std::vector<cv::Point>::iterator end_point   = _points.end();
    std::vector<cv::Point>::iterator iter;
    int                              k = 0;
    for (iter = _points.begin(); iter != _points.end(); ++iter, ++k) {
        if (k == 0) {
            start_point = iter;
        }
        x[k]      = iter->x - start_point->x;
        y[k]      = iter->y;
        end_point = iter;
    }

    // if start_point or end_point is invalid
    if (start_point == _points.end() || end_point == _points.end() || start_point == end_point) {
        for (int i = 0; i < 256; ++i) {
            output_y[i] = 255 - i;
        }
        return 1;
    }

    // create array of x-coordinate of output points
    int     m = end_point->x - start_point->x;
    double* t = new double[m];   // array of x-coordinate of output points
    double* z = new double[m];   // array of y-coordinate of output points
                                 // initialize array of x-coordinate
    for (int i = 0; i < m; ++i) {
        t[i] = i;
    }

    // perform spline, output y-coordinate is stored in array z
    VEnhance::spline(x, y, n, t, m, z);
    // create output
    for (int i = 0; i < 256; ++i) {
        if (i < start_point->x) {
            output_y[i] = start_point->y;
        }
        else if (i >= start_point->x && i < end_point->x) {
            output_y[i] = CLIP_RANGE(z[i - start_point->x], 0, 255);
        }
        else {
            output_y[i] = end_point->y;
        }
    }
    delete[] x;
    delete[] y;
    delete[] t;
    delete[] z;
    return 0;
}

void Curve::draw(cv::Mat& mat)
{
    int       thinkness = 1;
    int       n         = 0;
    cv::Point lastPoint;
    // clear background
    mat.setTo(_back_color);
    std::vector<cv::Point>::iterator it;
    for (it = _points.begin(); it != _points.end(); ++it) {
        std::cout << "point:  " << it->x << ", " << it->y << std::endl;
    }

    // draw lines
    VEnhance::dot_line(mat, cv::Point(0, 0), cv::Point(255, 0), cv::Scalar(0, 0, 255), 1, 8, 4, 4);
    VEnhance::dot_line(mat, cv::Point(0, 255), cv::Point(255, 255), cv::Scalar(0, 0, 255), 1, 8, 4, 4);

    VEnhance::dot_line(mat, cv::Point(63, 0), cv::Point(63, 255), _color, 1, 8, 4, 4);
    VEnhance::dot_line(mat, cv::Point(127, 0), cv::Point(127, 255), _color, 1, 8, 4, 4);
    VEnhance::dot_line(mat, cv::Point(191, 0), cv::Point(191, 255), _color, 1, 8, 4, 4);
    VEnhance::dot_line(mat, cv::Point(0, 255 - 63), cv::Point(255, 255 - 63), _color, 1, 8, 4, 4);
    VEnhance::dot_line(mat, cv::Point(0, 255 - 127), cv::Point(255, 255 - 127), _color, 1, 8, 4, 4);
    VEnhance::dot_line(mat, cv::Point(0, 255 - 191), cv::Point(255, 255 - 191), _color, 1, 8, 4, 4);

    // create curve
    double z[256];
    calcCurve(z);
    for (int i = 1; i < 256; ++i) {
        cv::line(mat, cv::Point(i - 1, static_cast<int>(255 - z[i - 1])), cv::Point(i, static_cast<int>(255 - z[i])), _color, 1, 8);
    }
    // draw control points
    std::vector<cv::Point>::iterator iter, iter_next;
    for (iter = _points.begin(); iter != _points.end(); ++iter, ++n) {
        thinkness = (iter == _current) ? -1 : 1;
        cv::rectangle(mat, cv::Point(iter->x - 2, 255 - iter->y + 2), cv::Point(iter->x + 2, 255 - iter->y - 2), _color, thinkness, 8);
    }
}

void Curve::mouseDown(int x, int y)
{
    y              = 255 - y;
    _current       = add(x, y);
    _is_mouse_down = true;
}

bool Curve::mouseMove(int x, int y)
{
    y = 255 - y;
    if (_is_mouse_down) {
        if (_current != _points.end()) {
            int prev_x = 0;
            int next_x = 255;

            if (_current != _points.begin()) {
                int prev_y = (_current - 1)->y;
                prev_x     = (_current - 1)->x;

                // match the previous point
                if (_points.size() > 2 && ::abs(x - prev_x) <= _tolerance && ::abs(y - prev_y) <= _tolerance) {
                    _current--;
                    _current = _points.erase(_current);
                    // DEBUG_PRINT("erase previous");
                    return true;
                }

                // if x less than x of previou point
                if (x <= prev_x) {
                    // DEBUG_PRINT("less than prev_x");
                    return true;
                }
            }
            if ((_current + 1) != _points.end()) {
                int next_y = (_current + 1)->y;
                next_x     = (_current + 1)->x;

                // match the next point
                if (_points.size() > 2 && ::abs(x - next_x) <= _tolerance && ::abs(y - next_y) <= _tolerance) {
                    _current = _points.erase(_current);
                    // DEBUG_PRINT("erase next");
                    return true;
                }

                // if x great than x of next point
                if (x >= next_x) {
                    // DEBUG_PRINT("large than next_x");
                    return true;
                }
            }

            _current->x = CLIP_RANGE(x, 0, 255);
            _current->y = CLIP_RANGE(y, 0, 255);
            return true;
        }
    }
    return false;
}

void Curve::mouseUp(int x, int y)
{
    y              = 255 - y;
    _is_mouse_down = false;
}

void Curve::clearPoints()
{
    _points.clear();
}

int Curve::addPoint(const cv::Point& p)
{
    std::vector<cv::Point>::iterator iter = add(p.x, p.y);
    if (iter != _points.end())
        return 1;
    else
        return 0;
}

int Curve::deletePoint(const cv::Point& p)
{
    std::vector<cv::Point>::iterator iter;
    iter = find(p.x, p.y);
    if (iter != _points.end()) {
        if (_current == iter)
            _current = _points.end();
        _points.erase(iter);
        return 1;
    }
    return 0;
}

int Curve::movePoint(const cv::Point& p, int x, int y)
{
    std::vector<cv::Point>::iterator iter;
    iter = find(p.x, p.y);
    if (iter != _points.end()) {
        iter->x = x;
        iter->y = y;
        return 1;
    }
    return 0;
}

Curves::Curves()
{
    _CurrentChannel = &_RGBChannel;
}

Curves::~Curves()
{
}

void Curves::draw(cv::Mat& mat)
{
    if (_CurrentChannel)
        _CurrentChannel->draw(mat);
}

void Curves::mouseDown(int x, int y)
{
    if (_CurrentChannel)
        _CurrentChannel->mouseDown(x, y);
}

bool Curves::mouseMove(int x, int y)
{
    if (_CurrentChannel)
        return _CurrentChannel->mouseMove(x, y);
    return false;
}

void Curves::mouseUp(int x, int y)
{
    if (_CurrentChannel)
        _CurrentChannel->mouseUp(x, y);
}

void Curves::createColorTables(uchar colorTables[][256])
{
    double z[256];
    _BlueChannel.calcCurve(z);
    for (int i = 0; i < 256; ++i) {
        colorTables[0][i] = static_cast<uchar>(z[i]);
    }
    _GreenChannel.calcCurve(z);
    for (int i = 0; i < 256; ++i)
        colorTables[1][i] = static_cast<uchar>(z[i]);
    _RedChannel.calcCurve(z);
    for (int i = 0; i < 256; ++i) {
        colorTables[2][i] = static_cast<uchar>(z[i]);
    }
    uchar value;
    _RGBChannel.calcCurve(z);
    for (int i = 0; i < 256; ++i) {
        for (int c = 0; c < 3; c++) {
            value             = colorTables[c][i];
            colorTables[c][i] = static_cast<uchar>(z[value]);
        }
    }
}

int Curves::adjust(cv::InputArray src, cv::OutputArray dst, cv::InputArray mask)
{
    cv::Mat input = src.getMat();
    if (input.empty()) {
        return -1;
    }
    dst.create(src.size(), src.type());
    cv::Mat output = dst.getMat();

    bool    hasMask = true;
    cv::Mat msk     = mask.getMat();
    if (msk.empty())
        hasMask = false;

    const uchar* in;
    const uchar* pmask;
    uchar*       out;
    int          width    = input.cols;
    int          height   = input.rows;
    int          channels = input.channels();

    uchar colorTables[3][256];
    // create color tables
    createColorTables(colorTables);
    // adjust each pixel
    if (hasMask) {
#ifdef HAVE_OPENMP
#    pragma omp parallel for
#endif
        for (int y = 0; y < height; y++) {
            in    = input.ptr<uchar>(y);
            out   = output.ptr<uchar>(y);
            pmask = msk.ptr<uchar>(y);
            for (int x = 0; x < width; x++) {
                for (int c = 0; c < 3; c++) {
                    *out = static_cast<uchar>((colorTables[c][*in] * pmask[x] / 255.0) + (*in) * (255 - pmask[x]) / 255.0);
                    out++;
                    in++;
                }
                for (int c = 0; c < channels - 3; c++) {
                    *out++ = *in++;
                }
            }
        }
    }
    else {
#ifdef HAVE_OPENMP
#    pragma omp parallel for
#endif
        for (int y = 0; y < height; y++) {
            in  = input.ptr<uchar>(y);
            out = output.ptr<uchar>(y);
            for (int x = 0; x < width; x++) {
                for (int c = 0; c < 3; c++) {
                    *out++ = colorTables[c][*in++];
                }
                for (int c = 0; c < channels - 3; c++) {
                    *out++ = *in++;
                }
            }
        }
    }
    return 0;
}

Saturation::Saturation(int param)
{
    _nParameter = param;
}

Saturation::~Saturation() = default;

int Saturation::adjust(cv::InputArray input, cv::OutputArray output)
{
    cv::Mat src = input.getMat();
    if (input.empty()) {
        return -1;
    }
    output.create(src.size(), src.type());
    cv::Mat dst = output.getMat();
    for (int row = 0; row < src.rows; row++) {
        for (int col = 0; col < src.cols; col++) {
            uchar  B      = src.at<cv::Vec3b>(row, col)[0];
            uchar  G      = src.at<cv::Vec3b>(row, col)[1];
            uchar  R      = src.at<cv::Vec3b>(row, col)[2];
            uchar  rgbMax = std::max(std::max(R, G), B);
            uchar  rgbMin = std::min(std::min(R, G), B);
            double dDelta = static_cast<double>(rgbMax - rgbMin) / 255;
            double dValue = static_cast<double>(rgbMax + rgbMin) / 255;
            if (0 == dDelta) {
                dst.at<cv::Vec3b>(row, col)[0] = B;
                dst.at<cv::Vec3b>(row, col)[1] = G;
                dst.at<cv::Vec3b>(row, col)[2] = R;
                continue;   // 如果该像素点是灰色 不处理
            }
            double dL = dValue / 2;   // 按照公式计算明度L   [0,1]
            double dS;                // 饱和度S
            if (dL < 0.5) {
                dS = dDelta / dValue;
            }
            else {
                dS = dDelta / (2 - dValue);
            }
            double dPercent = static_cast<double>(_nParameter) / 100;
            double dAlpha;
            if (dPercent >= 0) {
                if (dPercent + dS >= 1) {
                    dAlpha = dS;
                }
                else {
                    dAlpha = 1 - dPercent;
                }
                dAlpha                         = 1 / dAlpha - 1;
                dst.at<cv::Vec3b>(row, col)[0] = static_cast<uchar>(B + (B - dL * 255) * dAlpha);
                dst.at<cv::Vec3b>(row, col)[1] = static_cast<uchar>(G + (G - dL * 255) * dAlpha);
                dst.at<cv::Vec3b>(row, col)[2] = static_cast<uchar>(R + (R - dL * 255) * dAlpha);
            }
            else {
                dAlpha                         = dPercent;
                dst.at<cv::Vec3b>(row, col)[0] = static_cast<uchar>(dL * 255 + (B - dL * 255) * (1 + dAlpha));
                dst.at<cv::Vec3b>(row, col)[1] = static_cast<uchar>(dL * 255 + (G - dL * 255) * (1 + dAlpha));
                dst.at<cv::Vec3b>(row, col)[2] = static_cast<uchar>(dL * 255 + (R - dL * 255) * (1 + dAlpha));
            }
        }
    }
    return 0;
}

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END