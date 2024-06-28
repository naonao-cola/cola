/**
 * @FilePath     : /cola/src/UtilsCtrl/ThreadPool/Task/UTaskGroup.h
 * @Description  :
 * @Author       : naonao
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-20 19:53:23
 **/
#ifndef CGRAPH_UTASKGROUP_H
#define CGRAPH_UTASKGROUP_H

#include <utility>

#include "../UThreadObject.h"

NAO_NAMESPACE_BEGIN

class UTaskGroup : public UThreadObject
{
public:
    explicit UTaskGroup() = default;
    NAO_NO_ALLOWED_COPY(UTaskGroup)

    /**
     * 直接通过函数来申明taskGroup
     * @param task
     * @param ttl
     * @param onFinished
     */
    explicit UTaskGroup(NAO_DEFAULT_CONST_FUNCTION_REF task, NMSec ttl = NAO_MAX_BLOCK_TTL,
                        NAO_CALLBACK_CONST_FUNCTION_REF onFinished = nullptr) noexcept
    {
        this->addTask(task)->setTtl(ttl)->setOnFinished(onFinished);
    }

    /**
     * 添加一个任务
     * @param task
     */
    UTaskGroup* addTask(NAO_DEFAULT_CONST_FUNCTION_REF task)
    {
        task_arr_.emplace_back(task);
        return this;
    }

    /**
     * 设置任务最大超时时间
     * @param ttl
     */
    UTaskGroup* setTtl(NMSec ttl)
    {
        this->ttl_ = ttl;
        return this;
    }

    /**
     * 设置执行完成后的回调函数
     * @param onFinished
     * @return
     */
    UTaskGroup* setOnFinished(NAO_CALLBACK_CONST_FUNCTION_REF onFinished)
    {
        this->on_finished_ = onFinished;
        return this;
    }

    /**
     * 获取最大超时时间信息
     * @return
     */
    NMSec getTtl() const { return this->ttl_; }

    /**
     * 清空任务组
     */
    NVoid clear() { task_arr_.clear(); }

    /**
     * 获取任务组大小
     * @return
     */
    NSize getSize() const
    {
        auto size = task_arr_.size();
        return size;
    }

private:
    std::vector<NAO_DEFAULT_FUNCTION> task_arr_;   // 任务消息
    NMSec ttl_ = NAO_MAX_BLOCK_TTL;   // 任务组最大执行耗时(如果是0的话，则表示不阻塞)
    NAO_CALLBACK_FUNCTION on_finished_ = nullptr;   // 执行函数任务结束

    friend class UThreadPool;
};

using UTaskGroupPtr = UTaskGroup*;
using UTaskGroupRef = UTaskGroup&;

NAO_NAMESPACE_END

#endif   // NAO_UTASKGROUP_H
