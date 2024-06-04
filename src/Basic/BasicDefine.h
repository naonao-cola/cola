/**
 * @FilePath     : /cola/src/Basic/BasicDefine.h
 * @Description  :
 * @Author       : naonao 1319144981@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : naonao 1319144981@qq.com
 * @LastEditTime : 2024-06-04 15:48:37
 * @Copyright    : G AUTOMOBILE RESEARCH INSTITUTE CO.,LTD Copyright (c) 2024.
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
