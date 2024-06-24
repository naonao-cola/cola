/**
 * @FilePath     : /cola/src/Dag/DagElement/DElementDefine.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 11:23:04
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 11:23:04
 **/

#ifndef NAO_DELEMENTDEFINE_H
#define NAO_DELEMENTDEFINE_H

#include "../DagObject.h"

NAO_NAMESPACE_BEGIN

const static NSize  NAO_DEFAULT_LOOP_TIMES      = 1;    // 默认循环次数信息
const static NLevel NAO_DEFAULT_ELEMENT_LEVEL   = 0;    // 默认的element级别，用于控制init函数
const static NIndex NAO_DEFAULT_BINDING_INDEX   = -1;   // 默认绑定线程id，-1表示不绑定
const static NMSec  NAO_DEFAULT_ELEMENT_TIMEOUT = 0;    // 默认element的超时时间

enum class DElementType
{
    ELEMENT         = 0x00000000,   // 元素
    NODE            = 0x00010000,   // 节点
    GROUP           = 0x00020000,   // 组
    CLUSTER         = 0x00020001,   // 簇
    REGION          = 0x00020002,   // 区域
    CONDITION       = 0x00020004,   // 条件
    SOME            = 0x00020008,   // 部分
    MUTABLE         = 0x0002000A,   // 异变
    MULTI_CONDITION = 0x00020014,   // 多条件
    ADAPTER         = 0x00040000,   // 适配器
    FUNCTION        = 0x00040001,   // 函数
    SINGLETON       = 0x00040002,   // 单例
    FENCE           = 0x00040004,   // 栅栏
    COORDINATOR     = 0x00040008,   // 协调
};


enum class DElementState
{
    CREATE  = 0x0000,   // 创建状态（暂未init的情况，包含 destroy之后的情况）
    NORMAL  = 0x1000,   // 正常执行状态
    CANCEL  = 0x1001,   // 取消状态
    YIELD   = 0x1002,   // 暂停状态
    TIMEOUT = 0x1010,   // 超时状态
};
using DPipelineState = DElementState;   // pipeline 同element共享状态


enum class DElementTimeoutStrategy
{
    AS_ERROR         = 1,   // 当做异常处理（默认）
    HOLD_BY_PIPELINE = 2,   // pipeline run执行完成之前，等待结束
    NO_HOLD          = 3,   // 不等待结束。非特殊场景，强烈不推荐使用，不排除个别平台会出现崩溃的情况
};

NAO_NAMESPACE_END

#endif   // NAO_DELEMENTDEFINE_H
