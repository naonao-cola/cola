
/**
 * @FilePath     : /cola/src/Dag/DagElement/DGroup/DCondition/DCondition.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-26 15:25:54
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 09:48:16
 **/
#include "DCondition.h"

NAO_NAMESPACE_BEGIN

DCondition::DCondition()
{
    element_type_ = DElementType::CONDITION;
    session_      = URandom<>::generateSession(NAO_STR_CONDITION);
}


NStatus DCondition::run()
{
    NAO_FUNCTION_BEGIN

    NIndex index = this->choose();
    if (internal::NAO_CONDITION_LAST_INDEX == index && !this->group_elements_arr_.empty()) {
        // 如果返回-1，则直接执行最后一个条件（模仿default功能）
        auto element = group_elements_arr_.back();
        status       = element->fatProcessor(NFunctionType::RUN);
    }
    else if (0 <= index && index < (NIndex)group_elements_arr_.size()) {
        // 如果返回的内容，在元素范围之内，则直接执行元素的内容。不在的话，则不执行任何操作，直接返回正确状态
        auto element = group_elements_arr_[index];
        status       = element->fatProcessor(NFunctionType::RUN);
    }

    NAO_FUNCTION_END
}


NSize DCondition::getRange() const
{
    return group_elements_arr_.size();
}


NVoid DCondition::dump(std::ostream& oss)
{
    dumpElement(oss);
    dumpGroupLabelBegin(oss);
    oss << 'p' << this << "[shape=diamond];\n";
    oss << "color=blue;\n";

    for (auto i = 0; i < group_elements_arr_.size(); ++i) {
        const auto& cur = group_elements_arr_[i];
        cur->dump(oss);

        const std::string& label = "[label=\"" + std::to_string(i) + "\"]";
        dumpEdge(oss, this, cur, label);
    }

    dumpGroupLabelEnd(oss);

    for (const auto& element : run_before_) {
        dumpEdge(oss, this, element);
    }
}


NBool DCondition::isSeparate(DElementCPtr a, DElementCPtr b) const
{
    return true;
}

NAO_NAMESPACE_END
