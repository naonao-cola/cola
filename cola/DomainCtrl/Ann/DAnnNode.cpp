/**
 * @FilePath     : /cola/src/DomainCtrl/Ann/DAnnNode.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 22:42:03
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 22:59:43
 **/
#include "DAnnNode.h"

NAO_NAMESPACE_BEGIN

DAnnNode::DAnnNode()
{
    setType(DNodeType::CPU);   // 计算密集型算子

    /** 初始化函数映射关系 */
    ann_func_arr_[static_cast<NUint>(DAnnFuncType::ANN_TRAIN)]      = &DAnnNode::train;
    ann_func_arr_[static_cast<NUint>(DAnnFuncType::ANN_SEARCH)]     = &DAnnNode::search;
    ann_func_arr_[static_cast<NUint>(DAnnFuncType::ANN_INSERT)]     = &DAnnNode::insert;
    ann_func_arr_[static_cast<NUint>(DAnnFuncType::ANN_UPDATE)]     = &DAnnNode::update;
    ann_func_arr_[static_cast<NUint>(DAnnFuncType::ANN_REMOVE)]     = &DAnnNode::remove;
    ann_func_arr_[static_cast<NUint>(DAnnFuncType::ANN_LOAD_MODEL)] = &DAnnNode::loadModel;
    ann_func_arr_[static_cast<NUint>(DAnnFuncType::ANN_SAVE_MODEL)] = &DAnnNode::saveModel;
    ann_func_arr_[static_cast<NUint>(DAnnFuncType::ANN_EDITION)]    = &DAnnNode::edition;
}


NStatus DAnnNode::run()
{
    /**
     * 整体流程思路
     * 1，先准备参数，并且确定走哪个功能函数（必须实现）
     * 2，执行具体功能函数
     * 3，如有参数修改，将最终的参数同步回主流程
     */
    NAO_FUNCTION_BEGIN
    const DAnnFuncType& funcType = prepareParam();
    if (unlikely(funcType <= DAnnFuncType::ANN_PREPARE_ERROR || funcType >= DAnnFuncType::ANN_MAX_SIZE)) {
        NAO_RETURN_ERROR_STATUS("error ann function type")
    }

    status = (this->*ann_func_arr_[static_cast<NUint>(funcType)])();
    NAO_FUNCTION_CHECK_STATUS

    status = refreshParam();
    NAO_FUNCTION_END
}


NStatus DAnnNode::train(){NAO_NO_SUPPORT}


NStatus DAnnNode::search(){NAO_NO_SUPPORT}


NStatus DAnnNode::insert(){NAO_NO_SUPPORT}


NStatus DAnnNode::update(){NAO_NO_SUPPORT}


NStatus DAnnNode::remove(){NAO_NO_SUPPORT}


NStatus DAnnNode::loadModel(){NAO_NO_SUPPORT}


NStatus DAnnNode::saveModel(){NAO_NO_SUPPORT}


NStatus DAnnNode::edition(){NAO_NO_SUPPORT}


NStatus DAnnNode::refreshParam(){NAO_NO_SUPPORT}

NAO_NAMESPACE_END
