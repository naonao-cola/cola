﻿/**
 * @FilePath     : /cola/src/NBasic/NBasicDefine.h
 * @Description  :
 * @Author       : naonao
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-20 15:13:57
 * @Copyright    :
**/
#ifndef NAO_BASICDEFINE_H
#define NAO_BASICDEFINE_H

#include <cstddef>

#define NAO_NAMESPACE_BEGIN                                          \
namespace nao {                                                      \

#define NAO_NAMESPACE_END                                            \
} /* end of namespace nao */                                         \

NAO_NAMESPACE_BEGIN

#define NAO_INTERNAL_NAMESPACE_BEGIN                                 \
namespace internal {                                                 \

#define NAO_INTERNAL_NAMESPACE_END                                   \
} /* end of namespace internal */                                    \

NAO_INTERNAL_NAMESPACE_BEGIN

using NCHAR = char;
using NUINT = unsigned int;
using NVOID = void;
using NINT = int;
using NLONG = long;
using NULONG = unsigned long;
using NBOOL = bool;
using NBIGBOOL = int;
using NFLOAT = float;
using NDOUBLE = double;
using NCONSTR = const char*;
using NSIZE = size_t;

NAO_INTERNAL_NAMESPACE_END
NAO_NAMESPACE_END

#endif //NAO_BASICDEFINE_H
