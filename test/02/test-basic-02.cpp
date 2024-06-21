/**
 * @FilePath     : /cola/test/02/test-basic-02.cpp
 * @Description  :
 * @Author       : naonao
 * @Date         : 2024-06-21 14:52:37
 * @Version      : 0.0.1
 * @LastEditors  : naonao
 * @LastEditTime : 2024-06-21 14:55:15
**/

#include "../Common/common.h"
using namespace nao;

void test_timer() {
    UTimer timer;
    timer.start(1000, [] {
        NAO_ECHO("Hello, naonao");
    });

    NAO_SLEEP_MILLISECOND(5500)
    timer.stop();
}



int main(){
    printf("----------------- timer -----------------\n");
    test_timer();
    return 0;
}