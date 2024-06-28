/**
 * @FilePath     : /cola/src/Dag/DagElement/DElementRepository.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 23:08:46
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 09:41:39
 **/
#ifndef NAO_DELEMENTREPOSITORY_H
#define NAO_DELEMENTREPOSITORY_H

#include "DElement.h"
#include "DElementObject.h"


NAO_NAMESPACE_BEGIN

class DElementRepository : public DElementObject
{
private:
    /**
     * 插入一个element
     * @param ptr
     * @return
     */
    NVoid insert(DElementPtr ptr);

    /**
     * 查找对应的element
     * @param ptr
     * @return
     */
    NBool find(DElementPtr ptr) const;

    /**
     * 给所有的element，设定线程池
     * @param ptr
     * @return
     */
    DElementRepository* setThreadPool(UThreadPoolPtr ptr);

    /**
     * 准备执行流程
     * @return
     */
    NStatus setup();

    /**
     * 等待所有的element执行结束
     * @return
     * @notice 主要针对异步流程
     */
    NStatus reset();

    /**
     * 设置所有内部的element状态
     * @param state
     * @return
     */
    NStatus pushAllState(const DElementState& state);

    /**
     * 用于判断是否是出于退出状态
     * @return
     */
    NBool isCancelState() const;

    NStatus init() override;

    NStatus destroy() override;

    NStatus run() final;

    ~DElementRepository() override;

private:
    DElementPtrSet elements_{};                         // 用于记录所有的element信息
    DElementState  cur_state_{DElementState::CREATE};   // 当前状态信息
    DElementPtrSet async_elements_{};                   // 所有异步执行的逻辑，到后来一次性统一获取执行结果信息

    friend class DPipeline;
    friend class DPerf;
};

using DElementRepositoryPtr = DElementRepository*;
using DElementRepositoryRef = DElementRepository&;

NAO_NAMESPACE_END

#endif   // NAO_DELEMENTREPOSITORY_H
