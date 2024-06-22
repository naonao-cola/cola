/**
 * @FilePath     : /cola/src/UtilsCtrl/Str/UStrDefine.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-22 23:47:19
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-22 23:54:05
 **/
#ifndef NAO_USTRDEINE_H
#define NAO_USTRDEINE_H

#include <string>
#include <vector>

#include "../UtilsObject.h"

NAO_NAMESPACE_BEGIN

typedef int Py_ssize_t;
#define MAX_32BIT_INT 2147483647

// 用于修复开始/结束切片值的辅助宏
#define ADJUST_INDICES(start, end, len) \
    if (end > len)                      \
        end = len;                      \
    else if (end < 0) {                 \
        end += len;                     \
        if (end < 0) end = 0;           \
    }                                   \
    if (start < 0) {                    \
        start += len;                   \
        if (start < 0) start = 0;       \
    }

#define LEFTSTRIP  0
#define RIGHTSTRIP 1
#define BOTHSTRIP  2

namespace splitOP {
/**
 * @brief 将字符串数组中的字符串颠倒顺序，前面的放在后面，后面的放在前面
 * @param result
 */
void reverse_strings(std::vector<std::string>& result)
{
    for (std::vector<std::string>::size_type i = 0; i < result.size() / 2; i++) {
        std::swap(result[i], result[result.size() - 1 - i]);
    }
}
/**
 * @brief		  去除空格，按空格分割字符串
 * @param str	   输入字符串
 * @param result   输出字符串
 * @param maxsplit 最大分割次数
 */
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

/**
 * @brief		从右侧去除空格，按空格分割字符串
 * @param str
 * @param result
 * @param maxsplit
 */
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

/**
 * @brief 匹配自身的结尾（方向> = 0）或开始（方向<0）查找substr，使用开始和结束参数。出错时返回-1，未找到返回0，找到返回1
 * @param self
 * @param substr
 * @param start
 * @param end
 * @param direction
 * @return
 */
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
    if (end - start >= slen) return (!std::memcmp(str + start, sub, slen));
    return 0;
}
}   // namespace match
NAO_NAMESPACE_END
#endif