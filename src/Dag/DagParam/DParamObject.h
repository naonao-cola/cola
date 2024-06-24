/**
 * @FilePath     : /cola/src/Dag/DagParam/DParamObject.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 11:45:59
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 11:49:13
**/
#ifndef NAO_DPARAMOBJECT_H
#define NAO_DPARAMOBJECT_H

#include "../DagObject.h"
#include  "../../UtilsCtrl/UtilsInclude.h"

NAO_NAMESPACE_BEGIN

class DParamObject : public DagObject {
protected:
    /**
     * GParam相关内容，无法通过run()方法执行
     * @return
     */
    NStatus run() final {
        NAO_NO_SUPPORT
    }
};

NAO_NAMESPACE_END

#endif