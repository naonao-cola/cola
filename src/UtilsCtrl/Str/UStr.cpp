﻿/**
 * @FilePath     : /cola/src/UtilsCtrl/Str/UStr.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-21 18:47:56
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-23 00:16:51
 **/
#include "UStr.h"
#include <sstream>


NAO_NAMESPACE_BEGIN

void UStr::split(const std::string& str, std::vector<std::string>& result, const std::string& sep, int maxsplit)
{
    result.clear();
    if (maxsplit < 0) maxsplit = MAX_32BIT_INT;
    if (sep.size() == 0) {
        splitOP::split_whitespace(str, result, maxsplit);
        return;
    }
    std::string::size_type i, j, len = str.size(), n = sep.size();
    i = j = 0;
    while (i + n <= len) {
        if (str[i] == sep[0] && str.substr(i, n) == sep) {
            if (maxsplit-- <= 0) break;
            result.push_back(str.substr(j, i - j));
            i = j = i + n;
        }
        else {
            i++;
        }
    }
    result.push_back(str.substr(j, len - j));
}

void UStr::rsplit(const std::string& str, std::vector<std::string>& result, const std::string& sep, int maxsplit)
{
    if (maxsplit < 0) {
        split(str, result, sep, maxsplit);
        return;
    }
    result.clear();
    if (maxsplit < 0) maxsplit = MAX_32BIT_INT;
    if (sep.size() == 0) {
        splitOP::rsplit_whitespace(str, result, maxsplit);
        return;
    }
    Py_ssize_t i, j, len = (Py_ssize_t)str.size(), n = (Py_ssize_t)sep.size();
    i = j = len;
    while (i >= n) {
        if (str[i - 1] == sep[n - 1] && str.substr(i - n, n) == sep) {
            if (maxsplit-- <= 0) break;
            result.push_back(str.substr(i, j - i));
            i = j = i - n;
        }
        else {
            i--;
        }
    }
    result.push_back(str.substr(0, j));
    splitOP::reverse_strings(result);
}

std::string UStr::do_strip(const std::string& str, int striptype, const std::string& chars)
{
    Py_ssize_t len = (Py_ssize_t)str.size(), i, j, charslen = (Py_ssize_t)chars.size();
    if (charslen == 0) {
        i = 0;
        if (striptype != RIGHTSTRIP) {   // 从左侧记录第一个不是空格的字符，记为i
            while (i < len && ::isspace(str[i])) {
                i++;
            }
        }
        j = len;
        if (striptype != LEFTSTRIP) {
            do {
                j--;
            } while (j >= i && ::isspace(str[j]));   // 从右侧记录第一个不是空格的字符，记为j
            j++;
        }
    }
    else {
        const char* sep = chars.c_str();
        i               = 0;
        if (striptype != RIGHTSTRIP) {
            while (i < len && memchr(sep, str[i], charslen)) {   // 从左侧记录第一个是char的字符，记为i
                i++;
            }
        }
        j = len;
        if (striptype != LEFTSTRIP) {
            do {
                j--;
            } while (j >= i && memchr(sep, str[j], charslen));   // 从右侧记录第一个是char的字符，记为j
            j++;
        }
    }
    if (i == 0 && j == len) {
        return str;
    }
    else {
        return str.substr(i, j - i);
    }
}

std::string UStr::strip(const std::string& str, const std::string& chars)
{
    return do_strip(str, BOTHSTRIP, chars);
}

std::string UStr::lstrip(const std::string& str, const std::string& chars)
{
    return do_strip(str, LEFTSTRIP, chars);
}

std::string UStr::rstrip(const std::string& str, const std::string& chars)
{
    return do_strip(str, RIGHTSTRIP, chars);
}

int UStr::find(const std::string& str, const std::string& sub, int start, int end)
{
    ADJUST_INDICES(start, end, (int)str.size());
    std::string::size_type result = str.find(sub, start);
    if (result == std::string::npos || (result + sub.size() > (std::string::size_type)end)) return -1;
    return (int)result;
}

