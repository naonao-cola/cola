/**
 * @FilePath     : /cola/src/NBasic/NValType.h
 * @Description  :
 * @Author       : naonao
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-22 11:51:06
 * @Copyright    :
 **/
#ifndef NAO_VALTYPE_H
#define NAO_VALTYPE_H

#include "NBasicDefine.h"
#include "NException.h"
#include "NStatus.h"


using NChar      = nao::internal::NCHAR;
using NCharPtr   = nao::internal::NCHAR*;
using NUint      = nao::internal::NUINT;
using NSize      = nao::internal::NSIZE;
using NVoid      = nao::internal::NVOID;
using NVoidPtr   = nao::internal::NVOID*;
using NInt       = nao::internal::NINT;
using NLong      = nao::internal::NLONG;
using NULong     = nao::internal::NULONG;
using NBool      = nao::internal::NBOOL;
using NIndex     = nao::internal::NINT;   // 表示标识信息，可以为负数
using NFloat     = nao::internal::NFLOAT;
using NDouble    = nao::internal::NDOUBLE;
using NConStr    = nao::internal::NCONSTR;   // 表示 const char*
using NBigBool   = nao::internal::NBIGBOOL;
using NLevel     = nao::internal::NINT;
using NSec       = nao::internal::NLONG;     // 表示秒信息, for second
using NMSec      = nao::internal::NLONG;     // 表示毫秒信息, for millisecond
using NFMSec     = nao::internal::NDOUBLE;   // 表示毫秒信息，包含小数点信息
using NStatus    = nao::internal::NSTATUS;
using NException = nao::internal::NEXCEPTION;

#endif   // NAO_VALTYPE_H
