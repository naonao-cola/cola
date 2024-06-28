
/**
 * @FilePath     : /cola/src/Dag/DagElement/DGroup/DCluster/DCluster.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-26 15:25:46
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 09:47:44
 **/
#include "DCluster.h"

NAO_NAMESPACE_BEGIN

DCluster::DCluster()
{
    element_type_ = DElementType::CLUSTER;
    session_      = URandom<>::generateSession(NAO_STR_CLUSTER);
}


NStatus DCluster::run()
{
    NAO_FUNCTION_BEGIN
    for (DElementPtr element : this->group_elements_arr_) {
        status = element->fatProcessor(NFunctionType::RUN);
        NAO_FUNCTION_CHECK_STATUS
    }

    NAO_FUNCTION_END
}


NVoid DCluster::dump(std::ostream& oss)
{
    dumpElement(oss);
    dumpGroupLabelBegin(oss);
    oss << 'p' << this << "[shape=point height=0];\n";
    oss << "color=blue;\n";

    DElementPtr pre = nullptr;
    for (auto i = 0; i < group_elements_arr_.size(); i++) {
        const auto& element = group_elements_arr_[i];
        element->dump(oss);

        if (0 != i) {
            // 如果不是开头的位置，则划线关联
            dumpEdge(oss, pre, element);
        }
        pre = element;
    }

    dumpGroupLabelEnd(oss);

    for (const auto& node : run_before_) {
        dumpEdge(oss, this, node);
    }
}


NBool DCluster::isSeparate(DElementCPtr a, DElementCPtr b) const
{
    return true;
}

NAO_NAMESPACE_END