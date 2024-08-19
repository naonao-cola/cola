/**
 * @FilePath     : /cola/cola/Vision/Ml/SVM/VSvm.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-18 19:11:03
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-08-19 16:10:22
 **/
#include "VSvm.h"
#include "../../../UtilsCtrl/FileSystem/UFs.h"
#include <utility>

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

#define Malloc(type, n) (type*)malloc((n) * sizeof(type))

void setDefaultParam()
{
    default_param_.svm_type    = C_SVC;
    default_param_.kernel_type = RBF;
    default_param_.degree      = 10;
    default_param_.gamma       = 0.03;
    default_param_.coef0       = 1.0;
    default_param_.nu          = 0.005;
    default_param_.cache_size  = 1024;
    default_param_.C           = 32.0;
    default_param_.eps         = 1e-4;
    default_param_.p           = 1.0;
    default_param_.nr_weight   = 0;
    default_param_.shrinking   = 1;
    default_param_.probability = 1;   // 后面添加,Release训练时需放开,否则SVM置信度为0
}

VSvm::VSvm(cv::Size train_size)
    : x_space_(nullptr)
    , svm_(nullptr)
    , train_size_(train_size)
{
    setDefaultParam();
    prob_.l = 0;
    prob_.y = nullptr;
    prob_.x = nullptr;
}

VSvm::VSvm()
    : svm_(nullptr)
    , x_space_(nullptr)
{
    setDefaultParam();
    prob_.l = 0;
    prob_.y = nullptr;
    prob_.x = nullptr;
}

VSvm::~VSvm(NVoid)
{
    free_model();
}

NBool VSvm::init(std::string svm_model_path, std::string model_name)
{
    base_path_  = std::move(svm_model_path);
    model_name_ = std::move(model_name);
    svm_        = svm_load_model(std::string(base_path_ + "\\" + model_name_).c_str());
    return svm_ != nullptr;
}

NVoid VSvm::getFileList(const std::string& filePath, const NInt& lable, const std::string& format)
{
    std::vector<std::string> file_list = nao::UFs::getAllFormatFiles(filePath, format);
    for (NSize i = 0; i < file_list.size(); i++) {
        train_data_.emplace_back(std::make_pair(lable, file_list[i]));
    }
}

NVoid VSvm::readTestFileList(const std::string& filePath, const std::string& format)
{
    std::vector<std::string> file_list = nao::UFs::getAllFormatFiles(filePath, format);
    for (NSize i = 0; i < file_list.size(); i++) {
        test_img_list_.emplace_back(file_list[i]);
    }
}

NVoid VSvm::processHogFeature(const cv::HOGDescriptor& hog)
{
    NSize train_img_size = train_data_.size();
    if (train_img_size <= 1) {
        throw std::logic_error("Sample size is too small");
        exit(0);
    }
    label_mat_ = cv::Mat(static_cast<NInt>(train_img_size), 1, CV_32FC1, cv::Scalar(0));   // 创建数据存库空间
    data_mat_  = cv::Mat(static_cast<NInt>(train_img_size), static_cast<NInt>(hog.getDescriptorSize()), CV_32FC1, cv::Scalar(0));
    for (NInt i = 0; i < train_img_size; i++) {
        cv::Mat src = cv::imread(train_data_[i].second, 0);
        if (src.empty()) {
            continue;
        }
        cv::Mat train_img;
        cv::resize(src, train_img, train_size_);
        std::vector<NFloat> descriptors;   // 结果数组
        descriptors.resize(hog.getDescriptorSize());
        try {
            hog.compute(train_img, descriptors, cv::Size(1, 1), cv::Size(0, 0));
        }
        catch (const cv::Exception& e) {
            std::cerr << "An error occurred when compute hog:" << e.what() << std::endl;
        }
        for (NInt j = 0; j < descriptors.size(); j++) {
            NFloat* ptr = data_mat_.ptr<NFloat>(i);
            ptr[j]      = descriptors[j];
        }
        label_mat_.ptr<NFloat>(i)[0] = static_cast<NFloat>(train_data_[i].first);
    }
}

