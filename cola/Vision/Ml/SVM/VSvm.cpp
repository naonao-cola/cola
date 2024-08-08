/**
 * @FilePath     : /cola/cola/Vision/Ml/SVM/VSvm.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-18 19:11:03
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-08-08 17:57:28
 **/
#include "VSvm.h"
#include "../../../UtilsCtrl/FileSystem/UFs.h"
#include <utility>

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

void setDefaultParam()
{
    default_param_.svm_type    = C_SVC;
    default_param_.kernel_type = RBF;
    default_param_.degree      = 10;
    default_param_.gamma       = 0.03;
    default_param_.coef0       = 1.0;
    default_param_.nu          = 0.005;
    default_param_.cache_size  = 1000;
    default_param_.C           = 32.0;
    default_param_.eps         = 1e-4;
    default_param_.p           = 1.0;
    default_param_.nr_weight   = 0;
    default_param_.shrinking   = 1;
    // 后面添加,Release训练时需放开,否则SVM置信度为0
    default_param_.probability = 1;
}

VSvm::VSvm(cv::Size train_size)
    : svm_(nullptr)
    , train_size_(train_size)
{
    setDefaultParam();
}

VSvm::VSvm()
    : svm_(nullptr)
{
    setDefaultParam();
}

VSvm::~VSvm(void)
{
    if (svm_ != nullptr) {
        svm_free_model_content(svm_);
    }
}

bool VSvm::init(std::string svm_model_path, std::string model_name)
{
    base_path_  = std::move(svm_model_path);
    model_name_ = std::move(model_name);
    svm_        = svm_load_model(std::string(base_path_ + "\\" + model_name_).c_str());
    return svm_ != nullptr;
}

void VSvm::getFileList(const std::string& filePath, const int& lable, const std::string& format /*= ".jpg"*/)
{
    std::vector<std::string> file_list = nao::UFs::getAllFormatFiles(filePath, format);
    for (size_t i = 0; i < file_list.size(); i++) {
        train_data_.emplace_back(std::make_pair(lable, file_list[i]));
    }
}

void VSvm::readTestFileList(const std::string& filePath, const std::string& format /*= ".jpg"*/)
{
    std::vector<std::string> file_list = nao::UFs::getAllFormatFiles(filePath, format);
    for (size_t i = 0; i < file_list.size(); i++) {
        test_img_list_.emplace_back(file_list[i]);
    }
}

void VSvm::processHogFeature(const cv::HOGDescriptor& hog)
{
    size_t train_img_size = train_data_.size();
    if (train_img_size <= 1) {
        throw std::logic_error("样本数量太少");
        exit(0);
    }
    // 创建数据存库空间
    label_mat_ = cv::Mat(static_cast<int>(train_img_size), 1, CV_32FC1, cv::Scalar(0));
    data_mat_  = cv::Mat(static_cast<int>(train_img_size), static_cast<int>(hog.getDescriptorSize()), CV_32FC1, cv::Scalar(0));

    for (int i = 0; i < train_img_size; i++) {
        cv::Mat src = cv::imread(train_data_[i].second, 0);
        if (src.empty()) {
            continue;
        }
        cv::Mat train_img;
        cv::resize(src, train_img, train_size_);
        // 结果数组
        std::vector<float> descriptors;
        descriptors.resize(hog.getDescriptorSize());
        try {
            hog.compute(train_img, descriptors, cv::Size(1, 1), cv::Size(0, 0));
        }
        catch (const cv::Exception& e) {
            std::cerr << "An error occurred when compute hog:" << e.what() << std::endl;
        }
        for (int j = 0; j < descriptors.size(); j++) {
            float* ptr = data_mat_.ptr<float>(i);
            ptr[j]     = descriptors[j];
        }
        label_mat_.ptr<float>(i)[0] = static_cast<float>(train_data_[i].first);
    }
}

void VSvm::trainLibSVM(svm_parameter& param /*= _default_param*/, const int& used /*= 0*/)
{
    // svm_prob读取
    if (used == 0) {
        param = default_param_;
    }
    svm_problem svm_prob;
    int         sample_size    = data_mat_.rows;
    int         feature_length = data_mat_.cols;
    // 准备空间
    svm_prob.l = sample_size;
    // 标签空间
    svm_prob.y = new double[sample_size];
    // 标签赋值
    for (int i = 0; i < sample_size; i++) {
        float* ptr    = data_mat_.ptr<float>(i);
        svm_prob.y[i] = ptr[0];
    }
    // 每个样本的特征向量
    svm_prob.x = new svm_node*[sample_size];
    // 为每一个特征向量赋值
    for (int i = 0; i < sample_size; i++) {
        // 每一个特征向量的维度
        svm_node* x_sapce = new svm_node[feature_length];
        float*    ptr     = data_mat_.ptr<float>(i);
        // 为每个向量的每一个维度赋值
        for (int j = 0; j < feature_length; j++) {
            x_sapce[j].index = j;
            x_sapce[j].value = ptr[j];
        }
        // 注意，结束符号，一开始忘记加了
        x_sapce[feature_length].index = -1;
        svm_prob.x[i]                 = x_sapce;
    }
    // 训练模型
    svm_model* svm_model = svm_train(&svm_prob, &param);
    // 保存路径
    std::string path = base_path_ + "\\" + model_name_;
    svm_save_model(path.c_str(), svm_model);
    // 删除申请的内存
    for (int i = 0; i < svm_prob.l; i++) {
        delete[] svm_prob.x[i];
    }
    delete[] svm_prob.x;
    delete[] svm_prob.y;
    // 释放模型
    svm_free_model_content(svm_model);
}

