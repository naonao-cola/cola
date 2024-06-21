/**
 * @FilePath     : /cola/src/UtilsCtrl/Lru/ULruNode.h
 * @Description  :
 * @Author       : naonao
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-20 16:28:18
 **/
#ifndef NAO_ULRUNODE_H
#define NAO_ULRUNODE_H

#include "../UtilsObject.h"

NAO_NAMESPACE_BEGIN

template<typename K, typename V>
struct ULruNode : public UtilsObject
{
    K key_;
    V value_;

    ULruNode(const K& key, const V& value)
    {
        this->key_   = key;
        this->value_ = value;
    }
};

NAO_NAMESPACE_END
#endif   // NAO_ULRUNODE_H