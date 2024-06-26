/**
 * @FilePath     : /cola/src/Dag/DagElement/DGroup/DMutable/DMutable.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-26 15:26:03
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-26 16:15:40
 **/
#include "DMutable.h"

NAO_NAMESPACE_BEGIN

DMutable::DMutable()
{
    element_type_ = DElementType::MUTABLE;
    session_      = URandom<>::generateSession(NAO_STR_MUTABLE);
    manager_      = NAO_SAFE_MALLOC_NOBJECT(DElementManager)
}


DMutable::~DMutable(){NAO_DELETE_PTR(manager_)}


NStatus DMutable::init()
{
    NAO_FUNCTION_BEGIN
    NAO_ASSERT_NOT_NULL(manager_)
    manager_->setThreadPool(thread_pool_);
    status = manager_->initEngine();
    NAO_FUNCTION_CHECK_STATUS

    status = DGroup::init();
    NAO_FUNCTION_END
}


NStatus DMutable::run()
{
    NAO_FUNCTION_BEGIN

    /**
     * 1. 初始化内容
     * 2. 通过外部复写 reshape()，来实现关系设定。其中，通过 --> 设定的，是会自动恢复visible的
     * 3. 通过 manager 执行
     */
    setup();
    status = reshape(group_elements_arr_);
    NAO_FUNCTION_CHECK_STATUS

    status = manager_->process({group_elements_arr_.begin(), group_elements_arr_.end()});
    NAO_FUNCTION_END
}


NStatus DMutable::destroy()
{
    NAO_FUNCTION_BEGIN
    for (auto* element : group_elements_arr_) {
        // 链路中，可能会将部分内容设置为 visible 的信息。这里统一恢复一下
        element->setVisible(true);
    }

    status = DGroup::destroy();
    NAO_FUNCTION_END
}


NBool DMutable::isSerializable() const
{
    // 情况随时可能发生变化，直接设定不可以 serialize 即可
    return false;
}


NVoid DMutable::setup()
{
    for (auto* element : group_elements_arr_) {
        element->run_before_.clear();
        element->dependence_.clear();
        element->setLoop(1);
        element->setVisible(false);
    }
}

NAO_NAMESPACE_END
