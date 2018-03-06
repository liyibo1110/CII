#include "assert.h"

//实际定义断言异常的地方
const Except_T Assert_Failed = {"Assertion failed"};

//避免和宏重名
void (assert)(int e){
    assert(e);
}