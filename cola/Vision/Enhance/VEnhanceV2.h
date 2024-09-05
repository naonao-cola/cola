/**
 * @FilePath     : /cola/cola/Vision/Enhance/VEnhanceV2.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-27 22:36:38
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-27 22:36:39
 * @Copyright (c) 2024 by G, All Rights Reserved.
 **/
#ifndef NAONAO_VENHANCEV2_H
#define NAONAO_VENHANCEV2_H

#include "../VisionObject.h"

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

/**
 * 单通道定义类调整
 */
class Curve
{
protected:
    cv::Scalar                       _color;
    cv::Scalar                       _back_color;
    NInt                              _tolerance;   // 鼠标按下或移动时，捕获曲线点的误差范围
    bool                             _is_mouse_down;
    std::vector<cv::Point>           _points;    // control points 曲线的所有控制点
    std::vector<cv::Point>::iterator _current;   // pointer to current point 当前控制点的指针

    std::vector<cv::Point>::iterator find(NInt x);
    std::vector<cv::Point>::iterator find(NInt x, NInt y);
    std::vector<cv::Point>::iterator add(NInt x, NInt y);

public:
    Curve();
    virtual ~Curve();
    NInt  calcCurve(NDouble* z);      // 供内部调用的方法：计算曲线
    void draw(cv::Mat& mat);        // 将曲线画在mat上
    void mouseDown(NInt x, NInt y);   // 当鼠标按下，请调用mouseDown()方法
    bool mouseMove(NInt x, NInt y);   // 当鼠标移动，请调用mouseMove()方法
    void mouseUp(NInt x, NInt y);     // 当鼠标抬起，请调用mouseUp()方法

    // 以下方法用于：用编程方式生成曲线
    void clearPoints();                                 // 清除曲线上所有的点
    NInt  addPoint(const cv::Point& p);                  // 增加一个点
    NInt  deletePoint(const cv::Point& p);               // 删除一个点
    NInt  movePoint(const cv::Point& p, NInt x, NInt y);   // 移动一个点
};

/**
 * 多通道的曲线的定义、绘制、实施调整。
 */
class Curves
{
protected:
    void createColorTables(uchar colorTables[][256]);

public:
    Curves();
    virtual ~Curves();
    Curve  _RGBChannel;               // RGB总通道
    Curve  _RedChannel;               // Red通道
    Curve  _GreenChannel;             // Green通道
    Curve  _BlueChannel;              // Blue通道
    Curve* _CurrentChannel;           // 当前通道的指针
    void   draw(cv::Mat& mat);        // 将曲线画在mat上
    void   mouseDown(NInt x, NInt y);   // 当鼠标按下，请调用mouseDown()方法
    bool   mouseMove(NInt x, NInt y);   // 当鼠标移动，请调用mouseMove()方法
    void   mouseUp(NInt x, NInt y);     // 当鼠标抬起，请调用mouseUp()方法
    // 实施曲线调整
    NInt adjust(cv::InputArray src, cv::OutputArray dst, cv::InputArray mask = cv::noArray());
};

/*
**饱和度调整类,饱和度目前只写到三通道图片，单通道先不考虑
**参考链接：https://blog.csdn.net/xingyanxiao/article/details/48035537
*/
class Saturation
{
public:
    explicit Saturation(NInt param);
    ~Saturation();
    // 调整系数
    NInt _nParameter;
    //= = = = = = = = = = = = = = = = = = = =
    // @FuncName:
    // @Brief:     饱和度函数
    // @InputParameter:
    // @OutputParameter:
    // @Returns:
    // @Others:
    //= = = = = = = = = = = = = = = = = = = =
    NInt adjust(cv::InputArray input, cv::OutputArray output);
};

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END

#endif