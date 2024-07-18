/*
 * @FilePath     : /cola/cola/UtilsCtrl/Config/UConfig.inl
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-16 08:52:44
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-16 09:04:51
 */
#ifndef NAONAO_UCONFIG_INL
#define NAONAO_UCONFIG_INL

#include "UConfig.h"

NAO_NAMESPACE_BEGIN

template<class T>
T UConfig::read(const std::string& key) const
{
    auto p = contents_index_.find(key);
    if (p == contents_index_.end()) {
        throw Key_not_found(key);
    }
    size_t index = p->second;
    return string_as_T<T>(contents_[index]);
}

template<class T>
T UConfig::read(const std::string& key, const T& value) const
{
    auto p = contents_index_.find(key);
    if (p == contents_index_.end()) {
        return value;
    }
    size_t index = p->second;
    return string_as_T<T>(contents_[index]);
}

template<class T>
bool UConfig::readInto(T& var, const std::string& key) const
{
    auto p     = contents_index_.find(key);
    bool found = (p != contents_index_.end());
    if (found) {
        size_t index = p->second;
        var          = string_as_T<T>(contents_[index]);
    }
    return found;
}

template<class T>
bool UConfig::readInto(T& var, const std::string& key, const T& value) const
{
    auto p     = contents_index_.find(key);
    bool found = (p != contents_index_.end());
    if (found) {
        size_t index = p->second;
        var          = string_as_T<T>(contents_[index]);
    }
    else {
        var = value;
    }
    return found;
}

template<class T>
void UConfig::add(const std::string& in_key, const T& in_value)
{
    std::string v   = T_as_string(in_value);
    std::string key = in_key;
    Trim(key);
    Trim(v);
    contents_index_.insert(std::pair<std::string, int>(key, contents_size_));
    contents_.push_back(v);
    contents_size_++;
}

template<class T>
std::string UConfig::T_as_string(const T& t)
{
    std::ostringstream ost;
    ost << t;
    return ost.str();
}

template<class T>
T UConfig::string_as_T(const std::string& s)
{
    T                  t;
    std::istringstream ist(s);
    ist >> t;
    return t;
}

template<>
inline std::string UConfig::string_as_T<std::string>(const std::string& s)
{
    return s;
}

template<>
inline bool UConfig::string_as_T<bool>(const std::string& s)
{
    bool        b   = true;
    std::string sup = s;
    for (std::string::iterator p = sup.begin(); p != sup.end(); ++p) {
        *p = toupper(*p);   // 转化为大写
    }
    if (sup == std::string("FALSE") || sup == std::string("F") || sup == std::string("NO") || sup == std::string("N") || sup == std::string("0") || sup == std::string("NONE")) {
        b = false;
    }
    return b;
}

NAO_NAMESPACE_END
#endif