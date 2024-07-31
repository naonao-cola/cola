/**
 * @FilePath     : /cola/cola/Vision/Feature/Blob/VBlob.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-31 14:02:49
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-31 16:20:53
 * @Copyright (c) 2024 by G, All Rights Reserved.
 **/
#include "VBlob.h"

#include <math.h>

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

VBlob::VBlob()
{
    bComplete_ = false;
    std::vector<tBLOB_FEATURE>().swap(BlobResult_);
}

VBlob::~VBlob()
{
    Release();
}
void VBlob::Release()
{
    bComplete_ = false;
    // 初始化向量
    if (BlobResult_.size() != 0) {
        for (int i = 0; i < BlobResult_.size(); i++) {
            std::vector<cv::Point>().swap(BlobResult_[i].ptIndexs);
            std::vector<cv::Point>().swap(BlobResult_[i].ptContours);
        }
        std::vector<tBLOB_FEATURE>().swap(BlobResult_);
    }
}

bool VBlob::DoBlobCalculate(cv::Mat ThresholdBuffer, cv::Mat GrayBuffer, int nMaxDefectCount)
{
    Release();
    // 如果没有画面,则返回
    if (ThresholdBuffer.empty()) {
        return false;
    }
    // 如果不是1频道
    if (ThresholdBuffer.channels() != 1) {
        return false;
    }
    // 如果Gray画面不存在X&1频道
    bool bGrayEmpty = false;
    if (GrayBuffer.empty() || GrayBuffer.channels() != 1) {
        GrayBuffer = ThresholdBuffer.clone();
        bGrayEmpty = true;
    }

    cv::Mat matLabel;
    cv::Mat matStats;
    cv::Mat matCentroid;
    matLabel = cv::Mat(ThresholdBuffer.size(), CV_32SC1);

    __int64 nTotalLabel = 0;
    if (ThresholdBuffer.type() == CV_8U) {
        nTotalLabel = cv::connectedComponentsWithStats(ThresholdBuffer, matLabel, matStats, matCentroid, 8, CV_32S, cv::CCL_GRANA) - 1;
    }
    else {
        cv::Mat matSrc8bit = cv::Mat(ThresholdBuffer.size(), CV_8UC1);
        ThresholdBuffer.convertTo(matSrc8bit, CV_8UC1, 1. / 16.);
        nTotalLabel = cv::connectedComponentsWithStats(matSrc8bit, matLabel, matStats, matCentroid, 8, CV_32S, cv::CCL_GRANA) - 1;
        matSrc8bit.release();
    }

    if (nTotalLabel < 0) {
        // 禁用内存
        if (bGrayEmpty) {
            GrayBuffer.release();
        }
        if (!matLabel.empty()) {
            matLabel.release();
        }
        if (!matStats.empty()) {
            matStats.release();
        }
        if (!matCentroid.empty()) {
            matCentroid.release();
        }
        return false;
    }
    // 异常处理
    if (nTotalLabel >= nMaxDefectCount) {
        nTotalLabel = nMaxDefectCount - 1;
    }

    // 默认Feature计算8bit
    if (GrayBuffer.type() == CV_8UC1) {
        DoFeatureBasic_8bit(matLabel, matStats, matCentroid, GrayBuffer, static_cast<int>(nTotalLabel));
    }
    else {
        DoFeatureBasic_16bit(matLabel, matStats, matCentroid, GrayBuffer, static_cast<int>(nTotalLabel));
    }

    if (bGrayEmpty) {
        GrayBuffer.release();
    }
    if (!matLabel.empty()) {
        matLabel.release();
    }
    if (!matStats.empty()) {
        matStats.release();
    }
    if (!matCentroid.empty()) {
        matCentroid.release();
    }
    bComplete_ = true;
    return true;
}

