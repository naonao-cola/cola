/**
 * @FilePath     : /cola/cola/UtilsCtrl/Container/USmallVector.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-08-12 14:19:54
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-09-05 10:50:49
 * @Copyright (c) 2024 by G, All Rights Reserved.
 **/
#ifndef NAO_USMALLVECTOR_H
#define NAO_USMALLVECTOR_H

#include <cstring>
#include <memory>
#include <vector>


#include "UtilsCtrl/UtilsObject.h"

NAO_NAMESPACE_BEGIN

template<class T, NSize CAPACITY = 8>
class USmallVector : public UtilsObject
{
public:
    explicit USmallVector()
    {
        capacity_  = 0;
        cur_index_ = 0;
    }

    ~USmallVector() override{NAO_DELETE_PTR_ARRAY(data_)}

    /**
     * 插入具体内容
     * @param val
     * @param isUnique
     * @return
     */
    NVoid insert(const T& val, NBool isUnique = true)
    {
        if (isUnique && hasValue(val)) {
            return;
        }

        if (cur_index_ >= capacity_) {
            // 插入超限的时候，开始扩容
            NSize curCapacity = capacity_ + CAPACITY;
            T*    curData     = new T[curCapacity];
            NAO_ASSERT_NOT_NULL_THROW_ERROR(curData)

            std::move(data_, data_ + capacity_, curData);
            NAO_DELETE_PTR_ARRAY(data_)
            data_     = curData;
            capacity_ = curCapacity;
        }
        data_[cur_index_++] = val;
    }

    NSize size() const { return cur_index_; }

    NBool empty() const { return 0 == cur_index_; }

    NVoid clear()
    {
        // 这里只清空内容，不回收内存信息
        cur_index_ = 0;
        memset(data_, 0, sizeof(T) * capacity_);
    }

    /**
     * 判断内部是否包含该值
     * @param val
     * @return
     */
    NBool hasValue(const T& val) const
    {
        NBool result = false;
        if (!data_) {
            return result;
        }
        for (NSize i = 0; i < cur_index_; i++) {
            if (val == data_[i]) {
                result = true;
                break;
            }
        }

        return result;
    }

    /**
     * 给vector赋值
     * @return
     */
    std::vector<T> asVector() const
    {
        std::vector<T> vec;
        if (!data_) {
            return vec;
        }
        for (const auto& x : *this) {
            vec.emplace_back(x);
        }
        return vec;
    }

protected:
    class UIter
    {
    public:
        explicit UIter(T* ptr)
            : ptr_(ptr)
        {
        }
        T&     operator*() const { return *ptr_; }
        T*     operator->() { return ptr_; }
        UIter& operator++()
        {
            ++ptr_;
            return *this;
        }
        UIter operator++(int)
        {
            UIter iter = *this;
            ++(*this);
            return iter;
        }
        NBool operator==(const UIter& iter) const { return ptr_ == iter.ptr_; }
        NBool operator!=(const UIter& iter) const { return ptr_ != iter.ptr_; }


    private:
        T* ptr_{nullptr};
    };

public:
    UIter begin() const { return UIter(data_); }
    UIter end() const { return UIter(data_ + cur_index_); }
    T     front() const { data_[0]; }
    T     back() const { data_[cur_index_ - 1]; }

private:
    T*    data_      = nullptr;   // 存放具体数据
    NSize cur_index_ = 0;         // 当前元素数量
    NSize capacity_  = 0;         // 数组容量
};

NAO_NAMESPACE_END

#endif   // NAO_USMALLVECTOR_H