int UStr::rfind(const std::string& str, const std::string& sub, int start, int end)
{
    ADJUST_INDICES(start, end, (int)str.size());
    std::string::size_type result = str.rfind(sub, end);
    if (result == std::string::npos || result < (std::string::size_type)start || (result + sub.size() > (std::string::size_type)end)) return -1;
    return (int)result;
}

int UStr::index(const std::string& str, const std::string& sub, int start, int end)
{
    return find(str, sub, start, end);
}

int UStr::rindex(const std::string& str, const std::string& sub, int start, int end)
{
    return rfind(str, sub, start, end);
}

std::string UStr::slice(const std::string& str, int start, int end)
{
    ADJUST_INDICES(start, end, (int)str.size());
    if (start >= end) return "";
    return str.substr(start, end - start);
}

void UStr::partition(const std::string& str, const std::string& sep, std::vector<std::string>& result)
{
    result.resize(3);
    int index = find(str, sep);
    if (index < 0) {
        result[0] = str;
        result[1] = "";
        result[2] = "";
    }
    else {
        result[0] = str.substr(0, index);
        result[1] = sep;
        result[2] = str.substr(index + sep.size(), str.size());
    }
}

void UStr::rpartition(const std::string& str, const std::string& sep, std::vector<std::string>& result)
{
    result.resize(3);
    int index = rfind(str, sep);
    if (index < 0) {
        result[0] = "";
        result[1] = "";
        result[2] = str;
    }
    else {
        result[0] = str.substr(0, index);
        result[1] = sep;
        result[2] = str.substr(index + sep.size(), str.size());
    }
}

std::string UStr::join(const std::string& str, const std::vector<std::string>& seq)
{
    std::vector<std::string>::size_type seqlen = seq.size(), i;
    if (seqlen == 0) return "";
    if (seqlen == 1) return seq[0];
    std::string result(seq[0]);
    for (i = 1; i < seqlen; ++i) {
        result += str + seq[i];
    }
    return result;
}

bool UStr::endswith(const std::string& str, const std::string& suffix, int start, int end)
{
    int result = match::_string_tailmatch(str, suffix, (Py_ssize_t)start, (Py_ssize_t)end, +1);
    // if (result == -1) // TODO: Error condition
    return static_cast<bool>(result);
}

bool UStr::startswith(const std::string& str, const std::string& prefix, int start, int end)
{
    int result = match::_string_tailmatch(str, prefix, (Py_ssize_t)start, (Py_ssize_t)end, -1);
    // if (result == -1) // TODO: Error condition
    return static_cast<bool>(result);
}

bool UStr::isalnum(const std::string& str)
{
    std::string::size_type len = str.size(), i;
    if (len == 0) return false;
    if (len == 1) return ::isalnum(str[0]);
    for (i = 0; i < len; ++i) {
        if (!::isalnum(str[i])) return false;
    }
    return true;
}

bool UStr::isalpha(const std::string& str)
{
    std::string::size_type len = str.size(), i;
    if (len == 0) return false;
    if (len == 1) return ::isalpha((int)str[0]);
    for (i = 0; i < len; ++i) {
        if (!::isalpha((int)str[i])) return false;
    }
    return true;
}

bool UStr::isdigit(const std::string& str)
{
    std::string::size_type len = str.size(), i;
    if (len == 0) return false;
    if (len == 1) return ::isdigit(str[0]);
    for (i = 0; i < len; ++i) {
        if (!::isdigit(str[i])) return false;
    }
    return true;
}

bool UStr::islower(const std::string& str)
{
    std::string::size_type len = str.size(), i;
    if (len == 0) return false;
    if (len == 1) return ::islower(str[0]);
    for (i = 0; i < len; ++i) {
        if (!::islower(str[i])) return false;
    }
    return true;
}