NVoid VSvm::trainLibSVM(svm_parameter& param, const NInt& used)
{
    free_model();
    setDefaultParam();
    if (used == 0) {
        param = default_param_;
    }
    NLong len      = data_mat_.rows;
    NLong dim      = data_mat_.cols;
    NLong elements = len * dim;
    prob_.l        = len;
    prob_.y        = Malloc(NDouble, prob_.l);
    prob_.x        = Malloc(struct svm_node*, prob_.l);
    x_space_       = Malloc(struct svm_node, elements + len);
    NInt j        = 0;
    for (NInt l = 0; l < len; l++) {
        prob_.x[l] = &x_space_[j];
        for (NInt d = 0; d < dim; d++) {
            x_space_[j].index = d + 1;
            x_space_[j].value = *data_mat_.ptr<NFloat>(l, d);
            j++;
        }
        x_space_[j++].index = -1;
        prob_.y[l]          = *label_mat_.ptr<NFloat>(l);
    }
    svm_model*  svm_model = svm_train(&prob_, &param);
    std::string path      = base_path_ + "\\" + model_name_;
    svm_save_model(path.c_str(), svm_model);
}

NVoid VSvm::write(std::string file)
{
    NInt          sample_size    = data_mat_.rows;
    NInt          feature_length = data_mat_.cols;
    std::ofstream fout;
    fout.open(file, std::ios::binary);
    for (NInt i = 0; i < sample_size; i++) {
        std::string line = std::to_string(static_cast<NInt>(*label_mat_.ptr<NFloat>(i)));
        NFloat*     ptr  = data_mat_.ptr<NFloat>(i);
        for (NInt j = 0; j < feature_length; j++) {
            line = line + " " + std::to_string(j + 1) + ":";
            line = line + std::to_string(ptr[j]);
        }
        fout << line << std::endl;
    }
    fout.close();
}

NVoid VSvm::test_cross()
{
    free_model();
    setDefaultParam();
    svm_parameter param    = default_param_;
    NLong         len      = data_mat_.rows;
    NLong         dim      = data_mat_.cols;
    NLong         elements = len * dim;
    prob_.l                = len;
    prob_.y                = Malloc(NDouble, prob_.l);
    prob_.x                = Malloc(struct svm_node*, prob_.l);
    x_space_               = Malloc(struct svm_node, elements + len);
    NInt j                 = 0;
    for (NInt l = 0; l < len; l++) {
        prob_.x[l] = &x_space_[j];
        for (NInt d = 0; d < dim; d++) {
            x_space_[j].index = d + 1;
            x_space_[j].value = *data_mat_.ptr<NFloat>(l, d);
            j++;
        }
        x_space_[j++].index = -1;
        prob_.y[l]          = *label_mat_.ptr<NFloat>(l);
    }

    NDouble* target = Malloc(NDouble, prob_.l);
    svm_cross_validation(&prob_, &param, 7, target);
    NInt    total_correct = 0;
    NDouble total_error   = 0;
    NDouble sumv          = 0;
    NDouble sumy          = 0;
    NDouble sumvv         = 0;
    NDouble sumyy         = 0;
    NDouble sumvy         = 0;
    if (param.svm_type == EPSILON_SVR || param.svm_type == NU_SVR) {
        for (NInt i = 0; i < prob_.l; i++) {
            NDouble y = prob_.y[i];
            NDouble v = target[i];
            total_error += (v - y) * (v - y);
            sumv += v;
            sumy += y;
            sumvv += v * v;
            sumyy += y * y;
            sumvy += v * y;
        }
        nao::NAO_ECHO("Cross Validation Mean squared error = %g\n", total_error / prob_.l);
        nao::NAO_ECHO("Cross Validation Squared correlation coefficient = %g\n",
                      ((prob_.l * sumvy - sumv * sumy) * (prob_.l * sumvy - sumv * sumy)) / ((prob_.l * sumvv - sumv * sumv) * (prob_.l * sumyy - sumy * sumy)));
    }
    else {
        for (NInt i = 0; i < prob_.l; i++) {
            if (target[i] == prob_.y[i]) {
                ++total_correct;
            }
        }
        nao::NAO_ECHO("Cross Validation Accuracy = %g%%\n", 100.0 * total_correct / prob_.l);
    }
    free(target);
}
NDouble VSvm::testLibSVM(const cv::Mat& src, const cv::HOGDescriptor& hog, NDouble prob_estimates[])
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
    std::vector<NFloat> descriptors;
    descriptors.resize(hog.getDescriptorSize());
    hog.compute(test_img, descriptors, cv::Size(1, 1), cv::Size(0, 0));
    svm_node* inputVector = new svm_node[descriptors.size() + 1];
    for (NInt i = 0; i < descriptors.size(); i++) {
        inputVector[i].index = i;
        inputVector[i].value = descriptors[i];
    }
    inputVector[descriptors.size()].index = -1;
    NDouble resultLabel                   = svm_predict_probability(svm_, inputVector, prob_estimates);   // 分类结果
    delete[] inputVector;
    return resultLabel;
}

