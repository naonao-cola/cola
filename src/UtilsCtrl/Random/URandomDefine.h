/**
 * @FilePath     : /cola/src/UtilsCtrl/Random/URandomDefine.h
 * @Description  :
 * @Author       : naonao
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-20 19:57:54
 **/
#ifndef NAO_URANDOMDEFINE_H
#define NAO_URANDOMDEFINE_H

#include <random>

#include "../UtilsObject.h"

NAO_NAMESPACE_BEGIN

using NAO_RANDOM_MT19937       = std::mt19937;         // 梅森旋转法
using NAO_RANDOM_MINSTD_RAND   = std::minstd_rand;     // 线性同余法
using NAO_RANDOM_RANLUX24_BASE = std::ranlux24_base;   // 滞后Fibonacci

const static NInt NAO_REAL_RANDOM = 0;

NAO_NAMESPACE_END

#endif   // NAO_URANDOMDEFINE_H