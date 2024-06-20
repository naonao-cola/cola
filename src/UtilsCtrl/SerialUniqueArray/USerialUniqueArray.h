/**
 * @FilePath     : /cola/src/UtilsCtrl/SerialUniqueArray/USerialUniqueArray.h
 * @Description  :
 * @Author       : naonao
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-20 19:57:30
 **/
#ifndef NAO_USERIALUNIQUEARRAY_H
#define NAO_USERIALUNIQUEARRAY_H

#include <set>
#include <vector>


#include "../UtilsObject.h"

NAO_NAMESPACE_BEGIN

template<class T> class USerialUniqueArray : public UtilsObject
{
public:
    /**
     * 不重复的插入顺序容器中
     * @param val
     * @return
     */
    NVoid uniqueAdd(const T& val)
    {
        // 如果没有，就插入array中
        if (inner_set_.find(val) == inner_set_.end()) {
            inner_set_.insert(val);
            inner_array_.push_back(val);
        }
    }

    /**
     * 获取不重复的有序array信息
     * @param arr
     * @return
     */
    NVoid getUniqueArray(std::vector<T>& arr)
    {
        for (const auto& iter : inner_array_) {
            arr.push_back(iter);
        }
    }

    /**
     * 清空所有内部信息
     * @return
     */
    NVoid clear()
    {
        inner_set_.clear();
        inner_array_.clear();
    }

    ~USerialUniqueArray() override { clear(); }

private:
    std::set<T>    inner_set_;     // 内部set，留比对使用
    std::vector<T> inner_array_;   // 内部array，最终提供的排序不重复内容
};

NAO_NAMESPACE_END

#endif   // NAO_USERIALUNIQUEARRAY_H
