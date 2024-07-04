/**
 * @FilePath     : /cola/cola/UtilsCtrl/Finally/UFinally.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-28 15:49:08
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-07-04 18:38:30
 **/
#ifndef NAO_UDEFER_H
#define NAO_UDEFER_H

#include "../UtilsObject.h"

NAO_NAMESPACE_BEGIN

template<class F>
struct UFinally : public UtilsObject
{
private:
    F    func_;
    bool enable_;

public:
    UFinally(std::nullptr_t = nullptr)
        : enable_(false)
    {
    }

    UFinally(std::convertible_to<F> auto&& func)
        : func_(std::forward<decltype(func)>(func))
        , enable_(true)
    {
    }

    UFinally(UFinally&& that)
        : func_(std::move(that.func_))
        , enable_(that.enable_)
    {
        that.enable_ = false;
    }

    UFinally& operator=(UFinally&& that)
    {
        if (this != &that) {
            if (enable_) {
                func_();
            }
            func_        = std::move(that.func_);
            enable_      = that.enable_;
            that.enable_ = false;
        }
        return *this;
    }

    void reset()
    {
        if (enable_) {
            func_();
        }
        enable_ = false;
    }

    void release() { enable_ = false; }

    ~UFinally()
    {
        if (enable_) {
            func_();
        }
    }
};

template<class F>
UFinally(F&&) -> UFinally<std::decay_t<F>>;

NAO_NAMESPACE_END

#endif