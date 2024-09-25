/**
 * @FilePath     : /cola/cola/UtilsCtrl/Base64/UBase64.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-09-25 11:52:18
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-09-25 12:04:39
 * @Copyright (c) 2024 by G, All Rights Reserved.
 **/
#ifndef NAO_DHASH_H
#define NAO_DHASH_H

#include <cstddef>
#include <cstdint>

#include "../UtilsObject.h"

NAO_NAMESPACE_BEGIN

class UBase64 : public UtilsObject
{
public:
    /**
     * @brief: 编码，
     * @param Data
     * @param in_len
     * @return
     * @note :
     **/
    static std::string enCode(const char* Data, unsigned int in_len);

    /**
     * @brief: 解码，
     * @param Data
     * @return
     * @note :
     **/
    static std::string deCode(std::string const& encoded_string);
};

NAO_NAMESPACE_END
#endif