/**
 * @FilePath     : /cola/cola/Vision/Xml/VXml.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-08 15:02:17
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-10 17:06:34
 **/
#ifndef NAO_VXML_H
#define NAO_VXML_H


#include <fstream>
#include <iostream>
#include <map>
#include <time.h>

#include "../VisionObject.h"

NAO_NAMESPACE_BEGIN
NAO_VISION_NAMESPACE_BEGIN

class VXmlw : public VisionObject
{

public:
    typedef std::map<std::string, int>           mapsi;
    typedef std::map<std::string, int>::iterator mapsiit;
    explicit VXmlw(const int& type_w, const std::string& file_name);

    VXmlw() = default;
    ~VXmlw() override;

    /**
     * @brief 获取写文件的对象
     * @return
     */
    cv::FileStorage get_xml_write_object();

    /**
     * @brief 模板函数，写数值
     * @tparam T
     * @param key 键名
     * @param value 键值
     */
    template<typename T>
    void writeValue(const std::string& key, const T& value);
    template<typename T>
    void writeValue(const std::string& key, const std::vector<T>& value);

    /**
     * @brief 写字符串
     * @param key 键名
     * @param content 键值
     */
    void writeString(const std::string& key, const std::string& content);
    void writeString(const std::string& key, const std::vector<std::string>& contents);

    /**
     * @brief 模板函数，写点数组(注意数组内的每个值，类型一致)
     * @tparam T
     * @param key 键名
     * @param point 键值
     */
    template<typename T, is_instance<T, cv::Point_>>
    void writePoint(const std::string& key, const T& point);
    template<typename T, is_instance<T, cv::Point_>>
    void writePoint(const std::string& key, const std::vector<T>& points);

    /**
     * @brief 写矩阵
     * @param key 键名
     * @param matrix 键值
     */
    void writeMat(const std::string& key, const cv::Mat& matrix);
    void writeMat(const std::string& key, const std::vector<cv::Mat>& matrixs);

    /**
     * @brief 写入带有叶子节点的节点。子节点名称个数与数据个数一致，(内容一一对应)目前子节点内存放统一类型的数据，
     * 后续根据需要修改存放不同类型的数据。子节点里面也可以放入数组，目前只支持单个数据，后续再改
     * @tparam T
     * @param parentNodeName 父节点名
     * @param childNodeName  各个子节点的名字
     * @param data
     */
    template<typename T>
    void writeWithLeaf(const std::string& parentNodeName, const std::vector<std::string>& childNodeName, std::vector<T>& data);

    /**
     * @brief 开始写入父节点,父节点名字不允许重复, StartParentNode 与 EndParentNode 必须成对出现，在中间写入子节点。
     * @param parentNodeName  开始节点
     */
    void startParentNode(const std::string& parentNodeName);
    template<typename T>
    void writeChildNode(const std::string& childNodeName, T& data);
    void endParentNode();

private:
    cv::FileStorage fs_;
    int             type_w_;
    std::string     file_name_;
    mapsi           parent_node_;
};


class VXmlr : public VisionObject
{
public:
    explicit VXmlr(const std::string& file_name);
    ~VXmlr() override;

    /**
     * @brief 模板函数，读数值
     * @tparam T
     * @param key 键名
     * @param value  键值
     */
    template<typename T>
    void readValue(const std::string& key, T& value);
    template<typename T>
    void readValue(const std::string& key, std::vector<T>& value);

    /**
     * @brief 读单个字符串,读字符串数组
     * @param key 键名
     * @param content 键值
     */
    void readString(const std::string& key, std::string& content);
    void readString(const std::string& key, std::vector<std::string>& contents);

    /**
     * @brief 模板函数，读点,读点数组
     * @tparam T
     * @param key 键名
     * @param point 键值
     */
    template<typename T, is_instance<T, cv::Point_>>
    void readPoint(const std::string& key, T& point);
    template<typename T, is_instance<T, cv::Point_>>
    void readPoint(const std::string& key, std::vector<T>& points);

    /**
     * @brief 读矩阵
     * @param key 键名
     * @param matrix 键值
     */
    void readMat(const std::string& key, cv::Mat& matrix);
    void readMat(const std::string& key, std::vector<cv::Mat>& matrixs);

    /**
     * @brief 读出带有叶子节点的节点。子节点名称个数与数据个数一致，目前子节点内存放统一类型的数据。
     * 与上面的写入数据对应。读的时候验证一下，是按照顺序读取的
     * @tparam T
     * @param parentNodeName  父节点名
     * @param childNodeName   各个子节点的名字
     * @param data
     */
    template<typename T>
    void readWithLeaf(const std::string& parentNodeName, const std::vector<std::string>& childNodeName, std::vector<T>& data);

    /**
     * @brief 读出带有叶子节点的节点。此函数相对于写函数的 写入不同类型数据的读函数,读数据时务必保证父节点存在。
     * @tparam T
     * @param parentNodeName 父节点名
     * @param childNodeName 子节点的名字
     * @param data 子节点数据
     */
    template<typename T>
    void readChildNode(const std::string& parentNodeName, const std::string& childNodeName, T& data);

    /**
     * @brief 逐层读取。此函数相对于写函数的 写入不同类型数据的读函数,读数据时务必保证父节点存在。
     * @tparam T
     * @param parentNodeName 父节点名
     * @param childNodeName 子节点的名字
     * @param data 子节点数据
     */
    template<typename T>
    void readChildNode(const std::vector<std::string>& parentNodeName, const std::string& childNodeName, T& data);

private:
    std::string file_name_;
};

NAO_VISION_NAMESPACE_END
NAO_NAMESPACE_END

#include "VXml.inl"
#endif   // NAO_VXML_H