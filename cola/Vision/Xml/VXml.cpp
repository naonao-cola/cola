/**
 * @FilePath     : /cola/cola/Vision/Xml/VXml.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-08 15:04:32
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-08 15:04:33
 **/
#include "VXml.h"

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

VXmlw::VXmlw(const int& type_w, const std::string& file_name)
{
    file_name_ = file_name;
    type_w_    = type_w;
    time_t raw_time;
    time(&raw_time);
    switch (type_w_) {
    case 1:
        fs_ = cv::FileStorage(file_name_, cv::FileStorage::WRITE);
        fs_ << "WIRTE_TIME" << asctime(localtime(&raw_time));
        break;
    case 2: fs_ = cv::FileStorage(file_name_, cv::FileStorage::APPEND); fs_ << "WRITE_TIME" << asctime(localtime(&raw_time));
    default: break;
    }
    if (!fs_.isOpened()) {
        std::cerr << file_name_ << "file open false" << std::endl;
    }
}

VXmlw::~VXmlw()
{
    fs_.release();
    if (type_w_ == 1) {
        std::cout << "xml rewrite finish" << std::endl;
    }
    else {
        std::cout << "xml append finish" << std::endl;
    }
}

cv::FileStorage VXmlw::get_xml_write_object()
{
    return fs_;
}

void VXmlw::writeString(const std::string& key, const std::string& content)
{
    fs_ << key << content;
}

void VXmlw::writeString(const std::string& key, const std::vector<std::string>& contents)
{
    fs_ << key << "[" << contents << "]";
}

void VXmlw::writeMat(const std::string& key, const cv::Mat& matrix)
{
    fs_ << key << matrix;
}

void VXmlw::writeMat(const std::string& key, const std::vector<cv::Mat>& matrixs)
{
    fs_ << key << "[" << matrixs << "]";
}

void VXmlw::startParentNode(const std::string& parentNodeName)
{
    mapsiit it = parent_node_.find(parentNodeName);
    if (it == parent_node_.end()) {
        fs_ << parentNodeName << "{";
        parent_node_.insert(std::make_pair(parentNodeName, 1));
    }
    else
        std::cerr << "this parent node already exist " << std::endl;
}

void VXmlw::endParentNode()
{
    fs_ << "}";
}


VXmlr::VXmlr(const std::string& file_name)
{
    file_name_ = file_name;
}

VXmlr::~VXmlr()
{
}

void VXmlr::readString(const std::string& key, std::string& content)
{
    cv::FileStorage fs(file_name_, cv::FileStorage::READ);
    fs[key] >> content;
    fs.release();
}

void VXmlr::readString(const std::string& key, std::vector<std::string>& contents)
{
    cv::FileStorage fs(file_name_, cv::FileStorage::READ);
    fs[key][0] >> contents;
    fs.release();
}

void VXmlr::readMat(const std::string& key, cv::Mat& matrix)
{
    cv::FileStorage fs(file_name_, cv::FileStorage::READ);
    fs[key] >> matrix;
    fs.release();
}

void VXmlr::readMat(const std::string& key, std::vector<cv::Mat>& matrixs)
{
    cv::FileStorage fs(file_name_, cv::FileStorage::READ);
    fs[key][0] >> matrixs;
    fs.release();
}

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END
