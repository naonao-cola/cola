/**
 * @FilePath     : /cola/tutorial/04/test_dag_node.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-28 14:48:09
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-08-12 15:40:33
 **/

#include "../Common/TestNode/HelloDagNode.h"
#include "../Common/common.h"

void test_hello_dag()
{
    nao::DPipelinePtr pipeline = nao::DPipelineFactory::create();   // 创建一个pipeline，用于执行图框架信息
    nao::DElementPtr  hcg      = nullptr;
    pipeline->registerDElement<HelloCGraphNode>(&hcg);   // 注册一个HelloCGraphNode节点，功能就是打印 "Hello, CGraph." 信息
    pipeline->process();                                 // 运行pipeline
    nao::DPipelineFactory::remove(pipeline);
}

void test_simple()
{
    /* 创建图对应的pipeline */
    nao::DPipelinePtr pipeline = nao::DPipelineFactory::create();
    /* 定义GElementPtr类型的变量 */
    nao::DElementPtr a, b, c, d = nullptr;

    /**
     * 注册节点，其中MyNode1和MyNode2必须为GNode的子类，否则无法通过编译。
     * status+= 操作，可以用于记录链路异常问题
     * */
    NStatus status = pipeline->registerDElement<MyNode1>(&a, {}, "nodeA");   // 将名为nodeA，无执行依赖的node信息，注册入pipeline中
    status += pipeline->registerDElement<MyNode2>(&b, {a}, "nodeB");         // 将名为nodeB，依赖a执行的node信息，注册入pipeline中
    status += pipeline->registerDElement<MyNode1>(&c, {a}, "nodeC");
    status += pipeline->registerDElement<MyNode2>(&d, {b, c}, "nodeD");   // 将名为nodeD，依赖{b,c}执行的node信息，注册入pipeline中
    if (!status.isOK()) {
        return;   // 使用时，请对所有CGraph接口的返回值做判定。今后tutorial例子中省略该操作。
    }

    /**
    UThreadPoolConfig config;             // （可选）推荐根据自己设定的dag逻辑，来配置调度信息
    config.default_thread_size_ = 2;
    config.secondary_thread_size_ = 0;    // 更多配置信息，请参考 UThreadPoolDefine.h 中的描述
    pipeline->setUniqueThreadPoolConfig(config);
    */

    /* 图信息初始化，准备开始计算 */
    status += pipeline->init();

    /* 运行流图信息。初始化后，支持多次循环计算 */
    for (int i = 0; i < 3; i++) {
        status += pipeline->run();
        nao::NAO_ECHO("==== tutorial_simple, loop : [%d], and run status = [%d].", i + 1, status.getCode());
    }

    /* 图信息逆初始化，准备结束计算 */
    status += pipeline->destroy();
    nao::DPipelineFactory::remove(pipeline);
}

void test_cluster()
{
    NStatus           status;
    nao::DPipelinePtr pipeline = nao::DPipelineFactory::create();
    nao::DElementPtr  a, b_cluster, c, d = nullptr;

    b_cluster = pipeline->createDGroup<nao::DCluster>({pipeline->createDNode<MyNode1>(nao::DNodeInfo("nodeB1", 1)),     // 创建名为nodeB1的node信息，并将其放入b_cluster中
                                                       pipeline->createDNode<MyNode1>(nao::DNodeInfo("nodeB2", 3)),     // 创建名为nodeB2且自循环3次的node信息，并将其放入b_cluster中
                                                       pipeline->createDNode<MyNode2>(nao::DNodeInfo("nodeB3", 1))});   // 创建cluster信息，包含了三个node信息

    /* 正式使用时，请对所有返回值进行判定 */
    status = pipeline->registerDElement<MyNode1>(&a, {}, "nodeA", 1);                      // 将名为nodeA的node信息，注册入pipeline中
    status += pipeline->registerDElement<nao::DCluster>(&b_cluster, {a}, "clusterB", 2);   // 将名为clusterB，依赖a执行且自循环2次的cluster信息，注册入pipeline中
    status += pipeline->registerDElement<MyNode1>(&c, {a}, "nodeC", 1);
    status += pipeline->registerDElement<MyNode2>(&d, {b_cluster, c}, "nodeD", 2);
    if (!status.isOK()) {
        return;
    }

    /**
     * 如果想查看当前 pipeline 的流图结构信息，
     * 请调用 dump()方法，并且将输出的内容(不包含node内部的打印信息)，
     * 复制到 https://dreampuf.github.io/GraphvizOnline/ 中查看效果
     */
    // pipeline->dump();

    /* process函数，相当于 init(),run()*n,destroy()函数，同时调用 */
    status = pipeline->process();
    nao::NAO_ECHO("pipeline process status is : [%d]", status.getCode());
    nao::DPipelineFactory::clear();   // clear表示清空所有的pipeline信息
}

