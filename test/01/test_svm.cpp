﻿/**
 * @FilePath     : /cola/test/01/test_svm.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-18 19:51:49
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-08-06 21:08:38
 **/
#include <filesystem>
#include "../Common/common.h"
#include "../Common/config.h"

using namespace nao::vision;


void test_svm()
{
    std::cout << "current_path: " << CURRENT_PATH << std::endl;
    std::string current_path = CURRENT_PATH;
    std::string               ok_string =  current_path + R"(/data/img/svm/1)";
    std::string               ng_string =  current_path + R"(/data/img/svm/0)";
    std::vector<std::string>  ok_file   = nao::UFs::getAllFormatFiles(ok_string);
    std::vector<std::string>  ng_file   = nao::UFs::getAllFormatFiles(ng_string);

    std::vector<cv::Mat> ok_img_vec;
    std::vector<cv::Mat> ng_img_vec;
    for (auto item : ok_file) {
        ok_img_vec.push_back(cv::imread(item));
    }
    for (auto item : ng_file) {
        ng_img_vec.push_back(cv::imread(item));
    }

    VHog ok_hog_transform(ok_img_vec, 11, 8, 4, cv::Size(88, 34), 1);
    VHog ng_hog_transform(ng_img_vec, 11, 8, 4, cv::Size(88, 34), 1);
    cv::Mat          ok_feature = ok_hog_transform();
    cv::Mat          ng_feature = ng_hog_transform();
    std::vector<int> ok_label(ok_img_vec.size(), 1);
    std::vector<int> ng_label(ng_img_vec.size(), 0);

    VSvm svm_obj(cv::Size(88, 34));
    svm_obj.init(R"(./)", "lc_3_8.mdl");
    svm_obj.addFeatureLabel(ok_feature, ok_label);
    svm_obj.addFeatureLabel(ng_feature, ng_label);
    svm_obj.copyFeatureLabel();
    svm_obj.train_cross();
    svm_obj.test();
    return;
}


void test_pca()
{
    std::string              current_path = CURRENT_PATH;
    std::string              ok_string    = current_path + R"(/data/img/svm/1)";
    std::vector<std::string> ok_file = nao::UFs::getAllFormatFiles(ok_string);
    std::vector<cv::Mat> ok_img_vec;
    for (auto item : ok_file) {
        ok_img_vec.push_back(cv::imread(item));
    }
    VHog ok_hog_transform(ok_img_vec, 11, 8, 4, cv::Size(88, 34), 1);
    cv::Mat ok_feature = ok_hog_transform();

    VPCA pca;
    cv::Mat dst = pca.reduce(ok_feature);
    pca.write(R"(./pca.xml)");
}

void test_pca_read()
{
    std::string              current_path = CURRENT_PATH;
    std::string              ok_string    = current_path + R"(/data/img/svm/1)";
    std::vector<std::string> ok_file      = nao::UFs::getAllFormatFiles(ok_string);
    std::vector<cv::Mat>     ok_img_vec;
    for (auto item : ok_file) {
        ok_img_vec.push_back(cv::imread(item));
    }
    VHog    ok_hog_transform(ok_img_vec, 11, 8, 4, cv::Size(88, 34), 1);
    cv::Mat ok_feature = ok_hog_transform();

    VPCA    pca;
    pca.read(R"(./pca.xml)");
    cv::Mat dst = pca.reduce_single(ok_feature.row(0));
}

int main()
{
    //test_svm();
    test_pca();
    test_pca_read();
    return 0;
}