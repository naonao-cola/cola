/**
 * @FilePath     : /cola/cola/Vision/Feature/Blob/VBlobDefine.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-31 10:52:14
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-31 10:52:14
 * @Copyright (c) 2024 by G, All Rights Reserved.
 **/
#ifndef NAONAO_VBLOBDEFINE_H
#define NAONAO_VBLOBDEFINE_H

#include "../../VisionObject.h"

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

// 灰度级
#define IMAGE_MAX_GV 256
// Blob图片
#define BLOB_DRAW_ROTATED_BOX 0x000010
#define BLOB_DRAW_BOUNDING_BOX 0x000020
#define BLOB_DRAW_BLOBS 0x000080
#define BLOB_DRAW_BLOBS_CONTOUR 0x000100

// 不良
enum ENUM_BOLB_FEATURE
{
    E_FEATURE_AREA = 0,        // 客体面积
    E_FEATURE_BOX_AREA,        // bounding box面积
    E_FEATURE_BOX_RATIO,       // 客体面积 / Bounding Box面积比例
    E_FEATURE_SUM_GV,          // 亮度累积
    E_FEATURE_MIN_GV,          // 最小亮度
    E_FEATURE_MAX_GV,          // 最大亮度
    E_FEATURE_MEAN_GV,         // 平均亮度
    E_FEATURE_DIFF_GV,         // (背景-对象)亮度差异
    E_FEATURE_BK_GV,           // 背景亮度
    E_FEATURE_STD_DEV,         // 标准偏差
    E_FEATURE_SEMU,            // SEMU
    E_FEATURE_COMPACTNESS,     // 客体有多接近圆的形状?
    E_FEATURE_MIN_GV_RATIO,    // 对象最小亮度/背景亮度
    E_FEATURE_MAX_GV_RATIO,    // 对象最大亮度/背景亮度
    E_FEATURE_DIFF_GV_RATIO,   // 对象的平均亮度/背景亮度
    E_FEATURE_PERIMETER,       // 外环线的长度
    E_FEATURE_ROUNDNESS,       //
    E_FEATURE_ELONGATION,      // Box 横向/纵向
    E_FEATURE_BOX_X,           // Bounding Box横向长度
    E_FEATURE_BOX_Y,           // Bounding Box纵向长度

    E_FEATURE_MIN_BOX_AREA,    // Feret’s area
    E_FEATURE_MINOR_AXIS,      // 长轴
    E_FEATURE_MAJOR_AXIS,      // 短轴
    E_FEATURE_AXIS_RATIO,      // 长/短轴
    E_FEATURE_MIN_BOX_RATIO,   // 对象面积/ Min Bounding Box面积比例(Area porosity)

    E_FEATURE_GV_UP_COUNT_0,     // 亮度数量
    E_FEATURE_GV_UP_COUNT_1,     // 亮度数量
    E_FEATURE_GV_UP_COUNT_2,     // 亮度数量
    E_FEATURE_GV_DOWN_COUNT_0,   // 亮度数量
    E_FEATURE_GV_DOWN_COUNT_1,   // 亮度数量
    E_FEATURE_GV_DOWN_COUNT_2,   // 亮度数量

    E_FEATURE_MEANAREA_RATIO,   // MeanAreaRatio choikwangil

    E_FEATURE_GVAREA_RATIO_TEST,   // 04.17 choikwangil

    E_FEATURE_IS_EDGE_C,        // 看他在不在外围
    E_FEATURE_IS_EDGE_V,        // 位于郊区的垂直位置
    E_FEATURE_IS_EDGE_H,        // 它位于郊区吗？
    E_FEATURE_IS_EDGE_CENTER,   // 不在外围的位置

    E_FEATURE_COUNT
};

