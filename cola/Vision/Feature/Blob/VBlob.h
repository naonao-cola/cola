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
    void Release();

    // 运行Blob算法
    bool DoBlobCalculate(cv::Mat ThresholdBuffer, cv::Mat GrayBuffer = cv::Mat(), int nMaxDefectCount = 99999);
    bool DoBlobCalculate(cv::Mat ThresholdBuffer, cv::Rect rectROI, cv::Mat GrayBuffer = cv::Mat(), int nMaxDefectCount = 99999);
    // 坐标校正
    void CoordApply(cv::Rect rectROI, int nTotalLabel);
    bool DoFeatureBasic_8bit(cv::Mat& matLabel, cv::Mat& matStats, cv::Mat& matCentroid, cv::Mat& GrayBuffer, int nTotalLabel);
    bool DoFeatureBasic_16bit(cv::Mat& matLabel, cv::Mat& matStats, cv::Mat& matCentroid, cv::Mat& GrayBuffer, int nTotalLabel);

    std::vector<tBLOB_FEATURE> DoDefectBlobSingleJudgment(const std::vector<STRU_DEFECT_ITEM>& EngineerBlockDefectJudge);
    bool                       DoFiltering(tBLOB_FEATURE& tBlobResult, int nBlobFilter, int nSign, double dValue);
    bool                       Compare(double dFeatureValue, int nSign, double dValue);

protected:
    bool                       bComplete_;    // 确认Blob是否已完成。
    std::vector<tBLOB_FEATURE> BlobResult_;   // Blob结果列表
};   // class VBlob

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END

#endif