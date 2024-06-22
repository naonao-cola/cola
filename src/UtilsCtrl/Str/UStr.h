/**
 * @FilePath     : /cola/src/UtilsCtrl/Str/UStr.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-21 18:16:21
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-23 00:13:18
 **/
#ifndef NAO_USTR_H
#define NAO_USTR_H

#include "../UtilsObject.h"
#include "UStrDefine.h"
NAO_NAMESPACE_BEGIN

class UStr : public UtilsObject
{
    /**
     * @brief	sep作为分隔符，分割字符串。如果maxsplit大于-1，则最多执行maxsplit拆分。如果sep为“”，则空格是分隔符。
     * @param str
     * @param result
     * @param sep
     * @param maxsplit
     */
    static void split(const std::string& str, std::vector<std::string>& result, const std::string& sep = "", int maxsplit = -1);

    /**
     * @brief 从右侧分割,sep作为分隔符，分割字符串。如果maxsplit大于-1，则最多执行maxsplit拆分。如果sep为“”，则空格是分隔符。
     * @param str
     * @param result
     * @param sep
     * @param maxsplit
     */
    static void rsplit(const std::string& str, std::vector<std::string>& result, const std::string& sep = "", int maxsplit = -1);

    /**
     * @brief 从str两端去除char字符，char为空，则去除空格
     * @param str
     * @param striptype
     * @param chars
     * @return
     */
    static std::string do_strip(const std::string& str, int striptype, const std::string& chars);

    /**
     * @brief 返回，删除前导和尾随字符的字符串副本。如果chars为“”，将删除空白字符。
     * @param str
     * @param chars
     * @return
     */
    static std::string strip(const std::string& str, const std::string& chars = "");

    /**
     * @brief 返回，删除前导字符的字符串副本。如果省略字符或无字符，则删除空白字符。
     * @param str
     * @param chars
     * @return
     */
    static std::string lstrip(const std::string& str, const std::string& chars = "");

    /**
     * @brief 返回，删除尾随字符的字符串副本。如果省略字符或无字符，则删除空白字符。
     * @param str
     * @param chars
     * @return
     */
    static std::string rstrip(const std::string& str, const std::string& chars = "");

    /**
    * @brief 返回找到子字符串sub的字符串中的最低索引，以便sub包含在范围[start，end]中。可选参数start和end解释为切片表示法。
            如果找不到sub，则返回-1。
    * @param str
    * @param sub
    * @param start
    * @param end
    * @return
*/
    static int find(const std::string& str, const std::string& sub, int start = 0, int end = MAX_32BIT_INT);

    /**
     * @brief 从右侧进行查找,返回找到子字符串sub的字符串中的最低索引，以便sub包含在范围[start，end]中。
     *		  可选参数start和end解释为切片表示法。如果找不到sub，则返回-1。
     * @param str
     * @param sub
     * @param start
     * @param end
     * @return
     */
    static int rfind(const std::string& str, const std::string& sub, int start = 0, int end = MAX_32BIT_INT);

    /**
     * @brief 在str中寻找sub
     * @param str
     * @param sub
     * @param start
     * @param end
     * @return
     */
    static int index(const std::string& str, const std::string& sub, int start = 0, int end = MAX_32BIT_INT);

    /**
     * @brief 从右侧开始，在str中寻找sub
     * @param str
     * @param sub
     * @param start
     * @param end
     * @return
     */
    static int rindex(const std::string& str, const std::string& sub, int start = 0, int end = MAX_32BIT_INT);

    /**
     * @brief 对字符串进行切片，返回[start,end]之间的字符串。
     * @param str
     * @param start
     * @param end
     * @return
     */
    static std::string slice(const std::string& str, int start = 0, int end = MAX_32BIT_INT);


    /**
     * @brief 把字符串从sep的第一次出现分开。三个字符串将始终放在结果中。如果找到sep，字符串将是sep之前的文本，sep本身，
     *		  以及剩余的文本。 如果sep是未找到，将返回原始字符串和两个空字符串。
     * @param str
     * @param sep
     * @param result
     */
    static void partition(const std::string& str, const std::string& sep, std::vector<std::string>& result);

    /**
     * @brief 从右侧开始，把字符串从sep的第一次出现分开。三个字符串将始终放在结果中。如果找到sep，字符串将是sep之前的文本，sep本身，以及剩余的文本。
     *		  如果sep是未找到，将返回原始字符串和两个空字符串。
     * @param str
     * @param sep
     * @param result
     */
    static void rpartition(const std::string& str, const std::string& sep, std::vector<std::string>& result);


    /**
     * @brief 返回，序列seq中字符串的串联。元素之间的分隔符是str参数
     * @param str
     * @param seq
     * @return
     */
    static std::string join(const std::string& str, const std::vector<std::string>& seq);

    /**
     * @brief 如果字符串以指定的后缀结尾，则返回True，否则返回False。使用可选开始位置，从该位置开始测试。对于可选结束，
     *		  停止在该位置进行比较。
     * @param str
     * @param suffix
     * @param start
     * @param end
     * @return
     */
    static bool endswith(const std::string& str, const std::string& suffix, int start = 0, int end = MAX_32BIT_INT);

