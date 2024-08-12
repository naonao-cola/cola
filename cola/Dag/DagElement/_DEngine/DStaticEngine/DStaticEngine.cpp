/**
 * @FilePath     : /cola/cola/Dag/DagElement/_DEngine/DStaticEngine/DStaticEngine.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-08-12 13:47:40
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-08-12 13:52:31
 * @Copyright (c) 2024 by G, All Rights Reserved.
**/
#include "DStaticEngine.h"

NAO_NAMESPACE_BEGIN

NStatus DStaticEngine::setup(const DSortedDElementPtrSet& elements) {
    NAO_FUNCTION_BEGIN
    element_mat_.clear();

    DElementPtrArr curArr;
    NSize totalSize = 0;
    for (auto element : elements) {
        NAO_ASSERT_NOT_NULL(element)
        element->beforeRun();
        if (element->dependence_.empty()) {
            curArr.push_back(element);
            totalSize++;
        }
    }

    while (!curArr.empty()) {
        element_mat_.emplace_back(curArr);
        DElementPtrArr runnableArr = curArr;
        curArr.clear();
        for (auto element : runnableArr) {
            for (auto cur : element->run_before_) {
                if (0 == (--cur->left_depend_)) {
                    curArr.push_back(cur);
                    totalSize++;
                }
            }
        }
    }

    NAO_RETURN_ERROR_STATUS_BY_CONDITION(totalSize != elements.size(),
                                            "static engine parse error");
    NAO_FUNCTION_END
}


NStatus DStaticEngine::run() {
    NAO_FUNCTION_BEGIN

    for (const auto& arr : element_mat_) {
        std::vector<std::future<NStatus>> futures;
        DElementPtrArr macros;
        for (auto* element : arr) {
            if (element->isMacro()
                && NAO_DEFAULT_BINDING_INDEX == element->getBindingIndex()) {
                // 未绑定线程的微任务，直接放到 macros 中，减少线程切换
                macros.emplace_back(element);
            } else {
                auto fut = thread_pool_->commit([element] {
                    return element->fatProcessor(NFunctionType::RUN);
                }, element->getBindingIndex());
                futures.emplace_back(std::move(fut));
            }
        }

        for (DElementPtr macro : macros) {
            status += macro->fatProcessor(NFunctionType::RUN);
        }

        for (auto& future : futures) {
            status += future.get();
        }
        NAO_FUNCTION_CHECK_STATUS
    }

    NAO_FUNCTION_END
}

NAO_NAMESPACE_END
