/**
 * @FilePath     : /cola/src/UtilsCtrl/Hash/UHash.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-22 12:14:15
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-22 12:21:01
 **/
#ifndef NAO_DHASH_H
#define NAO_DHASH_H

#include <cstddef>
#include <cstdint>

#include "../UtilsObject.h"

NAO_NAMESPACE_BEGIN

class UHash : public UtilsObject
{
public:
    static uint32_t Hash(const char* data, size_t n, uint32_t seed);
};

NAO_NAMESPACE_END
#endif