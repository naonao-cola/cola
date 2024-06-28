/**
 * @FilePath     : /cola/src/Dag/DagElement/_DEngine/DTopoEngine/DTopoEngine.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-26 11:46:08
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-26 11:48:41
 **/
#include <queue>

#include "DTopoEngine.h"

NAO_NAMESPACE_BEGIN

NStatus DTopoEngine::setup(const DSortedDElementPtrSet& elements)
{
    NAO_FUNCTION_BEGIN

    topo_elements_.clear();
    std::queue<DElementPtr> readyQueue;
    for (auto* element : elements) {
        element->left_depend_ = element->dependence_.size();
        if (0 == element->left_depend_) {
            readyQueue.push(element);
        }
    }

    while (!readyQueue.empty()) {
        auto* cur = readyQueue.front();
        readyQueue.pop();
        topo_elements_.push_back(cur);

        for (auto* element : cur->run_before_) {
            if (0 == --element->left_depend_) {
                readyQueue.push(element);
            }
        }
    }

    NAO_RETURN_ERROR_STATUS_BY_CONDITION(topo_elements_.size() != elements.size(), "topo engine parse size is not right")

    NAO_FUNCTION_END
}


NStatus DTopoEngine::run()
{
    NAO_FUNCTION_BEGIN
    for (auto* element : topo_elements_) {
        status += element->fatProcessor(NFunctionType::RUN);
        NAO_FUNCTION_CHECK_STATUS
    }
    NAO_FUNCTION_END
}


NStatus DTopoEngine::afterRunCheck(){/**
                                      * topology 纯靠顺序执行。如果有异常，会直接爆出
                                      * 故，不在这里做判定了
                                      */
                                     NAO_EMPTY_FUNCTION}

NAO_NAMESPACE_END
