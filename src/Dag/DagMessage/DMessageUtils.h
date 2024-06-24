/**
 * @FilePath     : /cola/src/Dag/DagMessage/DMessageUtils.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 17:00:07
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 17:49:18
 **/
#ifndef NAO_DMESSAGEUTILS_H
#define NAO_DMESSAGEUTILS_H

#include "DMessageManagerSingleton.h"

NAO_NAMESPACE_BEGIN

/*******************************
 * 以下是 send recv 的功能对应的接口
 *******************************/
/* 创建一个 topic */
#define NAO_CREATE_MESSAGE_TOPIC(Type, topic, size) nao::DMessageManagerSingleton::get()->template createTopic<Type>(topic, size);

/* 删除一个 topic */
#define NAO_REMOVE_MESSAGE_TOPIC(topic) nao::DMessageManagerSingleton::get()->removeTopic(topic);

/* 发送一个 message param */
#define NAO_SEND_MPARAM(Type, topic, value, strategy) nao::DMessageManagerSingleton::get()->template sendTopicValue<Type>(topic, value, strategy);

/* 接收一个 message param */
#define NAO_RECV_MPARAM(Type, topic, value) nao::DMessageManagerSingleton::get()->template recvTopicValue<Type>(topic, value);

/* 有等待时间的接收一个 message param */
#define NAO_RECV_MPARAM_WITH_TIMEOUT(Type, topic, value, timeout) \
    nao::DMessageManagerSingleton::get()->template recvTopicValue<Type>(topic, value, timeout);

/*******************************
 * 以下是 pub sub 的功能对应的接口
 *******************************/
/* 绑定一个topic，并且返回 conn id 信息 */
#define NAO_BIND_MESSAGE_TOPIC(Type, topic, size) nao::DMessageManagerSingleton::get()->template bindTopic<Type>(topic, size);

/* 发布一个消息 */
#define NAO_PUB_MPARAM(Type, topic, value, strategy) nao::DMessageManagerSingleton::get()->template pubTopicValue<Type>(topic, value, strategy);

/* 订阅一个消息 */
#define NAO_SUB_MPARAM(Type, connId, value) nao::DMessageManagerSingleton::get()->template subTopicValue<Type>(connId, value);

/* 有等待时间的订阅一个消息 */
#define NAO_SUB_MPARAM_WITH_TIMEOUT(Type, connId, value, timeout) \
    nao::DMessageManagerSingleton::get()->template subTopicValue<Type>(connId, value, timeout);

/* 删除消息 */
#define NAO_DROP_MESSAGE_TOPIC(topic) nao::DMessageManagerSingleton::get()->dropTopic(topic);


/****************************
 * 以下针对所有功能
 ****************************/
/* 清空所有的topic */
#define NAO_CLEAR_MESSAGES() nao::DMessageManagerSingleton::get()->clear();

NAO_NAMESPACE_END

#endif   // NAO_DMESSAGEUTILS_H
