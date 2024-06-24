/**
 * @FilePath     : /cola/src/DomainCtrl/Ann/DAnnParam.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 22:42:43
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 22:42:44
 **/
#ifndef NAO_DANNPARAM_H
#define NAO_DANNPARAM_H

#include <string>

#include "DAnnDefine.h"
#include "DAnnObject.h"


NAO_NAMESPACE_BEGIN

struct DAnnParam : public DAnnObject, public DParam
{
    NSize       dim_          = 0;       // 维度信息
    NSize       cur_vec_size_ = 0;       // 当前向量个数
    NSize       max_vec_size_ = 0;       // 最大向量个数
    NBool       normalize_    = false;   // 是否归一化处理
    std::string ann_model_path_;         // 模型路径
    std::string train_file_path_;        // 读取待训练数据文件的位置
};

NAO_NAMESPACE_END

#endif   // NAO_DANNPARAM_H