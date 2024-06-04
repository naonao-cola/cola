/**
 * @FilePath     : /cola/src/Basic/Exception.h
 * @Description  :
 * @Author       : naonao 1319144981@qq.com
 * @Version      : 0.0.1
 * @LastEditors  : naonao 1319144981@qq.com
 * @LastEditTime : 2024-06-04 15:55:49
 * @Copyright    : G AUTOMOBILE RESEARCH INSTITUTE CO.,LTD Copyright (c) 2024.
**/
#ifndef NAO_EXCEPTION_H
#define NAO_EXCEPTION_H

#include <string>
#include <exception>
#include "StrDefine.h"

NAO_NAMESPACE_BEGIN
NAO_INTERNAL_NAMESPACE_BEGIN

class NEXCEPTION : public std::exception {
public:
    explicit NEXCEPTION(const std::string& info,
                        const std::string& locate = CGRAPH_EMPTY) {
        /**
         * 这里的设计，和CStatus有一个联动
         * 如果不了解具体情况，不建议做任何修改
         */
        exception_info_ = locate + " | " + info;
    }

    /**
     * 获取异常信息
     * @return
     */
    const char* what() const noexcept override {
        return exception_info_.c_str();
    }

private:
    std::string exception_info_;            // 异常状态信息
};

NAO_INTERNAL_NAMESPACE_END
NAO_NAMESPACE_END

#endif //NAO_EXCEPTION_H
