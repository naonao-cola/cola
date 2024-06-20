/**
 * @FilePath     : /cola/src/UtilsCtrl/Random/URandom.h
 * @Description  :
 * @Author       : naonao
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-20 19:57:45
 **/
#ifndef NAO_URANDOM_H
#define NAO_URANDOM_H

#include <random>

#include "../UtilsObject.h"
#include "URandomDefine.h"

NAO_NAMESPACE_BEGIN

/** 当传入的seed为 CGRAPH_REAL_RANDOM 的时候，表示完全随机。其他值为固定随机值 */
template<typename T = NFloat, NInt SEED = NAO_REAL_RANDOM, typename TEngine = NAO_RANDOM_MT19937>
class URandom : public UtilsObject
{
public:
    /**
     * 构建随机一维向量
     * @param data
     * @param dim
     * @param minValue
     * @param maxValue
     * @return
     * @notice 暂时不支持int类型的随机数生成
     */
    static NStatus generate(std::vector<T>& data, NSize dim, const T& minValue, const T& maxValue)
    {
        std::random_device                rd;
        TEngine                           eng(NAO_REAL_RANDOM == SEED ? rd() : SEED);
        std::uniform_real_distribution<T> urd(minValue, maxValue);

        data.clear();
        data.reserve(dim);
        for (NSize i = 0; i < dim; i++) {
            data.emplace_back(urd(eng));
        }

        return NStatus();
    }

    /**
     * 构建随机二维向量
     * @param data
     * @param height
     * @param column
     * @param minValue
     * @param maxValue
     * @return
     */
    static NStatus generate(std::vector<std::vector<T>>& data, NSize height, NSize column,
                            const T& minValue, const T& maxValue)
    {
        std::random_device                rd;
        TEngine                           eng(NAO_REAL_RANDOM == SEED ? rd() : SEED);
        std::uniform_real_distribution<T> urd(minValue, maxValue);

        data.clear();
        data.reserve(height);
        std::vector<T> arr;

        for (NSize i = 0; i < height; i++) {
            arr.reserve(column);
            for (NSize j = 0; j < column; j++) {
                arr.emplace_back(urd(eng));
            }
            data.emplace_back(std::move(arr));
        }

        return NStatus();
    }

    /**
     * 生成随机session信息
     * @param key
     * @param size
     * @return
     */
    static std::string generateSession(const std::string& key = NAO_UNKNOWN, NSize size = 3)
    {
        std::string         session;   // 形式是 a-b-c-key，其中 a表示6位随机数字
        std::vector<NFloat> vec;
        URandom<NFloat>::generate(vec, size, 100000, 999999);
        for (NSize i = 0; i < size; i++) {
            session += std::to_string(int(vec[i]));
            session += '-';
        }
        session += key;
        return session;
    }
};

NAO_NAMESPACE_END

#endif   // NAO_URANDOM_H