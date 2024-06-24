/**
 * @FilePath     : /cola/src/Dag/DagDaemon/DTemplateDaemon.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 18:09:45
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 18:22:35
 **/

#ifndef NAO_DTEMPLATEDAEMON_H
#define NAO_DTEMPLATEDAEMON_H

#include "DDaemon.h"

NAO_NAMESPACE_BEGIN

template<class ...Args>
class DTemplateDaemon : public DDaemon {
};

NAO_NAMESPACE_END

#endif //NAO_DTEMPLATEDAEMON_H