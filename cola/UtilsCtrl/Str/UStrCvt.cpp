/**
 * @FilePath     : /cola/cola/UtilsCtrl/Str/UStrCvt.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-07-15 13:18:21
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-15 14:46:33
 **/

#include "UStrCvt.h"

#if defined(_WIN32)
    #include <windows.h>
#elif defined(__linux__)
    #include <cstdlib>
    #include <cstring>
#endif

NAO_NAMESPACE_BEGIN

#if defined(_WIN32)

wchar_t* UStrCvt::AnsiToUnicode(const char* lpszStr)
{
    wchar_t* lpUnicode = nullptr;
    int      nLen      = 0;
    if (nullptr == lpszStr) {
        return nullptr;
    }
    nLen = MultiByteToWideChar(CP_ACP, 0, lpszStr, -1, nullptr, 0);
    if (0 == nLen) {
        return nullptr;
    }
    lpUnicode = new wchar_t[nLen + 1];
    if (nullptr == lpUnicode) {
        return nullptr;
    }
    memset(lpUnicode, 0, sizeof(wchar_t) * (nLen + 1));
    nLen = MultiByteToWideChar(CP_ACP, 0, lpszStr, -1, lpUnicode, nLen);
    if (0 == nLen) {
        delete[] lpUnicode;
        return nullptr;
    }
    return lpUnicode;
}


char* UStrCvt::UnicodeToAnsi(const wchar_t* lpszStr)
{
    char* lpAnsi = nullptr;
    int   nLen   = 0;
    if (nullptr == lpszStr) {
        return nullptr;
    }
    nLen = WideCharToMultiByte(CP_ACP, 0, lpszStr, -1, nullptr, 0, nullptr, nullptr);
    if (0 == nLen) {
        return nullptr;
    }
    lpAnsi = new char[nLen + 1];
    if (nullptr == lpAnsi) {
        return nullptr;
    }
    memset(lpAnsi, 0, nLen + 1);
    nLen = WideCharToMultiByte(CP_ACP, 0, lpszStr, -1, lpAnsi, nLen, nullptr, nullptr);
    if (0 == nLen) {
        delete[] lpAnsi;
        return nullptr;
    }
    return lpAnsi;
}

char* UStrCvt::AnsiToUtf8(const char* lpszStr)
{
    wchar_t* lpUnicode = nullptr;
    char*    lpUtf8    = nullptr;
    int      nLen      = 0;
    if (nullptr == lpszStr) {
        return nullptr;
    }
    nLen = MultiByteToWideChar(CP_ACP, 0, lpszStr, -1, nullptr, NULL);
    if (0 == nLen) {
        return nullptr;
    }
    lpUnicode = new wchar_t[nLen + 1];
    if (nullptr == lpUnicode) {
        return nullptr;
    }
    memset(lpUnicode, 0, sizeof(wchar_t) * (nLen + 1));
    nLen = MultiByteToWideChar(CP_ACP, 0, lpszStr, -1, lpUnicode, nLen);
    if (0 == nLen) {
        delete[] lpUnicode;
        return nullptr;
    }
    nLen = WideCharToMultiByte(CP_UTF8, 0, lpUnicode, -1, nullptr, 0, nullptr, nullptr);
    if (0 == nLen) {
        delete[] lpUnicode;
        return nullptr;
    }
    lpUtf8 = new char[nLen + 1];
    if (nullptr == lpUtf8) {
        delete[] lpUnicode;
        return nullptr;
    }
    memset(lpUtf8, 0, nLen + 1);
    nLen = WideCharToMultiByte(CP_UTF8, 0, lpUnicode, -1, lpUtf8, nLen, nullptr, nullptr);
    if (0 == nLen) {
        delete[] lpUnicode;
        delete[] lpUtf8;
        return nullptr;
    }
    delete[] lpUnicode;
    return lpUtf8;
}


char* UStrCvt::Utf8ToAnsi(const char* lpszStr)
{
    wchar_t* lpUnicode = nullptr;
    char*    lpAnsi    = nullptr;
    int      nLen      = 0;
    if (nullptr == lpszStr) {
        return nullptr;
    }
    nLen = MultiByteToWideChar(CP_UTF8, 0, lpszStr, -1, nullptr, NULL);
    if (0 == nLen) {
        return nullptr;
    }
    lpUnicode = new wchar_t[nLen + 1];
    if (nullptr == lpUnicode) {
        return nullptr;
    }
    memset(lpUnicode, 0, sizeof(wchar_t) * (nLen + 1));
    nLen = MultiByteToWideChar(CP_UTF8, 0, lpszStr, -1, lpUnicode, nLen);
    if (0 == nLen) {
        delete[] lpUnicode;
        return nullptr;
    }
    nLen = WideCharToMultiByte(CP_ACP, 0, lpUnicode, -1, nullptr, 0, nullptr, nullptr);
    if (0 == nLen) {
        delete[] lpUnicode;
        return nullptr;
    }
    lpAnsi = new char[nLen + 1];
    if (nullptr == lpAnsi) {
        delete[] lpUnicode;
        return nullptr;
    }
    memset(lpAnsi, 0, nLen + 1);
    nLen = WideCharToMultiByte(CP_ACP, 0, lpUnicode, -1, lpAnsi, nLen, nullptr, nullptr);
    if (0 == nLen) {
        delete[] lpUnicode;
        delete[] lpAnsi;
        return nullptr;
    }
    delete[] lpUnicode;
    return lpAnsi;
}

