/**
 * @FilePath     : /cola/src/UtilsCtrl/ThreadPool/Lock/USpinLock.h
 * @Description  :
 * @Author       : naonao
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-20 19:53:04
 **/
#ifndef NAO_USPINLOCK_H
#define NAO_USPINLOCK_H

#include <atomic>

#include "../UThreadObject.h"

NAO_NAMESPACE_BEGIN

class USpinLock : public UThreadObject
{
public:
    /**
     * 加锁
     */
    NVoid lock()
    {
        // memory_order_acquire 后面访存指令勿重排至此条指令之前
        while (flag_.test_and_set(std::memory_order_acquire)) {}
    }

    /**
     * 解锁
     */
    NVoid unlock()
    {
        // memory_order_release 前面访存指令勿重排到此条指令之后
        flag_.clear(std::memory_order_release);
    }

    /**
     * 尝试加锁。若未加锁，会上锁
     * @return
     */
    NBool tryLock() { return !flag_.test_and_set(); }

private:
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;   // 标志位
};

NAO_NAMESPACE_END

#endif   // NAO_USPINLOCK_H
