﻿/**
 * @FilePath     : /cola/cola/Vision/Ml/SVM/VSvm.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-18 19:10:54
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-20 12:18:40
 **/
#ifndef NAONAO_VSVM_H
#define NAONAO_VSVM_H

#include "../../VisionObject.h"
NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

extern "C" {
#include "svm.h"   // From the VLFeat C library
}

static svm_parameter default_param_;

class VSvm : public VisionObject
{
public:
    explicit VSvm(cv::Size train_size);
    VSvm();
    ~VSvm(void) override;

    /**
     * @brief 加载路径，初始化模型,模型与训练图片的上级在同一目录。
     * @param svm_model_path 模型的路径(不带模型名称与斜线)，
     * @param model_name  模型的名字
     * @return  模型加载失败返回false
     */
    bool init(std::string svm_model_path, std::string model_name);

    /**
     * @brief 获取训练图片的路径与标签，到成员变量
     * @param filePath  图片所在的路径
     * @param lable     图片的标签
     * @param format    图片的格式，默认为jpg格式
     */
    void getFileList(const std::string& filePath, const int& lable, const std::string& format = "(.*)(.png|jpg|bmp)");

    /**
     * @brief 读取测试图片到成员变量的测试图片路径。
     * @param filePath 图片所在路径
     * @param format   默认格式为jpg图片
     */
    void readTestFileList(const std::string& filePath, const std::string& format = "(.*)(.png|jpg|bmp)");

    /**
     * @brief 提取训练图片的hog特征,赋值到成员变量的数据中
     * @param img_size 训练图片的大小，图片缩放到统一大小的格式
     * @param hog      提取hog特征时，hog的大小
     * @other hog特征子的大小可以参考 https://blog.csdn.net/chaipp0607/article/details/70888899
     * @ 图片大小在 20 * 10时，hog大小为 cv::HOGDescriptor hog(Size(20, 10), Size(8, 4), Size(4, 2), Size(4, 2), 9);
     */
    void processHogFeature(const cv::HOGDescriptor& hog);

    /**
     * @brief 使用svm进行训练
     * @param param  有默认参数进行训练，也可以自己传参
     * @param used   使用默认参数的表示
     */
    void trainLibSVM(svm_parameter& param = default_param_, const int& used = 0);

    /**
     * @brief 用已有的模型检测图片
     * @param src 输入图片
     * @param train_size
     * @param hog
     * @param prob_estimates 分类概率
     * @return
     */
    double testLibSVM(const cv::Mat& src, const cv::HOGDescriptor& hog, double prob_estimates[]);

    /**
     * @brief 用样本进行测试模型,判断模型准确率
     * @param train_size 训练模型时的图片统一的大小
     * @param hog 训练模型时的hog大小
     */
    void test(const cv::HOGDescriptor& hog);

    /**
     * @brief 赋值特征值，与标签到成员变量。先添加到暂存变量
     * @param feature 别的工程提取到的特征值，很多的行向量，每一行代表一个样本的多维特征值，单通道 float型数据
     * @param lable   数据的标签。行向量所对应的标签
     */
    void addFeatureLabel(const cv::Mat& feature, const std::vector<int>& label);

    /**
     * @brief 从暂存区中取出特征值与标签，因为上面的addFeatureLabel函数是多次调用，所以需要使用暂存区
     */
    void copyFeatureLabel();

    /**
     * @brief 用其他的特征提取，进行svm识别
     * @param feature 样本的特征值，是一个行向量，一行多列，单通道 float型数据
     * @prob_estimates 分类的概率
     * @return         分类的类别
     */
    double testFeatureLibSVM(const cv::Mat& feature, double prob_estimates[]);

    /**
     * @brief 用其他的特征提取进行的svm检测，对样本正确率判别
     */
    void test();

private:
    // the list of train images  //the lable of train image
    std::vector<std::pair<int, std::string>> train_data_;
    // 其他方式提取的特征值与标签的暂存区
    std::vector<std::pair<int, cv::Mat>> train_feature_data_;
    // the list of test images
    std::vector<std::string> test_img_list_;
    // the path  and name of model
    std::string base_path_;
    std::string model_name_;
    // feature data and lable
    cv::Mat    data_mat_;
    cv::Mat    label_mat_;
    svm_model* svm_;
    cv::Size   train_size_;
};   // class VSvm


NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END

#endif   // NAONAO_VSVM_H
