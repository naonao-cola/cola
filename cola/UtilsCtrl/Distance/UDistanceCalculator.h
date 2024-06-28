/**
 * @FilePath     : /cola/src/UtilsCtrl/Distance/UDistanceCalculator.h
 * @Description  : 用于距离计算的类，支持自定义扩展
 * @Author       : naonao
 * @Date         : 2024-06-21 10:54:21
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-21 11:54:09
**/
#ifndef NAO_UDISTANCECALCULATOR_H
#define NAO_UDISTANCECALCULATOR_H

#include <vector>

#include "UDistanceObject.h"
#include "UDistance.h"

NAO_NAMESPACE_BEGIN

template<typename TSrc, typename TRes, typename UDist,
        const NBool needCheck = false,
        c_enable_if_t<std::is_base_of<UDistance<TSrc, TRes>, UDist>::value, int> = 0>
class UDistanceCalculator : public UDistanceObject {
public:
    /**
     * 计算距离信息
     * @param v1
     * @param v2
     * @param dim1
     * @param dim2
     * @param result
     * @param ext
     * @return
     */
    NStatus calculate(const TSrc* v1, const TSrc* v2,
                      NSize dim1, NSize dim2,
                      TRes& result,
                      NVoidPtr ext = nullptr) {
        NAO_FUNCTION_BEGIN
        status = needCheck ? distance_.check(v1, v2, dim1, dim2, ext) : NStatus();
        NAO_FUNCTION_CHECK_STATUS

        status = distance_.calc(v1, v2, dim1, dim2, result, ext);
        NAO_FUNCTION_END
    }

    /**
     * 计算距离信息
     * @param v1
     * @param v2
     * @param result
     * @param ext
     * @return
     */
    NStatus calculate(const std::vector<TSrc>& v1,
                      const std::vector<TSrc>& v2,
                      TRes& result,
                      NVoidPtr ext = nullptr) {
        return calculate(v1.data(), v2.data(), v1.size(), v2.size(), result, ext);
    }

    /**
     * 批量计算距离信息
     * @param query
     * @param nodes
     * @param results
     * @param ext
     * @return
     */
    NStatus calculate(const std::vector<TSrc>& query,
                      const std::vector<std::vector<TSrc> >& nodes,
                      std::vector<TRes>& results,
                      NVoidPtr ext = nullptr) {
        NAO_FUNCTION_BEGIN
        results.clear();
        results.reserve(nodes.size());    // 预分配空间信息
        for (const std::vector<TSrc>& node : nodes) {
            TRes result = 0;    // 在这里初始化一下，防止calculate()实现过程中，不复位result的情况
            status += calculate(query, node, result, ext);
            results.emplace_back(result);
        }
        NAO_FUNCTION_END
    }

    /**
     * 将数据归一化
     * @param v
     * @param dim
     * @param ext
     * @return
     */
    NStatus normalize(TSrc* v, NSize dim, NVoidPtr ext = nullptr) {
        NAO_FUNCTION_BEGIN
        // 这里强行将一个向量copy一份传入判断，目的是为了是的 normalize 和 calculate 的判断逻辑，保持一致
        status = needCheck ? distance_.check(v, v, dim, dim, ext) : CStatus();
        NAO_FUNCTION_CHECK_STATUS

        status = distance_.normalize(v, dim, ext);
        NAO_FUNCTION_END
    }

    /**
     * 数据归一化
     * @param v
     * @param ext
     * @return
     */
    NStatus normalize(std::vector<TSrc>& v, NVoidPtr ext = nullptr) {
        return normalize(v.data(), v.size(), ext);
    }


private:
    UDist distance_;        // 具体的距离计算类
};

NAO_NAMESPACE_END

#endif //NAO_UDISTANCECALCULATOR_H
