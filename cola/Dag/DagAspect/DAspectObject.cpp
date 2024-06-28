/**
 * @FilePath     : /cola/src/Dag/DagAspect/DAspectObject.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 20:30:49
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 09:39:06
 **/
#include "DAspectObject.h"
#include "../DagElement/DElementInclude.h"

NAO_NAMESPACE_BEGIN

DAspectObject::DAspectObject()
{
    session_ = URandom<>::generateSession(NAO_STR_ASPECT);
}


const std::string& DAspectObject::getName() const
{
    NAO_ASSERT_NOT_NULL_THROW_ERROR(belong_);
    return belong_->getName();
}


DAspectObject::~DAspectObject(){NAO_DELETE_PTR(param_)}


DAspectObjectPtr DAspectObject::setBelong(DElementPtr belong)
{
    NAO_ASSERT_NOT_NULL_THROW_ERROR(belong)
    NAO_THROW_EXCEPTION_BY_CONDITION(!belong->isRegistered(), "[" + belong->getName() + "] can not add aspect for the reason of no register");
    belong_ = belong;
    this->setDParamManager(belong->param_manager_);
    this->setDEventManager(belong->event_manager_);
    return this;
}

NAO_NAMESPACE_END