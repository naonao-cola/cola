/**
 * @FilePath     : /cola/src/Dag/DagElement/DAdapter/DAdapter.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-26 13:48:15
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 09:45:00
 **/
#ifndef NAO_DADAPTER_H
#define NAO_DADAPTER_H

#include "../DElement.h"

NAO_NAMESPACE_BEGIN

class DAdapter : public DElement
{
protected:
    explicit DAdapter() { element_type_ = DElementType::ADAPTER; }

    friend class DPipeline;
};

using DAdapterPtr = DAdapter*;

NAO_NAMESPACE_END

#endif   // CGRAPH_GADAPTER_H