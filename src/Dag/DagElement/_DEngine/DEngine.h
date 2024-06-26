/**
 * @FilePath     : /cola/src/Dag/DagElement/_DEngine/DEngine.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-26 11:29:25
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-26 11:29:26
**/
#ifndef NAO_DENGINE_H
#define NAO_DENGINE_H

#include "DEngineDefine.h"
#include "../DElementObject.h"
#include "../DElementSorter.h"

NAO_NAMESPACE_BEGIN

class DEngine : public DElementObject {
protected:
    explicit DEngine() = default;

    /**
     * 将所有注册进入 pipeline的内容，进行分析和解构
     * @param elements
     * @return
     */
    virtual NStatus setup(const DSortedDElementPtrSet& elements) = 0;

    /**
     * 执行完毕后，确认运行是否正常
     * @return
     */
    virtual NStatus afterRunCheck() = 0;

    /**
     * 计算出来最终计算的index值
     * @param element
     * @return
     */
    inline NIndex calcIndex(DElementPtr element) const {
        /**
         * 如果没有设定绑定线程的话，就用默认调度策略
         * 否则的话，会走绑定的thread。
         * 如果设定的 binding_index_ >= thread 总数，会在 threadpool 层做统一判定
         */
        auto bindingIndex = element->getBindingIndex();
        return NAO_DEFAULT_BINDING_INDEX == bindingIndex
               ? schedule_strategy_ : bindingIndex;
    }

    /**
     * 分析所有的可以设置 linkable 的数据
     * @param elements
     * @return
     */
    NVoid link(const DSortedDElementPtrSet& elements) {
        /**
         * 认定图可以连通的判定条件：
         * 1，当前元素仅有一个依赖
         * 2，当前元素依赖的节点，只有一个后继
         * 3，当前元素的依赖的后继，仍是当前节点
         * 4，前后元素绑定机制是一样的
         */
        linked_size_ = 0;
        for (DElementPtr element : elements) {
            element->linkable_ = false;    // 防止出现之前的留存逻辑。确保只有当前链接关系下，需要设置 linkable的，才会设置为 true
            if (1 == element->dependence_.size()
                && 1 == (*element->dependence_.begin())->run_before_.size()
                && (*(element->dependence_.begin()))->run_before_.find(element) != (*(element->dependence_.begin()))->run_before_.end()
                && element->getBindingIndex() == (*(element->dependence_.begin()))->getBindingIndex()) {
                element->linkable_ = true;
                linked_size_++;
            }
        }
    }


protected:
    UThreadPoolPtr thread_pool_ { nullptr };                    // 内部执行的线程池
    int schedule_strategy_ = NAO_DEFAULT_TASK_STRATEGY;      // 调度策略
    NSize linked_size_ = 0;                                     // 标记有多少个element，是 linkable 的数据

    friend class GElementManager;
    friend class GPipeline;
    friend class GMutable;
};

using DEnginePtr = DEngine *;

NAO_NAMESPACE_END

#endif //NAO_DENGINE_H
