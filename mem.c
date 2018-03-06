#include "mem.h"
#include <stdlib.h>
#include <stddef.h>
#include "assert.h"
#include "except.h"

//实际定义全局异常
const Except_T Mem_failed = {"Allocation Failed"};

void *memAlloc(long nbytes, const char *file, int line){
    void *ptr;
    assert(nbytes > 0);
    ptr = malloc(nbytes);
    if(ptr == NULL){
        if(file == NULL){   //如果是直接被调用
            RAISE(Mem_failed);
        }else{
            exceptRaise(&Mem_failed, file, line);
        }   
    }
    return ptr;
}

void *memCalloc(long count, long nbytes, const char *file, int line){
    void *ptr;
    assert(count > 0);
    assert(nbytes > 0);
    ptr = calloc(count, nbytes);
    if(ptr == NULL){
        if(file == NULL){   //如果是直接被调用
            RAISE(Mem_failed);
        }else{
            exceptRaise(&Mem_failed, file, line);
        }  
    }
    return ptr;
}

void memFree(void *ptr, const char *file, int line){
    if(ptr){    //只处理传入的ptr为NULL的情况
        free(ptr);
    }
}

void *memResize(void *ptr, long nbytes, const char *file, int line){
    assert(ptr);
    assert(nbytes > 0);
    ptr = realloc(ptr, nbytes); //并没有处理返回的是否原来的ptr的问题
    if(ptr == NULL){
        if(file == NULL){   //如果是直接被调用
            RAISE(Mem_failed);
        }else{
            exceptRaise(&Mem_failed, file, line);
        }   
    }
    return ptr;
}