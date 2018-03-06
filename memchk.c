#include <stdlib.h>
#include <string.h>
#include "assert.h"
#include "except.h"
#include "mem.h"

//用来做内存对齐，申请内存时会自动对齐，释放时候也会检查是否对齐（不对齐则说明有问题）
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

//只在这个文件里使用，其实就是个hash表，桶初始化为2048个，hash相同的，通过link字段来形成内部链表
static struct descriptor{
    struct descriptor *free;
    struct descriptor *link;
    void *ptr;
    long size;
    const char *file;
    int line;
} *htab[2048];

//定义空闲集合，一开始free字段指向它自己
static struct descriptor freelist = {&freelist};

//定义hash算法，右移3位按位与
#define hash(p, t) (((unsigned long)(p)>>3) & (sizeof (t)/sizeof ((t)[0])-1))

//根据给定ptr，从hash表里寻找匹配的元素并返回
static struct descriptor *find(const void *ptr){
    struct descriptor *d = htab[hash(ptr, htab)];
    while(d && d->ptr != ptr){
        d = d->link;
    }
    return d;
}

//高级版函数实现
void memFree(void *ptr, const char *file, int line){
    if(ptr){
        struct descriptor *d;
        //检测清理的可行性（1要对齐，2要能找到这个ptr对应的描述符，3是描述符的free要为NULL，即没有被free化）
        if(((unsigned long)ptr) % (sizeof(union align)) != 0
            ||  (d = find(ptr)) == NULL || d->free){
                exceptRaise(&Assert_Failed, file, line);
        }
        //没问题了，不真调用free，而是压到freelist的循环链表里面
        //IDE或编译器在这里可能会有警告提示d可能未初始化，但d到这里不会是野指针
        d->free = freelist.free;
        freelist.free = d;
    }
}

/* void *memAlloc(long nbytes, const char *file, int line){
    struct descriptor *d;
    void *ptr;
    assert(nbytes > 0);
    //修剪nbytes
    nbytes = ((nbytes + sizeof(union align) - 1)  /
                (sizeof (union align))) * (sizeof (union align));
} */

/* void *memResize(void *ptr, long nbytes, const char *file, int line){
    struct descriptor *d;
    void *newptr;
    assert(ptr);
    assert(nbytes > 0);
     //检测清理的可行性（1要对齐，2要能找到这个ptr对应的描述符，3是描述符的free要为NULL，即没有被free化）
    if(((unsigned long)ptr) % (sizeof(union align)) != 0
        ||  (d = find(ptr)) == NULL || d->free){
            exceptRaise(&Assert_Failed, file, line);
    }

} */