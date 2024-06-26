/**
 * @FilePath     : /cola/src/Dag/DagElement/DGroup/DGroup.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-26 15:11:36
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-26 15:11:36
**/
#include "DGroup.h"

NAO_NAMESPACE_BEGIN

DGroup::DGroup() {
    element_type_ = DElementType::GROUP;
}

NStatus DGroup::init() {
    NAO_FUNCTION_BEGIN

    for (DElementPtr element : group_elements_arr_) {
        NAO_ASSERT_NOT_NULL(element)
        status += element->fatProcessor(NFunctionType::INIT);
    }
    NAO_FUNCTION_CHECK_STATUS

    is_init_ = true;
    NAO_FUNCTION_END
}


NStatus DGroup::destroy() {
    NAO_FUNCTION_BEGIN

    for (DElementPtr element : group_elements_arr_) {
        NAO_ASSERT_NOT_NULL(element)
        status += element->fatProcessor(NFunctionType::DESTROY);
    }
    NAO_FUNCTION_CHECK_STATUS

    is_init_ = false;
    NAO_FUNCTION_END
}


NStatus DGroup::addElement(DElementPtr element) {
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_NOT_NULL(element)

    this->group_elements_arr_.emplace_back(element);
    NAO_FUNCTION_END
}


NVoid DGroup::dumpGroupLabelBegin(std::ostream& oss) {
    oss << "subgraph ";
    oss << "cluster_p" << this;    // cluster_ 是 graphviz的关键字，和CGraph中GCluster逻辑无关
    oss << " {\nlabel=\"";

    if (name_.empty()) {
        oss << 'p' << this;
    } else {
        oss << name_;
    }

    if (this->loop_ > 1) {
        oss << "(loop=" << this->loop_ << ")";
    }

    dumpPerfInfo(oss);
    oss << "\";\n";
}


NVoid DGroup::dumpGroupLabelEnd(std::ostream& oss) {
    oss << "}\n";
}


NBool DGroup::isSerializable() const {
    /**
     * 针对group的情况，应该是所有在其中的element 都是可以串行的，才认定为可串行
     * 但是在 region和 multiCondition中，有针对性的判断
     */
    return std::all_of(group_elements_arr_.begin(), group_elements_arr_.end(),
                       [](DElementPtr element) {
        return element->isSerializable();
    });
}


NStatus DGroup::addManagers(DParamManagerPtr paramManager,
                            DEventManagerPtr eventManager) {
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_NOT_NULL(paramManager, eventManager)
    NAO_ASSERT_INIT(false)

    this->setDParamManager(paramManager);
    this->setDEventManager(eventManager);
    for (DElementPtr element : group_elements_arr_) {
        NAO_ASSERT_NOT_NULL(element)
        status += element->addManagers(paramManager, eventManager);
    }

    NAO_FUNCTION_END
}


NBool DGroup::isSeparate(DElementCPtr a, DElementCPtr b) const {
    return false;
}

NAO_NAMESPACE_END