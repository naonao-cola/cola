/**
 * @FilePath     : /cola/tutorial/Common/TestNode/HelloDagNode.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-28 14:45:45
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 14:45:45
 **/
#ifndef NAO_HELLODAGNODE_H
#define NAO_HELLODAGNODE_H

#include "../common.h"

class HelloCGraphNode : public nao::DNode
{
public:
    NStatus run() override
    {
        NStatus status;
        std::cout << "Hello, DAG." << std::endl;
        return status;
    }
};

class MyNode1 : public nao::DNode
{
public:
    NStatus run() override
    {
        NStatus status;
        nao::NAO_ECHO("[%s], enter MyNode1 run function. Sleep for 1 second ... ", this->getName().c_str());
        NAO_SLEEP_SECOND(1)
        return status;
    }
};


class MyNode2 : public nao::DNode
{
public:
    // 执行过程中，init 和 destroy 方法，单次执行，可选择性实现
    // run 方法可以多次执行，且必须实现
    NStatus init() override
    {
        NStatus status;
        nao::NAO_ECHO("[INIT] [%s], enter MyNode2 init function.", this->getName().c_str());
        return status;
    }

    NStatus run() override
    {
        NStatus status;
        nao::NAO_ECHO("[%s], enter MyNode2 run function. Sleep for 2 second ... ", this->getName().c_str());
        NAO_SLEEP_SECOND(2)
        return status;
    }

    NStatus destroy() override
    {
        NStatus status;
        nao::NAO_ECHO("[DESTROY] [%s], enter MyNode2 destroy function.", this->getName().c_str());
        return status;
    }
};

#endif   // NAO_HELLODAGNODE_H