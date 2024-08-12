/**
 * @FilePath     : /cola/src/Dag/DagElement/_DEngine/DEngineDefine.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-26 11:29:49
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-26 11:29:49
 **/
#ifndef NAO_DENGINEDEFINE_H
#define NAO_DENGINEDEFINE_H

#include "../DElementDefine.h"

NAO_NAMESPACE_BEGIN

enum class DEngineType
{
    DYNAMIC = 0, /** 动态图运行 */
    TOPO    = 1, /** 拓扑运行 */
    STATIC  = 2, /** 静态图运行 */
};

NAO_INTERNAL_NAMESPACE_BEGIN
enum class DEngineDagType
{
    COMMON       = 0, /** 传统的dag类型 */
    ALL_SERIAL   = 1, /** 所有的element都是串行的 */
    ALL_PARALLEL = 2, /** 所有的element都是并行的 */
};
NAO_INTERNAL_NAMESPACE_END

NAO_NAMESPACE_END

#endif   // NAO_DENGINEDEFINE_H