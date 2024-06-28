/**
 * @FilePath     : /cola/src/UtilsCtrl/Str/UStrDefine.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-22 23:47:19
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 09:52:37
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

#define LEFTSTRIP 0
#define RIGHTSTRIP 1
#define BOTHSTRIP 2

namespace splitOP {
/**
 * @brief 将字符串数组中的字符串颠倒顺序，前面的放在后面，后面的放在前面
 * @param result
 */
void reverse_strings(std::vector<std::string>& result);

/**
 * @brief		  去除空格，按空格分割字符串
 * @param str	   输入字符串
 * @param result   输出字符串
 * @param maxsplit 最大分割次数
 */
void split_whitespace(const std::string& str, std::vector<std::string>& result, int maxsplit);

/**
 * @brief		从右侧去除空格，按空格分割字符串
 * @param str
 * @param result
 * @param maxsplit
 */
void rsplit_whitespace(const std::string& str, std::vector<std::string>& result, int maxsplit);
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
int _string_tailmatch(const std::string& self, const std::string& substr, Py_ssize_t start, Py_ssize_t end, int direction);
}   // namespace match
NAO_NAMESPACE_END
#endif