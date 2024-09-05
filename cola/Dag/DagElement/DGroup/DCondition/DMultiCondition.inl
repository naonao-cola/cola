/*
 * @FilePath     : /cola/src/Dag/DagElement/DGroup/DCondition/DMultiCondition.inl
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-26 15:25:54
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 09:49:07
 */
#ifndef NAO_DMULTICONDITION_INL
#define NAO_DMULTICONDITION_INL

#include "DMultiCondition.h"
NAO_NAMESPACE_BEGIN

template<DMultiConditionType type>
DMultiCondition<type>::DMultiCondition()
{
    element_type_ = DElementType::MULTI_CONDITION;
    session_      = URandom<>::generateSession(NAO_STR_MULTI_CONDITION);
}


template<DMultiConditionType type>
NStatus DMultiCondition<type>::run()
{
    NAO_FUNCTION_BEGIN

    switch (type) {
    case DMultiConditionType::SERIAL: status = serialRun(); break;
    case DMultiConditionType::PARALLEL: status = parallelRun(); break;
    default: NAO_RETURN_ERROR_STATUS("unknown multi condition type")
    }

    NAO_FUNCTION_END
}


template<DMultiConditionType type>
NStatus DMultiCondition<type>::serialRun()
{
    NAO_FUNCTION_BEGIN

    for (auto cur : this->group_elements_arr_) {
        if (cur->isMatch()) {
            // 仅依次执行 match 的逻辑
            status = cur->fatProcessor(NFunctionType::RUN);
            NAO_FUNCTION_CHECK_STATUS
        }
    }

    NAO_FUNCTION_END
}


template<DMultiConditionType type>
NStatus DMultiCondition<type>::parallelRun()
{
    NAO_FUNCTION_BEGIN
    std::vector<std::future<NStatus>> futures;
    for (DElementPtr cur : this->group_elements_arr_) {
        if (!cur->isMatch()) {
            continue;    // 不满足条件，则不执行
        }

        futures.emplace_back(std::move(this->thread_pool_->commit([cur] {
            return cur->fatProcessor(NFunctionType::RUN);
        }, cur->binding_index_)));
    }

    for (auto& fut : futures) {
        /**
         * 如果有异常值的化，也等待所有内容计算完成后，统一返回
         * 暂时没有处理超时的情况。预计今后会统一处理
         */
        status += fut.get();
    }

    NAO_FUNCTION_END
}


template<DMultiConditionType type>
NIndex DMultiCondition<type>::choose()
{
    // GMultiCondition 是不可能执行到 choose()方法的。这里直接抛出异常好了
    NAO_THROW_EXCEPTION("GMultiCondition no need choose function")
    return 0;
}


template<DMultiConditionType type>
NBool DMultiCondition<type>::isSerializable() const
{
    if (DMultiConditionType::PARALLEL == type && group_elements_arr_.size() > 1) {
        /**
         * 如果是PARALLEL模式的话，并且其中的元素个数大于1，则一定不可以串行执行
         * PARALLEL模式中，仅有一个元素的情况，和 SERIAL模式的判断方式一样，
         * 均为判断其中所有的element是否可以并行
         * 故放在下面的条件中判断了
         */
        return false;
    }

    return DGroup::isSerializable();
}


template<DMultiConditionType type>
NBool DMultiCondition<type>::isSeparate(DElementCPtr a, DElementCPtr b) const
{
    NBool result = false;
    switch (type) {
    case DMultiConditionType::SERIAL: result = true; break;
    case DMultiConditionType::PARALLEL: result = false; break;
    default: NAO_THROW_EXCEPTION("unknown multi condition type in isSeparateWith function")
    }
    return result;
}

NAO_NAMESPACE_END

#endif   // NAO_DMULTICONDITION_INL
