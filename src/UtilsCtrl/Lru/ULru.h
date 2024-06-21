/**
 * @FilePath     : /cola/src/UtilsCtrl/Lru/ULru.h
 * @Description  :
 * @Author       : naonao
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-20 19:58:15
 **/
#ifndef NAO_ULRU_H
#define NAO_ULRU_H

#include <list>
#include <unordered_map>

#include "../UtilsObject.h"
#include "ULruNode.h"

NAO_NAMESPACE_BEGIN

template<typename K, typename V, NSize DefaultCapacity = 16>
class ULru : public UtilsObject
{
public:
    explicit ULru()
    {
        cur_size_ = 0;
        capacity_ = DefaultCapacity;
    }

    ~ULru() override { clear(); }

    /**
     * 信息插入Lru
     * @param key
     * @param value
     */
    NVoid put(const K& key, const V& value)
    {
        auto cur = cache_.find(key);
        if (cur != cache_.end()) {
            /* 如果在cache中找到，则删除（稍后更新至最前） */
            nodes_.erase(cache_[key]);
            cache_.erase(key);
            cur_size_--;
        }
        else if (cur_size_ >= capacity_) {
            /* 如果超限，则删除最后一个节点信息 */
            auto back = nodes_.back();
            cache_.erase(back.key_);
            nodes_.pop_back();
            cur_size_--;
        }

        nodes_.emplace_front(ULruNode<K, V>(key, value));
        cache_[key] = nodes_.begin();
        cur_size_++;
    }

    /**
     * 从Lru中获取节点信息
     * @param key
     * @param value
     * @return
     */
    NBool get(const K& key, V& value)
    {
        NBool result = false;
        auto  cur    = cache_.find(key);
        if (cur != cache_.end()) {
            value = cur->second->value_;
            put(key, value);
            result = true;   // 表示从缓存中获取到了
        }

        return result;
    }

    /**
     * 获取capacity信息
     * @return
     */
    NSize getCapacity() { return this->capacity_; }

    /**
     * 清空Lru中的信息
     */
    NVoid clear()
    {
        nodes_.clear();
        cache_.clear();
        cur_size_ = 0;
    }


private:
    NSize                                                               cur_size_;   // 当前尺寸
    NSize                                                               capacity_;   // 最大容量
    std::unordered_map<K, typename std::list<ULruNode<K, V>>::iterator> cache_;      // 缓存信息
    std::list<ULruNode<K, V>>                                           nodes_;      // 节点链表
};

NAO_NAMESPACE_END
#endif   // NAO_ULRU_H