char* UStrCvt::UnicodeToUtf8(const wchar_t* lpszStr)
{
    char* lpUtf8 = nullptr;
    int   nLen   = 0;
    if (nullptr == lpszStr) {
        return nullptr;
    }
    nLen = WideCharToMultiByte(CP_UTF8, 0, lpszStr, -1, nullptr, 0, nullptr, nullptr);
    if (0 == nLen) {
        return nullptr;
    }
    lpUtf8 = new char[nLen + 1];
    if (nullptr == lpUtf8) {
        return nullptr;
    }
    memset(lpUtf8, 0, nLen + 1);
    nLen = WideCharToMultiByte(CP_UTF8, 0, lpszStr, -1, lpUtf8, nLen, nullptr, nullptr);
    if (0 == nLen) {
        delete[] lpUtf8;
        return nullptr;
    }
    return lpUtf8;
}

wchar_t* UStrCvt::Utf8ToUnicode(const char* lpszStr)
{
    wchar_t* lpUnicode = nullptr;
    int      nLen      = 0;
    if (nullptr == lpszStr) {
        return nullptr;
    }
    nLen = MultiByteToWideChar(CP_UTF8, 0, lpszStr, -1, nullptr, 0);
    if (0 == nLen) {
        return nullptr;
    }
    lpUnicode = new wchar_t[nLen + 1];
    if (nullptr == lpUnicode) {
        return nullptr;
    }
    memset(lpUnicode, 0, sizeof(wchar_t) * (nLen + 1));
    nLen = MultiByteToWideChar(CP_UTF8, 0, lpszStr, -1, lpUnicode, nLen);
    if (0 == nLen) {
        delete[] lpUnicode;
        return nullptr;
    }
    return lpUnicode;
}

bool UStrCvt::AnsiToUnicode(const char* lpszAnsi, wchar_t* lpszUnicode, int nLen)
{
    int nRet = MultiByteToWideChar(CP_ACP, 0, lpszAnsi, -1, lpszUnicode, nLen);
    return ((0 == nRet) ? FALSE : TRUE) != 0;
}

bool UStrCvt::UnicodeToAnsi(const wchar_t* lpszUnicode, char* lpszAnsi, int nLen)
{
    int nRet = WideCharToMultiByte(CP_ACP, 0, lpszUnicode, -1, lpszAnsi, nLen, nullptr, nullptr);
    return ((0 == nRet) ? FALSE : TRUE) != 0;
}

bool UStrCvt::AnsiToUtf8(const char* lpszAnsi, char* lpszUtf8, int nLen)
{
    wchar_t* lpszUnicode = AnsiToUnicode(lpszAnsi);
    if (nullptr == lpszUnicode) {
        return FALSE;
    }
    int nRet = static_cast<int>(UnicodeToUtf8(lpszUnicode, lpszUtf8, nLen));
    delete[] lpszUnicode;
    return ((0 == nRet) ? FALSE : TRUE) != 0;
}

bool UStrCvt::Utf8ToAnsi(const char* lpszUtf8, char* lpszAnsi, int nLen)
{
    wchar_t* lpszUnicode = Utf8ToUnicode(lpszUtf8);
    if (nullptr == lpszUnicode) {
        return FALSE;
    }
    int nRet = static_cast<int>(UnicodeToAnsi(lpszUnicode, lpszAnsi, nLen));
    delete[] lpszUnicode;
    return ((0 == nRet) ? FALSE : TRUE) != 0;
}

bool UStrCvt::UnicodeToUtf8(const wchar_t* lpszUnicode, char* lpszUtf8, int nLen)
{
    int nRet = WideCharToMultiByte(CP_UTF8, 0, lpszUnicode, -1, lpszUtf8, nLen, nullptr, nullptr);
    return ((0 == nRet) ? FALSE : TRUE) != 0;
}

bool UStrCvt::Utf8ToUnicode(const char* lpszUtf8, wchar_t* lpszUnicode, int nLen)
{
    int nRet = MultiByteToWideChar(CP_UTF8, 0, lpszUtf8, -1, lpszUnicode, nLen);
    return ((0 == nRet) ? FALSE : TRUE) != 0;
}

