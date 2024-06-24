/**
 * @FilePath     : /cola/src/Dag/DagParam/DMessageParam.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 13:48:53
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 13:48:53
**/
#ifndef NAO_DMESSAGEPARAM_H
#define NAO_DMESSAGEPARAM_H

#include "DParamObject.h"

NAO_NAMESPACE_BEGIN

class DMessageParam : public DParamObject {
    /**
     * 用于消息传递的参数时，建议实现构造拷贝 和 赋值拷贝函数
     * 否则，针对指针类型变量，可能会出现深浅拷贝的问题
     * @param param
     * @return
     */
};

NAO_NAMESPACE_END

#endif //NAO_DMESSAGEPARAM_H