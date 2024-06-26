/**
 * @FilePath     : /cola/src/Dag/DagElement/DNode/DNodeDefine.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-26 12:36:48
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-26 12:36:48
 **/
#ifndef NAO_DNODEDEFINE_H
#define NAO_DNODEDEFINE_H

#include <set>
#include <string>
#include <utility>


#include "../DElement.h"

NAO_NAMESPACE_BEGIN

enum class DNodeType
{
    BASIC = 0,   // 默认情况
    IO    = 1,   // IO节点
    CPU   = 2,   // CPU节点
    GPU   = 3,   // GPU节点
};

/**
 * 功能节点信息
 * 创建GCluster和GRegion的中的GNode的时候，使用到的信息
 * 创建GCluster的时候，dependence为空
 * 创建GRegion的时候，dependence不为空。若为空，则表示可直接执行
 */
struct DNodeInfo : public NStruct
{
    std::string    name_;                           // 名称
    NSize          loop_{NAO_DEFAULT_LOOP_TIMES};   // 循环次数
    DElementPtrSet dependence_;                     // 依赖节点

    // 无依赖版本，适用于cluster创建
    explicit DNodeInfo(const std::string& name = NAO_EMPTY, NSize loop = NAO_DEFAULT_LOOP_TIMES)
    {
        this->name_ = name;
        this->loop_ = loop;
    }

    // 有依赖版本，适用于region创建
    explicit DNodeInfo(const DElementPtrSet& dependence = std::initializer_list<DElementPtr>(), const std::string& name = NAO_EMPTY,
                       NSize loop = NAO_DEFAULT_LOOP_TIMES)
    {
        this->name_       = name;
        this->loop_       = loop;
        this->dependence_ = dependence;
    }
};

NAO_NAMESPACE_END

#endif   // NAO_DNODEDEFINE_H