    /**
     * @brief 如果字符串以指定的后缀开始，则返回True，否则返回False。使用可选开始位置，从该位置开始测试。对于可选结束，停止在该位置进行比较。
     * @param str
     * @param prefix
     * @param start
     * @param end
     * @return
     */
    static bool startswith(const std::string& str, const std::string& prefix, int start = 0, int end = MAX_32BIT_INT);


    /**
     * @brief 如果字符串中的所有字符是否都是数字，都是数字则返回true，否则返回false。
     * @param str
     * @return
     */
    static bool isalnum(const std::string& str);

    /**
     * @brief 如果字符串中的所有字符都是字母，则返回true，否则返回false
     * @param str
     * @return
     */
    static bool isalpha(const std::string& str);

    /**
     * @brief 检查所传的字符是否是十进制数字字符。是则返回true.
     * @param str
     * @return
     */
    static bool isdigit(const std::string& str);

    /**
     * @brief 检查所传的字符是否是小写字符。是则返回true.
     * @param str
     * @return
     */
    static bool islower(const std::string& str);

    /**
     * @brief 是否大写
     * @param str
     * @return
     */
    static bool isupper(const std::string& str);

    /**
     * @brief 检查所传的字符是否全部是空格。是则返回true.
     * @param str
     * @return
     */
    static bool isspace(const std::string& str);

    /**
     * @brief 如果该字符串是一个带大写字母的字符串并且至少有一个字符，则返回true，即大写字母只能跟在无大小写的字符后面，
     *		  而小写字母只能跟在大写字符后面。 否则返回false。
     * @param str
     * @return
     */
    static bool istitle(const std::string& str);


    /**
     * @brief 返回只有第一个字符大写的字符串副本。
     * @param str
     * @return
     */
    static std::string capitalize(const std::string& str);

    /**
     * @brief 返回转换为小写的字符串的副本。
     * @param str
     * @return
     */
    static std::string lower(const std::string& str);

    /**
     * @brief 返回转换为大写的字符串的副本。
     * @param str
     * @return
     */
    static std::string upper(const std::string& str);

    /**
     * @brief 返回将大写字符转换为小写的字符串副本，反之亦然。
     * @param str
     * @return
     */
    static std::string swapcase(const std::string& str);

    /**
     * @brief 返回字符串的基于标题的版本：单词以大写字符开头，其余所有大小写字符均为小写。
     * @param str
     * @return
     */
    static std::string title(const std::string& str);


    /**
     * @brief 返回字符串的副本，其中删除了在deletechars中出现的所有字符，并且其余字符已通过给定的转换表进行映射，，table长度不为256，则返回原来的str
     *		  字符映射关系 ，该转换表的长度必须为256。
     * @param str
     * @param table
     * @param deletechars
     * @return
     */
    static std::string translate(const std::string& str, const std::string& table, const std::string& deletechars = "");

    /**
     * @brief 返回左侧被0填充的字符串， 如果width小于str.size（），则返回原始字符串。
     * @param str
     * @param width
     * @return
     */
    static std::string zfill(const std::string& str, int width);


    /**
     * @brief 左对齐，右侧填充空格，返回的长度为width.如果width小于str.size（），则返回原始字符串。
     * @param str
     * @param width
     * @return
     */
    static std::string ljust(const std::string& str, int width);

    /**
     * @brief 右对齐，左侧填充空格。返回的长度为width。如果width小于str.size（），则返回原始字符串。
     * @param str
     * @param width
     * @return
     */
    static std::string rjust(const std::string& str, int width);

    /**
     * @brief 字符串居中，左右填充空格
     * @param str
     * @param width
     * @return
     */
    static std::string center(const std::string& str, int width);

    /**
     * @brief 返回字符串的副本，其中所有制表符都使用空格进行扩展。 如果未指定tabsize，则假定制表符大小为8个字符。将table转换为空格
     * @param str
     * @param tabsize
     * @return
     */
    static std::string expandtabs(const std::string& str, int tabsize = 8);


    /**
     * @brief 返回子字符串sub在字符串S中的出现次数[start:end]. 可选参数start和end解释为切片表示法。
     * @param str
     * @param substr
     * @param start
     * @param end
     * @return
     */
    static int count(const std::string& str, const std::string& substr, int start = 0, int end = MAX_32BIT_INT);

    /**
     * @brief 将str中的所有子字符串old都替换为new。如果给定了可选参数count，则只替换第一次出现的count。
     * @param str
     * @param oldstr
     * @param newstr
     * @param count
     * @return
     */
    static std::string replace(const std::string& str, const std::string& oldstr, const std::string& newstr, int count = -1);


    /**
     * @brief 返回字符串中的行列表，在行边界处中断。 除非给出了keepends且为true，否则换行符不包含在结果列表中。
     * @param str
     * @param result
     * @param keepends
     */
    static void splitlines(const std::string& str, std::vector<std::string>& result, bool keepends = false);

    /**
     * @brief 一起返回串联的N次字符串的副本。对应于__mul__(+)运算符。
     * @param str
     * @param n
     * @return
     */
    static std::string mul(const std::string& str, int n);
};

NAO_NAMESPACE_END
#endif