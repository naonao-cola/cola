/**
 * @FilePath     : /cola/src/Basic/Descinfo.h
 * @Description  :
 * @Author       : naonao
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-04 16:45:09
 * @Copyright    :
**/
#ifndef NAO_DESCINFO_H
#define NAO_DESCINFO_H

#include <string>
#include "NBasicDefine.h"
NAO_NAMESPACE_BEGIN

class NDescInfo {
public:
    /**
     * 获取名称信息
     * @return
     */
    virtual const std::string& getName() const {
        return name_.empty() ? session_ : name_;
    }

    /**
     * 获取唯一id信息
     * @return
     */
    const std::string& getSession() const {
        return session_;
    }

    /**
     * 获取描述信息
     * @return
     */
    const std::string& getDescription() const {
        return description_;
    }

    /**
     * 设置名称信息
     * @param name
     * @return
     */
    virtual auto setName(const std::string& name)
    -> decltype(this) {
        name_ = name;
        return this;
    }

    /**
     * 设置描述信息
     * @param description
     * @return
     */
    virtual auto setDescription(const std::string& description)
    -> decltype(this) {
        description_ = description;
        return this;
    }

    virtual ~NDescInfo() = default;

protected:
    std::string name_;                               // 名字
    std::string session_;                            // 唯一id信息
    std::string description_;                        // 描述信息
};

NAO_NAMESPACE_END

#endif //NAO_DESCINFO_H
