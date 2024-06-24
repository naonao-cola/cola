/**
 * @FilePath     : /cola/src/Dag/DagParam/DParamManger.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 11:51:24
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 12:24:27
 **/

#ifndef NAO_DPARAMMANGER_H
#define NAO_DPARAMMANGER_H

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "../DagManager.h"
#include "../DagObject.h"
#include "DParam.h"


NAO_NAMESPACE_BEGIN
class DParamManager : public DParamObject, public DagManager<DParam>
{
public:
    /**
     * 创建一个特定类型的参数
     * @tparam T
     * @param key
     * @param backtrace 是否开启记录调用链路功能
     * @return
     */
    template<typename T, c_enable_if_t<std::is_base_of<DParam, T>::value, int> = 0>
    NStatus create(const std::string& key, NBool backtrace = false);

    /**
     * 获取一个特定类型的参数
     * @tparam T
     * @param key
     * @return
     */
    template<typename T, c_enable_if_t<std::is_base_of<DParam, T>::value, int> = 0>
    T* get(const std::string& key);

    /**
     * 删除特定的param信息
     * @param key
     * @return
     */
    NStatus removeByKey(const std::string& key);

    /**
     * 获取所有的 DParam keys 信息
     * @param keys
     * @return
     */
    std::vector<std::string> getKeys();

protected:
    explicit DParamManager();
    ~DParamManager() override;
    NStatus init() final;
    NStatus destroy() final;

    NStatus clear() final;

    /**
     * 初始化所有的参数信息
     * @return
     */
    NStatus setup();

    /**
     * 重置所有的GParam信息
     * @param curStatus
     * @return
     */
    NVoid resetWithStatus(const NStatus& curStatus);

    NAO_NO_ALLOWED_COPY(DParamManager)


};
NAO_NAMESPACE_END
#endif   // NAO_DPARAMMANGER_H