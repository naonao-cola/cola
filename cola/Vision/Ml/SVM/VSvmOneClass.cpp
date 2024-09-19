#include "VSvmOneClass.h"


NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

#define Malloc(type, n) (type*)malloc((n) * sizeof(type))

SvmOne::SvmOne(const std::string& filename)
{
    x_space_ = nullptr;
    model_   = nullptr;
    prob_.l  = 0;
    prob_.x  = nullptr;
    prob_.y  = nullptr;
    if (!filename.empty()) {
        load_model(filename);
    }
}

SvmOne::~SvmOne()
{
    free_model();
    free_param();
    free_dataset();
}

void SvmOne::param_init(int svm_type, int kernel_type, int degree, double gamma, double coef0, double nu, double C, double eps, double cache_size, double p, int shrinking, int probability,
                        const std::vector<std::pair<int, double>>& nr_weight)
{
    param_.svm_type    = svm_type;
    param_.kernel_type = kernel_type;
    param_.degree      = degree;
    param_.coef0       = coef0;
    param_.gamma       = gamma;
    param_.nu          = nu;
    param_.C           = C;
    param_.eps         = eps;
    param_.cache_size  = cache_size;
    param_.p           = p;
    param_.shrinking   = shrinking;
    param_.probability = 0;
    param_.nr_weight   = nr_weight.size();
    if (param_.nr_weight > 0) {
        param_.weight_label = (int*)malloc(sizeof(int) * param_.nr_weight);
        param_.weight       = (double*)malloc(sizeof(double) * param_.nr_weight);
        for (const auto& item : nr_weight) {
            param_.weight_label[param_.nr_weight - 1] = item.first;
            param_.weight[param_.nr_weight - 1]       = item.second;
        }
    }
    else {
        param_.weight       = nullptr;
        param_.weight_label = nullptr;
    }
}

void SvmOne::one_class_svm_param_init()
{
    // -s 2 -t 2 -d 3 -g 0.01 -r 0 -n 0.001 -c 1 -e 0.001 -m 200 -p 0.1 -h 1 -b 0
    param_init(ONE_CLASS, RBF, 5, 0.01, 0, 0.015, 32, 1e-3, 500);
}

void SvmOne::read_problem(cv::Mat& dataset, const std::vector<double>& label)
{
    if (dataset.empty() || (label.empty() && param_.svm_type != ONE_CLASS)) {
        return;
    }

    free_dataset();
    auto len      = dataset.rows;
    auto dim      = dataset.cols;
    auto elements = len * dim;

    prob_.l  = len;
    prob_.y  = Malloc(double, prob_.l);
    prob_.x  = Malloc(struct svm_node*, prob_.l);
    x_space_ = Malloc(struct svm_node, elements + len);

    int j = 0;
    for (int l = 0; l < len; l++) {
        prob_.x[l] = &x_space_[j];
        for (int d = 0; d < dim; d++) {
            x_space_[j].index = d + 1;
            x_space_[j].value = *dataset.ptr<float>(l, d);
            j++;
        }
        x_space_[j++].index = -1;
        if (param_.svm_type != ONE_CLASS) {
            prob_.y[l] = label[l];
        }
        else {
            prob_.y[l] = 1;
        }
    }
}

double SvmOne::train(cv::Mat& dataset, const std::vector<double>& label, int nr_fold)
{
    if (dataset.rows != label.size()) {
        return -1;
    }

    free_model();
    read_problem(dataset, label);
    if (prob_.l <= 0) {
        return -1;
    }
    if (param_.gamma < 1e-6) {
        param_.gamma = 1.0 / double(dataset.cols);
    }
    auto error_log = svm_check_parameter(&prob_, &param_);
    if (error_log != nullptr) {
        std::cout << error_log;
        return 0;
    }
    model_ = svm_train(&prob_, &param_);
    double accaurcy;
    if (nr_fold > 1) {
        accaurcy = cross_validation(nr_fold);
    }
    else {
        accaurcy = clf_validation(dataset, label);
    }
    return accaurcy;
}

