/**
 * @FilePath     : /cola/cola/UtilsCtrl/Defer/UDefer.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-28 15:49:08
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 16:07:32
 **/
#ifndef NAO_UDEFER_H
#define NAO_UDEFER_H

#include "../UtilsObject.h"

NAO_NAMESPACE_BEGIN

template<typename F>
struct NDefer : public NStruct
{
    NDefer(F&& f)
        : _f(std::forward<F>(f))
    {}
    ~NDefer() { _f(); }
    typename std::remove_reference<F>::type _f;
};

template<typename F>
inline NDefer<F> create_defer(F&& f)
{
    return NDefer<F>(std::forward<F>(f));
}

#define _nao_defer_name_cat(x, n) x##n
#define _nao_defer_make_name(x, n) _nao_defer_name_cat(x, n)
#define _nao_defer_name _nao_defer_make_name(_nao_defer_, __LINE__)
// Defer 功能
#define DEFER(e) auto _nao_defer_name = nao::create_defer([&]() { e; })

NAO_NAMESPACE_END

#endif