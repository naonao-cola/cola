/**
 * @FilePath     : /cola/src/DomainCtrl/Ann/DAnnDefine.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 22:40:36
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 22:49:45
 **/

#ifndef NAO_DANNDEFINE_H
#define NAO_DANNDEFINE_H

#include "DAnnObject.h"

NAO_NAMESPACE_BEGIN

enum class D_ANN_FUNC_TYPE
{
    ANN_PREPARE_ERROR = 0,   // 参数处理错误
    ANN_TRAIN         = 1,   // 训练
    ANN_SEARCH        = 2,   // 查询
    ANN_INSERT        = 3,   // 插入
    ANN_UPDATE        = 4,   // 修改
    ANN_REMOVE        = 5,   // 删除
    ANN_LOAD_MODEL    = 6,   // 加载模型
    ANN_SAVE_MODEL    = 7,   // 保存模型
    ANN_EDITION       = 8,   // 其他
    ANN_MAX_SIZE      = 9    // 标记大小使用，无实意
};

NAO_NAMESPACE_END

using DAnnFuncType = nao::D_ANN_FUNC_TYPE;

#endif   // NAO_DANNDEFINE_H