std::pair<double, double> SvmOne::predict(svm_node* svm_node_data)
{
    double result;
    double dec_value;
    if (svm_check_probability_model(model_)) {
        auto prob_vector = Malloc(double, model_->nr_class);
        result           = svm_predict_probability(model_, svm_node_data, prob_vector);
        for (int k = 0; k < model_->nr_class; k++) {
            if (model_->label[k] == result) {
                dec_value = prob_vector[k];
                break;
            }
        }
        delete[] prob_vector;
    }
    else {
        result = svm_predict_values(model_, svm_node_data, &dec_value);
    }
    delete[] svm_node_data;
    return {result, dec_value};
}


std::pair<double, double> SvmOne::predict(const cv::Mat& data)
{
    svm_node* svm_node_data        = new svm_node[data.cols + 1];
    svm_node_data[data.cols].index = -1;
    for (int i = 0; i < data.cols; i++) {
        svm_node_data[i].index = i + 1;
        svm_node_data[i].value = data.ptr<float>(0)[i];
    }
    return std::move(predict(svm_node_data));
}

int SvmOne::load_model(const std::string& model_path)
{
    free_model();
    model_ = svm_load_model(model_path.c_str());
    if (model_ == nullptr) {
        return -1;
    }
    return 0;
}

int SvmOne::save_model(const std::string& model_path)
{
    return svm_save_model(model_path.data(), model_);
}

double SvmOne::clf_validation(const cv::Mat& dataset, const std::vector<double>& label)
{
    if (((label.size() != dataset.rows) || (model_->param.svm_type != ONE_CLASS))) {
        return -1;
    }

    double    total_correct           = 0;
    svm_node* svm_node_data           = new svm_node[dataset.cols + 1];
    svm_node_data[dataset.cols].index = -1;

    for (int i = 0; i < dataset.rows; i++) {
        for (int j = 0; j < dataset.cols; j++) {
            svm_node_data[j].index = j + 1;
            svm_node_data[j].value = *dataset.ptr<float>(i, j);
        }
        if (model_->param.svm_type == ONE_CLASS) {
            if (svm_predict(model_, svm_node_data) == int(1)) {
                ++total_correct;
            }
        }
        else {
            if (svm_predict(model_, svm_node_data) == int(label[i])) {
                ++total_correct;
            }
        }
    }
    delete[] svm_node_data;
    return total_correct / (dataset.rows * 1.f);
}

double SvmOne::cross_validation(int nr_fold)
{
    int    i;
    int    total_correct = 0;
    double total_error   = 0;
    double sumv = 0, sumy = 0, sumvv = 0, sumyy = 0, sumvy = 0;
    auto   target = Malloc(double, prob_.l);
    svm_cross_validation(&prob_, &param_, nr_fold, target);
    if (param_.svm_type == EPSILON_SVR || param_.svm_type == NU_SVR) {
        for (i = 0; i < prob_.l; i++) {
            double y = prob_.y[i];
            double v = target[i];
            total_error += (v - y) * (v - y);
            sumv += v;
            sumy += y;
            sumvv += v * v;
            sumyy += y * y;
            sumvy += v * y;
        }
        printf("Cross Validation Mean squared error = %g\n", total_error / prob_.l);
        printf("Cross Validation Squared correlation coefficient = %g\n",
               ((prob_.l * sumvy - sumv * sumy) * (prob_.l * sumvy - sumv * sumy)) / ((prob_.l * sumvv - sumv * sumv) * (prob_.l * sumyy - sumy * sumy)));
    }
    else {
        for (i = 0; i < prob_.l; i++)
            if (int(target[i]) == int(prob_.y[i])) {
                ++total_correct;
            }
        printf("Cross Validation Accuracy = %g%%\n", 100.0 * total_correct / prob_.l);
    }
    free(target);
    return 100.0 * total_correct / prob_.l;
}

void SvmOne::free_model()
{
    svm_free_and_destroy_model(&model_);
}

bool SvmOne::free_param()
{
    if (param_.weight != nullptr) {
        svm_destroy_param(&param_);
        return true;
    }
    return false;
}

bool SvmOne::free_dataset()
{
    if (prob_.l != 0) {
        prob_.l = 0;
        if (prob_.y != nullptr) {
            free(prob_.y);
            prob_.y = nullptr;
        }
        if (prob_.x != nullptr) {
            free(prob_.x);
            prob_.x = nullptr;
        }
        if (x_space_ != nullptr) {
            free(x_space_);
            x_space_ = nullptr;
        }
        return true;
    }
    return false;
}

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END