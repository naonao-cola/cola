/**
 * @FilePath     : /cola/src/Dag/DagMessage/DMessage.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 16:57:51
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 17:11:31
 **/
#ifndef NAO_DMESSAGE_H
#define NAO_DMESSAGE_H

#include <memory>

#include "DMessageDefine.h"
#include "DMessageObject.h"


NAO_NAMESPACE_BEGIN
/*
消息类，每个消息里面有个环形队列
*/
template<typename T, NUInt capacity = NAO_DEFAULT_RINGBUFFER_SIZE, c_enable_if_t<std::is_base_of<DMessageParam, T>::value, int> = 0>
class DMessage : public DMessageObject
{
public:
    explicit DMessage(NUInt size = capacity) { queue_.setCapacity(size); }

    /**
     * 析构函数。释放前，要先释放队列中所有的信息
     */
    ~DMessage() override { queue_.clear(); }

    /**
     * 写入参数
     * @tparam TImpl
     * @param value
     * @param strategy
     * @return
     */
    template<class TImpl, c_enable_if_t<std::is_base_of<T, TImpl>::value, int> = 0>
    NVoid send(const TImpl& value, DMessagePushStrategy strategy)
    {
        queue_.push(value, strategy);
    }

    /**
     * 写入智能指针类型的参数
     * @tparam TImpl
     * @param value
     * @param strategy
     * @return
     */
    template<class TImpl, c_enable_if_t<std::is_base_of<T, TImpl>::value, int> = 0>
    NVoid send(std::unique_ptr<TImpl>& value, DMessagePushStrategy strategy)
    {
        queue_.push(value, strategy);
    }

    /**
     * 获取参数
     * @param value
     * @param timeout
     * @return
     */
    template<class TImpl, c_enable_if_t<std::is_base_of<T, TImpl>::value, int> = 0>
    NStatus recv(TImpl& value, NMSec timeout)
    {
        return queue_.waitPopWithTimeout(value, timeout);
    }

    /**
     * 通过智能指针的方式传递
     * @tparam TImpl
     * @param value
     * @param timeout
     * @return
     */
    template<class TImpl, c_enable_if_t<std::is_base_of<T, TImpl>::value, int> = 0>
    NStatus recv(std::unique_ptr<TImpl>& value, NMSec timeout)
    {
        return queue_.waitPopWithTimeout(value, timeout);
    }

    /**
     * 获取容量大小
     * @return
     */
    NUInt getCapacity() const { return queue_.getCapacity(); }

private:
    UAtomicRingBufferQueue<T, capacity> queue_;
};


template<typename T, NUInt capacity = NAO_DEFAULT_RINGBUFFER_SIZE>
using DMessagePtr = DMessage<T, capacity>*;

NAO_NAMESPACE_END

#endif   // NAO_DMESSAGE_H
