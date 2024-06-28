/**
 * @FilePath     : /cola/cola/UtilsCtrl/Timer/UTimerV2.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-28 16:10:47
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 16:32:20
 **/
#ifndef NAO_UTIMERV2_H
#define NAO_UTIMERV2_H

#include "../UtilsObject.h"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <stdint.h>
#include <utility>


NAO_NAMESPACE_BEGIN

class UTimerV2 : public UtilsObject
{

public:
    using MS        = std::chrono::milliseconds;
    using Clock     = std::chrono::high_resolution_clock;
    using TimePoint = Clock::time_point;

    UTimerV2();
    ~UTimerV2() override;

    class Task
    {
    public:
        Task(const NInt& id, const NInt& f, const TimePoint& t, NAO_DEFAULT_FUNCTION timeOutFun, const NBool& loopFlag)
            : id_(id)
            , timeOutFuction_(std::move(timeOutFun))
            , count_(0)
            , expireTime_(t)
            , loop_(loopFlag)
            , frequency_(MS(f)){};

        NInt      getID() const { return id_; }
        MS        getFrequence() const { return frequency_; }
        TimePoint getExpireTimet() const { return expireTime_; }
        int64_t   getCount() const { return count_; }
        NBool     getLoop() const { return loop_; }
        NVoid     runTimeOutFunction()
        {
            timeOutFuction_();
            count_++;
            if (loop_) {
                expireTime_ = Clock::now() + frequency_;
            }
        }

        NVoid setFuncNull() { timeOutFuction_ = nullptr; }

    private:
        NInt                 id_;               // 任务id
        MS                   frequency_;        // 任务频率
        NAO_DEFAULT_FUNCTION timeOutFuction_;   // 任务函数
        int64_t              count_;            // 任务执行次数
        TimePoint            expireTime_;       // 任务的超时时间
        NBool                loop_;             // 是否循环
    };   // class Task

    // 比较函数
    struct TimerCmp
    {
        bool operator()(const std::shared_ptr<Task>& a, const std::shared_ptr<Task>& b) { return (a->getExpireTimet() > b->getExpireTimet()); }
    };
    void run_every(const NInt& id, const NInt& ms, const NAO_DEFAULT_FUNCTION& task);
    void run_later(const NInt& id, const NInt& ms, const NAO_DEFAULT_FUNCTION& task);
    void stop();

private:
    void loop();
    void clear(std::priority_queue<std::shared_ptr<Task>, std::vector<std::shared_ptr<Task>>, TimerCmp>& v);
    std::priority_queue<std::shared_ptr<Task>, std::vector<std::shared_ptr<Task>>, TimerCmp> tasks_;
    std::priority_queue<std::shared_ptr<Task>, std::vector<std::shared_ptr<Task>>, TimerCmp> tmp_;
    std::thread                                                                              thread_;
    std::atomic_bool                                                                         stop_;
    std::mutex                                                                               lock_;
};

NAO_NAMESPACE_END

#endif   // NAO_UTIMERV2_H
