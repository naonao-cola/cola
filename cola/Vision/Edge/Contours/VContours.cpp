/**
 * @FilePath     : /cola/cola/Vision/Edge/Contours/VContours.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-08-09 12:04:59
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-08-09 20:27:47
 * @Copyright (c) 2024 by G, All Rights Reserved.
 **/
#include "VContours.h"

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

std::vector<std::vector<cv::Point>> VContours::get_contours(const cv::Mat& src)
{
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i>              filter_hierarchy;
    cv::findContours(src, contours, filter_hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
    return contours;
}

ClipperLib::Path VContours::contour2path(const std::vector<cv::Point>& contour)
{
    ClipperLib::Path path;
    for (const auto& pt : contour) {
        path << ClipperLib::IntPoint(pt.x, pt.y);
    }
    return path;
}

ClipperLib::Paths VContours::contours2paths(const std::vector<std::vector<cv::Point>>& contours)
{
    ClipperLib::Paths paths;
    for (const auto& contour : contours) {
        ClipperLib::Path path;
        for (const auto& pt : contour) {
            path << ClipperLib::IntPoint(pt.x, pt.y);
        }
        paths.push_back(path);
    }
    return paths;
}

std::vector<cv::Point> VContours::path2contour(const ClipperLib::Path& path)
{
    std::vector<cv::Point> contour;
    for (const auto& pt : path) {
        contour.emplace_back(pt.X, pt.Y);
    }
    return contour;
}
std::vector<std::vector<cv::Point>> VContours::path2contours(const ClipperLib::Paths& paths)
{
    std::vector<std::vector<cv::Point>> contours;
    for (const auto& path : paths) {
        std::vector<cv::Point> contour;
        for (const auto& pt : path) {
            contour.emplace_back(pt.X, pt.Y);
        }
        contours.push_back(contour);
    }
    return contours;
}

std::vector<cv::Point> VContours::contours_and(const std::vector<cv::Point>& lcontour, const std::vector<cv::Point>& rcontour)
{

    ClipperLib::Path subject = contour2path(lcontour);
    ClipperLib::Path clip    = contour2path(rcontour);
    // 创建Clipper对象
    ClipperLib::Clipper c;
    // 添加路径到Clipper对象
    c.AddPath(subject, ClipperLib::ptSubject, true);
    c.AddPath(clip, ClipperLib::ptClip, true);

    // 结果路径
    ClipperLib::Paths solution;

    // 计算交集
    c.Execute(ClipperLib::ctIntersection, solution, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);

    std::vector<cv::Point> dst;
    for (const ClipperLib::Path& path : solution) {
        for (const ClipperLib::IntPoint& pt : path) {
            dst.emplace_back(pt.X, pt.Y);
        }
    }
    return dst;
}

std::vector<cv::Point> VContours::contours_or(const std::vector<cv::Point>& lcontour, const std::vector<cv::Point>& rcontour)
{

    ClipperLib::Path subject = contour2path(lcontour);
    ClipperLib::Path clip    = contour2path(rcontour);
    // 创建Clipper对象
    ClipperLib::Clipper c;
    // 添加路径到Clipper对象
    c.AddPath(subject, ClipperLib::ptSubject, true);
    c.AddPath(clip, ClipperLib::ptClip, true);

    // 结果路径
    ClipperLib::Paths solution;

    // 计算交集
    c.Execute(ClipperLib::ctUnion, solution, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);
    std::vector<cv::Point> dst;
    for (const ClipperLib::Path& path : solution) {
        for (const ClipperLib::IntPoint& pt : path) {
            dst.emplace_back(pt.X, pt.Y);
        }
    }
    return dst;
}

std::vector<cv::Point> VContours::contours_not(const std::vector<cv::Point>& lcontour, const std::vector<cv::Point>& rcontour)
{
    ClipperLib::Path subject = contour2path(lcontour);
    ClipperLib::Path clip    = contour2path(rcontour);
    // 创建Clipper对象
    ClipperLib::Clipper c;
    // 添加路径到Clipper对象
    c.AddPath(subject, ClipperLib::ptSubject, true);
    c.AddPath(clip, ClipperLib::ptClip, true);

    // 结果路径
    ClipperLib::Paths solution;

    // 计算交集
    c.Execute(ClipperLib::ctDifference, solution, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);
    std::vector<cv::Point> dst;
    for (const ClipperLib::Path& path : solution) {
        for (const ClipperLib::IntPoint& pt : path) {
            dst.emplace_back(pt.X, pt.Y);
        }
    }
    return dst;
}

std::vector<cv::Point> VContours::contours_xor(const std::vector<cv::Point>& lcontour, const std::vector<cv::Point>& rcontour)
{
    ClipperLib::Path subject = contour2path(lcontour);
    ClipperLib::Path clip    = contour2path(rcontour);
    // 创建Clipper对象
    ClipperLib::Clipper c;
    // 添加路径到Clipper对象
    c.AddPath(subject, ClipperLib::ptSubject, true);
    c.AddPath(clip, ClipperLib::ptClip, true);

    // 结果路径
    ClipperLib::Paths solution;

    // 计算交集
    c.Execute(ClipperLib::ctXor, solution, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);
    std::vector<cv::Point> dst;
    for (const ClipperLib::Path& path : solution) {
        for (const ClipperLib::IntPoint& pt : path) {
            dst.emplace_back(pt.X, pt.Y);
        }
    }
    return dst;
}
std::vector<cv::Point> VContours::contours_offset(const std::vector<cv::Point>& contour, NDouble off_value)
{
    ClipperLib::Path          path = contour2path(contour);
    ClipperLib::ClipperOffset co;
    co.AddPath(path, ClipperLib::jtMiter, ClipperLib::etClosedPolygon);
    ClipperLib::Paths solution;
    co.Execute(solution, off_value);
    std::vector<cv::Point> dst;
    for (const ClipperLib::Path& path : solution) {
        for (const ClipperLib::IntPoint& pt : path) {
            dst.emplace_back(pt.X, pt.Y);
        }
    }
    return dst;
}

NInt VContours::PointInPolygon(const std::vector<cv::Point>& contour, const cv::Point& point)
{
    ClipperLib::Path     path = contour2path(contour);
    ClipperLib::IntPoint testPoint(point.x, point.y);
    return ClipperLib::PointInPolygon(testPoint, path);
}

NDouble VContours::contours_area(const std::vector<cv::Point>& contour)
{
    ClipperLib::Path path = contour2path(contour);
    return ClipperLib::Area(path);
}
NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END