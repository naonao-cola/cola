/**
 * @FilePath     : /cola/src/UtilsCtrl/Distance/UDistance.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-21 10:53:31
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-21 10:53:31
**/
#ifndef NAO_UDISTANCE_H
#define NAO_UDISTANCE_H

#include "UDistanceObject.h"

NAO_NAMESPACE_BEGIN

/** 传入的类型和计算结果的类型，可能不同。一般默认相同 */
template<typename TSrc, typename TRes = TSrc>
class UDistance : public UDistanceObject {
public:
    /**
     * 计算距离信息
     * @param v1 向量1
     * @param v2 向量2
     * @param dim1 向量1的维度
     * @param dim2 向量2的维度
     * @param result 结果信息
     * @param ext 可扩展信息
     * @return
     */
    virtual NStatus calc(const TSrc* v1, const TSrc* v2, NSize dim1, NSize dim2, TRes& result, NVoidPtr ext) = 0;

    /**
     * 判断入参信息是否符合
     * @param v1
     * @param v2
     * @param dim1
     * @param dim2
     * @param ext
     * @return
     */
    virtual NStatus check(const TSrc* v1, const TSrc* v2, NSize dim1, NSize dim2, NVoidPtr ext);

    /**
     * 将数据归一化
     * @param v
     * @param dim
     * @param ext
     * @return
     */
    virtual NStatus normalize(TSrc* v, NSize dim, NVoidPtr ext);
};

NAO_NAMESPACE_END

#include "UDistance.inl"

#endif //NAO_UDISTANCE_H
