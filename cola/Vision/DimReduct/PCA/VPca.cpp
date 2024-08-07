/**
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

cv::Mat VPCA::reduce(const cv::Mat& data, int dimension, double Retained)
{
    // 1. 计算均值
    cv::Mat mean;
    cv::reduce(data, mean, 0, cv::REDUCE_AVG);
    // 2. 数据准备：数据中心化
    cv::Mat data_mean = data.clone();
    for (int i = 0; i < data.rows; i++) {
        data_mean.row(i) -= mean;
    }
    // 3. PCA计算，构建pca的时候使用中心化数据，投影的时候不需要
    cv::PCA pca(data_mean, cv::Mat(), cv::PCA::DATA_AS_ROW, dimension);
    // 4. 获取特征值和特征向量
    eigenvalues_  = pca.eigenvalues;
    eigenvectors_ = pca.eigenvectors;
    eigenmean_    = pca.mean;
    // 5. 计算信息保留的比例
    float totalVariance    = cv::sum(eigenvalues_)[0];
    float varianceRetained = 0.0f;
    int   k                = 0;   // 保留的主成分数量
    for (int i = 0; i < eigenvalues_.rows; i++) {
        eigenvecWithVal_.push_back({eigenvalues_.at<float>(i), eigenvectors_.row(i)});
        varianceRetained += eigenvalues_.at<float>(i);
        if (varianceRetained / totalVariance >= Retained) {   // 保留90%的信息
            k = i + 1;
            break;
        }
    }
    std::sort(eigenvecWithVal_.begin(), eigenvecWithVal_.end(), compareEigenvectors);
    cv::Mat dst = pca.project(data);   // 映射新空间
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
    cv::Mat singleVector    = data.clone();
    cv::Mat projectedVector = pca.project(singleVector);
    return projectedVector;
}
NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END