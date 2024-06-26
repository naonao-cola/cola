/**
 * @FilePath     : /cola/src/Dag/DagElement/DAdapter/DFunction/DFunction.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-26 14:01:45
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-26 14:06:22
**/
#include "DFunction.h"

NAO_NAMESPACE_BEGIN

DFunctionPtr DFunction::setFunction(const NFunctionType& type,
                                    NAO_NSTATUS_CONST_FUNCTION_REF func) {
    NAO_ASSERT_INIT_THROW_ERROR(false)
    NAO_ASSERT_NOT_NULL_THROW_ERROR(func)

    switch (type) {
        case NFunctionType::INIT: init_function_ = func; break;
        case NFunctionType::RUN: run_function_ = func; break;
        case NFunctionType::DESTROY: destroy_function_ = func; break;
        default: return nullptr;     // 不可能出现的情况
    }

    return this;
}


DFunction::DFunction() {
    this->element_type_ = DElementType::FUNCTION;
    session_ = URandom<>::generateSession(NAO_STR_FUNCTION);
}


NStatus DFunction::init() {
    return init_function_ ? init_function_() : NStatus();
}


NStatus DFunction::run() {
    return run_function_ ? run_function_() : NStatus();
}


NStatus DFunction::destroy() {
    return destroy_function_ ? destroy_function_() : NStatus();
}

NAO_NAMESPACE_END