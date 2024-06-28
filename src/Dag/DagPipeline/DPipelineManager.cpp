/**
 * @FilePath     : /cola/src/Dag/DagPipeline/DPipelineManager.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-28 10:38:17
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 10:38:18
**/
#include <algorithm>

#include "DPipelineManager.h"

NAO_NAMESPACE_BEGIN

NStatus DPipelineManager::init() {
    NAO_FUNCTION_BEGIN
    if (!used_list_.empty()) {
        // 初始化的时候，不应该有 used 的内容
        NAO_RETURN_ERROR_STATUS("used list status error")
    }

    for (auto pipeline : free_list_) {
        status += pipeline->init();
    }
    NAO_FUNCTION_CHECK_STATUS
    is_init_ = true;
    NAO_FUNCTION_END
}


NStatus DPipelineManager::run() {
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_INIT(true)
    DPipelinePtr pipeline = fetch();
    if (nullptr == pipeline) {
        NAO_RETURN_ERROR_STATUS("no free pipeline current")
    }

    /**
     * 这里执行，是外部线程执行的。获取的status就是这个pipeline run的结果
     * 无论如何，执行完成之后，都要将pipeline返回到 free_list 中去
     */
    status = pipeline->run();
    status += release(pipeline);

    NAO_FUNCTION_END
}


NStatus DPipelineManager::destroy() {
    NAO_FUNCTION_BEGIN
    if (!used_list_.empty()) {
        NAO_RETURN_ERROR_STATUS("used list status error")
    }

    for (auto pipeline : free_list_) {
        status += pipeline->destroy();
    }
    NAO_FUNCTION_CHECK_STATUS

    is_init_ = false;
    NAO_FUNCTION_END
}


NStatus DPipelineManager::add(DPipelinePtr ptr) {
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_NOT_NULL(ptr)
    NAO_ASSERT_INIT(false)

    free_list_.push_back(ptr);
    NAO_FUNCTION_END
}


NStatus DPipelineManager::clear() {
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_INIT(false)

    /**
     * 这里只是清空list中的信息，并不真正销毁 pipeline对象
     * 因为pipeline 对象统一在 Factory中管控
     */
    used_list_.clear();
    free_list_.clear();
    NAO_FUNCTION_END
}


NBool DPipelineManager::find(DPipelinePtr ptr) const {
    /**
     * 如果在其中一个位置，找到了，就返回 true了
     * 非线程安全，建议 init 之前使用
     */
    NBool result = std::any_of(used_list_.begin(), used_list_.end(),
                         [ptr] (DPipelinePtr cur) {
       return (ptr == cur);
    });

    result |= std::any_of(free_list_.begin(), free_list_.end(),
                          [ptr] (DPipelinePtr cur) {
       return (ptr == cur);
    });

    return result;
}


NStatus DPipelineManager::remove(DPipelinePtr ptr) {
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_NOT_NULL(ptr)
    NAO_ASSERT_INIT(false)
    free_list_.remove(ptr);    // 在非初始化的情况下，仅可能在 free_list 中包含

    NAO_FUNCTION_END
}


NSize DPipelineManager::getSize() const {
    NSize size = free_list_.size() + used_list_.size();
    return size;
}


DPipelineManager::~DPipelineManager() {
    clear();
}


DPipelinePtr DPipelineManager::fetch() {
    DPipelinePtr pipeline = nullptr;
    NAO_LOCK_GUARD lk(mutex_);
    if (free_list_.empty()) {
        // 如果没有 free的了，则直接返回空了
        return pipeline;
    }

    pipeline = free_list_.front();
    free_list_.pop_front();    // 如果有的话，就fetch出去了
    used_list_.push_back(pipeline);
    return pipeline;
}


NStatus DPipelineManager::release(DPipelinePtr ptr) {
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_NOT_NULL(ptr)
    NAO_LOCK_GUARD lk(mutex_);
    NBool result = std::any_of(used_list_.begin(), used_list_.end(),
                               [ptr] (DPipelinePtr cur) {
                                   return (ptr == cur);
                               });
    if (!result) {
        NAO_RETURN_ERROR_STATUS("pipeline is not used")
    }

    used_list_.remove(ptr);
    free_list_.push_front(ptr);    // 最先执行的，放回头部，可能会增加cache触达

    NAO_FUNCTION_END
}

NAO_NAMESPACE_END