void test_region()
{
    NStatus           status;
    nao::DPipelinePtr pipeline = nao::DPipelineFactory::create();
    nao::DElementPtr  a, b_region, c = nullptr;

    nao::DElementPtr b1, b2, b3, b4 = nullptr;
    b1 = pipeline->createDNode<MyNode1>(nao::DNodeInfo({}, "nodeB1", 1));     // 创建名为nodeB1的node信息
    b2 = pipeline->createDNode<MyNode2>(nao::DNodeInfo({b1}, "nodeB2", 2));   // 创建名为nodeB2且自循环2次的node信息
    b3 = pipeline->createDNode<MyNode1>(nao::DNodeInfo({b1}, "nodeB3", 1));
    b4 = pipeline->createDNode<MyNode1>(nao::DNodeInfo({b2, b3}, "nodeB4", 1));

    b_region = pipeline->createDGroup<nao::DRegion>({b1, b2, b3, b4});   // 将 b1、b2、b3、b4 注册入b_region中
    if (nullptr == b_region) {
        return;
    }

    status += pipeline->registerDElement<MyNode1>(&a, {}, "nodeA", 1);
    status += pipeline->registerDElement<nao::DRegion>(&b_region, {a}, "regionB", 2);   // 将名为regionB，依赖a执行且自循环2次的region信息，注册入pipeline中
    status += pipeline->registerDElement<MyNode2>(&c, {b_region}, "nodeC", 1);
    if (!status.isOK()) {
        return;
    }

    status = pipeline->process();
    nao::NAO_ECHO("pipeline process status is : [%d]", status.getCode());

    /**
     * 如果想查看pipeline内部，各部分(element)的运行耗时情况，
     * 请调用 perf()方法，并且将输出的内容(不包含node内部的打印信息)，
     * 复制到 https://dreampuf.github.io/GraphvizOnline/ 查看效果
     * 具体字段解释，请参考函数头文件备注信息
     */
    // pipeline->perf();

    nao::DPipelineFactory::remove(pipeline);
}

void test_complex()
{
    NStatus           status;
    nao::DPipelinePtr pipeline = nao::DPipelineFactory::create();
    nao::DElementPtr  a, b_cluster, c, d_region, e = nullptr;

    b_cluster = pipeline->createDGroup<nao::DCluster>({pipeline->createDNode<MyNode1>(nao::DNodeInfo("nodeB1", 1)),   // 创建名为nodeB1的node信息，并将其放入b_cluster中
                                                       pipeline->createDNode<MyNode1>(nao::DNodeInfo("nodeB2", 3)),   // 创建名为nodeB2且自循环3次的node信息，并将其放入b_cluster中
                                                       pipeline->createDNode<MyNode2>(nao::DNodeInfo("nodeB3", 1))});

    nao::DElementPtr d1, d2, d3, d4, d23_cluster = nullptr;
    d1          = pipeline->createDNode<MyNode1>(nao::DNodeInfo({}, "nodeD1", 1));
    d2          = pipeline->createDNode<MyNode1>(nao::DNodeInfo("nodeD2", 1));   // 创建node，稍后放入cluster中
    d3          = pipeline->createDNode<MyNode1>(nao::DNodeInfo("nodeD3", 1));
    d23_cluster = pipeline->createDGroup<nao::DCluster>({d2, d3}, {d1}, "clusterD23", 1);
    d4          = pipeline->createDNode<MyNode2>(nao::DNodeInfo({d1}, "nodeD4", 1));
    d_region    = pipeline->createDGroup<nao::DRegion>({d1, d23_cluster, d4});   // 创建名为d_region的region信息，并将{d1,d23_cluster,d4}放入其中

    status += pipeline->registerDElement<MyNode1>(&a, {}, "nodeA", 1);
    status += pipeline->registerDElement<nao::DCluster>(&b_cluster, {}, "clusterB", 1);
    status += pipeline->registerDElement<MyNode1>(&c, {a, b_cluster}, "nodeC", 1);
    status += pipeline->registerDElement<nao::DRegion>(&d_region, {a, b_cluster}, "regionD", 2);   // 将名为regionD，依赖{a,b_cluster}执行且自循环2次的region信息，注册入pipeline中
    status += pipeline->registerDElement<MyNode1>(&e, {c, d_region}, "nodeE", 1);
    if (!status.isOK()) {
        return;
    }

    status += pipeline->process();
    nao::DPipelineFactory::remove(pipeline);
}
int main()
{
    // test_hello_dag();
    // test_simple();
    // test_cluster();
    // test_region();
    test_complex();
    return 0;
}