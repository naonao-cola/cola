﻿/**
 * @FilePath     : /cola/cola/NBasic/NStatus.h
 * @Description  :
 * @Author       : naonao
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-09-05 10:19:47
 * @Copyright    :
 **/
#ifndef NAO_STATUS_H
#define NAO_STATUS_H

#include "NBasicDefine.h"
#include "NStrDefine.h"
#include <string>


NAO_NAMESPACE_BEGIN
NAO_INTERNAL_NAMESPACE_BEGIN

/**
 * 说明：
 * 返回值为0，表示正常逻辑
 * 返回值为负整数，表示error逻辑，程序终止执行
 * 自定义返回值，请务必遵守以上约定
 */
static const int STATUS_OK    = 0;    /** 正常流程返回值 */
static const int STATUS_ERR   = -1;   /** 异常流程返回值 */
static const int STATUS_CRASH = -996; /** 异常流程返回值 */

class NSTATUS
{
public:
    explicit NSTATUS() = default;

    explicit NSTATUS(const std::string& errorInfo)
    {
        this->error_code_ = STATUS_ERR;   // 默认的error code信息
        this->error_info_ = errorInfo;
    }

    explicit NSTATUS(int errorCode, const std::string& errorInfo)
    {
        this->error_code_ = errorCode;
        this->error_info_ = errorInfo;
    }

    NSTATUS(const NSTATUS& status)
    {
        if (status.error_code_ == error_code_) {
            return;
        }
        this->error_code_ = status.error_code_;
        this->error_info_ = status.error_info_;
    }

    NSTATUS(const NSTATUS&& status) noexcept
    {
        if (status.error_code_ == error_code_) {
            return;
        }

        this->error_code_ = status.error_code_;
        this->error_info_ = status.error_info_;
    }

    NSTATUS& operator=(const NSTATUS& status)
    {
        if (this->error_code_ != status.error_code_) {
            // 如果status是正常的话，则所有数据保持不变
            this->error_code_ = status.error_code_;
            this->error_info_ = status.error_info_;
        }
        return (*this);
    }

    NSTATUS& operator+=(const NSTATUS& cur)
    {
        /**
         * 如果当前状态已经异常，则不做改动
         * 如果当前状态正常，并且传入的状态是异常的话，则返回异常
         */
        if (!this->isErr() && cur.isErr()) {
            this->error_code_ = cur.error_code_;
            this->error_info_ = cur.error_info_;
        }
        return (*this);
    }

    /**
     * 恢复状态信息
     */
    void reset()
    {
        if (this->error_code_ != STATUS_OK) {
            this->error_code_ = STATUS_OK;
            this->error_info_.clear();
        }
    }

    /**
     * 获取异常值信息
     * @return
     */
    int getCode() const { return this->error_code_; }

    /**
     * 获取异常信息
     * @return
     */
    const std::string& getInfo() const { return this->error_info_; }


    /**
     * 判断当前状态是否可行
     * @return
     */
    bool isOK() const { return STATUS_OK == error_code_; }

    /**
     * 判断当前状态是否可行
     * @return
     */
    bool isErr() const
    {
        return error_code_ < STATUS_OK;   // 约定异常信息，均为负值
    }

    /**
     * 判断当前状态是否是崩溃了
     * @return
     */
    bool isCrash() const { return STATUS_CRASH == error_code_; }

    /**
     * 设置异常信息
     * @param code
     * @param info
     * @return
     */
    NSTATUS* setInfo(int code, const std::string& info)
    {
        error_code_ = code;
        error_info_ = (STATUS_OK == error_code_) ? NAO_EMPTY : info;
        return this;
    }

    /**
     * 设置异常信息
     * @param info
     * @return
     */
    NSTATUS* setErrorInfo(const std::string& info)
    {
        error_code_ = STATUS_ERR;
        error_info_ = info;
        return this;
    }

private:
    int         error_code_ = STATUS_OK;   // 错误码信息
    std::string error_info_;               // 错误信息描述
};

NAO_INTERNAL_NAMESPACE_END
NAO_NAMESPACE_END

#endif   // NAO_STATUS_H