std::wstring UStrCvt::AnsiToUnicode(const std::string& strAnsi)
{
    std::wstring strUnicode;
    wchar_t*     lpszUnicode = AnsiToUnicode(strAnsi.c_str());
    if (lpszUnicode != nullptr) {
        strUnicode = lpszUnicode;
        delete[] lpszUnicode;
    }
    return strUnicode;
}
std::string UStrCvt::UnicodeToAnsi(const std::wstring& strUnicode)
{
    std::string strAnsi;
    char*       lpszAnsi = UnicodeToAnsi(strUnicode.c_str());
    if (lpszAnsi != nullptr) {
        strAnsi = lpszAnsi;
        delete[] lpszAnsi;
    }
    return strAnsi;
}

std::string UStrCvt::AnsiToUtf8(const std::string& strAnsi)
{
    std::string strUtf8;
    char*       lpszUtf8 = AnsiToUtf8(strAnsi.c_str());
    if (lpszUtf8 != nullptr) {
        strUtf8 = lpszUtf8;
        delete[] lpszUtf8;
    }
    return strUtf8;
}

std::string UStrCvt::Utf8ToAnsi(const std::string& strUtf8)
{
    std::string strAnsi;
    char*       lpszAnsi = Utf8ToAnsi(strUtf8.c_str());
    if (lpszAnsi != nullptr) {
        strAnsi = lpszAnsi;
        delete[] lpszAnsi;
    }
    return strAnsi;
}

std::string UStrCvt::UnicodeToUtf8(const std::wstring& strUnicode)
{
    std::string strUtf8;
    char*       lpszUtf8 = UnicodeToUtf8(strUnicode.c_str());
    if (lpszUtf8 != nullptr) {
        strUtf8 = lpszUtf8;
        delete[] lpszUtf8;
    }
    return strUtf8;
}

std::wstring UStrCvt::Utf8ToUnicode(const std::string& strUtf8)
{
    std::wstring strUnicode;
    wchar_t*     lpszUnicode = Utf8ToUnicode(strUtf8.c_str());
    if (lpszUnicode != nullptr) {
        strUnicode = lpszUnicode;
        delete[] lpszUnicode;
    }
    return strUnicode;
};
#elif defined(__linux__)

char* UStrCvt::AnsiToUtf8(const char* lpszStr)
{
    wchar_t* lpUnicode = nullptr;
    char*    lpUtf8    = nullptr;
    int      nLen      = 0;

    if (nullptr == lpszStr)
        return nullptr;

    nLen = mbstowcs(nullptr, lpszStr, 0);
    if (nLen <= 0)
        return nullptr;

    lpUnicode = new wchar_t[nLen + 1];
    if (nullptr == lpUnicode)
        return nullptr;

    mbstowcs(lpUnicode, lpszStr, nLen);
    lpUnicode[nLen] = L'\0';

    nLen = wcstombs(nullptr, nullptr, 0);
    if (nLen <= 0) {
        delete[] lpUnicode;
        return nullptr;
    }

    lpUtf8 = new char[nLen + 1];
    if (nullptr == lpUtf8) {
        delete[] lpUnicode;
        return nullptr;
    }

    wcstombs(lpUtf8, lpUnicode, nLen);
    lpUtf8[nLen] = '\0';

    delete[] lpUnicode;

    return lpUtf8;
}

char* UStrCvt::Utf8ToAnsi(const char* lpszStr)
{
    wchar_t* lpUnicode = nullptr;
    char*    lpAnsi    = nullptr;
    int      nLen      = 0;

    if (nullptr == lpszStr)
        return nullptr;

    // 获取UTF-8字符串长度
    nLen = mbstowcs(nullptr, lpszStr, 0);
    if (nLen <= 0)
        return nullptr;

    lpUnicode = new wchar_t[nLen + 1];
    if (nullptr == lpUnicode)
        return nullptr;

    // 转换UTF-8到宽字符
    mbstowcs(lpUnicode, lpszStr, nLen);
    lpUnicode[nLen] = L'\0';

    // 获取ANSI字符串长度
    nLen = wcstombs(nullptr, lpUnicode, 0);
    if (nLen <= 0) {
        delete[] lpUnicode;
        return nullptr;
    }

    lpAnsi = new char[nLen + 1];
    if (nullptr == lpAnsi) {
        delete[] lpUnicode;
        return nullptr;
    }

    // 转换宽字符到ANSI
    wcstombs(lpAnsi, lpUnicode, nLen);
    lpAnsi[nLen] = '\0';

    delete[] lpUnicode;

    return lpAnsi;
}

std::string UStrCvt::AnsiToUtf8(const std::string& strAnsi)
{
    std::string strUtf8;

    char* lpszUtf8 = AnsiToUtf8(strAnsi.c_str());
    if (lpszUtf8 != nullptr) {
        strUtf8 = lpszUtf8;
        delete[] lpszUtf8;
    }

    return strUtf8;
}

std::string UStrCvt::Utf8ToAnsi(const std::string& strUtf8)
{
    std::string strAnsi;

    char* lpszAnsi = Utf8ToAnsi(strUtf8.c_str());
    if (lpszAnsi != nullptr) {
        strAnsi = lpszAnsi;
        delete[] lpszAnsi;
    }

    return strAnsi;
}
#endif
NAO_NAMESPACE_END