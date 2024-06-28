/**
 * @FilePath     : /cola/src/Dag/DagElement/DAdapter/DFunction/DFunction.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-26 14:01:26
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 09:47:30
 **/
#ifndef NAO_DFUNCTION_H
#define NAO_DFUNCTION_H

#include "../DAdapter.h"

NAO_NAMESPACE_BEGIN

class DFunction : public DAdapter
{
public:
    /**
     * 设置执行函数具体内容
     * @param type
     * @param func
     * @return
     */
    DFunction* setFunction(const NFunctionType& type, NAO_NSTATUS_CONST_FUNCTION_REF func);

    // 针对GFunction，是需要写成public的，否则在外部的 lambda中，无法获取
    NAO_DECLARE_DPARAM_MANAGER_WRAPPER

    NAO_DECLARE_DEVENT_MANAGER_WRAPPER

private:
    explicit DFunction();

    NStatus init() final;

    NStatus run() final;

    NStatus destroy() final;

private:
    NAO_NSTATUS_FUNCTION init_function_    = nullptr;
    NAO_NSTATUS_FUNCTION run_function_     = nullptr;
    NAO_NSTATUS_FUNCTION destroy_function_ = nullptr;

    friend class GPipeline;
};

using DFunctionPtr  = DFunction*;
using DFunctionPPtr = DFunctionPtr*;

NAO_NAMESPACE_END

#endif   // NAO_DFUNCTION_H
