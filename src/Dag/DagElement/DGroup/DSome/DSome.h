/**
 * @FilePath     : /cola/src/Dag/DagElement/DGroup/DSome/DSome.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-26 15:26:20
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 09:49:17
**/

#ifndef NAO_DSOME_H
#define NAO_DSOME_H

#include <condition_variable>
#include <memory>
#include <mutex>


#include "../DGroup.h"

        NAO_NAMESPACE_BEGIN

        template<NInt TriggerNum = 1>
        class DSome : public DGroup
{
protected:
    explicit DSome();

    NStatus addElement(DElementPtr element) final;

    NStatus run() final;

    NBool isSerializable() const final;

    NVoid dump(std::ostream& oss) final;

    NBool isHold() final;

    NStatus checkSuitable() final;

    NAO_NO_ALLOWED_COPY(DSome)

private:
    NInt    left_num_ = 0;   // 还剩的触发结束的个数
    NStatus cur_status_;     // 记录异步时刻的当前状态信息

    std::mutex              lock_;
    std::condition_variable cv_;

    friend class DPipeline;
    friend class DDynamicEngine;
    friend class UAllocator;
};

NAO_NAMESPACE_END

#include "DSome.inl"

#endif   // NAO_DSOME_H
