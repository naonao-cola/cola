/**
 * @FilePath     : /cola/src/Dag/DagParam/DPassedDefaultParam.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 11:56:06
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 11:56:06
 **/
#ifndef NAO_DPASSEDDEFAULTPARAM_H
#define NAO_DPASSEDDEFAULTPARAM_H

#include "DPassedParam.h"

NAO_NAMESPACE_BEGIN

class DPassedDefaultParam final : public DPassedParam
{
protected:
    /**
     * 针对原生类型的实现
     * 目的是防止无法生成默认类型的 GAspectParam 参数
     * 无任何实意，不支持继承
     */
    NVoid clone(DPassedParamPtr param) final {}
};

using DAspectDefaultParam = DPassedDefaultParam;
using DDaemonDefaultParam = DPassedDefaultParam;
using DEventDefaultParam  = DPassedDefaultParam;

NAO_NAMESPACE_END

#endif   // NAO_DPASSEDDEFAULTPARAM_H