/**
 * @FilePath     : /cola/src/Dag/DagMessage/DMessageManagerSingleton.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 17:01:09
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 09:43:16
 **/
#include "DMessageManagerSingleton.h"

NAO_NAMESPACE_BEGIN

/**
 * GMessageManager单例的句柄信息
 * message管理类，必须在这里实现。
 * 如果是放在 GMessageManager.h 文件中，以外部申明static变量的形式存在
 * 在cpp文件中引用的时候，会出现被多次构造的情况
 * 参考：https://github.com/ChunelFeng/CGraph/issues/221
 */
USingleton<DMessageManager<>, USingletonType::LAZY, false> DMessageManagerSingleton::singleton_;

NAO_NAMESPACE_END
