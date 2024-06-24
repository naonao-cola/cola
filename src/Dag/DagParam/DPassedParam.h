/**
 * @FilePath     : /cola/src/Dag/DagParam/DPassedParam.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 11:55:27
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 13:59:02
 **/
#ifndef NAO_DPASSEDPARAM_H
#define NAO_DPASSEDPARAM_H

#include <unordered_map>

#include "DParamObject.h"

NAO_NAMESPACE_BEGIN

class DPassedParam : public DParamObject
{
public:
    /**
     * 所有的 GPassedParam 类型，内部需要实现一个 clone 方法
     * 将外部函数传到内部，如aspect和daemon
     * @param param
     */
    virtual NVoid clone(DPassedParam* param) = 0;
};


/**
 * 这种类型的参数，可以用于aspect和daemon中，方便参数传递
 * 不同的地方，用不同的名字，对应的内容是同一个
 */
using DAspectParam     = DPassedParam;
using DDaemonParam     = DPassedParam;
using DElementParam    = DPassedParam;
using DEventParam      = DPassedParam;
using DPassedParamPtr  = DPassedParam*;
using DAspectParamPtr  = DAspectParam*;
using DDaemonParamPtr  = DDaemonParam*;
using DElementParamPtr = DElementParam*;
using DEventParamPtr   = DEventParam*;

using DElementParamMap = std::unordered_map<std::string, DElementParamPtr>;

NAO_NAMESPACE_END

#endif   // NAO_DPASSEDPARAM_H