void VSvm::write(std::string file)
{
    int           sample_size    = data_mat_.rows;
    int           feature_length = data_mat_.cols;
    std::ofstream fout;
    fout.open(file, std::ios::binary);
    for (int i = 0; i < sample_size; i++) {
        std::string line = std::to_string(static_cast<int>(*label_mat_.ptr<float>(i)));
        float*      ptr  = data_mat_.ptr<float>(i);
        for (int j = 0; j < feature_length; j++) {
            line = line + " " + std::to_string(j + 1) + ":";
            line = line + std::to_string(ptr[j]);
        }
        fout << line << std::endl;
    }
    fout.close();
}

void VSvm::test_cross()
{
    nao::NAO_ECHO("enter test_cross start");
    svm_problem   svm_prob;
    svm_parameter param          = default_param_;
    int           sample_size    = data_mat_.rows;
    int           feature_length = data_mat_.cols;
    svm_prob.l                   = sample_size;
    svm_prob.y                   = new double[sample_size];
    for (int i = 0; i < sample_size; i++) {
        float* ptr    = label_mat_.ptr<float>(i);
        svm_prob.y[i] = ptr[0];
    }
    svm_prob.x = new svm_node*[sample_size];
    for (int i = 0; i < sample_size; i++) {
        svm_node* x_sapce = new svm_node[feature_length];
        float*    ptr     = data_mat_.ptr<float>(i);
        for (int j = 0; j < feature_length; j++) {
            x_sapce[j].index = j;
            x_sapce[j].value = ptr[j];
        }
        x_sapce[feature_length].index = -1;
        svm_prob.x[i]                 = x_sapce;
    }
    nao::NAO_ECHO("test_cross start");
    double* target = new double[sample_size];
    svm_cross_validation(&svm_prob, &param, 9, target);
    nao::NAO_ECHO("test_cross end");
    int    total_correct = 0;
    double total_error   = 0;
    double sumv          = 0;
    double sumy          = 0;
    double sumvv         = 0;
    double sumyy         = 0;
    double sumvy         = 0;
    if (param.svm_type == EPSILON_SVR || param.svm_type == NU_SVR) {
        for (int i = 0; i < svm_prob.l; i++) {
            double y = svm_prob.y[i];
            double v = target[i];
            total_error += (v - y) * (v - y);
            sumv += v;
            sumy += y;
            sumvv += v * v;
            sumyy += y * y;
            sumvy += v * y;
        }
        printf("Cross Validation Mean squared error = %g\n", total_error / svm_prob.l);
        printf("Cross Validation Squared correlation coefficient = %g\n",
               ((svm_prob.l * sumvy - sumv * sumy) * (svm_prob.l * sumvy - sumv * sumy)) / ((svm_prob.l * sumvv - sumv * sumv) * (svm_prob.l * sumyy - sumy * sumy)));
    }
    else {
        for (int i = 0; i < svm_prob.l; i++)
            if (target[i] == svm_prob.y[i]) {
                ++total_correct;
            }
        printf("Cross Validation Accuracy = %g%%\n", 100.0 * total_correct / svm_prob.l);
    }
    nao::NAO_ECHO("test_cross Validation end");
    for (int i = 0; i < svm_prob.l; i++) {
        delete[] svm_prob.x[i];
    }
    delete[] svm_prob.x;
    delete[] svm_prob.y;
    delete[] target;
}
double VSvm::testLibSVM(const cv::Mat& src, const cv::HOGDescriptor& hog, double prob_estimates[])
{
    if (src.empty()) {
        return -1;
    }
    cv::Mat test_img;
    cv::resize(src, test_img, train_size_);
#ifdef NDEBUG
    // 什么也不做
    ((void)0);
#else
    cv::imshow("input img", test_img);
    cv::waitKey(0);
#endif   // _DEBUG
    std::vector<float> descriptors;
    descriptors.resize(hog.getDescriptorSize());
    hog.compute(test_img, descriptors, cv::Size(1, 1), cv::Size(0, 0));
    svm_node* inputVector = new svm_node[descriptors.size() + 1];
    for (int i = 0; i < descriptors.size(); i++) {
        inputVector[i].index = i;
        inputVector[i].value = descriptors[i];
    }
    inputVector[descriptors.size()].index = -1;
    double resultLabel                    = svm_predict_probability(svm_, inputVector, prob_estimates);   // 分类结果
    delete[] inputVector;
    return resultLabel;
}

