/**
 * @FilePath     : /cola/tutorial/06/test_svm.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-18 19:51:49
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-09-25 12:42:33
 **/
#include "../Common/common.h"
#include "../Common/config.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <string>



using namespace nao::vision;

void test_svm()
{
    std::cout << "current_path: " << CURRENT_PATH << std::endl;
    std::string              current_path = CURRENT_PATH;
    std::string              ok_string    = current_path + R"(/data/img/svm/1)";
    std::string              ng_string    = current_path + R"(/data/img/svm/0)";
    std::vector<std::string> ok_file      = nao::UFs::getAllFormatFiles(ok_string);
    std::vector<std::string> ng_file      = nao::UFs::getAllFormatFiles(ng_string);

    std::vector<cv::Mat> ok_img_vec;
    std::vector<cv::Mat> ng_img_vec;
    for (const auto& item : ok_file) {
        cv::Mat tmp = cv::imread(item);
        if (tmp.empty()) {
            std::cout << "test_svm ok img is empty: " << item << std::endl;
            continue;
        }
        ok_img_vec.push_back(tmp);
    }
    for (const auto& item : ng_file) {
        cv::Mat tmp = cv::imread(item);
        if (tmp.empty()) {
            std::cout << "test_svm ng img is empty: " << item << std::endl;
            continue;
        }
        ng_img_vec.push_back(tmp);
    }

    VHog ok_hog_transform(ok_img_vec, 11, 8, 4, cv::Size(88, 34), 1);
    VHog ng_hog_transform(ng_img_vec, 11, 8, 4, cv::Size(88, 34), 1);
    ok_hog_transform.img_type_ = VHog::IMG_TYPE::RGB;
    ng_hog_transform.img_type_ = VHog::IMG_TYPE::RGB;

    cv::Mat          ok_feature = ok_hog_transform();
    cv::Mat          ng_feature = ng_hog_transform();
    std::vector<int> ok_label(ok_img_vec.size(), 1);
    std::vector<int> ng_label(ng_img_vec.size(), 0);

    VSvm svm_obj(cv::Size(88, 34));
    svm_obj.init(R"(./)", "lc_3_8.mdl");
    svm_obj.addFeatureLabel(ok_feature, ok_label);
    svm_obj.addFeatureLabel(ng_feature, ng_label);
    svm_obj.copyFeatureLabel();
    svm_obj.test_cross();
    nao::NAO_ECHO("test_cross done");
    svm_obj.trainLibSVM();
    nao::NAO_ECHO("trainLibSVM done");
    svm_obj.test();
    nao::NAO_ECHO("svm done");
}

void test_pca()
{
    std::cout << "current_path: " << CURRENT_PATH << std::endl;
    std::string              current_path = CURRENT_PATH;
    std::string              ok_string    = current_path + R"(/data/img/svm/1)";
    std::vector<std::string> ok_file      = nao::UFs::getAllFormatFiles(ok_string);
    std::vector<cv::Mat>     ok_img_vec;
    ok_img_vec.reserve(ok_file.size());
    for (const auto& item : ok_file) {
        ok_img_vec.push_back(cv::imread(item));
    }
    VHog    ok_hog_transform(ok_img_vec, 11, 8, 4, cv::Size(88, 34), 1);
    cv::Mat ok_feature = ok_hog_transform();

    VPCA    pca;
    cv::Mat dst = pca.reduce(ok_feature, 30);
    pca.write(R"(./pca.xml)");
    nao::NAO_ECHO("pca done");
}

void test_pca_read()
{
    std::cout << "current_path: " << CURRENT_PATH << std::endl;
    std::string              current_path = CURRENT_PATH;
    std::string              ok_string    = current_path + R"(/data/img/svm/1)";
    std::vector<std::string> ok_file      = nao::UFs::getAllFormatFiles(ok_string);
    std::vector<cv::Mat>     ok_img_vec;
    ok_img_vec.reserve(ok_file.size());
    for (const auto& item : ok_file) {
        ok_img_vec.push_back(cv::imread(item));
    }
    VHog    ok_hog_transform(ok_img_vec, 11, 8, 4, cv::Size(88, 34), 1);
    cv::Mat ok_feature = ok_hog_transform();
    VPCA    pca;
    pca.read(R"(./pca.xml)");
    cv::Mat dst = pca.reduce_single(ok_feature.row(0));
    nao::NAO_ECHO("pca_read done");
}

void test_base64()
{
    std::cout << "current_path: " << CURRENT_PATH << std::endl;
    std::string current_path = CURRENT_PATH;
    std::string ok_string    = current_path + R"(/data/img/svm/1/0.jpg)";
    nao::NAO_ECHO("img path %s", ok_string.c_str());

    std::fstream f;
    f.open(ok_string.c_str(), std::ios::in | std::ios::binary);
    f.seekg(0, std::ios_base::end);    // 设置偏移量至文件结尾
    std::streampos sp   = f.tellg();   // 获取文件大小
    int            size = sp;

    char* buffer = (char*)malloc(sizeof(char) * size);
    f.seekg(0, std::ios_base::beg);   // 设置偏移量至文件开头
    f.read(buffer, size);             // 将文件内容读入buffer
    std::cout << "file size:" << size << std::endl;

    std::string imgBase64 = nao::UBase64::enCode(buffer, size);   // 编码
    std::cout << "img base64 encode size:" << imgBase64.size() << std::endl;

    std::string imgdecode64 = nao::UBase64::deCode(imgBase64);   // 解码
    std::cout << "img decode size:" << imgdecode64.size() << std::endl;

    const char*   p = imgdecode64.c_str();
    std::ofstream fout("./result.jpg", std::ios::out | std::ios::binary);
    if (!fout) {
        std::cout << "error" << std::endl;
    }
    else {
        std::cout << "Success!" << std::endl;
        fout.write(p, size);
    }
    fout.close();
}
int main()
{
    printf("----------------- svm -----------------\n");
    test_svm();
    printf("----------------- pca -----------------\n");
    test_pca();
    printf("----------------- pca_read -----------------\n");
    test_pca_read();
    printf("----------------- test_base64 -----------------\n");
    test_base64();
    return 0;
}