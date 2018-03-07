#include "assert.h"
#include "mem.h"
#include "stack.h"


/**
 * 构造一个新的Stack 
 */
Stack stackNew(void){
    Stack stk;
    NEW(stk);
    stk->count = 0;
    stk->head = NULL;
    return stk;
}

/**
 * 检测Stack是否为空栈
 */ 
bool stackEmpty(Stack stk){
    assert(stk);
    return stk->count == 0;
}

/**
 * 压栈
 */ 
void stackPush(Stack stk, void *value){
    assert(stk);
    struct elem *t;
    NEW(t);
    t->value = value;
    t->link = stk->head;
    stk->head = t;
    stk->count++;
}

/**
 * 出栈
 */ 
void *stackPop(Stack stk){
    assert(stk);
    assert(stk->count > 0);
    struct elem *t = stk->head;
    stk->head = t->link;
    stk->count--;
    void *value = t->value;
    FREE(t);
    return value;
}

/**
 * 回收
 */ 
void stackFree(Stack stk){
    assert(stk);
    //先回收链表
    struct elem *t, *u;
    for(t = stk->head ; t; t = u){
        u = t->link;    //将下一个缓存，不然t就没了
        FREE(t);
    }
    FREE(stk);
}