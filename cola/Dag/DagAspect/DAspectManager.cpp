/**
 * @FilePath     : /cola/src/Dag/DagAspect/DAspectManager.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 20:02:52
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 22:34:29
 **/
#include "DAspectManager.h"

NAO_NAMESPACE_BEGIN

DAspectManager::DAspectManager()
{
    aspect_arr_ = {};
}


DAspectManager::~DAspectManager()
{
    clear();
}


NStatus DAspectManager::reflect(const internal::DAspectType& type, const NStatus& curStatus)
{
    NAO_FUNCTION_BEGIN

    for (DAspectPtr aspect : aspect_arr_) {
        switch (type) {
        /**
         * 仅针对Begin对应的内容，进行返回值判断
         * run()方法切面更容易被执行，故放在最前方判断
         */
        case internal::DAspectType::BEGIN_RUN:
            status = aspect->beginRun();
            break;
        case internal::DAspectType::FINISH_RUN:
            aspect->finishRun(curStatus);
            break;
        case internal::DAspectType::BEGIN_INIT:
            status = aspect->beginInit();
            break;
        case internal::DAspectType::FINISH_INIT:
            aspect->finishInit(curStatus);
            break;
        case internal::DAspectType::BEGIN_DESTROY:
            status = aspect->beginDestroy();
            break;
        case internal::DAspectType::FINISH_DESTROY:
            aspect->finishDestroy(curStatus);
            break;
        case internal::DAspectType::ENTER_CRASHED:
            aspect->enterCrashed();
            break;
        default:
            status = NErrStatus("unknown aspect type");
        }

        NAO_FUNCTION_CHECK_STATUS
    }
    NAO_FUNCTION_END
}


NStatus DAspectManager::add(DAspectPtr aspect)
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_NOT_NULL(aspect)
    aspect_arr_.emplace_back(aspect);
    NAO_FUNCTION_END
}


NSize DAspectManager::getSize() const
{
    auto size = aspect_arr_.size();
    return size;
}


NStatus DAspectManager::clear()
{
    NAO_FUNCTION_BEGIN
    for (DAspectPtr aspect : aspect_arr_) {
        NAO_DELETE_PTR(aspect)
    }

    aspect_arr_.clear();
    NAO_FUNCTION_END
}


NStatus DAspectManager::popLast()
{
    NAO_FUNCTION_BEGIN
    NAO_RETURN_ERROR_STATUS_BY_CONDITION(0 == getSize(), "no aspect to pop")

    auto* last = aspect_arr_.back();
    NAO_DELETE_PTR(last);
    aspect_arr_.pop_back();   // 弹出最后一个
    NAO_FUNCTION_END
}


NVoidPtr DAspectManager::setDParamManager(DParamManagerPtr pm)
{
    for (auto* cur : aspect_arr_) {
        cur->setDParamManager(pm);
    }
    return this;
}


NVoidPtr DAspectManager::setDEventManager(DEventManagerPtr em)
{
    for (auto* cur : aspect_arr_) {
        cur->setDEventManager(em);
    }
    return this;
}

NAO_NAMESPACE_END