NVoid VSvm::test(const cv::HOGDescriptor& hog)
{
    NInt        errorCount = 0;
    std::string path       = base_path_ + "\\" + model_name_;
    svm_                   = svm_load_model(path.c_str());
    for (NInt i = 0; i < train_data_.size(); i++) {
        cv::Mat src = cv::imread(train_data_[i].second, 0);
        if (src.empty()) {
            continue;
        }
        NDouble prob[2] = {0};
        NInt    ret     = (NInt)testLibSVM(src, hog, prob);
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
    NDouble errorPercentage = 0.0;
    errorPercentage         = errorCount / (train_data_.size() * 1.0);
    std::cout << "Error rate：" << errorPercentage << std::endl;
    std::cout << "Accuracy: " << 1 - errorPercentage << std::endl;
}

/*-----------------------------------------------------------------------------------------------------------*/
NVoid VSvm::addFeatureLabel(const cv::Mat& feature, const std::vector<NInt>& label)
{
    if (feature.rows != label.size()) {
        throw std::runtime_error("The number of training pictures does not match the number of labels");
        exit(0);
    }
    NInt sample_length = feature.rows;
    for (NInt i = 0; i < sample_length; i++) {
        // 包含左边界，不包含右边界，取出每一个样本的特征值
        cv::Mat single_sample = feature.rowRange(i, i + 1).clone();
        train_feature_data_.emplace_back(std::make_pair(label[i], single_sample));
    }
}

NVoid VSvm::copyFeatureLabel()
{
    nao::NAO_ECHO("Copying feature values and labels in the staging area");
    NInt sample_length = static_cast<NInt>(train_feature_data_.size());
    if (sample_length <= 1) {
        nao::NAO_ECHO("Too few samples");
        return;
    }
    label_mat_ = cv::Mat(static_cast<NInt>(sample_length), 1, CV_32FC1, cv::Scalar(0));
    for (NInt i = 0; i < sample_length; i++) {
        label_mat_.ptr<NFloat>(i)[0] = static_cast<NFloat>(train_feature_data_[i].first);
        data_mat_.push_back(train_feature_data_[i].second);
    }
    nao::NAO_ECHO("Copy feature values and labels in the staging area");
}

NDouble VSvm::testFeatureLibSVM(const cv::Mat& feature, NDouble prob_estimates[])
{
    if (feature.empty()) {
        throw std::runtime_error("The input feature map is empty");
        exit(0);
    }
    NInt      feature_length = feature.cols;
    svm_node* inputVector    = new svm_node[feature_length + 1];
    for (NInt i = 0; i < feature_length; i++) {
        inputVector[i].index = i;
        inputVector[i].value = feature.ptr<NFloat>(0)[i];
    }
    inputVector[feature_length].index = -1;
    NDouble resultLabel               = svm_predict_probability(svm_, inputVector, prob_estimates);   // 分类结果
    delete[] inputVector;
    return resultLabel;
}

NVoid VSvm::test()
{
    nao::NAO_ECHO("Test with a sample of the staging area");
    NInt        errorCount = 0;
    std::string path       = base_path_ + "\\" + model_name_;
    svm_                   = svm_load_model(path.c_str());
    std::vector<NInt> ret_vec;
    for (NInt i = 0; i < train_feature_data_.size(); i++) {
        if (train_feature_data_[i].second.empty()) {
            continue;
        }
        NDouble prob[2] = {0};
        cv::Mat imput   = train_feature_data_[i].second.clone();
        NInt    ret     = (NInt)testFeatureLibSVM(imput, prob);
        ret_vec.emplace_back(ret);
        if (ret != train_feature_data_[i].first) {
            errorCount++;
        }
    }
    NDouble errorPercentage = 0.0;
    errorPercentage         = errorCount / (train_feature_data_.size() * 1.0);
    nao::NAO_ECHO("test Error rate： = %g%%", errorPercentage);
    nao::NAO_ECHO("test Accuracy = %g%%\n", 1 - errorPercentage);
}

NVoid VSvm::free_model()
{
    if (svm_ != nullptr) {
        svm_free_and_destroy_model(&svm_);
        svm_destroy_param(&default_param_);
        if (prob_.y != nullptr) {
            free(prob_.y);
        }
        if (prob_.x != nullptr) {
            free(prob_.x);
        }
        if (x_space_ != nullptr) {
            free(x_space_);
        }
    }
}

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END