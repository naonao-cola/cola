/**
 * @FilePath     : /cola/src/Dag/DagParam/DParamUtils.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 11:55:45
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 14:53:21
 **/
#ifndef NAO_DPARAMUTILS_H
#define NAO_DPARAMUTILS_H

#include "../DagObject.h"

NAO_NAMESPACE_BEGIN

/** 创建参数信息 */
#define NAO_CREATE_DPARAM(Type, key) this->template createDParam<Type>(key);

#define NAO_CREATE_DPARAM_WITH_BACKTRACE(Type, key) this->template createDParam<Type>(key, true);

/** 获取参数信息 */
#define NAO_GET_DPARAM(Type, key) this->template getDParam<Type>(key);

/** 获取参数信息，为空则抛出异常 */
#define NAO_GET_DPARAM_WITH_NO_EMPTY(Type, key) this->template getDParamWithNoEmpty<Type>(key);

/** 删除一个参数 */
#define NAO_DELETE_DPARAM(key) this->removeDParam(key);

/** 获取element内部参数信息 */
#define NAO_GET_EPARAM(Type, key) this->template getEParam<Type>(key);

/** 上参数写锁 */
#define NAO_PARAM_WRITE_CODE_BLOCK(param) nao::NAO_WRITE_LOCK __paramWLock__((param)->_param_shared_lock_);

/** 上参数读锁 */
#define NAO_PARAM_READ_CODE_BLOCK(param) nao::NAO_READ_LOCK __paramRLock__((param)->_param_shared_lock_);

NAO_NAMESPACE_END


#endif   // NAO_DPARAMUTILS_H
