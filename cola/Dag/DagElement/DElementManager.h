/**
 * @FilePath     : /cola/src/Dag/DagElement/DElementManager.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 23:07:22
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 09:41:16
 **/
#ifndef NAO_DELEMENTMANAGER_H
#define NAO_DELEMENTMANAGER_H

#include "DElement.h"
#include "DElementSorter.h"
#include "_DEngine/DEngineInclude.h"

NAO_NAMESPACE_BEGIN

class DElementManager : public DElementObject, public DagManager<DElement>
{
protected:
    explicit DElementManager() = default;
    ~DElementManager() override;
    NAO_NO_ALLOWED_COPY(DElementManager);

    NStatus init() final;
    NStatus run() final;
    NStatus destroy() final;

    /**
     * 添加一个元素信息
     * @param element
     * @return
     */
    NStatus add(DElementPtr element) final;

    /**
     * 删除传入元素
     * @param element
     */
    NStatus remove(DElementPtr element) final;

    /**
     * 查找传入元素
     * @param element
     * @return
     */
    NBool find(DElementPtr element) const final;

    /**
     * 清空信息
     * @return
     */
    NStatus clear() final;

    /**
     * 设置图引擎运行模式
     * @param engineType
     * @return
     */
    DElementManager* setEngineType(DEngineType engineType);

    /**
     * 构造执行引擎
     * @param strategy
     * @return
     */
    NStatus initEngine();

    /**
     * 设置线程池
     * @param ptr
     * @return
     */
    DElementManager* setThreadPool(UThreadPoolPtr ptr);

    /**
     * 获取最大的并发数
     * @return
     */
    NSize calcMaxParaSize();

    /**
     * 查看是否可以串行执行
     * @return
     */
    NBool checkSerializable();

    /**
     * 剪裁多余的连边信息
     * @return
     */
    NSize trim();

    /**
     * 加入数据，并且执行
     * @param elements
     * @return
     */
    NStatus process(const DSortedDElementPtrSet& elements);

private:
    DSortedDElementPtrSet manager_elements_;                    // 保存节点信息的内容
    DEnginePtr            engine_{nullptr};                     // 执行引擎
    DEngineType           engine_type_{DEngineType::DYNAMIC};   // 引擎执行方式
    UThreadPoolPtr        thread_pool_{nullptr};                // 线程池
    NBool                 auto_check_enable_ = true;            // 是否自动实现后校验逻辑

    friend class DPipeline;
    friend class DRegion;
    friend class DMutable;
    friend class DPerf;
    friend class UAllocator;
};

using DElementManagerPtr = DElementManager*;

NAO_NAMESPACE_END

#endif   // NAO_DELEMENTMANAGER_H
