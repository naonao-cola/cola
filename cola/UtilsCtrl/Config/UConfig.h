/**
 * @FilePath     : /cola/cola/UtilsCtrl/Config/UConfig.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-16 08:52:11
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-16 08:52:27
 **/
#ifndef NAONAO_UCONFIG_H_
#define NAONAO_UCONFIG_H_

#include <algorithm>
#include <fstream>
#include <iostream>
#include <locale.h>
#include <map>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <vector>



#include "../UtilsObject.h"

NAO_NAMESPACE_BEGIN

class UConfig : public UtilsObject
{
    typedef std::pair<std::string, int>                PAIR;
    typedef std::map<std::string, int>::iterator       mapi;
    typedef std::map<std::string, int>::const_iterator mapci;

protected:
    std::string                delimiter_;             // 键和值之间的分隔符，默认设置为 =
    std::string                comment_;               // 注释开头默认设置为 #
    std::map<std::string, int> contents_index_;        // 存储键和插入的顺序，作为索引。
    std::vector<std::string>   contents_;              // 根据索引，写入真正的值。
    std::vector<std::string>   notes_;                 // 每个键值对上面对应的注释
    int                        max_size_      = 128;   // 默认字符最大字节数128
    int                        contents_size_ = 0;     // 存储键值对的未曾删除的个数
    std::string                description_;

public:
    /// <summary>
    /// 构造一个Config，从给定文件中获取键和值
    /// </summary>
    /// <param name="filename"></param>
    /// <param name="delimiter"></param>
    /// <param name="comment"></param>
    explicit UConfig(const std::string& filename, std::string delimiter = "=", std::string comment = "#");
    UConfig();

    /// <summary>
    /// 读取对应于键的值
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="in_key"></param>
    /// <returns></returns>
    template<class T>
    T read(const std::string& in_key) const;

    /// <summary>
    /// 返回与键对应的值或给定的默认值，找到了返回对应的值，未找到则返回默认值in_value
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="in_key"></param>
    /// <param name="in_value"></param>
    /// <returns></returns>
    template<class T>
    T read(const std::string& in_key, const T& in_value) const;

    /// <summary>
    /// 获取与key对应的值并存储在var中，找到返回true,未找到返回false
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="out_var"></param>
    /// <param name="in_key"></param>
    /// <returns></returns>
    template<class T>
    bool readInto(T& out_var, const std::string& in_key) const;

    /// <summary>
    /// 获取key对应的out_var,未找到则将输出设置为默认的in_value，并且返回false，找到了，则将out_var设置为对应的值，并且返回true.
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="out_var"></param>
    /// <param name="in_key"></param>
    /// <param name="in_value"></param>
    /// <returns></returns>
    template<class T>
    bool readInto(T& out_var, const std::string& in_key, const T& in_value) const;

    bool fileExist(const std::string& filename);
    void readFile(const std::string& filename, std::string delimiter = "=", std::string comment = "#");
    bool keyExists(const std::string& in_key) const;


    /// <summary>
    /// 添加一个键值对
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="in_key"></param>
    /// <param name="in_value"></param>
    template<class T>
    void add(const std::string& in_key, const T& in_value);

    /// <summary>
    ///  添加注释，没有注释的话，请添加 #\n，务必保证，键值对的个数与注释的条目 数量一致。注释自己添加# 与 回车
    /// </summary>
    /// <param name="notes"></param>
    void addNotes(const std::string& notes);

    /// <summary>
    /// 添加文件开头描述部分。注释自己添加# 与 \n回车
    /// </summary>
    /// <param name="des"></param>
    void addDescription(const std::string& des);

    /// <summary>
    /// 移除一个键值对
    /// </summary>
    /// <param name="in_key"></param>
    void remove(const std::string& in_key);
   	friend bool CmpByValue(const PAIR lhs, const PAIR rhs);
    /// <summary>
    /// 返回键值对的分隔符
    /// </summary>
    /// <returns></returns>
    std::string getDelimiter() const { return delimiter_; }

    /// <summary>
    /// 返回注释的开头字符
    /// </summary>
    /// <returns></returns>
    std::string getComment() const { return comment_; }

    /// <summary>
    /// 设置新的键值对分隔符
    /// </summary>
    /// <param name="in_s"></param>
    /// <returns></returns>
    std::string setDelimiter(const std::string& in_s)
    {
        std::string old = delimiter_;
        delimiter_      = in_s;
        return old;
    }

    /// <summary>
    /// 设置新的注释开头字符
    /// </summary>
    /// <param name="in_s"></param>
    /// <returns></returns>
    std::string setComment(const std::string& in_s)
    {
        std::string old = comment_;
        comment_        = in_s;
        return old;
    }

    /// <summary>
    /// 友元函数
    /// </summary>
    /// <param name="os"></param>
    /// <param name="cf"></param>
    /// <returns></returns>

    friend std::ostream& operator<<(std::ostream& os, const UConfig& cf);
    /// <summary>
    /// https://blog.csdn.net/qq_40968179/article/details/104384232
    /// </summary>
    /// <param name="is"></param>
    /// <param name="cf"></param>
    /// <returns></returns>
    friend std::istream& operator>>(std::istream& is, UConfig& cf);

    /// <summary>
    /// map按值比较
    /// </summary>
    /// <param name="lhs"></param>
    /// <param name="rhs"></param>
    /// <returns></returns>
    friend bool CmpByValue(const PAIR lhs, const PAIR rhs);

    /// <summary>
    /// 设置最大字节数
    /// </summary>
    /// <param name="maxSize"></param>
    void setMaxSize(int maxSize) { max_size_ = maxSize; };

protected:
    template<class T>
    static std::string T_as_string(const T& t);
    template<class T>
    static T    string_as_T(const std::string& s);
    static void Trim(std::string& inout_s);

public: /*异常类型*/
    struct File_not_found
    {
        std::string filename;
        File_not_found(const std::string& filename_ = std::string())
            : filename(filename_)
        {
        }
    };
    struct Key_not_found
    {   // thrown only by T read(key) variant of read()
        std::string key;
        Key_not_found(const std::string& key_ = std::string())
            : key(key_)
        {
        }
    };
};

NAO_NAMESPACE_END
#include "UConfig.inl"
#endif   // NAONAO_UCONFIG_H_