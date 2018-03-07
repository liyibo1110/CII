#include "stack.h"
#include <stdio.h>

int main(void){

    //测试构造
    Stack stk = stackNew();
    printf("new OK\n");

    //测试空检测
    bool isEmpty = stackEmpty(stk);
    printf("is empty? %s\n", isEmpty ? " YES" : "NO");

    //测试压栈
    stackPush(stk, "abc");
    printf("push OK\n");

    isEmpty = stackEmpty(stk);
    printf("is empty? %s\n", isEmpty ? " YES" : "NO");

    //测试出栈
    char *value = (char *)stackPop(stk);
    printf("pop OK\n");
    printf("value is: %s\n" ,value);

    isEmpty = stackEmpty(stk);
    printf("is empty? %s\n", isEmpty ? " YES" : "NO");

    //测试释放
    stackFree(stk);
    printf("free OK\n");
}