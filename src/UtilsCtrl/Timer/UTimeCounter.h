/**
 * @FilePath     : /cola/src/UtilsCtrl/Timer/UTimeCounter.h
 * @Description  :
 * @Author       : naonao
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-20 19:56:57
 **/
#ifndef NAO_UTIMECOUNTER_H
#define NAO_UTIMECOUNTER_H

#include <chrono>
#include <string>


#include "../UtilsObject.h"

NAO_NAMESPACE_BEGIN

class UTimeCounter : public UtilsObject
{
public:
    explicit UTimeCounter()
    {
        key_      = NAO_DEFAULT;
        start_ts_ = std::chrono::steady_clock::now();
    }

    explicit UTimeCounter(const std::string& key)
    {
        start_ts_ = std::chrono::steady_clock::now();
        key_      = key;
    }

    ~UTimeCounter() override
    {
        std::chrono::duration<double, std::milli> span =
            std::chrono::steady_clock::now() - start_ts_;
        nao::NAO_ECHO("[%s]: time counter is : [%0.2lf] ms", key_.c_str(), span.count());
    }

private:
    std::chrono::steady_clock::time_point start_ts_;
    std::string                           key_;
};

NAO_NAMESPACE_END

#endif   // NAO_UTIMECOUNTER_H