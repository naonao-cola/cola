/**
 * @FilePath     : /cola/src/Dag/DagManager.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 11:08:42
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 11:13:07
**/
#ifndef NAO_DAGMANAGER_H
#define NAO_DAGMANAGER_H


#include <string>

#include "../NBasic/NBasicInclude.h"
#include "DagObject.h"

NAO_NAMESPACE_BEGIN

template<typename T,
        c_enable_if_t<std::is_base_of<GraphObject, T>::value, int> = 0>
class GraphManager : public NObject {
protected:
    /**
     * 添加一个object
     * @param object
     * @return
     */
    virtual NStatus add(T* object) {
        NAO_NO_SUPPORT
    }

    /**
     * 删除一个object
     * @param object
     * @return
     */
    virtual NStatus remove(T* object) {
        NAO_NO_SUPPORT
    }

    /**
     * 查找一个object
     * @param object
     * @return
     */
    virtual NBool find(T* object) const {
        return false;
    }

    /**
     * 创建一个object
     * @param key
     * @return
     */
    virtual NStatus create(const std::string& key) {
        NAO_NO_SUPPORT
    }

    /**
     * 获取一个object
     * @param key
     * @return
     */
    virtual T* get(const std::string& key) {
        return nullptr;
    }

    /**
     * 重置所有object
     * @return
     */
    virtual NStatus reset() {
        NAO_NO_SUPPORT
    }

    /**
     * 获取大小信息
     * @return
     */
    virtual NSize getSize() const {
        return 0;
    }

    /**
     * 清空所有object，必须实现的内容
     * @return
     */
    virtual NStatus clear() = 0;

    /**
     * 执行函数，默认为执行失败
     * @return
     */
    NStatus run() override {
        NAO_NO_SUPPORT
    }
};

NAO_NAMESPACE_END

#endif //NAO_DAGMANAGER_H