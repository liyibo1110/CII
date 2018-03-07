#ifndef STACK_H_
#define STACK_H_

#include <stdbool.h>

struct Stack_T {
    int count;
    struct elem{
        void *value;
        struct elem *link;
    } *head;
};

typedef struct Stack_T *Stack;
Stack stackNew(void);
bool stackEmpty(Stack stk);
void stackPush(Stack stk, void *value);
void *stackPop(Stack stk);
void stackFree(Stack stk);
#endif