/**
 * @FilePath     : /cola/src/Dag/DagPipeline/_DSchedule/DSchedule.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-28 10:22:18
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 10:22:18
 **/
#include "DSchedule.h"

NAO_NAMESPACE_BEGIN

NStatus DSchedule::init()
{
    NAO_FUNCTION_BEGIN

    if (internal::DScheduleType::UNIQUE == type_) {
        /**
         * 如果是 unique 的数据，则在这里初始化一下
         * 如果是 shared 的数据，则不做任何操作
         */
        NAO_DELETE_PTR(unique_tp_)
        unique_tp_ = UAllocator::safeMallocTemplateNObject<UThreadPool>(true, config_);
    }
    else if (internal::DScheduleType::SHARED == type_) {
        NAO_ASSERT_NOT_NULL(shared_tp_)
        // 首先，要确定外部传入的线程池，已经初始化过了
        if (!shared_tp_->isInit()) {
            NAO_RETURN_ERROR_STATUS("shared thread pool is not init.")
        }
    }

    NAO_FUNCTION_END
}


NStatus DSchedule::destroy()
{
    NAO_FUNCTION_BEGIN

    if (internal::DScheduleType::UNIQUE == type_) {
        // unique 模式下，init的时候创建线程，就在destroy的时候，给析构掉了
        NAO_DELETE_PTR(this->unique_tp_)
    }

    NAO_FUNCTION_END
}


NStatus DSchedule::run(){NAO_NO_SUPPORT}


NStatus DSchedule::makeType(UThreadPoolPtr tp)
{
    NAO_FUNCTION_BEGIN
    NAO_DELETE_PTR(this->unique_tp_)
    shared_tp_ = tp;   // 用外部传入的线程池，来代替内部原有的线程池

    // 如果是nullptr的话，就当做是恢复使用默认本地线程池
    type_ = (nullptr == tp) ? internal::DScheduleType::UNIQUE : internal::DScheduleType::SHARED;
    NAO_FUNCTION_END
}


UThreadPoolPtr DSchedule::getThreadPool()
{
    /**
     * 返回一个可用的 pool
     * utp和stp，应该是有且仅有一个为非空的
     */
    return unique_tp_ == nullptr ? shared_tp_ : unique_tp_;
}


DSchedule::~DSchedule()
{
    destroy();
}

NAO_NAMESPACE_END
