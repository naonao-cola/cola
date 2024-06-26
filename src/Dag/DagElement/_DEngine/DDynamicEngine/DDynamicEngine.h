/**
 * @FilePath     : /cola/src/Dag/DagElement/_DEngine/DDynamicEngine/DDynamicEngine.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-26 11:36:02
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-26 11:36:02
 **/
#ifndef NAO_DDYNAMICENGINE_H
#define NAO_DDYNAMICENGINE_H

#include <mutex>

#include "../DEngine.h"

NAO_NAMESPACE_BEGIN

class DDynamicEngine : public DEngine
{
protected:
    explicit DDynamicEngine() = default;

    NStatus setup(const DSortedDElementPtrSet& elements) override;

    NStatus run() override;

    NStatus afterRunCheck() override;

    /**
     * 记录当前 elements 数据信息
     * @param elements
     * @return
     */
    NVoid mark(const DSortedDElementPtrSet& elements);

    /**
     * 分析当前的信息，主要用于区分dag的类型
     * @return
     */
    NVoid analysisDagType(const DSortedDElementPtrSet& elements);

    /**
     * 动态图运行
     * @param
     * @return
     */
    NVoid asyncRunAndWait();

    /**
     * 动态图运行前重置
     * @param
     * @return
     */
    NVoid beforeRun();

    /**
     * element 运行element
     * @param element
     * @param affinity 是否本地执行
     * @return
     */
    NVoid process(DElementPtr element, NBool affinity);

    /**
     * element 运行完成处理
     * @param element
     * @return
     */
    NVoid afterElementRun(DElementPtr element);

    /**
     * 动态图运行等待
     * @param
     * @return
     */
    NVoid fatWait();

    /**
     * 并发的执行所有的element
     * @return
     */
    NVoid parallelRunAll();

    /**
     * 串行的执行所有element
     * @return
     */
    NVoid serialRunAll();

private:
    DElementPtrArr           total_element_arr_;                               // pipeline中所有的元素信息集合
    DElementPtrArr           front_element_arr_;                               // 没有依赖的元素信息
    NSize                    total_end_size_    = 0;                           // 图结束节点数量
    NSize                    finished_end_size_ = 0;                           // 执行结束节点数量
    NStatus                  cur_status_;                                      // 当前全局的状态信息
    internal::DEngineDagType dag_type_ = {internal::DEngineDagType::COMMON};   // 当前元素的排布形式

    std::mutex              lock_;
    std::condition_variable cv_;

    friend class UAllocator;
};

NAO_NAMESPACE_END

#endif   // NAO_DDYNAMICENGINE_H