// Blob 구조체
struct tBLOB_FEATURE
{
    bool      bFiltering;       // 有/无过滤
    cv::Rect  rectBox;          // Bounding Box
    long      nArea;            // 客体面积
    long      nBoxArea;         // bounding box面积
    float     fBoxRatio;        // Bounding Box面积比率/对象面积 ( Rectangularity(= Extent)
    cv::Point ptCenter;         // 中心点
    long      nSumGV;           // 亮度累积
    long      nMinGV;           // 最小亮度
    long      nMaxGV;           // 最大亮度
    float     fMeanGV;          // 平均亮度
    float     fDiffGV;          // (背景-对象)亮度差异
    float     fBKGV;            // 背景亮度
    float     fStdDev;          // 标准偏差
    float     fSEMU;            // SEMU
    float     fCompactness;     // 客体有多接近圆的形状?
    float     nMinGVRatio;      // 对象最小亮度/背景亮度
    float     nMaxGVRatio;      // 对象最大亮度/背景亮度
    float     fDiffGVRatio;     // 对象的平均亮度/背景亮度
    float     fPerimeter;       // 外环线的长度
    float     fRoundness;       //
    float     fElongation;      // Box横向/纵向
    float     fMinBoxArea;      // Feret’s area
    float     fMinorAxis;       // 长轴 ( Feret’s Diameter )
    float     fMajorAxis;       // (Feret ' s diameter垂直的最长轴的长度/ Breath)
    float     fAxisRatio;       // 长轴/短轴
    float     fAngle;           // 横轴夹角(最小秒矩轴)
    float     fMinBoxRatio;     // Min Bounding Box面积比例/对象面积(Area porosity)
    float     fMeanAreaRatio;   // choikwangil

    float fAreaPer;    // choikwangil 04.20
    long  nJudge_GV;   // choikwangil 04.20
    long  nIn_Count;   // choikwangil 04.20

    __int64 nHist[IMAGE_MAX_GV];   // 客体直方图

    cv::Size2f BoxSize;   // Box width and length

    std::vector<cv::Point> ptIndexs;     // Blob像素坐标
    std::vector<cv::Point> ptContours;   // Blob外围坐标
    bool                   fromAI;       // 表明它是否来自AI算法
    double                 confidence;   // 置信度
    int                    AICode;       // the AI result item classfication code

    // 구조체 초기화
    tBLOB_FEATURE()
        : nArea(0)
    {
        bFiltering = false;

        rectBox        = cv::Rect(0, 0, 0, 0);
        nArea          = 0;
        nBoxArea       = 0;
        fBoxRatio      = 0.0f;
        nSumGV         = 0;
        nMinGV         = 0;
        nMaxGV         = 0;
        fMeanGV        = 0.0f;
        fDiffGV        = 0.0f;
        fBKGV          = 0.0f;
        fStdDev        = 0.0f;
        fSEMU          = 0.0f;
        fCompactness   = 0.0f;
        nMinGVRatio    = 0.0f;
        nMaxGVRatio    = 0.0f;
        fDiffGVRatio   = 0.0f;
        fPerimeter     = 0.0f;
        fRoundness     = 0.0f;
        fElongation    = 0.0f;
        fMinBoxArea    = 0.0f;
        fMinorAxis     = 0.0f;
        fMajorAxis     = 0.0f;
        fAxisRatio     = 0.0f;
        fAngle         = 0.0f;
        fMinBoxRatio   = 0.0f;
        fMeanAreaRatio = 0.0f;   // choikwangil
        fAreaPer       = 0.0f;   // choikwangil 04.20
        nJudge_GV      = 0;      // choikwangil 04.20
        nIn_Count      = 0;      // choikwangil 04.20
        memset(nHist, 0, sizeof(__int64) * IMAGE_MAX_GV);
        ptCenter = cv::Point2f(0, 0);
        BoxSize  = cv::Size2f(0, 0);
        // 矢量初始化
        std::vector<cv::Point>().swap(ptIndexs);
        std::vector<cv::Point>().swap(ptContours);
    }
};

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END

#endif   // NAONAO_VBLOBDEFINE_H