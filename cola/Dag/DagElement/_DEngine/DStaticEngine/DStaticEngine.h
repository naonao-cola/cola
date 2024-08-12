/**
 * @FilePath     : /cola/cola/Dag/DagElement/_DEngine/DStaticEngine/DStaticEngine.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-08-12 13:47:20
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-08-12 13:47:20
 * @Copyright (c) 2024 by G, All Rights Reserved.
**/
#ifndef NAO_DSTATICENGINE_H
#define NAO_DSTATICENGINE_H

#include "../DEngine.h"

NAO_NAMESPACE_BEGIN

class DStaticEngine : public DEngine {
protected:
    explicit DStaticEngine() = default;

    NStatus setup(const DSortedDElementPtrSet& elements) override;

    NStatus run() override;

private:
    DElementPtrMat2D element_mat_ {};        // 解图后的elements信息

    friend class UAllocator;
    friend class GElementManager;
};

NAO_NAMESPACE_END

#endif //NAO_GSTATICENGINE_H