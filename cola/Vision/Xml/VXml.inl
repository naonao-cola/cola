/*
 * @FilePath     : /cola/cola/Vision/Xml/VXml.inl
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-08 15:04:16
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-10 17:06:46
 */
#ifndef NAO_VXML_INL
#define NAO_VXML_INL

#include "VXml.h"

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

template<typename T>
void VXmlw::writeValue(const std::string& key, const T& value)
{
    fs_ << key << value;
}

template<typename T>
void VXmlw::writeValue(const std::string& key, const std::vector<T>& value)
{
    fs_ << key << "[";
    for (int i = 0; i < value.size(); i++)
        fs_ << value[i];
    fs_ << "]";
}

template<typename T, is_instance<T, cv::Point_>>
void VXmlw::writePoint(const std::string& key, const T& point)
{
    fs_ << key << point;
}

template<typename T, is_instance<T, cv::Point_>>
void VXmlw::writePoint(const std::string& key, const std::vector<T>& points)
{
    fs_ << key << "[" << points << "]";
}

template<typename T>
void VXmlw::writeWithLeaf(const std::string& parentNodeName, const std::vector<std::string>& childNodeName, std::vector<T>& data)
{
    if (childNodeName.size() != data.size())
        std::cerr << "the count is not same" << std::endl;
    fs_ << parentNodeName << "{";
    for (std::size_t i = 0; i < childNodeName.size(); i++)
        fs_ << childNodeName[i] << data[i];
    fs_ << "}";
}

template<typename T>
void VXmlw::writeChildNode(const std::string& childNodeName, T& data)
{
    fs_ << childNodeName << data;
}

template<typename T>
void VXmlr::readValue(const std::string& key, T& value)
{
    cv::FileStorage fs(file_name_, cv::FileStorage::READ);
    value = T(fs[key]);
    fs.release();
}

template<typename T>
void VXmlr::readValue(const std::string& key, std::vector<T>& value)
{
    cv::FileStorage      fs(file_name_, cv::FileStorage::READ);
    cv::FileNode         arrayName = fs[key];
    cv::FileNodeIterator it = arrayName.begin(), it_end = arrayName.end();
    for (; it != it_end; it++) {
        value.push_back((T)(*it));
    }
    fs.release();
}

template<typename T, is_instance<T, cv::Point_>>
void VXmlr::readPoint(const std::string& key, T& point)
{
    cv::FileStorage fs(file_name_, cv::FileStorage::READ);
    fs[key] >> point;
    fs.release();
}

template<typename T, is_instance<T, cv::Point_>>
void VXmlr::readPoint(const std::string& key, std::vector<T>& points)
{
    cv::FileStorage fs(file_name_, cv::FileStorage::READ);
    fs[key][0] >> points;
    fs.release();
}

template<typename T>
void VXmlr::readWithLeaf(const std::string& parentNodeName, const std::vector<std::string>& childNodeName, std::vector<T>& data)
{
    cv::FileStorage      fs(file_name_, cv::FileStorage::READ);
    cv::FileNode         parentNode = fs[parentNodeName];
    cv::FileNodeIterator it = parentNode.begin(), it_end = parentNode.end();   // 此处指针是指向第一个子节点
    int                  i = 0;
    for (; it != it_end, i < childNodeName.size(); it++, i++) {
        T tmp;
        (*it) >> tmp;
        data.push_back(tmp);
    }
    fs.release();
}

template<typename T>
void VXmlr::readChildNode(const std::string& parentNodeName, const std::string& childNodeName, T& data)
{
    cv::FileStorage fs(file_name_, cv::FileStorage::READ);
    cv::FileNode    tmpNode = fs[parentNodeName];
    if (tmpNode.isNone()) {
        std::cerr << "parent node : " << parentNodeName << "not exist" << std::endl;
        fs.release();
    }
    tmpNode[childNodeName] >> data;
    fs.release();
}

template<typename T>
void VXmlr::readChildNode(const std::vector<std::string>& parentNodeName, const std::string& childNodeName, T& data)
{
    cv::FileStorage fs(file_name_, cv::FileStorage::READ);
    cv::FileNode    tmpNode = fs[parentNodeName[0]];
    for (int i = 1; i < parentNodeName.size(); i++) {
        tmpNode = tmpNode[parentNodeName[i]];
        if (tmpNode.isNone()) {
            std::cerr << "parent node : " << parentNodeName[i] << "not exist" << std::endl;
            fs.release();
        }
    }
    tmpNode[childNodeName] >> data;
    fs.release();
}

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END

#endif   // !NAO_VXML_INL