bool VBlob::DoFeatureBasic_8bit(cv::Mat& matLabel, cv::Mat& matStats, cv::Mat& matCentroid, cv::Mat& GrayBuffer, int nTotalLabel)
{
    if (nTotalLabel <= 0) {
        return true;
    }
    float fVal = 4.F * CV_PI;
    BlobResult_.resize(nTotalLabel);

#ifdef _DEBUG
#else
#    pragma omp parallel for
#endif
    for (int idx = 1; idx <= nTotalLabel; idx++) {
        int nBlobNum                            = idx - 1;
        BlobResult_.at(nBlobNum).rectBox.x      = matStats.at<int>(idx, cv::CC_STAT_LEFT);
        BlobResult_.at(nBlobNum).rectBox.y      = matStats.at<int>(idx, cv::CC_STAT_TOP);
        BlobResult_.at(nBlobNum).rectBox.width  = matStats.at<int>(idx, cv::CC_STAT_WIDTH);
        BlobResult_.at(nBlobNum).rectBox.height = matStats.at<int>(idx, cv::CC_STAT_HEIGHT);

        // 对象周围(用于背景GV)
        int nOffSet = 20;

        int nSX = BlobResult_.at(nBlobNum).rectBox.x - nOffSet;
        int nSY = BlobResult_.at(nBlobNum).rectBox.y - nOffSet;
        int nEX = BlobResult_.at(nBlobNum).rectBox.x + BlobResult_.at(nBlobNum).rectBox.width + nOffSet + nOffSet;
        int nEY = BlobResult_.at(nBlobNum).rectBox.y + BlobResult_.at(nBlobNum).rectBox.height + nOffSet + nOffSet;

        if (nSX < 0) {
            nSX = 0;
        }
        if (nSY < 0) {
            nSY = 0;
        }
        if (nSX >= GrayBuffer.cols) {
            continue;
        }
        if (nSY >= GrayBuffer.rows) {
            continue;
        }
        if (nEX >= GrayBuffer.cols) {
            nEX = GrayBuffer.cols - 1;
        }
        if (nEY >= GrayBuffer.rows) {
            nEY = GrayBuffer.rows - 1;
        }

        cv::Rect rectTemp(nSX, nSY, nEX - nSX + 1, nEY - nSY + 1);

        __int64 nCount_in  = 0;
        __int64 nCount_out = 0;
        __int64 nSum_in    = 0;   // defect区域
        __int64 nSum_out   = 0;   // defect排除区域

        cv::Mat matTmp_src   = GrayBuffer(rectTemp);   // 原始ROI
        cv::Mat matTmp_label = matLabel(rectTemp);     // Label的ROI
        cv::Mat matTemp      = cv::Mat(rectTemp.height, rectTemp.width, CV_8UC1);
        for (int y = 0; y < rectTemp.height; y++) {
            int*   ptrLabel = reinterpret_cast<int*>(matTmp_label.ptr(y));
            uchar* ptrGray  = matTmp_src.ptr(y);
            uchar* ptrTemp  = matTemp.ptr(y);
            for (int x = 0; x < rectTemp.width; x++, ptrLabel++, ptrGray++, ptrTemp++) {
                // 对象
                if (*ptrLabel == idx) {
                    nSum_in += *ptrGray;
                    nCount_in++;

                    // 在标签向量中存储像素坐标
                    BlobResult_.at(nBlobNum).ptIndexs.emplace_back(nSX + x, nSY + y);

                    *ptrTemp = static_cast<uchar>(255);

                    BlobResult_.at(nBlobNum).nHist[*ptrGray]++;
                }
                // 其他情况下背景
                else {
                    // 如果标签编号为0...
                    // 因为可以引用其他对象......添加条件
                    if (*ptrLabel == 0) {
                        nSum_out += *ptrGray;
                        nCount_out++;
                    }
                }
            }
        }

        // 亮度累计值
        BlobResult_.at(nBlobNum).nSumGV = nSum_in;
        // 对象面积
        BlobResult_.at(nBlobNum).nArea = nCount_in;   // matStats.at<int>(idx, CC_STAT_AREA);
        // Box Area
        BlobResult_.at(nBlobNum).nBoxArea = BlobResult_.at(nBlobNum).rectBox.width * BlobResult_.at(nBlobNum).rectBox.height;
        // Bounding Box面积比率/对象面积(Rectangulaty(=Extent))
        BlobResult_.at(nBlobNum).fBoxRatio = BlobResult_.at(nBlobNum).nArea / static_cast<float>(BlobResult_.at(nBlobNum).nBoxArea);
        // 拯救Elongation
        BlobResult_.at(nBlobNum).fElongation = BlobResult_.at(nBlobNum).rectBox.width / (float)BlobResult_.at(nBlobNum).rectBox.height;
        // 获取stdDev
        cv::Scalar m;
        cv::Scalar s;
        cv::meanStdDev(matTmp_src, m, s, matTemp);
        BlobResult_.at(nBlobNum).fStdDev = static_cast<float>(s[0]);
        // 拯救Contours
        std::vector<std::vector<cv::Point>> ptContours;
        std::vector<std::vector<cv::Point>>().swap(ptContours);
        cv::findContours(matTemp, ptContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

        // 获取Perimeter
        if (ptContours.size() != 0) {
            // ROI画面,需要校正
            // 复制轮廓坐标结果
            for (int m = 0; m < ptContours.size(); m++) {
                for (int k = 0; k < ptContours.at(m).size(); k++) {
                    BlobResult_.at(nBlobNum).ptContours.emplace_back(ptContours.at(m)[k].x + nSX, ptContours.at(m)[k].y + nSY);
                }
            }
        }
        else {
            // 因为是原始坐标,所以不用校正。
            // 复制轮廓坐标结果
            BlobResult_.at(nBlobNum).ptContours.resize(static_cast<int>(BlobResult_.at(nBlobNum).ptIndexs.size()));
            std::copy(BlobResult_.at(nBlobNum).ptIndexs.begin(), BlobResult_.at(nBlobNum).ptIndexs.end(), BlobResult_.at(nBlobNum).ptContours.begin());
        }
        BlobResult_.at(nBlobNum).fPerimeter = static_cast<float>(cv::arcLength(BlobResult_.at(nBlobNum).ptContours, true));
        std::vector<std::vector<cv::Point>>().swap(ptContours);
        // 获取Roundness
        BlobResult_.at(nBlobNum).fRoundness = (fVal * BlobResult_.at(nBlobNum).nArea) / (BlobResult_.at(nBlobNum).fPerimeter * BlobResult_.at(nBlobNum).fPerimeter);
        // 对象有多接近原样？(周长^2/4*Pi*面积)
        BlobResult_.at(nBlobNum).fCompactness = (BlobResult_.at(nBlobNum).fPerimeter * BlobResult_.at(nBlobNum).fPerimeter) / (fVal * float(BlobResult_.at(nBlobNum).nArea));
        // 获取Defect GV
        BlobResult_.at(nBlobNum).fMeanGV = nSum_in / static_cast<float>(nCount_in);
        // 获取背景GV
        BlobResult_.at(nBlobNum).fBKGV = nSum_out / static_cast<float>(nCount_out);
        // 求GV差值(背景-对象)
        BlobResult_.at(nBlobNum).fDiffGV = BlobResult_.at(nBlobNum).fBKGV - BlobResult_.at(nBlobNum).fMeanGV;

        // min,获取max GV
        double valMin = NAN;
        double valMax = NAN;
        cv::minMaxLoc(matTmp_src, &valMin, &valMax, 0, 0, matTemp);
        BlobResult_.at(nBlobNum).nMinGV = static_cast<long>(valMin);
        BlobResult_.at(nBlobNum).nMaxGV = static_cast<long>(valMax);
        // 对象最小亮度/对象平均亮度
        BlobResult_.at(nBlobNum).nMinGVRatio = BlobResult_.at(nBlobNum).nMinGV / BlobResult_.at(nBlobNum).fBKGV;
        // 对象最大亮度/对象平均亮度
        BlobResult_.at(nBlobNum).nMaxGVRatio = BlobResult_.at(nBlobNum).nMaxGV / BlobResult_.at(nBlobNum).fBKGV;
        // 背景亮度/对象平均亮度
        BlobResult_.at(nBlobNum).fDiffGVRatio = BlobResult_.at(nBlobNum).fMeanGV / BlobResult_.at(nBlobNum).fBKGV;
        // 获取Center Point
        BlobResult_.at(nBlobNum).ptCenter.x = static_cast<int>(matCentroid.at<double>(idx, 0));
        BlobResult_.at(nBlobNum).ptCenter.y = static_cast<int>(matCentroid.at<double>(idx, 1));
        // 拯救SEMU
        if (BlobResult_.at(nBlobNum).fDiffGV == 0.0) {
            if (BlobResult_.at(nBlobNum).fBKGV == 0) {
                BlobResult_.at(nBlobNum).fSEMU = 1.0 / (1.97F / (cv::pow(static_cast<float>(BlobResult_.at(nBlobNum).nArea), 0.33F) + 0.72F));
            }
            else {
                BlobResult_.at(nBlobNum).fSEMU = (0.000001 / BlobResult_.at(nBlobNum).fBKGV) / (1.97 / (cv::pow(static_cast<float>(BlobResult_.at(nBlobNum).nArea), 0.33F) + 0.72F));
            }
        }
        else {
            if (BlobResult_.at(nBlobNum).fBKGV == 0) {
                BlobResult_.at(nBlobNum).fSEMU =
                    (fabs(BlobResult_.at(nBlobNum).fMeanGV - BlobResult_.at(nBlobNum).fBKGV) / 0.000001) / (1.97 / (cv::pow((float)BlobResult_.at(nBlobNum).nArea, 0.33F) + 0.72F));
            }
            else {
                BlobResult_.at(nBlobNum).fSEMU = (fabs(BlobResult_.at(nBlobNum).fMeanGV - BlobResult_.at(nBlobNum).fBKGV) / BlobResult_.at(nBlobNum).fBKGV) /
                                                 (1.97 / (cv::pow((float)BlobResult_.at(nBlobNum).nArea, 0.33F) + 0.72F));
            }
        }
        cv::RotatedRect BoundingBox = cv::minAreaRect(BlobResult_.at(nBlobNum).ptIndexs);
        // Box width and length
        BlobResult_.at(nBlobNum).BoxSize = BoundingBox.size;
        // Angle between the horizontal axis
        BlobResult_.at(nBlobNum).fAngle = BoundingBox.angle;
        // Minor Axis & Major Axis
        if (BoundingBox.size.width > BoundingBox.size.height) {
            BlobResult_.at(nBlobNum).fMinorAxis = BoundingBox.size.width;
            BlobResult_.at(nBlobNum).fMajorAxis = BoundingBox.size.height;
        }
        else {
            BlobResult_.at(nBlobNum).fMinorAxis = BoundingBox.size.height;
            BlobResult_.at(nBlobNum).fMajorAxis = BoundingBox.size.width;
        }

        // Feret’s area
        BlobResult_.at(nBlobNum).fMinBoxArea = BlobResult_.at(nBlobNum).fMinorAxis * BlobResult_.at(nBlobNum).fMajorAxis;

        // Axis Ratio
        if (BlobResult_.at(nBlobNum).fMajorAxis > 0) {
            BlobResult_.at(nBlobNum).fAxisRatio = BlobResult_.at(nBlobNum).fMinorAxis / BlobResult_.at(nBlobNum).fMajorAxis;
        }
        else {
            BlobResult_.at(nBlobNum).fAxisRatio = 0.F;
        }

        // Min Bounding Box面积比/对象面积(区域孔隙率)
        BlobResult_.at(nBlobNum).fMinBoxRatio = BlobResult_.at(nBlobNum).fMinBoxArea / static_cast<float>(BlobResult_.at(nBlobNum).nArea);
        // choikwangil
        BlobResult_.at(nBlobNum).fMeanAreaRatio = BlobResult_.at(nBlobNum).fMeanGV / static_cast<float>(BlobResult_.at(nBlobNum).nArea);
        // 取消分配
        matTmp_src.release();
        matTmp_label.release();
        matTemp.release();
    }
    return true;
}

bool VBlob::DoFeatureBasic_16bit(cv::Mat& matLabel, cv::Mat& matStats, cv::Mat& matCentroid, cv::Mat& GrayBuffer, int nTotalLabel)
{
    // 如果有一个结果
    if (nTotalLabel <= 0) {
        return true;
    }
    float fVal = 4.F * CV_PI;
    BlobResult_.resize(nTotalLabel);

#ifdef _DEBUG
#else
#    pragma omp parallel for
#endif
    for (int idx = 1; idx <= nTotalLabel; idx++) {
        int nBlobNum = idx - 1;

        BlobResult_.at(nBlobNum).rectBox.x      = matStats.at<int>(idx, cv::CC_STAT_LEFT);
        BlobResult_.at(nBlobNum).rectBox.y      = matStats.at<int>(idx, cv::CC_STAT_TOP);
        BlobResult_.at(nBlobNum).rectBox.width  = matStats.at<int>(idx, cv::CC_STAT_WIDTH);
        BlobResult_.at(nBlobNum).rectBox.height = matStats.at<int>(idx, cv::CC_STAT_HEIGHT);

        // 对象周围(用于背景GV)
        int nOffSet = 20;

        int nSX = BlobResult_.at(nBlobNum).rectBox.x - nOffSet;
        int nSY = BlobResult_.at(nBlobNum).rectBox.y - nOffSet;
        int nEX = BlobResult_.at(nBlobNum).rectBox.x + BlobResult_.at(nBlobNum).rectBox.width + nOffSet + nOffSet;
        int nEY = BlobResult_.at(nBlobNum).rectBox.y + BlobResult_.at(nBlobNum).rectBox.height + nOffSet + nOffSet;

        if (nSX < 0) {
            nSX = 0;
        }
        if (nSY < 0) {
            nSY = 0;
        }
        if (nEX >= GrayBuffer.cols) {
            nEX = GrayBuffer.cols - 1;
        }
        if (nEY >= GrayBuffer.rows) {
            nEY = GrayBuffer.rows - 1;
        }

        cv::Rect rectTemp(nSX, nSY, nEX - nSX + 1, nEY - nSY + 1);

        __int64 nCount_in  = 0;
        __int64 nCount_out = 0;
        __int64 nSum_in    = 0;   // defect区域
        __int64 nSum_out   = 0;   // defect排除区域

        cv::Mat matTmp_src   = GrayBuffer(rectTemp);   // 原始ROI
        cv::Mat matTmp_label = matLabel(rectTemp);     // Label的ROI
        cv::Mat matTemp      = cv::Mat(rectTemp.height, rectTemp.width, CV_8UC1);

        for (int y = 0; y < rectTemp.height; y++) {
            int*    ptrLabel = reinterpret_cast<int*>(matTmp_label.ptr(y));
            ushort* ptrGray  = reinterpret_cast<ushort*>(matTmp_src.ptr(y));
            uchar*  ptrTemp  = matTemp.ptr(y);

            for (int x = 0; x < rectTemp.width; x++, ptrLabel++, ptrGray++, ptrTemp++) {
                // 对象
                if (*ptrLabel == idx) {
                    nSum_in += *ptrGray;
                    nCount_in++;
                    // 在标签向量中存储像素坐标
                    BlobResult_.at(nBlobNum).ptIndexs.emplace_back(nSX + x, nSY + y);
                    *ptrTemp = static_cast<uchar>(255);
                    BlobResult_.at(nBlobNum).nHist[static_cast<int>(*ptrGray)]++;
                }
                // 其他情况下背景
                else {
                    // 如果标签编号为0...
                    // 因为可以引用其他对象......添加条件
                    if (*ptrLabel == 0) {
                        nSum_out += *ptrGray;
                        nCount_out++;
                    }
                }
            }
        }
        // 亮度累计值
        BlobResult_.at(nBlobNum).nSumGV = nSum_in;
        // 对象面积
        BlobResult_.at(nBlobNum).nArea = nCount_in;   // matStats.at<int>(idx, CC_STAT_AREA);
        // Box Area
        BlobResult_.at(nBlobNum).nBoxArea = BlobResult_.at(nBlobNum).rectBox.width * BlobResult_.at(nBlobNum).rectBox.height;
        // Bounding Box面积比率/对象面积(Rectangulaty(=Extent))
        BlobResult_.at(nBlobNum).fBoxRatio = BlobResult_.at(nBlobNum).nArea / static_cast<float>(BlobResult_.at(nBlobNum).nBoxArea);
        // 拯救Elongation
        BlobResult_.at(nBlobNum).fElongation = BlobResult_.at(nBlobNum).rectBox.width / static_cast<float>(BlobResult_.at(nBlobNum).rectBox.height);
        // 获取stdDev
        cv::Scalar m;
        cv::Scalar s;
        cv::meanStdDev(matTmp_src, m, s, matTemp);
        BlobResult_.at(nBlobNum).fStdDev = static_cast<float>(s[0]);
        // 拯救Contours
        std::vector<std::vector<cv::Point>> ptContours;
        std::vector<std::vector<cv::Point>>().swap(ptContours);
        cv::findContours(matTemp, ptContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

        // 获取Perimeter
        if (ptContours.size() != 0) {
            // ROI画面,需要校正
            // 复制轮廓坐标结果
            for (int m = 0; m < ptContours.size(); m++) {
                for (int k = 0; k < ptContours.at(m).size(); k++) {
                    BlobResult_.at(nBlobNum).ptContours.push_back(cv::Point(ptContours.at(m)[k].x + nSX, ptContours.at(m)[k].y + nSY));
                }
            }
        }
        else {
            // 因为是原始坐标,所以不用校正。
            // 复制轮廓坐标结果
            BlobResult_.at(nBlobNum).ptContours.resize(static_cast<int>(BlobResult_.at(nBlobNum).ptIndexs.size()));
            std::copy(BlobResult_.at(nBlobNum).ptIndexs.begin(), BlobResult_.at(nBlobNum).ptIndexs.end(), BlobResult_.at(nBlobNum).ptContours.begin());
        }
        BlobResult_.at(nBlobNum).fPerimeter = static_cast<float>(cv::arcLength(BlobResult_.at(nBlobNum).ptContours, true));
        std::vector<std::vector<cv::Point>>().swap(ptContours);

        // 获取Roundness
        BlobResult_.at(nBlobNum).fRoundness = (fVal * BlobResult_.at(nBlobNum).nArea) / (BlobResult_.at(nBlobNum).fPerimeter * BlobResult_.at(nBlobNum).fPerimeter);

        // 对象有多接近原样？(周长^2/4*Pi*面积)
        BlobResult_.at(nBlobNum).fCompactness = (BlobResult_.at(nBlobNum).fPerimeter * BlobResult_.at(nBlobNum).fPerimeter) / (fVal * float(BlobResult_.at(nBlobNum).nArea));

        // 获取Defect GV
        BlobResult_.at(nBlobNum).fMeanGV = nSum_in / static_cast<float>(nCount_in);

        // 获取背景GV
        BlobResult_.at(nBlobNum).fBKGV = nSum_out / static_cast<float>(nCount_out);

        // 求GV差值(背景-对象)
        BlobResult_.at(nBlobNum).fDiffGV = BlobResult_.at(nBlobNum).fBKGV - BlobResult_.at(nBlobNum).fMeanGV;

        // min,获取max GV
        double valMin = NAN;
        double valMax = NAN;
        cv::minMaxLoc(matTmp_src, &valMin, &valMax, 0, 0, matTemp);
        BlobResult_.at(nBlobNum).nMinGV = static_cast<long>(valMin);
        BlobResult_.at(nBlobNum).nMaxGV = static_cast<long>(valMax);

        // 对象最小亮度/对象平均亮度
        BlobResult_.at(nBlobNum).nMinGVRatio = BlobResult_.at(nBlobNum).nMinGV / BlobResult_.at(nBlobNum).fBKGV;
        // 对象最大亮度/对象平均亮度
        BlobResult_.at(nBlobNum).nMaxGVRatio = BlobResult_.at(nBlobNum).nMaxGV / BlobResult_.at(nBlobNum).fBKGV;
        // 背景亮度/对象平均亮度
        BlobResult_.at(nBlobNum).fDiffGVRatio = BlobResult_.at(nBlobNum).fMeanGV / BlobResult_.at(nBlobNum).fBKGV;
        // 获取Center Point
        BlobResult_.at(nBlobNum).ptCenter.x = static_cast<int>(matCentroid.at<double>(idx, 0));
        BlobResult_.at(nBlobNum).ptCenter.y = static_cast<int>(matCentroid.at<double>(idx, 1));

        // 拯救SEMU
        if (BlobResult_.at(nBlobNum).fDiffGV == 0.0) {
            if (BlobResult_.at(nBlobNum).fBKGV == 0) {
                BlobResult_.at(nBlobNum).fSEMU = 1.0 / (1.97F / (cv::pow((float)BlobResult_.at(nBlobNum).nArea, 0.33F) + 0.72F));
            }
            else {
                BlobResult_.at(nBlobNum).fSEMU = (0.000001 / BlobResult_.at(nBlobNum).fBKGV) / (1.97 / (cv::pow((float)BlobResult_.at(nBlobNum).nArea, 0.33F) + 0.72F));
            }
        }
        else {
            if (BlobResult_.at(nBlobNum).fBKGV == 0) {
                BlobResult_.at(nBlobNum).fSEMU =
                    (fabs(BlobResult_.at(nBlobNum).fMeanGV - BlobResult_.at(nBlobNum).fBKGV) / 0.000001) / (1.97 / (cv::pow((float)BlobResult_.at(nBlobNum).nArea, 0.33F) + 0.72F));
            }
            else {
                BlobResult_.at(nBlobNum).fSEMU = (fabs(BlobResult_.at(nBlobNum).fMeanGV - BlobResult_.at(nBlobNum).fBKGV) / BlobResult_.at(nBlobNum).fBKGV) /
                                                 (1.97 / (cv::pow((float)BlobResult_.at(nBlobNum).nArea, 0.33F) + 0.72F));
            }
        }

        cv::RotatedRect BoundingBox = cv::minAreaRect(BlobResult_.at(nBlobNum).ptIndexs);
        // Box width and length
        BlobResult_.at(nBlobNum).BoxSize = BoundingBox.size;
        // Angle between the horizontal axis
        BlobResult_.at(nBlobNum).fAngle = BoundingBox.angle;
        // Minor Axis & Major Axis
        if (BoundingBox.size.width > BoundingBox.size.height) {
            BlobResult_.at(nBlobNum).fMinorAxis = BoundingBox.size.width;
            BlobResult_.at(nBlobNum).fMajorAxis = BoundingBox.size.height;
        }
        else {
            BlobResult_.at(nBlobNum).fMinorAxis = BoundingBox.size.height;
            BlobResult_.at(nBlobNum).fMajorAxis = BoundingBox.size.width;
        }

        // Feret’s area
        BlobResult_.at(nBlobNum).fMinBoxArea = BlobResult_.at(nBlobNum).fMinorAxis * BlobResult_.at(nBlobNum).fMajorAxis;

        // Axis Ratio
        if (BlobResult_.at(nBlobNum).fMajorAxis > 0) {
            BlobResult_.at(nBlobNum).fAxisRatio = BlobResult_.at(nBlobNum).fMinorAxis / BlobResult_.at(nBlobNum).fMajorAxis;
        }
        else {
            BlobResult_.at(nBlobNum).fAxisRatio = 0.F;
        }

        // Min Bounding Box面积比/对象面积(区域孔隙率)
        BlobResult_.at(nBlobNum).fMinBoxRatio = BlobResult_.at(nBlobNum).fMinBoxArea / static_cast<float>(BlobResult_.at(nBlobNum).nArea);
        // choikwangil
        BlobResult_.at(nBlobNum).fMeanAreaRatio = BlobResult_.at(nBlobNum).fMeanGV / static_cast<float>(BlobResult_.at(nBlobNum).nArea);
        // 取消分配
        matTmp_src.release();
        matTmp_label.release();
        matTemp.release();
    }
    return true;
}

bool VBlob::DoBlobCalculate(cv::Mat ThresholdBuffer, cv::Rect rectROI, cv::Mat GrayBuffer, int nMaxDefectCount)
{
    Release();
    // 如果没有画面,则返回
    if (ThresholdBuffer.empty()) {
        return false;
    }
    // 如果不是1频道
    if (ThresholdBuffer.channels() != 1) {
        return false;
    }
    // 如果Gray画面不存在X&1频道
    bool bGrayEmpty = false;
    if (GrayBuffer.empty() || GrayBuffer.channels() != 1) {
        GrayBuffer = ThresholdBuffer.clone();
        bGrayEmpty = true;
    }

    cv::Mat matLabel;
    cv::Mat matStats;
    cv::Mat matCentroid;
    matLabel            = cv::Mat(ThresholdBuffer.size(), CV_32SC1, false);
    __int64 nTotalLabel = 0;

    if (ThresholdBuffer.type() == CV_8U) {
        nTotalLabel = cv::connectedComponentsWithStats(ThresholdBuffer, matLabel, matStats, matCentroid, 8, CV_32S, cv::CCL_GRANA) - 1;
    }
    else {
        cv::Mat matSrc8bit = cv::Mat(ThresholdBuffer.size(), CV_8UC1);
        ThresholdBuffer.convertTo(matSrc8bit, CV_8UC1, 1. / 16.);
        nTotalLabel = cv::connectedComponentsWithStats(matSrc8bit, matLabel, matStats, matCentroid, 8, CV_32S, cv::CCL_GRANA) - 1;
        matSrc8bit.release();
    }
    // 异常处理
    if (nTotalLabel < 0) {
        // 禁用内存
        if (bGrayEmpty) {
            GrayBuffer.release();
        }
        if (!matLabel.empty()) {
            matLabel.release();
        }
        if (!matStats.empty()) {
            matStats.release();
        }
        if (!matCentroid.empty()) {
            matCentroid.release();
        }
        return false;
    }
    // 异常处理
    if (nTotalLabel >= nMaxDefectCount) {
        nTotalLabel = nMaxDefectCount - 1;
    }
    // 默认Feature计算8bit
    if (GrayBuffer.type() == CV_8UC1) {
        DoFeatureBasic_8bit(matLabel, matStats, matCentroid, GrayBuffer, static_cast<int>(nTotalLabel));
    }
    else {
        DoFeatureBasic_16bit(matLabel, matStats, matCentroid, GrayBuffer, static_cast<int>(nTotalLabel));
    }

    CoordApply(rectROI, nTotalLabel);
    // 禁用内存
    if (bGrayEmpty) {
        GrayBuffer.release();
    }
    if (!matLabel.empty()) {
        matLabel.release();
    }
    if (!matStats.empty()) {
        matStats.release();
    }
    if (!matCentroid.empty()) {
        matCentroid.release();
    }
    // Blob完成
    bComplete_ = true;
    return true;
}

// 坐标校正
void VBlob::CoordApply(cv::Rect rectROI, int nTotalLabel)
{
    for (int nBlobNum = 0; nBlobNum < nTotalLabel; nBlobNum++) {
        BlobResult_.at(nBlobNum).rectBox.x += rectROI.tl().x;
        BlobResult_.at(nBlobNum).rectBox.y += rectROI.tl().y;
        BlobResult_.at(nBlobNum).ptCenter.x += rectROI.tl().x;
        BlobResult_.at(nBlobNum).ptCenter.y += rectROI.tl().y;
        for (int idx = 0; idx < BlobResult_.at(nBlobNum).ptIndexs.size(); idx++) {
            BlobResult_.at(nBlobNum).ptIndexs[idx].x += rectROI.tl().x;
            BlobResult_.at(nBlobNum).ptIndexs[idx].y += rectROI.tl().y;
        }
        for (int idx = 0; idx < BlobResult_.at(nBlobNum).ptContours.size(); idx++) {
            BlobResult_.at(nBlobNum).ptContours[idx].x += rectROI.tl().x;
            BlobResult_.at(nBlobNum).ptContours[idx].y += rectROI.tl().y;
        }
    }
}

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END