#include <stdlib.h>
#include <string.h>
#include "assert.h"
#include "except.h"
#include "mem.h"

#define NALLOC 4096
#define NDESCRIPTORS 512

//实际定义全局异常
const Except_T Mem_Failed = {"Allocation Failed"};

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

static struct descriptor *dalloc(void *ptr, long size, 
                                const char *file, int line){
    //用来做分配池，一次申请512组空间，也就是说每512个描述符本身是内存相邻的
    static struct descriptor *avail;    
    static int nleft;   //剩余可分配的描述符全局计数
    if(nleft <= 0){ //512个描述符都用完了，就要重新申请512个
        //指向第一个描述符位置
        avail = malloc(NDESCRIPTORS * sizeof(*avail));
        if(avail == NULL){
            return NULL;
        }
        nleft = NDESCRIPTORS;
    }
    //初始化描述符的各个字段
    avail->free = NULL;
    avail->link = NULL;
    avail->ptr = ptr;
    avail->size = size;
    avail->file = file;
    avail->line = line;
    nleft--;
    return avail++; //移到下一个空白的内存区
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

void *memAlloc(long nbytes, const char *file, int line){
    struct descriptor *d;
    void *ptr;
    assert(nbytes > 0);
    //修剪nbytes
    nbytes = ((nbytes + sizeof(union align) - 1)  /
                (sizeof (union align))) * (sizeof (union align));
    /**
     * 开始分配内存，使用first-fit分配算法，始终从freelist链中遍历。
     * 如果都没有，则申请，之后先放入freelist链，然后重新遍历
     */
    for(d = freelist.free ; d; d = d->free){
        //空闲链表里如果找到了能装下的空闲块
        //描述符里的ptr是可以被多个描述符共享使用的，只是每个ptr指向的区域不一样
        if(d->size > nbytes){
            d->size -= nbytes;
            ptr = (char *)d->ptr + d->size; 
            //申请新的描述符，原来的还在，此时2个描述符对同一段ptr形成共享关系
            if((d = dalloc(ptr, nbytes, file, line)) != NULL){
                unsigned h = hash(ptr, htab);
                d->link = htab[h];  //链接到hash表相应桶的头部，这样就不需要整体移动了
                htab[h] = d;    //修改该桶的头结点为新的描述符
                return ptr;
            }else{
                {
					if (file == NULL)
						RAISE(Mem_Failed);
					else
						exceptRaise(&Mem_Failed, file, line);
				}
            }
        }
        //到头了也没找到，则要分配新的空闲块，然后再次进入循环
        if(d == &freelist){
            struct descriptor *newptr;
            //分配新块
            if((ptr = malloc(nbytes + NALLOC)) == NULL || 
                (newptr = dalloc(ptr, nbytes + NALLOC, __FILE__, __LINE__)) == NULL){
                if(file == NULL){
                    RAISE(Mem_Failed);
                }else{
                    exceptRaise(&Mem_Failed, file, line);
                }
            }

            newptr->free = freelist.free;
            freelist.free = newptr;
        }
    }
    assert(0);  //压根就不能出来
    return NULL;
} 

void *memCalloc(long count, long nbytes, const char *file, int line){
    void *ptr;
    assert(count > 0);
    assert(nbytes > 0);
    ptr = memAlloc(count*nbytes, file, line);
    memset(ptr, '\0', count*nbytes);
    return ptr;
}

void *memResize(void *ptr, long nbytes, const char *file, int line){
    struct descriptor *d;
    void *newptr;
    assert(ptr);
    assert(nbytes > 0);
     //检测清理的可行性（1要对齐，2要能找到这个ptr对应的描述符，3是描述符的free要为NULL，即没有被free化）
    if(((unsigned long)ptr) % (sizeof(union align)) != 0
        ||  (d = find(ptr)) == NULL || d->free){
            exceptRaise(&Assert_Failed, file, line);
    }
    //先按照新的大小，申请一段新空间
    newptr = memAlloc(nbytes, file, line);
    //然后再将旧空间的内容复制过去
    memcpy(newptr, ptr, nbytes < d->size ? nbytes : d->size);
    //还要free原来的ptr
    memFree(ptr, file, line);
    return newptr;
}