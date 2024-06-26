/**
 * @FilePath     : /cola/src/Dag/DagAspect/DAspect.h
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-24 19:47:52
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-24 19:47:52
 **/
#ifndef NAO_DASPECT_H
#define NAO_DASPECT_H

#include <vector>

#include "DAspectObject.h"

NAO_NAMESPACE_BEGIN

class DAspect : public DAspectObject
{
public:
    /**
     * init()函数切面开始期间动作
     */
    virtual NStatus beginInit();

    /**
     * init()函数切面结束期间动作
     * @param curStatus
     */
    virtual NVoid finishInit(const NStatus& curStatus);

    /**
     * run()函数切面开始期间动作
     */
    virtual NStatus beginRun();

    /**
     * run()函数切面结束期间动作
     * @param curStatus
     */
    virtual NVoid finishRun(const NStatus& curStatus);

    /**
     * destroy()函数切面开始期间动作
     */
    virtual NStatus beginDestroy();

    /**
     * destroy()函数切面结束期间动作
     * @param curStatus
     */
    virtual NVoid finishDestroy(const NStatus& curStatus);

    /**
     * 进入crash的逻辑
     * @return
     */
    virtual NVoid enterCrashed();
};

using DAspectPtr    = DAspect*;
using DAspectPtrArr = std::vector<DAspectPtr>;

NAO_NAMESPACE_END

#endif   // NAO_DASPECT_H
