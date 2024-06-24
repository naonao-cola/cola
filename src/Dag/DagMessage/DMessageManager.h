/**
 * @FilePath     : /cola/src/Dag/DagMessage/DMessageManager.h
 * @Description  : 消息管理类
 * @Author       : naonao
 * @Date         : 2024-06-24 16:59:36
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 16:59:37
 **/
#ifndef NAO_DMESSAGEMANAGER_H
#define NAO_DMESSAGEMANAGER_H

#include <memory>
#include <set>
#include <string>
#include <unordered_map>


#include "DMessage.h"
#include "DMessageDefine.h"

NAO_NAMESPACE_BEGIN

template<typename T = DMessageParam, c_enable_if_t<std::is_base_of<DMessageParam, T>::value, int> = 0>
class DMessageManager : public DMessageObject, public DagManager<DMessage<T>>
{
public:
    /**
     * 创建 topic
     * @tparam TImpl
     * @param topic
     * @param size
     * @return
     */
    template<typename TImpl, c_enable_if_t<std::is_base_of<T, TImpl>::value, int> = 0>
    NStatus createTopic(const std::string& topic, NUint size)
    {
        NAO_FUNCTION_BEGIN

        auto innerTopic = internal::SEND_RECV_PREFIX + topic;   // 中间做一层映射，用来区分是 PubSub的，还是SendRecv的
        auto result     = send_recv_message_map_.find(innerTopic);
        if (result != send_recv_message_map_.end()) {
            // 如果类型和size完全匹配的话，则直接返回创建成功。否则返回错误
            auto curTopic = result->second;
            status        = (typeid(*curTopic).name() == typeid(DMessage<TImpl>).name() && curTopic->getCapacity() == size)
                                ? NStatus()
                                : NErrStatus("create topic [" + topic + "] duplicate");
        }
        else {
            // 创建一个 topic信息
            auto message = UAllocator::safeMallocTemplateNObject<DMessage<TImpl>, NUint>(size);
            send_recv_message_map_.insert(std::pair<const std::string&, DMessagePtr<T>>(innerTopic, DMessagePtr<T>(message)));
        }

        NAO_FUNCTION_END
    }

    /**
     * 删除 topic
     * @param topic
     * @return
     */
    NStatus removeTopic(const std::string& topic)
    {
        NAO_FUNCTION_BEGIN
        auto innerTopic = internal::SEND_RECV_PREFIX + topic;
        auto result     = send_recv_message_map_.find(innerTopic);
        if (result == send_recv_message_map_.end()) {
            NAO_RETURN_ERROR_STATUS("no find [" + topic + "] topic");
        }

        NAO_DELETE_PTR(result->second);
        send_recv_message_map_.erase(result);

        NAO_FUNCTION_END
    }

    /**
     * 根据传入的topic，获得信息
     * @tparam TImpl
     * @param topic
     * @param value
     * @param timeout
     * @return
     */
    template<typename TImpl, c_enable_if_t<std::is_base_of<T, TImpl>::value, int> = 0>
    NStatus recvTopicValue(const std::string& topic, TImpl& value, NMSec timeout = NAO_MAX_BLOCK_TTL)
    {
        NAO_FUNCTION_BEGIN
        auto innerTopic = internal::SEND_RECV_PREFIX + topic;
        auto result     = send_recv_message_map_.find(innerTopic);
        if (result == send_recv_message_map_.end()) {
            NAO_RETURN_ERROR_STATUS("no find [" + topic + "] topic");
        }

        auto message = (DMessagePtr<TImpl>)(result->second);
        NAO_ASSERT_NOT_NULL(message);

        status = message->recv(value, timeout);
        NAO_FUNCTION_END
    }

    /**
     * 根据传入的topic，获得信息。仅针对传入智能指针的情况
     * @tparam TImpl
     * @param topic
     * @param value
     * @param timeout
     * @return
     * @notice 这里的逻辑，跟上面的函数一致。里面调用了底层RingBuffer的同名不同入参的接口。
     *         本人暂时没有能力完成接口的统一。如果有了解这一块内容的朋友，欢迎交流指正。
     */
    template<typename TImpl, c_enable_if_t<std::is_base_of<T, TImpl>::value, int> = 0>
    NStatus recvTopicValue(const std::string& topic, std::unique_ptr<TImpl>& value, NMSec timeout = NAO_MAX_BLOCK_TTL)
    {
        NAO_FUNCTION_BEGIN
        auto innerTopic = internal::SEND_RECV_PREFIX + topic;
        auto result     = send_recv_message_map_.find(innerTopic);
        if (result == send_recv_message_map_.end()) {
            NAO_RETURN_ERROR_STATUS("no find [" + topic + "] topic");
        }

        auto message = (DMessagePtr<TImpl>)(result->second);
        NAO_ASSERT_NOT_NULL(message);

        status = message->recv(value, timeout);
        NAO_FUNCTION_END
    }

