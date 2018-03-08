#include "arena.h"
#include "assert.h"
#include "except.h"
#include <stdlib.h>
#include <string.h>

#define THRESHOLD 10

const Except_T Arena_NewFailed = {"Arena_NewFailed"};
const Except_T Arena_Failed = {"Arena_Failed"};

union align {
    int i;
    long l;
    long *lp;
    void *p;
    void (*fp)(void);
    float f;
    double d;
    long double ld;
};

union header{
    Arena b;   
    union align a;
};

static Arena freechunks;
static int nfree;

Arena arenaNew(void){
    Arena arena = malloc(sizeof(*arena));
    if(arena == NULL){
        RAISE(Arena_NewFailed);
    }
    arena->prev = NULL;
    arena->avail = NULL;
    arena->limit = NULL;
    return arena;
}

void arenaDispose(Arena ap){
    assert(ap);
    arenaFree(ap);
    free(ap);
    ap = NULL;
}

void *arenaAlloc(Arena arena, long nbytes, const char *file, int line){
    assert(arena);
    assert(nbytes > 0);
    //内存对齐
    nbytes = ((nbytes + sizeof(union align) - 1)  /
                (sizeof (union align))) * (sizeof (union align));
    //如果申请的字节超过剩余的，则需要扩展arena
    while(nbytes > arena->limit - arena->avail){
        Arena ptr;
        char *limit;
        //先从freechunks获取
        if((ptr = freechunks) != NULL){
            //如果进来了，说明直接使用了free过的空闲存储块
            freechunks = freechunks->prev;
            nfree--;
            limit = ptr->limit; //被复用的空闲存储块当初指向的limit，就是新版的limit
        }else{  //freechunks也没了，则用malloc分配
            long m = sizeof(union header) + nbytes + 10*1024;   //多分配10k
            ptr = malloc(m);
            if(ptr == NULL){
                if(file == NULL){
                    RAISE(Arena_Failed);
                }else{
                    exceptRaise(&Arena_Failed, file, line);
                }
            }
            limit  = (char *)ptr + m;   //直接指向新存储块的尾部
        }
        /**
         * 最重要的一句，直接把原来已满的结构体，复制给新的结构体，相当于完成初始内容构造
         * 即将原来的avail和limit，复制给了新存储块（prev本来为NULL，复制了也是NULL）
         * 重点核心是要理解，新的存储块，不是追加在后面，是每次插入到中间的
         */ 
        *ptr = *arena;  
        //修改原来存储块的各个字段
        arena->avail = (char *)((union header *)ptr + 1);   //avail指向下一个存储块开头，但要跳过存储块结构本身
        arena->limit = limit;
        arena->prev = ptr;
        //回到while再次参与判断，因为可能从freechunks里复用的，不一定大小就够了
    }
    arena->avail += nbytes; //修改avail位置，代表已分配完成
    return arena->avail - nbytes;   //返回之前的起始内存位置
}

void *arenaCalloc(Arena arena, int count, long nbytes, const char *file, int line){
    void *ptr;
    assert(count > 0);
    ptr = arenaAlloc(arena, count*nbytes, file, line);
    memset(ptr, '\0', count*nbytes);
    return ptr;
}

void arenaFree(Arena arena){
    assert(arena);
    while(arena->prev){
        struct Arena_T tmp = *arena->prev;  //局部变量，每次循环完就会销毁
        if(nfree < THRESHOLD){  //如果小于10个空闲块就尝试留着复用，否则直接干掉
            arena->prev->prev = freechunks;
			freechunks = arena->prev;
			nfree++;
			freechunks->limit = arena->limit;
        }else{
            free(arena->prev);
        }
        *arena = tmp;
    }
    assert(arena->limit == NULL);
    assert(arena->avail == NULL);
}