bool UStr::isupper(const std::string& str)
{
    std::string::size_type len = str.size(), i;
    if (len == 0) return false;
    if (len == 1) return ::isupper(str[0]);
    for (i = 0; i < len; ++i) {
        if (!::isupper(str[i])) return false;
    }
    return true;
}

bool UStr::isspace(const std::string& str)
{
    std::string::size_type len = str.size(), i;
    if (len == 0) return false;
    if (len == 1) return ::isspace(str[0]);
    for (i = 0; i < len; ++i) {
        if (!::isspace(str[i])) return false;
    }
    return true;
}

bool UStr::istitle(const std::string& str)
{
    std::string::size_type len = str.size(), i;
    if (len == 0) return false;
    if (len == 1) return ::isupper(str[0]);
    bool cased = false, previous_is_cased = false;
    for (i = 0; i < len; ++i) {
        if (::isupper(str[i])) {
            if (previous_is_cased) {
                return false;
            }
            previous_is_cased = true;
            cased             = true;
        }
        else if (::islower(str[i])) {
            if (!previous_is_cased) {
                return false;
            }
            previous_is_cased = true;
            cased             = true;
        }
        else {
            previous_is_cased = false;
        }
    }
    return cased;
}

std::string UStr::capitalize(const std::string& str)
{
    std::string            s(str);
    std::string::size_type len = s.size(), i;
    if (len > 0) {
        if (::islower(s[0])) s[0] = (char)::toupper(s[0]);
    }
    for (i = 1; i < len; ++i) {
        if (::isupper(s[i])) s[i] = (char)::tolower(s[i]);
    }
    return s;
}

std::string UStr::lower(const std::string& str)
{
    std::string            s(str);
    std::string::size_type len = s.size(), i;
    for (i = 0; i < len; ++i) {
        if (::isupper(s[i])) s[i] = (char)::tolower(s[i]);
    }
    return s;
}

std::string UStr::upper(const std::string& str)
{
    std::string            s(str);
    std::string::size_type len = s.size(), i;
    for (i = 0; i < len; ++i) {
        if (::islower(s[i])) s[i] = (char)::toupper(s[i]);
    }
    return s;
}

std::string UStr::swapcase(const std::string& str)
{
    std::string            s(str);
    std::string::size_type len = s.size(), i;
    for (i = 0; i < len; ++i) {
        if (::islower(s[i]))
            s[i] = (char)::toupper(s[i]);
        else if (::isupper(s[i]))
            s[i] = (char)::tolower(s[i]);
    }
    return s;
}

std::string UStr::title(const std::string& str)
{
    std::string            s(str);
    std::string::size_type len               = s.size(), i;
    bool                   previous_is_cased = false;
    for (i = 0; i < len; ++i) {
        int c = s[i];
        if (::islower(c)) {
            if (!previous_is_cased) {
                s[i] = (char)::toupper(c);
            }
            previous_is_cased = true;
        }
        else if (::isupper(c)) {
            if (previous_is_cased) {
                s[i] = (char)::tolower(c);
            }
            previous_is_cased = true;
        }
        else {
            previous_is_cased = false;
        }
    }
    return s;
}

std::string UStr::translate(const std::string& str, const std::string& table, const std::string& deletechars)
{
    std::string            s;
    std::string::size_type len = str.size(), dellen = deletechars.size();
    if (table.size() != 256) {
        // TODO : raise exception instead
        return str;
    }
    // if nothing is deleted, use faster code
    if (dellen == 0) {
        s = str;
        for (std::string::size_type i = 0; i < len; ++i) {
            s[i] = table[s[i]];
        }
        return s;
    }
    int trans_table[256];
    for (int i = 0; i < 256; i++) {
        trans_table[i] = table[i];
    }
    for (std::string::size_type i = 0; i < dellen; i++) {
        trans_table[(int)deletechars[i]] = -1;
    }
    for (std::string::size_type i = 0; i < len; ++i) {
        if (trans_table[(int)str[i]] != -1) {
            s += table[str[i]];
        }
    }
    return s;
}