    /**
     * 根据传入的topic，输入信息
     * @tparam TImpl
     * @param topic
     * @param value
     * @param strategy
     * @return
     */
    template<typename TImpl, c_enable_if_t<std::is_base_of<T, TImpl>::value, int> = 0>
    NStatus sendTopicValue(const std::string& topic, const TImpl& value, DMessagePushStrategy strategy)
    {
        NAO_FUNCTION_BEGIN
        auto innerTopic = internal::SEND_RECV_PREFIX + topic;
        auto result     = send_recv_message_map_.find(innerTopic);
        if (result == send_recv_message_map_.end()) {
            NAO_RETURN_ERROR_STATUS("no find [" + topic + "] topic");
        }

        auto message = static_cast<DMessagePtr<T>>(result->second);
        NAO_ASSERT_NOT_NULL(message);

        message->send(value, strategy);
        NAO_FUNCTION_END
    }

    /**
     * 根据传入的topic，输入智能指针类型的信息
     * @tparam TImpl
     * @param topic
     * @param value
     * @param strategy
     * @return
     */
    template<typename TImpl, c_enable_if_t<std::is_base_of<T, TImpl>::value, int> = 0>
    NStatus sendTopicValue(const std::string& topic, std::unique_ptr<TImpl>& value, DMessagePushStrategy strategy)
    {
        NAO_FUNCTION_BEGIN
        auto innerTopic = internal::SEND_RECV_PREFIX + topic;
        auto result     = send_recv_message_map_.find(innerTopic);
        if (result == send_recv_message_map_.end()) {
            NAO_RETURN_ERROR_STATUS("no find [" + topic + "] topic");
        }

        auto message = static_cast<DMessagePtr<T>>(result->second);
        NAO_ASSERT_NOT_NULL(message);

        message->send(value, strategy);
        NAO_FUNCTION_END
    }

    /**
     * 绑定对应的topic信息，并且获取 conn_id 信息
     * @tparam TImpl
     * @param topic
     * @param size
     * @return
     */
    template<typename TImpl, c_enable_if_t<std::is_base_of<T, TImpl>::value, int> = 0>
    NIndex bindTopic(const std::string& topic, NUint size)
    {
        auto innerTopic = internal::PUB_SUB_PREFIX + topic;
        auto message    = UAllocator::safeMallocTemplateNObject<DMessage<TImpl>, NUint>(size);

        NAO_LOCK_GUARD lock(bind_mutex_);
        NIndex         connId = (++cur_conn_id_);
        auto           result = pub_sub_message_map_.find(innerTopic);
        if (result != pub_sub_message_map_.end()) {
            // 如果之前有的话，则在后面添加一个
            auto& messageSet = result->second;
            messageSet.insert((DMessagePtr<T>)message);
        }
        else {
            // 如果是这个topic第一次被绑定，则创建一个对应的set信息
            std::set<DMessagePtr<T>> messageSet;
            messageSet.insert((DMessagePtr<T>)message);
            pub_sub_message_map_[innerTopic] = messageSet;
        }
        conn_message_map_[connId] = (DMessagePtr<T>)message;
        return connId;
    }

    /**
     * 开始发送对应topic的信息
     * @tparam TImpl
     * @param topic
     * @param value
     * @param strategy
     * @return
     */
    template<typename TImpl, c_enable_if_t<std::is_base_of<T, TImpl>::value, int> = 0>
    NStatus pubTopicValue(const std::string& topic, const TImpl& value, DMessagePushStrategy strategy)
    {
        NAO_FUNCTION_BEGIN
        auto innerTopic = internal::PUB_SUB_PREFIX + topic;
        auto result     = pub_sub_message_map_.find(innerTopic);
        if (result == pub_sub_message_map_.end()) {
            NAO_RETURN_ERROR_STATUS("no find [" + topic + "] topic");
        }

        auto& messageSet = result->second;
        for (auto msg : messageSet) {
            msg->send(value, strategy);   // 给所有订阅的信息，一次发送消息
        }
        NAO_FUNCTION_END
    }

