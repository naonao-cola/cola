/**
 * @FilePath     : /cola/src/Basic/NObject.h
 * @Description  :
 * @Author       : naonao
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-20 11:54:02
 * @Copyright    :
**/
#ifndef NAO_OBJECT_H
#define NAO_OBJECT_H

#include "NBasicDefine.h"
#include "NValType.h"
#include "NFuncType.h"
NAO_NAMESPACE_BEGIN

class NObject {
public:
    /**
     * 默认构造函数
     */
    explicit NObject() = default;

    /**
     * 初始化函数
     */
    virtual NSTATUS init() {
        NAO_EMPTY_FUNCTION
    }

    /**
     * 流程处理函数
     */
    virtual NSTATUS run() = 0;

    /**
     * 释放函数
     */
    virtual NSTATUS destroy() {
        NAO_EMPTY_FUNCTION
    }

    /**
     * 默认析构函数
     */
    virtual ~NObject() = default;
};

NAO_NAMESPACE_END
#endif //NAO_OBJECT_H
