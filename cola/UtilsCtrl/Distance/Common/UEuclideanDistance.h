/**
 * @FilePath     : /cola/cola/UtilsCtrl/Distance/Common/UEuclideanDistance.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-21 10:55:12
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-22 14:22:24
**/
#ifndef NAO_UEUCLIDEANDISTANCE_H
#define NAO_UEUCLIDEANDISTANCE_H

#include <cmath>

#include "../UDistance.h"

NAO_NAMESPACE_BEGIN

template<typename TSrc, typename TRes = TSrc, const NBool needSqrt = true>    // 默认给出的值，是开平方后的值
class UEuclideanDistance : public UDistance<TSrc, TRes> {
public:
    NStatus calc(const TSrc* v1, const TSrc* v2, NSize dim1, NSize dim2, TRes& result, NVoidPtr ext) override {
        NAO_FUNCTION_BEGIN
        result = 0;
        for (NSize i = 0; i < dim1; i++) {
            result += std::pow(v1[i] - v2[i], 2);
        }

        result = needSqrt ? std::sqrt(result) : result;
        NAO_FUNCTION_END
    }


    NStatus check(const TSrc* v1, const TSrc* v2, NSize dim1, NSize dim2, NVoidPtr ext) override {
        NAO_FUNCTION_BEGIN
        NAO_ASSERT_NOT_NULL(v1)
        NAO_ASSERT_NOT_NULL(v2)
        if (dim1 != dim2 || 0 == dim1 * dim2) {
            // 不相等，或者有任何一个值为0，则认为是异常
            NAO_RETURN_ERROR_STATUS("euclidean distance dim error")
        }

        NAO_FUNCTION_END
    }
};

NAO_NAMESPACE_END

#endif //NAO_UEUCLIDEANDISTANCE_H
