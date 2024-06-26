/**
 * @FilePath     : /cola/src/Dag/DagElement/DAdapter/DSingleton/DSingleton.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-26 13:51:17
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-26 13:51:17
**/
#ifndef NAO_DSINGLETON_H
#define NAO_DSINGLETON_H

#include "../DAdapter.h"

NAO_NAMESPACE_BEGIN

template<typename T>
class DSingleton : public DAdapter {
private:
    explicit DSingleton();

    NStatus init() final;
    NStatus run() final;
    NStatus destroy() final;

    NStatus addElementInfo(const std::set<DElementPtr> &dependElements,
                           const std::string &name, NSize loop) final;

    NStatus addManagers(DParamManagerPtr paramManager,
                        DEventManagerPtr eventManager) final;

    NBool isHold() final;

    NBool isMatch() final;

    NBool isRegistered() const final;

    const std::string& getName() const final;

private:
    static USingleton<T> s_singleton_;                    // 单例
    static std::atomic<NBool> s_is_init_;                 // 标志是否被初始化过

    friend class DPipeline;
};

template<typename T>
using DSingletonPtr = DSingleton<T> *;

NAO_NAMESPACE_END

#include "DSingleton.inl"

#endif //NAO_DSINGLETON_H
