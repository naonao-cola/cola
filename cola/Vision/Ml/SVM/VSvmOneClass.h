/**

 * @FilePath     : /cola/cola/Vision/Ml/SVM/VSvmOneClass.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-09-19 13:11:06
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-09-19 13:16:10
 * @Copyright (c) 2024 by G, All Rights Reserved.
**/

#ifndef NAO_VSVMONECLASS_H
#define NAO_VSVMONECLASS_H

#include "../../VisionObject.h"

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

#include "svm.h"

extern "C" {
#include "svm.h"   // From the VLFeat C library
}

class SvmOne : public VisionObject
{
public:
    explicit SvmOne(const std::string& filename = "");

    ~SvmOne() override;

    void param_init(int svm_type = C_SVC, int kernel_type = RBF, int degree = 3, double gamma = 0, double coef0 = 0, double nu = 0.5, double C = 1, double eps = 1e-3, double cache_size = 200,
                    double p = 0.1, int shrinking = 1, int probability = 0, const std::vector<std::pair<int, double>>& nr_weight = {});

    void one_class_svm_param_init();

    void read_problem(cv::Mat& dataset, const std::vector<double>& label);

    double train(cv::Mat& dataset, const std::vector<double>& label = {}, int nr_fold = 5);

    std::pair<double, double> predict(svm_node* svm_node_data);

    std::pair<double, double> predict(const cv::Mat& data);

    int load_model(const std::string& model_path);

    int save_model(const std::string& model_path);

    double clf_validation(const cv::Mat& dataset, const std::vector<double>& label = {});

    double cross_validation(int nr_fold);

    void free_model();

    bool free_param();

    bool free_dataset();

private:
    struct svm_model* model_;
    struct svm_parameter param_
    {
    };
    struct svm_problem prob_
    {
    };
    struct svm_node* x_space_;
};

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END

#endif   // NAO_VSVMONECLASS_H