void VSvm::test(const cv::HOGDescriptor& hog)
{
    int         errorCount = 0;
    std::string path       = base_path_ + "\\" + model_name_;
    svm_                   = svm_load_model(path.c_str());
    for (int i = 0; i < train_data_.size(); i++) {
        cv::Mat src = cv::imread(train_data_[i].second, 0);
        if (src.empty()) {
            continue;
        }
        double prob[2] = {0};
        int    ret     = (int)testLibSVM(src, hog, prob);
        if (ret != train_data_[i].first) {
            errorCount++;
            if (train_data_[i].first == 0) {
                cv::imwrite(base_path_ + "\\error\\0\\" + std::to_string(i) + ".jpg", src);
            }
            else if (train_data_[i].first == 1) {
                cv::imwrite(base_path_ + "\\error\\1\\" + std::to_string(i) + ".jpg", src);
            }
        }
    }
    double errorPercentage = 0.0;
    errorPercentage        = errorCount / (train_data_.size() * 1.0);
    std::cout << "Error rate：" << errorPercentage << std::endl;
    std::cout << "Accuracy: " << 1 - errorPercentage << std::endl;
}

/*-----------------------------------------------------------------------------------------------------------*/
void VSvm::addFeatureLabel(const cv::Mat& feature, const std::vector<int>& label)
{
    if (feature.rows != label.size()) {
        throw std::runtime_error("训练图片的数量与标签数量不一致");
        exit(0);
    }
    int sample_length = feature.rows;
    for (int i = 0; i < sample_length; i++) {
        // 包含左边界，不包含右边界，取出每一个样本的特征值
        cv::Mat single_sample = feature.rowRange(i, i + 1).clone();
        train_feature_data_.emplace_back(std::make_pair(label[i], single_sample));
    }
}

void VSvm::copyFeatureLabel()
{
    std::cout << "正在暂存区中拷贝特征值与标签" << std::endl;
    int sample_length = static_cast<int>(train_feature_data_.size());
    if (sample_length <= 1) {
        throw std::logic_error("样本数量太少");
        exit(0);
    }
    //_data_mat = cv::Mat(sample_length, _train_feature_data[0].second.cols, CV_32FC1, cv::Scalar(0));
    label_mat_ = cv::Mat(static_cast<int>(sample_length), 1, CV_32FC1, cv::Scalar(0));
    for (int i = 0; i < sample_length; i++) {
        label_mat_.ptr<float>(i)[0] = static_cast<float>(train_feature_data_[i].first);
        data_mat_.push_back(train_feature_data_[i].second);
    }
    std::cout << "暂存区拷贝特征值与标签完成" << std::endl;
}

double VSvm::testFeatureLibSVM(const cv::Mat& feature, double prob_estimates[])
{
    if (feature.empty()) {
        throw std::runtime_error("输入的特征图为空");
        exit(0);
    }
    int       feature_length = feature.cols;
    svm_node* inputVector    = new svm_node[feature_length + 1];
    for (int i = 0; i < feature_length; i++) {
        inputVector[i].index = i;
        inputVector[i].value = feature.ptr<float>(0)[i];
    }
    inputVector[feature_length].index = -1;
    double resultLabel                = svm_predict_probability(svm_, inputVector, prob_estimates);   // 分类结果
    delete[] inputVector;
    return resultLabel;
}

void VSvm::test()
{
    std::cout << "使用暂存区的样本进行测试" << std::endl;
    int         errorCount = 0;
    std::string path       = base_path_ + "\\" + model_name_;
    svm_                   = svm_load_model(path.c_str());
    std::vector<int> ret_vec;
    for (int i = 0; i < train_feature_data_.size(); i++) {
        if (train_feature_data_[i].second.empty())
            continue;
        double  prob[2] = {0};
        cv::Mat imput   = train_feature_data_[i].second.clone();
        int     ret     = (int)testFeatureLibSVM(imput, prob);
        ret_vec.emplace_back(ret);
        if (ret != train_feature_data_[i].first) {
            errorCount++;
        }
    }
    double errorPercentage = 0.0;
    errorPercentage        = errorCount / (train_feature_data_.size() * 1.0);
    std::cout << "Error rate：" << errorPercentage << std::endl;
    std::cout << "Accuracy: " << 1 - errorPercentage << std::endl;
}

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END