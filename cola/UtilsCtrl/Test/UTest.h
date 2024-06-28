/**
 * @FilePath     : /cola/cola/UtilsCtrl/Test/UTest.h
 * @Description  : 单元测试
 * @Author       : naonao
 * @Date         : 2024-06-28 16:55:33
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-28 18:16:40
 **/
#ifndef NAO_UTEST_H
#define NAO_UTEST_H

#include <cmath>
#include <iostream>
#include <stdlib.h>
#include <utility>
#include <vector>

#include "../UtilsObject.h"

NAO_NAMESPACE_BEGIN

// 测试失败案例的个数
extern NInt unit_test_failures;
/*
函数指针 返回值类型 ( * 指针变量名) ([形参列表]);
参考链接：https://blog.csdn.net/afei__/article/details/80549202
*/
// 函数指针
typedef NVoid (*TestFunc)();
// 函数测试结构体
struct Test : public NStruct
{
    Test(std::string testgroup, std::string testname, TestFunc test)
        : group(std::move(testgroup))
        , name(std::move(testname))
        , function(test){};
    std::string group, name;   // 类名，函数名
    TestFunc    function;      // 测试的函数
};

struct AddTest : public NStruct
{
    // 增加测试函数
    AddTest(Test* test);
};

#define TEST_CHECK_ASSERT(x)                                   \
    ((x) ? ((NVoid)0)                                          \
         : ((std::cout << __FILE__ << ":" << __LINE__ << ":\n" \
                       << "FAILED: " << #x << "\n"),           \
            (NVoid)++unit_test_failures))

#define TEST_CHECK_EQUAL(x, y)                                                             \
    (((x) == (y)) ? ((NVoid)0)                                                             \
                  : ((std::cout << __FILE__ << ":" << __LINE__ << ":\n"                    \
                                << "FAILED: " << #x << " == " << #y << "\n"                \
                                << "\tvalues were '" << (x) << "' and '" << (y) << "'\n"), \
                     (NVoid)++unit_test_failures))

#define TEST_CHECK_NE(x, y)                                                                \
    (((x) != (y)) ? ((NVoid)0)                                                             \
                  : ((std::cout << __FILE__ << ":" << __LINE__ << ":\n"                    \
                                << "FAILED: " << #x << " != " << #y << "\n"                \
                                << "\tvalues were '" << (x) << "' and '" << (y) << "'\n"), \
                     (NVoid)++unit_test_failures))

#define TEST_CHECK_LT(x, y)                                                               \
    (((x) < (y)) ? ((NVoid)0)                                                             \
                 : ((std::cout << __FILE__ << ":" << __LINE__ << ":\n"                    \
                               << "FAILED: " << #x << " < " << #y << "\n"                 \
                               << "\tvalues were '" << (x) << "' and '" << (y) << "'\n"), \
                    (NVoid)++unit_test_failures))

#define TEST_CHECK_GT(x, y)                                                               \
    (((x) > (y)) ? ((NVoid)0)                                                             \
                 : ((std::cout << __FILE__ << ":" << __LINE__ << ":\n"                    \
                               << "FAILED: " << #x << " > " << #y << "\n"                 \
                               << "\tvalues were '" << (x) << "' and '" << (y) << "'\n"), \
                    (NVoid)++unit_test_failures))

#define TEST_CHECK_LE(x, y)                                                                \
    (((x) <= (y)) ? ((NVoid)0)                                                             \
                  : ((std::cout << __FILE__ << ":" << __LINE__ << ":\n"                    \
                                << "FAILED: " << #x << " <= " << #y << "\n"                \
                                << "\tvalues were '" << (x) << "' and '" << (y) << "'\n"), \
                     (NVoid)++unit_test_failures))

#define TEST_CHECK_GE(x, y)                                                                \
    (((x) >= (y)) ? ((NVoid)0)                                                             \
                  : ((std::cout << __FILE__ << ":" << __LINE__ << ":\n"                    \
                                << "FAILED: " << #x << " >= " << #y << "\n"                \
                                << "\tvalues were '" << (x) << "' and '" << (y) << "'\n"), \
                     (NVoid)++unit_test_failures))

#define TEST_CHECK_CLOSE(x, y, tol)                                                                                          \
    ((std::abs((x) - (y)) < tol) ? ((NVoid)0)                                                                                \
                                 : ((std::cout << __FILE__ << ":" << __LINE__ << ":\n"                                       \
                                               << "FAILED: abs(" << #x << " - " << #y << ") < " << #tol << "\n"              \
                                               << "\tvalues were '" << (x) << "', '" << (y) << "' and '" << (tol) << "'\n"), \
                                    (NVoid)++unit_test_failures))

#define TEST_CHECK_THOW(S, E)                                           \
    try {                                                               \
        S;                                                              \
        throw "throwanything";                                          \
    }                                                                   \
    catch (E const& ex) {                                               \
    }                                                                   \
    catch (...) {                                                       \
        std::cout << __FILE__ << ":" << __LINE__ << ":\n"               \
                  << "FAILED: " << #E << " is expected to be thrown\n"; \
        ++unit_test_failures;                                           \
    }

#define TEST_CHECK_NO_THOW(S)                                        \
    try {                                                            \
        S;                                                           \
    }                                                                \
    catch (...) {                                                    \
        std::cout << __FILE__ << ":" << __LINE__ << ":\n"            \
                  << "FAILED: exception thrown from " << #S << "\n"; \
        ++unit_test_failures;                                        \
    }

#define ADD_TEST(group, name)                                                              \
    static NVoid test_##group##_##name();                                                  \
    AddTest      addtest_##group##_##name(new Test(#group, #name, test_##group##_##name)); \
    static NVoid test_##group##_##name()


#define TEST_APP(app)                                                                                        \
    std::vector<Test*>& GetUnitTests()                                                                       \
    {                                                                                                        \
        static std::vector<Test*> unit_tests;                                                                \
        return unit_tests;                                                                                   \
    }                                                                                                        \
    AddTest::AddTest(Test* test)                                                                             \
    {                                                                                                        \
        GetUnitTests().push_back(test);                                                                      \
    };                                                                                                       \
    NInt main(NInt, char**)                                                                                  \
    {                                                                                                        \
        std::cout << "\n" << #app << "\n\n";                                                                 \
        for (size_t i = 0; i < GetUnitTests().size(); ++i) {                                                 \
            NInt _tmp = unit_test_failures;                                                                  \
            GetUnitTests()[i]->function();                                                                   \
            std::cout << "Test [" << GetUnitTests()[i]->group << "] [" << GetUnitTests()[i]->name << "] - "; \
            std::cout << (_tmp == unit_test_failures ? "PASSED" : "FAILED") << "\n";                         \
        }                                                                                                    \
        std::cout << "\n" << unit_test_failures << " tests failed\n\n";                                      \
        return 0;                                                                                            \
    }

NAO_NAMESPACE_END

#endif