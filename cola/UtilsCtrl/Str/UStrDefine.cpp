/**
 * @FilePath     : /cola/cola/UtilsCtrl/Str/UStrDefine.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 09:54:03
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 23:08:38
 **/
#include <cstring>
#include "UStrDefine.h"
NAO_NAMESPACE_BEGIN

namespace splitOP {

void reverse_strings(std::vector<std::string>& result)
{
    for (std::vector<std::string>::size_type i = 0; i < result.size() / 2; i++) {
        std::swap(result[i], result[result.size() - 1 - i]);
    }
}
void split_whitespace(const std::string& str, std::vector<std::string>& result, int maxsplit)
{
    std::string::size_type i, j, len = str.size();
    for (i = j = 0; i < len;) {
        while (i < len && ::isspace(str[i])) i++;   // 记录字符串开头的第一个不是空格的位置，记为j
        j = i;
        while (i < len && !::isspace(str[i])) i++;   // 记录字符串，再次是空格的位置，记为i,
        if (j < i) {
            if (maxsplit-- <= 0) break;
            result.push_back(str.substr(j, i - j));     // 提取j到i的字符串。
            while (i < len && ::isspace(str[i])) i++;   // 再次记录剩下的，第一个不是空格的位置，记为j。循环。
            j = i;
        }
    }
    if (j < len) {   // 将剩余的部分加入到结果
        result.push_back(str.substr(j, len - j));
    }
}
void rsplit_whitespace(const std::string& str, std::vector<std::string>& result, int maxsplit)
{
    std::string::size_type len = str.size();
    std::string::size_type i, j;
    for (i = j = len; i > 0;) {   // 从右侧开始
        while (i > 0 && ::isspace(str[i - 1])) i--;
        j = i;
        while (i > 0 && !::isspace(str[i - 1])) i--;
        if (j > i) {
            if (maxsplit-- <= 0) break;
            result.push_back(str.substr(i, j - i));
            while (i > 0 && ::isspace(str[i - 1])) i--;
            j = i;
        }
    }
    if (j > 0) {
        result.push_back(str.substr(0, j));   // 前面的字符串是最后放进去的，后面的字符串是最先放进去的。
    }
    // std::reverse( result, result.begin(), result.end() );
    reverse_strings(result);   // 此处颠倒顺序
}
}   // namespace splitOP

namespace match {

int _string_tailmatch(const std::string& self, const std::string& substr, Py_ssize_t start, Py_ssize_t end, int direction)
{
    Py_ssize_t  len  = (Py_ssize_t)self.size();
    Py_ssize_t  slen = (Py_ssize_t)substr.size();
    const char* sub  = substr.c_str();
    const char* str  = self.c_str();
    ADJUST_INDICES(start, end, len);
    if (direction < 0) {
        // startswith
        if (start + slen > len) return 0;
    }
    else {
        // endswith
        if (end - start < slen || start > len) return 0;
        if (end - slen > start) start = end - slen;
    }
    if (end - start >= slen) return (!memcmp(str + start, sub, slen));
    return 0;
}
}   // namespace match
NAO_NAMESPACE_END