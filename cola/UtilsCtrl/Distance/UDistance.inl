/**
 * @FilePath     : /cola/src/UtilsCtrl/Distance/UDistance.inl
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-21 10:54:03
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-21 10:54:03
 */
#ifndef NAO_UDISTANCE_INL
#define NAO_UDISTANCE_INL

#include <cmath>

#include "UDistance.h"

NAO_NAMESPACE_BEGIN

template<typename TSrc, typename TRes>
NStatus UDistance<TSrc, TRes>::check(const TSrc* v1, const TSrc* v2, NSize dim1, NSize dim2, NVoidPtr ext) {
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_NOT_NULL(v1)
    NAO_ASSERT_NOT_NULL(v2)
    if (0 == dim1 * dim2) {
        // 理论上不应该传入 dim=0 向量
        NAO_RETURN_ERROR_STATUS("input dim error")
    }

    NAO_FUNCTION_END
}


template<typename TSrc, typename TRes>
NStatus UDistance<TSrc, TRes>::normalize(TSrc* v, NSize dim, NVoidPtr ext) {
    NAO_FUNCTION_BEGIN

    /** 这里不需要判定v为空的情况，需要的话可以通过开启 needCheck 逻辑来判断 */
    TSrc val = 0;
    for (NSize i = 0; i < dim; i++) {
        val += (v[i] * v[i]);
    }

    const TSrc& denominator = 1 / std::sqrt(val);    // 分母信息
    for (NSize i = 0; i < dim; i++) {
        v[i] = v[i] * denominator;
    }

    NAO_FUNCTION_END
}

NAO_NAMESPACE_END

#endif //NAO_UDISTANCE_INL