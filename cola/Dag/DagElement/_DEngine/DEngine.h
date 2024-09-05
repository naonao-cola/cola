/**
 * @FilePath     : /cola/cola/Dag/DagElement/_DEngine/DEngine.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-26 11:29:25
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-09-05 11:15:28
 **/
#ifndef NAO_DENGINE_H
#define NAO_DENGINE_H

#include "../DElementObject.h"
#include "../DElementSorter.h"
#include "DEngineDefine.h"


NAO_NAMESPACE_BEGIN

class DEngine : public DElementObject
{
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
    virtual NStatus afterRunCheck(){
        NAO_EMPTY_FUNCTION
    }

    /**
     * 分析所有的可以设置 linkable 的数据
     * @param elements
     * @return
     */
    NVoid link(const DSortedDElementPtrSet& elements)
    {
        /**
         * 认定图可以连通的判定条件：
         * 1，当前元素仅有一个后继
         * 2，当前元素的唯一后继，仅有一个前驱
         * 3，前后元素绑定机制是一样的
         */
        linked_size_ = 0;
        for (DElementPtr element : elements) {
            element->shape_ = internal::DElementShape::NORMAL;
            if (1 == element->run_before_.size()
                && 1 == (*element->run_before_.begin())->dependence_.size()
                && element->binding_index_ == (*(element->run_before_.begin()))->binding_index_) {
                element->shape_ = internal::DElementShape::LINKABLE;
                linked_size_++;
            }
        }
    }


protected:
    UThreadPoolPtr thread_pool_{nullptr};                            // 内部执行的线程池
    int            schedule_strategy_ = NAO_DEFAULT_TASK_STRATEGY;   // 调度策略
    NSize          linked_size_       = 0;                           // 标记有多少个element，是 linkable 的数据

    friend class DElementManager;
    friend class DPipeline;
    friend class DMutable;
};

using DEnginePtr = DEngine*;

NAO_NAMESPACE_END

#endif   // NAO_DENGINE_H