std::string UStr::zfill(const std::string& str, int width)
{
    int len = (int)str.size();
    if (len >= width) return str;
    std::string s(str);
    int         fill = width - len;
    s                = std::string(fill, '0') + s;
    if (s[fill] == '+' || s[fill] == '-') {   // 正负号交换
        s[0]    = s[fill];
        s[fill] = '0';
    }
    return s;
}

std::string UStr::ljust(const std::string& str, int width)
{
    std::string::size_type len = str.size();
    if (((int)len) >= width) return str;
    return str + std::string(width - len, ' ');
}

std::string UStr::rjust(const std::string& str, int width)
{
    std::string::size_type len = str.size();
    if (((int)len) >= width) return str;
    return std::string(width - len, ' ') + str;
}

std::string UStr::center(const std::string& str, int width)
{
    int len = (int)str.size();
    int marg, left;
    if (len >= width) return str;
    marg = width - len;
    left = marg / 2 + (marg & width & 1);
    return std::string(left, ' ') + str + std::string(marg - left, ' ');
}

std::string UStr::expandtabs(const std::string& str, int tabsize)
{
    std::string            s(str);
    std::string::size_type len = str.size(), i = 0;
    int                    offset = 0;
    int                    j      = 0;
    for (i = 0; i < len; ++i) {
        if (str[i] == '\t') {
            if (tabsize > 0) {
                int fillsize = tabsize - (j % tabsize);
                j += fillsize;
                s.replace(i + offset, 1, std::string(fillsize, ' '));
                offset += fillsize - 1;
            }
            else {
                s.replace(i + offset, 1, "");
                offset -= 1;
            }
        }
        else {
            j++;
            if (str[i] == '\n' || str[i] == '\r') {
                j = 0;
            }
        }
    }
    return s;
}

int UStr::count(const std::string& str, const std::string& substr, int start, int end)
{
    int nummatches = 0;
    int cursor     = start;
    while (1) {
        cursor = find(str, substr, cursor, end);
        if (cursor < 0) break;
        cursor += (int)substr.size();
        nummatches += 1;
    }
    return nummatches;
}

std::string UStr::replace(const std::string& str, const std::string& oldstr, const std::string& newstr, int count)
{
    int                    sofar  = 0;
    int                    cursor = 0;
    std::string            s(str);
    std::string::size_type oldlen = oldstr.size(), newlen = newstr.size();
    cursor = find(s, oldstr, cursor);
    while (cursor != -1 && cursor <= (int)s.size()) {
        if (count > -1 && sofar >= count) {
            break;
        }
        s.replace(cursor, oldlen, newstr);
        cursor += (int)newlen;
        if (oldlen != 0) {
            cursor = find(s, oldstr, cursor);
        }
        else {
            ++cursor;
        }
        ++sofar;
    }
    return s;
}

void UStr::splitlines(const std::string& str, std::vector<std::string>& result, bool keepends)
{
    result.clear();
    std::string::size_type len = str.size(), i, j, eol;
    for (i = j = 0; i < len;) {
        while (i < len && str[i] != '\n' && str[i] != '\r') i++;
        eol = i;
        if (i < len) {
            if (str[i] == '\r' && i + 1 < len && str[i + 1] == '\n') {
                i += 2;
            }
            else {
                i++;
            }
            if (keepends) {
                eol = i;
            }
        }
        result.push_back(str.substr(j, eol - j));
        j = i;
    }
    if (j < len) {
        result.push_back(str.substr(j, len - j));
    }
}

std::string UStr::mul(const std::string& str, int n)
{
    // Early exits
    if (n <= 0) return "";
    if (n == 1) return str;

    std::ostringstream os;
    for (int i = 0; i < n; ++i) os << str;
    return os.str();
}
NAO_NAMESPACE_END