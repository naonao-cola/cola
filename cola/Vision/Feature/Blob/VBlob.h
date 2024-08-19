/**
 * @FilePath     : /cola/cola/Vision/Feature/Blob/VBlob.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-31 11:24:32
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-31 17:23:28
 * @Copyright (c) 2024 by G, All Rights Reserved.
 **/
#ifndef NAONAO_VBLOB_H
#define NAONAO_VBLOB_H

#include "../../VisionObject.h"
#include "VBlobDefine.h"

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

class VBlob : public VisionObject
{
public:
    VBlob();
    ~VBlob() override;
    NVoid Release();

    // 运行Blob算法
    NBool DoBlobCalculate(cv::Mat ThresholdBuffer, cv::Mat GrayBuffer = cv::Mat(), NInt nMaxDefectCount = 99999);
    NBool DoBlobCalculate(cv::Mat ThresholdBuffer, cv::Rect rectROI, cv::Mat GrayBuffer = cv::Mat(), NInt nMaxDefectCount = 99999);
    // 坐标校正
    NVoid CoordApply(cv::Rect rectROI, NInt nTotalLabel);
    NBool DoFeatureBasic_8bit(cv::Mat& matLabel, cv::Mat& matStats, cv::Mat& matCentroid, cv::Mat& GrayBuffer, NInt nTotalLabel);
    NBool DoFeatureBasic_16bit(cv::Mat& matLabel, cv::Mat& matStats, cv::Mat& matCentroid, cv::Mat& GrayBuffer, NInt nTotalLabel);

    std::vector<tBLOB_FEATURE> DoDefectBlobSingleJudgment(const std::vector<STRU_DEFECT_ITEM>& EngineerBlockDefectJudge);
    NBool                       DoFiltering(tBLOB_FEATURE& tBlobResult, NInt nBlobFilter, NInt nSign, NDouble dValue);
    NBool                       Compare(NDouble dFeatureValue, NInt nSign, NDouble dValue);

protected:
    NBool                       bComplete_;    // 确认Blob是否已完成。
    std::vector<tBLOB_FEATURE> BlobResult_;   // Blob结果列表
};   // class VBlob

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END

#endif