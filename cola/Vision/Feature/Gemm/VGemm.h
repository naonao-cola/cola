/**
 * @FilePath     : /cola/cola/Vision/Feature/Gemm/VGemm.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-18 16:45:32
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-18 16:45:32
 **/
#ifndef NAONAO_VGEMM_H
#define NAONAO_VGEMM_H

/*
https://blog.csdn.net/m0_37772527/article/details/105712693
https://blog.csdn.net/qq_37059483/article/details/78292869
*/

#include "../../VisionObject.h"

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

struct GGCMFeatures;

class VGemm : public VisionObject
{
public:
    typedef std::vector<std::vector<NInt>> VecGGCM;
    VGemm();
    ~VGemm() override;

private:
    NInt grayLevel_;   // 将灰度共生矩阵划分为 grayLevel 个等级

public:
    /**
     * @brief vecGLCM,要进行初始化的共生矩阵,为二维方阵size, 二维矩阵的大小,必须与图像划分的灰度等级相等
     * @param vecGGCM
     * @param size
     */
    NVoid initGGCM(VecGGCM& vecGGCM, NInt size = 16);

    /**
     * @brief 计算灰度-梯度共生矩阵
     * @param inputImg
     * @param vecGGCM
     * @param tempVec_Gray
     * @param tempVec_Gradient
     */
    NVoid calGGCM(cv::Mat& inputImg, VecGGCM& vecGGCM /*, VecGGCM& tempVec_Gray, VecGGCM& tempVec_Gradient*/);

    /**
     * @brief 计算特征值
     * @param vecGGCM
     * @param features
     */
    NVoid getGGCMFeatures(VecGGCM& vecGGCM, GGCMFeatures& features);
};

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END

#endif   // NAONAO_VGEMM_H