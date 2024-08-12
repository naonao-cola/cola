/**
 * @FilePath     : /cola/tutorial/05/test_utest.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-28 17:26:52
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-08-12 15:40:16
 **/
#include "../Common/common.h"

using namespace nao;

TEST_APP(StringUnitTests)
ADD_TEST(string, endswith)
{
    TEST_CHECK_EQUAL(nao::UStr::endswith("", ""), true);
    TEST_CHECK_EQUAL(nao::UStr::endswith("", "a"), false);
    TEST_CHECK_EQUAL(nao::UStr::endswith("a", ""), true);
    TEST_CHECK_EQUAL(nao::UStr::endswith("", ".mesh"), false);
    TEST_CHECK_EQUAL(nao::UStr::endswith("help", ".mesh"), false);
    TEST_CHECK_EQUAL(nao::UStr::endswith("help", ".mesh", 0), false);
    TEST_CHECK_EQUAL(nao::UStr::endswith("help", ".mesh", 1), false);
    TEST_CHECK_EQUAL(nao::UStr::endswith("help", ".mesh", 1, 2), false);
    TEST_CHECK_EQUAL(nao::UStr::endswith("help", ".mesh", 1, 1), false);
    TEST_CHECK_EQUAL(nao::UStr::endswith("help", ".mesh", 1, -1), false);
    TEST_CHECK_EQUAL(nao::UStr::endswith("help", ".mesh", -1), false);
    TEST_CHECK_EQUAL(nao::UStr::endswith(".mesh", ".mesh"), true);
    TEST_CHECK_EQUAL(nao::UStr::endswith("a.mesh", ".mesh"), true);
    TEST_CHECK_EQUAL(nao::UStr::endswith("a.", "."), true);
    TEST_CHECK_EQUAL(nao::UStr::endswith("abcdef", "ef"), true);
    TEST_CHECK_EQUAL(nao::UStr::endswith("abcdef", "cdef"), true);
    TEST_CHECK_EQUAL(nao::UStr::endswith("abcdef", "cdef", 2), true);
    TEST_CHECK_EQUAL(nao::UStr::endswith("abcdef", "cdef", 3), false);
    TEST_CHECK_EQUAL(nao::UStr::endswith("abcdef", "cdef", 2, 3), true);
    TEST_CHECK_EQUAL(nao::UStr::endswith("abcdef", "cdef", -10), true);
}

ADD_TEST(string, find)
{
    TEST_CHECK_EQUAL(nao::UStr::find("", ""), 0);
    TEST_CHECK_EQUAL(nao::UStr::find("", "a"), -1);
    TEST_CHECK_EQUAL(nao::UStr::find("a", ""), 0);
    TEST_CHECK_EQUAL(nao::UStr::find("a", "a"), 0);
    TEST_CHECK_EQUAL(nao::UStr::find("abcdef", ""), 0);
    TEST_CHECK_EQUAL(nao::UStr::find("abcdef", "", -1), 5);
    TEST_CHECK_EQUAL(nao::UStr::find("abcdef", "", -2), 4);
    TEST_CHECK_EQUAL(nao::UStr::find("abcdef", "", -5), 1);
    TEST_CHECK_EQUAL(nao::UStr::find("abcdef", "", -6), 0);
    TEST_CHECK_EQUAL(nao::UStr::find("abcdef", "", -7), 0);

    TEST_CHECK_EQUAL(nao::UStr::find("abcdef", "def"), 3);
    TEST_CHECK_EQUAL(nao::UStr::find("abcdef", "def", 3), 3);
    TEST_CHECK_EQUAL(nao::UStr::find("abcdef", "def", 4), -1);
    TEST_CHECK_EQUAL(nao::UStr::find("abcdef", "def", -5), 3);
    TEST_CHECK_EQUAL(nao::UStr::find("abcdef", "def", -1), -1);

    TEST_CHECK_EQUAL(nao::UStr::find("abcabcabc", "bc", -2), 7);
    TEST_CHECK_EQUAL(nao::UStr::find("abcabcabc", "bc", -1), -1);
    TEST_CHECK_EQUAL(nao::UStr::find("abcabcabc", "bc", 0), 1);
    TEST_CHECK_EQUAL(nao::UStr::find("abcabcabc", "bc", 1), 1);
    TEST_CHECK_EQUAL(nao::UStr::find("abcabcabc", "bc", 2), 4);
    TEST_CHECK_EQUAL(nao::UStr::find("abcabcabc", "bc", 4), 4);
    TEST_CHECK_EQUAL(nao::UStr::find("abcabcabc", "bc", 7), 7);
    TEST_CHECK_EQUAL(nao::UStr::find("abcabcabc", "bc", 4, 3), -1);
    TEST_CHECK_EQUAL(nao::UStr::find("abcabcabc", "bc", 4, 4), -1);
    TEST_CHECK_EQUAL(nao::UStr::find("abcabcabc", "bc", 4, 5), -1);
    TEST_CHECK_EQUAL(nao::UStr::find("abcabcabc", "bc", 4, -1), 4);
    TEST_CHECK_EQUAL(nao::UStr::find("abcabcabc", "bc", 4, 6), 4);
}