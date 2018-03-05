#include "except.h"
#include <stdio.h>
#include <stdlib.h>

void exceptRaise(const Except_T *e, const char *file, int line){
    Except_Frame *p = exceptStack;
    if(p == NULL){  //整个异常栈到头了，只能关闭退出
        //各种打印
        fprintf(stderr, "Uncaught exception");
        if(e->reason){
            fprintf(stderr, " %s", e->reason);
        }else{
            fprintf(stderr, " at 0x%p", e);
        }
        if(file && line > 0){
            fprintf(stderr, " raised at %s:%d\n", file, line);
        }
        fprintf(stderr, "aborting...\n");
        fflush(stderr);
        abort();
    }
    p->exception = e;
    p->file = file;
    p->line = line;
    //弹栈
    exceptStack = exceptStack->prev;
    longjmp(p->env, exceptRaised);
}