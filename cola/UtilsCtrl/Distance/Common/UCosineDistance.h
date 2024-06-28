/**
 * @FilePath     : /cola/src/UtilsCtrl/Distance/Common/UCosineDistance.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-21 10:55:05
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-21 12:06:11
**/
#ifndef NAO_UCOSINEDISTANCE_H
#define NAO_UCOSINEDISTANCE_H

#include "../UDistance.h"

NAO_NAMESPACE_BEGIN

template<typename TSrc, typename TRes = TSrc>
class UCosineDistance : public UDistance<TSrc, TRes> {
public:
    NStatus calc(const TSrc* v1, const TSrc* v2, NSize dim1, NSize dim2, TRes& result, NVoidPtr ext) override {
        NAO_FUNCTION_BEGIN

        result = 0;     // 先清空
        TSrc val1 = 0;    // 用于记录vec1的长度
        TSrc val2 = 0;
        for (CSize i = 0; i < dim1; i++) {
            result += (v1[i] * v2[i]);
            val1 += (v1[i] * v1[i]);
            val2 += (v2[i] * v2[i]);
        }

        result = result / (std::sqrt(val1) * std::sqrt(val2));
        NAO_FUNCTION_END
    }
};

NAO_NAMESPACE_END


#endif //NAO_UCOSINEDISTANCE_H
