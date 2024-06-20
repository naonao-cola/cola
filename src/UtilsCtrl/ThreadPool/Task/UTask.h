/**
 * @FilePath     : /cola/src/UtilsCtrl/ThreadPool/Task/UTask.h
 * @Description  :
 * @Author       : naonao
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-20 19:53:15
 **/
#ifndef NAO_UTASK_H
#define NAO_UTASK_H

#include <memory>
#include <type_traits>
#include <vector>


#include "../UThreadObject.h"

NAO_NAMESPACE_BEGIN

class UTask : public NStatus
{
    struct TaskBased
    {
        explicit TaskBased() = default;
        virtual NVoid call() = 0;
        virtual ~TaskBased() = default;
    };

    // 退化以获得实际类型，修改思路参考：https://github.com/ChunelFeng/CThreadPool/pull/3
    template<typename F, typename T = typename std::decay<F>::type> struct TaskDerided : TaskBased
    {
        T func_;
        explicit TaskDerided(F&& func)
            : func_(std::forward<F>(func))
        {}
        NVoid call() final { func_(); }
    };

public:
    template<typename F>
    UTask(F&& func, int priority = 0)
        : impl_(new TaskDerided<F>(std::forward<F>(func)))
        , priority_(priority)
    {}

    NVoid operator()()
    {
        // impl_ 理论上不可能为空
        impl_ ? impl_->call() : throw NException("UTask inner function is nullptr");
    }

    UTask() = default;

    UTask(UTask&& task) noexcept
        : impl_(std::move(task.impl_))
        , priority_(task.priority_)
    {}

    UTask& operator=(UTask&& task) noexcept
    {
        impl_     = std::move(task.impl_);
        priority_ = task.priority_;
        return *this;
    }

    NBool operator>(const UTask& task) const
    {
        return priority_ < task.priority_;   // 新加入的，放到后面
    }

    NBool operator<(const UTask& task) const { return priority_ >= task.priority_; }

    NAO_NO_ALLOWED_COPY(UTask)

private:
    std::unique_ptr<TaskBased> impl_     = nullptr;
    int                        priority_ = 0;   // 任务的优先级信息
};


using UTaskRef    = UTask&;
using UTaskPtr    = UTask*;
using UTaskArr    = std::vector<UTask>;
using UTaskArrRef = std::vector<UTask>&;

NAO_NAMESPACE_END

#endif   // NAO_UTASK_H