    /**
     * 根据传入的 connId信息，来获取对应的message信息
     * @tparam TImpl
     * @param connId
     * @param value
     * @param timeout
     * @return
     */
    template<typename TImpl, c_enable_if_t<std::is_base_of<T, TImpl>::value, int> = 0>
    NStatus subTopicValue(NIndex connId, TImpl& value, NMSec timeout = NAO_MAX_BLOCK_TTL)
    {
        NAO_FUNCTION_BEGIN
        if (conn_message_map_.end() == conn_message_map_.find(connId)) {
            NAO_RETURN_ERROR_STATUS("no find [" + std::to_string(connId) + "] connect");
        }

        auto message = (GMessagePtr<TImpl>)(conn_message_map_[connId]);
        status       = message->recv(value, timeout);
        NAO_FUNCTION_END
    }

    /**
     * 根据传入的 connId信息，来获取对应的message信息。仅针对传入智能指针的情况
     * @tparam TImpl
     * @param connId
     * @param value
     * @param timeout
     * @return
     */
    template<typename TImpl, c_enable_if_t<std::is_base_of<T, TImpl>::value, int> = 0>
    NStatus subTopicValue(NIndex connId, std::unique_ptr<TImpl>& value, NMSec timeout = NAO_MAX_BLOCK_TTL)
    {
        NAO_FUNCTION_BEGIN
        if (conn_message_map_.end() == conn_message_map_.find(connId)) {
            NAO_RETURN_ERROR_STATUS("no find [" + std::to_string(connId) + "] connect");
        }

        auto message = (DMessagePtr<TImpl>)(conn_message_map_[connId]);
        status       = message->recv(value, timeout);
        NAO_FUNCTION_END
    }

    /**
     * 删除对应的topic信息
     * @param topic
     * @return
     */
    NStatus dropTopic(const std::string& topic)
    {
        NAO_FUNCTION_BEGIN
        auto innerTopic = internal::PUB_SUB_PREFIX + topic;
        auto result     = pub_sub_message_map_.find(innerTopic);
        if (result == pub_sub_message_map_.end()) {
            NAO_RETURN_ERROR_STATUS("no find [" + topic + "] topic");
        }

        auto& messageSet = result->second;
        for (auto msg : messageSet) {
            NAO_DELETE_PTR(msg)
        }
        pub_sub_message_map_.erase(innerTopic);
        NAO_FUNCTION_END
    }

    /**
     * 清空数据
     * @return
     */
    NStatus clear() final
    {
        NAO_FUNCTION_BEGIN
        for (auto& cur : send_recv_message_map_) {
            NAO_DELETE_PTR(cur.second)
        }

        for (auto& cur : pub_sub_message_map_) {
            for (auto iter : cur.second) {
                NAO_DELETE_PTR(iter);
            }
        }
        send_recv_message_map_.clear();
        pub_sub_message_map_.clear();
        cur_conn_id_ = 0;
        NAO_FUNCTION_END
    }


protected:
    NStatus init() final{/**
                          * 仅可能被 USingleton 类调用，自动构造时候使用
                          * 防止菱形引用的奇异
                          */
                         NAO_EMPTY_FUNCTION}

    NStatus destroy() final
    {
        NAO_EMPTY_FUNCTION
    }

    ~DMessageManager() override
    {
        clear();   // 释放所有的信息
    }

private:
    std::unordered_map<std::string, DMessagePtr<T>>           send_recv_message_map_;   // 记录 topic 和 message queue 信息
    std::unordered_map<std::string, std::set<DMessagePtr<T>>> pub_sub_message_map_;     // 记录 pub和sub的 message 的信息
    std::unordered_map<NIndex, DMessagePtr<T>>                conn_message_map_;        // 用于根据 index反推message信息
    NIndex                                                    cur_conn_id_ = 0;         // 记录当前的conn信息

    std::mutex bind_mutex_;

    template<typename U, USingletonType, NBool>
    friend class USingleton;
};

NAO_NAMESPACE_END

#endif   // NAO_DMESSAGEMANAGER_H
