/**
 * @FilePath     : /cola/src/Basic/NStruct.h
 * @Description  :
 * @Author       : naonao
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-20 11:56:29
 * @Copyright    :
 **/
#ifndef NAO_STRUCT_H
#define NAO_STRUCT_H

#include "NBasicDefine.h"
NAO_NAMESPACE_BEGIN

/**
 * 所有框架内部结构体定义的基类
 * 仅针对类似 pod 数据类型的定义
 */
class NStruct
{
public:
    ~NStruct() = default;
};

NAO_NAMESPACE_END
#endif   // CGRAPH_CSTRUCT_H
