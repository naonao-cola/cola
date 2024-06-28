/**
 * @FilePath     : /cola/src/Dag/DagMessage/DMessageManagerSingleton.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 17:00:39
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 09:43:26
 **/
#ifndef NAO_DMESSAGEMANAGERSINGLETON_H
#define NAO_DMESSAGEMANAGERSINGLETON_H

#include "DMessageManager.h"
#include "DMessageObject.h"


NAO_NAMESPACE_BEGIN

/*
单例模式，的消息管理类
*/
class DMessageManagerSingleton : public DMessageObject
{
public:
    /**
     * 获取 GMessageManager单例的句柄信息
     * @return
     */
    static DMessageManager<>* get() { return DMessageManagerSingleton::singleton_.get(); }

private:
    static USingleton<DMessageManager<>, USingletonType::LAZY, false> singleton_;   // message管理类。注意：这个单例，是不需要被init()的
};

NAO_NAMESPACE_END

#endif   // NAO_DMESSAGEMANAGERSINGLETON_H