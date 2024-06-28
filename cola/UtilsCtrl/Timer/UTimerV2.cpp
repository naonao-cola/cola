/**
 * @FilePath     : /cola/cola/UtilsCtrl/Timer/UTimerV2.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-28 16:35:17
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 16:54:27
 **/

#include "UTimerV2.h"

NAO_NAMESPACE_BEGIN

UTimerV2::UTimerV2()
    : stop_(false)
{
    thread_ = std::thread(&UTimerV2::loop, this);
}

UTimerV2::~UTimerV2()
{
    stop();
}

NVoid UTimerV2::loop()
{
    std::priority_queue<std::shared_ptr<Task>, std::vector<std::shared_ptr<Task>>, TimerCmp> temp;
    while (!stop_) {
        {
            NAO_UNIQUE_LOCK lck(lock_);
            if (tmp_.size() > 0) {
                tmp_.swap(temp);
            }
        }
        if (!temp.empty()) {
            std::shared_ptr<Task> t = temp.top();
            tasks_.push(t);
            temp.pop();
        }
        while (!tasks_.empty()) {
            std::shared_ptr<Task> delTimer = tasks_.top();
            if (std::chrono::duration_cast<MS>(delTimer->getExpireTimet() - Clock::now()).count() > 0) {
                continue;
            }
            delTimer->runTimeOutFunction();
            if (!delTimer->getLoop()) tasks_.pop();
        }
        if (stop_) return;
    }
}

NVoid UTimerV2::run_every(const NInt& id, const NInt& ms, const NAO_DEFAULT_FUNCTION& task)
{
    std::shared_ptr<Task> t = std::make_shared<Task>(id, ms, Clock::now() + MS(ms), task, 1);
    {
        NAO_UNIQUE_LOCK lck(lock_);
        tmp_.push(t);
    }
}


NVoid UTimerV2::run_later(const NInt& id, const NInt& ms, const NAO_DEFAULT_FUNCTION& task)
{
    std::shared_ptr<Task> t = std::make_shared<Task>(id, ms, Clock::now() + MS(ms), task, 0);
    {
        NAO_UNIQUE_LOCK lck(lock_);
        tmp_.push(t);
    }
}

NVoid UTimerV2::clear(std::priority_queue<std::shared_ptr<Task>, std::vector<std::shared_ptr<Task>>, TimerCmp>& v)
{
    while (!v.empty()) {
        v.pop();
    }
}

NVoid UTimerV2::stop()
{
    if (stop_) return;
    stop_ = true;
    thread_.join();
    this->clear(tasks_);
    this->clear(tmp_);
}

NAO_NAMESPACE_END