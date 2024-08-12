/**
 * @FilePath     : /cola/tutorial/01/test-basic-01.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-21 13:06:49
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-08-12 15:41:06
 **/
#include "../Common/common.h"
using namespace nao;

void test_lru()
{

    ULru<int, std::string, 3> lru;

    lru.put(1, "one");
    lru.put(2, "two");
    lru.put(3, "three");
    lru.put(4, "four");   // 此时，key=1的节点，已经被在缓存中删除了
    lru.put(5, "five");

    std::string val;
    int         key = 5;
    // 返回值的 true的情况下，表示获取成功。否则表示未获取到 value 的值
    if (lru.get(key, val)) {
        NAO_ECHO("key = %d, value is : [%s]", key, val.c_str());
    }
    else {
        NAO_ECHO("[%d] no get value", key);
    }

    key = 6;
    if (lru.get(key, val)) {
        NAO_ECHO("key = %d, value is : [%s]", key, val.c_str());
    }
    else {
        NAO_ECHO("[%d] no get value", key);
    }
}

void test_trie()
{
    UTrie trie;
    trie.insert("hello");
    trie.insert("help");
    trie.insert("cgraph");   // 插入几个单词

    NAO_ECHO("find [hello] result is : [%i]", trie.find("hello"));
    NAO_ECHO("find [cgraph] result is : [%i]", trie.find("cgraph"));

    trie.eraser("hello");   // 删去hello信息
    NAO_ECHO("eraser [hello], then find it, result is : [%i]", trie.find("hello"));

    trie.insert("hello");   // 重新插入hello信息
    NAO_ECHO("insert [hello] again, then find it, result is : [%i]", trie.find("hello"));
}


void test_trie_v2()
{
    UTrieV2 trieV2;
    trieV2.insert("hello");
    trieV2.insert("help");
    trieV2.insert("cgraph");   // 插入几个单词

    NAO_ECHO("find [hello] result is : [%i]", trieV2.find("hello"));
    NAO_ECHO("find [cgraph] result is : [%i]", trieV2.find("cgraph"));

    trieV2.eraser("hello");   // 删去hello信息
    NAO_ECHO("eraser [hello], then find it, result is : [%i]", trieV2.find("hello"));

    trieV2.insert("hello");   // 重新插入hello信息
    NAO_ECHO("insert [hello] again, then find it, result is : [%i]", trieV2.find("hello"));
}


template<typename TSrc, typename TRes>   // 自定义距离类型
class MyDistance : public nao::UDistance<TSrc, TRes>
{
public:
    NStatus calc(const TSrc* v1, const TSrc* v2, NSize dim1, NSize dim2, TRes& result, NVoidPtr ext) override
    {
        /**
         * 如果需要实现自定义距离
         * 只要生成 UDistance<TSrc, TRes> 的子类，并且实现其中的 calc 方法即可
         * normalize 和 check 函数可选择实现
         */
        NStatus status;
        result = 0;
        for (NSize i = 0; i < dim1; i++) {
            /* 自定义一种距离计算方式，距离的计算方式是：所有 向量1的元素乘以2，加上向量2的元素除以2 之和 */
            result += (v1[i] * 2 + v2[i] / 2);
        }

        return status;
    }
};

void test_distance()
{
    /**
     * 支持任意类型信息，如 float，int，double等
     * 并且支持结果类型转换
     */
    using DistSrcType = float;   // 向量值类型
    using DistResType = float;   // 计算结果类型
    const int dim     = 16;

    std::vector<DistSrcType> vec1;
    std::vector<DistSrcType> vec2;

    // 随机生成两个 DistSrcType 类型，dim 维度，且值在[0.0~1.0]的向量
    URandom<DistSrcType>::generate(vec1, dim, 0.0, 1.0);
    URandom<DistSrcType>::generate(vec2, dim, 0.0, 1.0);

    DistResType result = 0.0;
    // 计算欧氏距离
    UDistanceCalculator<DistSrcType, DistResType, UEuclideanDistance<DistSrcType, DistResType>> eucDist;
    eucDist.calculate(vec1, vec2, result);
    std::cout << "UDistanceEuclidean distance result is : " << result << std::endl;

    /**
     * 支持自定义距离计算
     * 自定义距离的计算逻辑是：所有 向量1的元素乘以2，加上 向量2的元素除以2 之和
     * 故 vec1->vec2 的距离和 vec2->vec1的距离，是不同的。
     * 下例中给出对应的展示
     */
    UDistanceCalculator<DistSrcType, DistResType, MyDistance<DistSrcType, DistResType>> myDist;
    myDist.calculate(vec1, vec2, result);
    std::cout << "MyDistance distance vec1 -> vec2 result is : " << result << std::endl;

    myDist.calculate(vec2, vec1, result);
    std::cout << "MyDistance distance vec2 -> vec1 result is : " << result << std::endl;
}


int main()
{
    printf("----------------- lru -----------------\n");
    test_lru();
    printf("----------------- trie -----------------\n");
    test_trie();
    printf("----------------- trie v2 -----------------\n");
    test_trie_v2();
    printf("----------------- distance -----------------\n");
    test_distance();
    return 0;
}