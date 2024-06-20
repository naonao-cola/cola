﻿/**
 * @FilePath     : /cola/src/Basic/NStdEx.h
 * @Description  :
 * @Author       : naonao
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-04 16:39:49
 * @Copyright    :
**/
#ifndef NAO_STDEX_H
#define NAO_STDEX_H

#include <memory>
#include <type_traits>

NAO_NAMESPACE_BEGIN

// 兼容 std::enable_if_t 的语法
template<bool B, typename T = void>
using c_enable_if_t = typename std::enable_if<B, T>::type;

// 兼容 std::make_unique 的语法
template<typename T, typename... Args>
typename std::unique_ptr<T> c_make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

NAO_NAMESPACE_END

#endif //NAO_STDEX_H