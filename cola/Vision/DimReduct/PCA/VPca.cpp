﻿/**
 * @FilePath     : /cola/cola/Vision/DimReduct/PCA/VPca.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-08-02 16:23:00
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-08-06 17:50:07
 * @Copyright (c) 2024 by G, All Rights Reserved.
 **/
#include "VPca.h"
NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

bool compareEigenvectors(const VPCA::Eigenvector& a, const VPCA::Eigenvector& b)
{
    return a.eigenvalue > b.eigenvalue;   // 降序排序
}

cv::Mat VPCA::reduce(const cv::Mat& data, NInt dimension, NDouble Retained)
{
    // 1. 计算均值
    cv::Mat mean;
    cv::reduce(data, mean, 0, cv::REDUCE_AVG);
    // 2. 数据准备：数据中心化
    // cv::Mat data_mean = data.clone();
    // for (NInti = 0; i < data.rows; i++) {
    //    data_mean.row(i) -= mean;
    //}
    // 3. PCA计算，构建pca的时候使用中心化数据，投影的时候不需要
    cv::PCA pca(data, mean, cv::PCA::DATA_AS_ROW, dimension);
    // 4. 获取特征值和特征向量
    eigenvalues_  = pca.eigenvalues;
    eigenvectors_ = pca.eigenvectors;
    eigenmean_    = pca.mean;
    // 5. 计算信息保留的比例
    NFloat totalVariance    = cv::sum(eigenvalues_)[0];
    NFloat varianceRetained = 0.0f;
    NInt  k                = 0;   // 保留的主成分数量
    for (NInt i = 0; i < eigenvalues_.rows; i++) {
        eigenvecWithVal_.push_back({eigenvalues_.at<NFloat>(i), eigenvectors_.row(i)});
        varianceRetained += eigenvalues_.at<NFloat>(i);
        if (varianceRetained / totalVariance >= Retained) {   // 保留90%的信息
            k = i + 1;
            break;
        }
    }
    std::sort(eigenvecWithVal_.begin(), eigenvecWithVal_.end(), compareEigenvectors);
    cv::Mat dst      = pca.project(data);      // 映射新空间
    cv::Mat back_mat = pca.backProject(dst);   // 反映射回来
    return dst;
}

void VPCA::write(std::string path)
{
    cv::FileStorage f_pca(path, cv::FileStorage::WRITE);
    f_pca << "mean" << eigenmean_;
    f_pca << "eigenvalues" << eigenvalues_;
    f_pca << "eigenvectors" << eigenvectors_;
    f_pca.release();
}

void VPCA::read(std::string path)
{
    cv::FileStorage f_pca(path, cv::FileStorage::READ);
    f_pca["mean"] >> eigenmean_;
    f_pca["eigenvalues"] >> eigenvalues_;
    f_pca["eigenvectors"] >> eigenvectors_;
    f_pca.release();
}

cv::Mat VPCA::reduce_single(const cv::Mat& data)
{
    cv::PCA pca;
    pca.mean                = eigenmean_;
    pca.eigenvalues         = eigenvalues_;
    pca.eigenvectors        = eigenvectors_;
    cv::Mat projectedVector = pca.project(data);
    return projectedVector;
}
NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END