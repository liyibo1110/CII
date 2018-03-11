#include "set.h"
#include "mem.h"
#include "assert.h"
#include <limits.h>

//x和y是原子性的，因此可以直接比较，如相等则返回0，不相等则返回1
static int cmpAtom(const void *x, const void *y){
    return x != y;
}

static unsigned hashAtom(const void *x){
    return (unsigned long)x >> 2;
}

Set setNew(int hint,
               int (*cmp)(const void *x, const void *y),
               unsigned (*hash)(const void *x)){

    assert(hint >= 0);
    Set set;
    int i;
    static int primes[] = {509, 509, 1021, 2053, 4093, 
                        8191, 16381, 32771, 65521, INT_MAX};
    for (i = 1; primes[i] < hint; i++){
        //只是为了给i指定最接近hint的值，桶的大小将为以上这些素数
        //此结构并不支持动态rehash扩容，而且hash算法也可以用外界自定义，因此能做的就是将桶大小强制为一个合适的素数了
    }
    set = ALLOC(sizeof(*set) + (primes[i - 1]) * sizeof(set->buckets[0]));
    set->size = primes[i - 1];
    set->cmp = (cmp ? cmp : cmpAtom); //传了则用，否则用默认的实现
    set->hash = (hash ? hash : hashAtom); //传了则用，否则用默认的实现
    set->length = 0;
    set->timestamp = 0;
    for (int j = 0; j < set->size; j++){
        set->buckets[j] = NULL;
    }
    return set;
}

int setMember(Set set, const void *member){
    assert(set);
    assert(member);
    struct member *p;
    int i = (*set->hash)(member) % set->size;
    //开始在特定桶里遍历寻找相应的key
    for (p = set->buckets[i]; p; p = p->link){
        if((*set->cmp)(member, p->member) == 0){
            break;  //找到则跳出
        }
    }
    return p != NULL;
}

void setPut(Set set, const void *member){
    assert(set);
    assert(member);
    struct member *p;
    //使用指针调用table的hash函数，然后和桶大小取模，生成桶索引
    int i = (*set->hash)(member) % set->size;
    //开始在特定桶里遍历寻找相应的key
    for (p = set->buckets[i]; p; p = p->link){
        if((*set->cmp)(member, p->member) == 0){
            break;  //找到则跳出
        }
    }
    if(p == NULL){  //没有则新增
        NEW(p);
        p->member = member;
        p->link = set->buckets[i];    //加入链表
        set->buckets[i] = p;
        set->length++;
    }else{  //已存在则修改value
       p->member = member; 
    }
    set->timestamp++;
}

//length即为member的总数
int setLength(Set set){
    assert(set);
    return set->length;
}

void *setRemove(Set set, const void *member){
    assert(set);
    assert(member);
    struct member **pp;
    set->timestamp++;
    //使用指针调用table的hash函数，然后和桶大小取模，生成桶索引
    int i = (*set->hash)(member) % set->size;
    //开始在特定桶里遍历寻找相应的key
    for (pp = &set->buckets[i]; *pp; pp = &((*pp)->link)){
        if((*set->cmp)(member, (*pp)->member) == 0){
            //找到则删除
            struct member *p = *pp;
            *pp = p->link;  //直接指向链表下一个节点
            FREE(p);
            set->length--;
            return (void *)member;
        }
    }
    return NULL;
}

void setFree(Set *set){
    assert(set);
    assert(*set);
    if((*set)->length > 0){
        struct member *p;
        struct member *q;
        for(int i = 0; i < (*set)->size; i++){
            for(p = (*set)->buckets[i]; p; p = q){
                q = p->link;    //需要将link另存为，因为p被free了
                FREE(p);
            }
        }
    }
}

void setMap(Set set,
              void apply(const void *member, void *cl),
              void *cl){
    assert(set);
    assert(apply);
    unsigned stamp = set->timestamp;  //暂存用来比较是否发生了变化
    struct member *p;
    for(int i = 0; i < set->size; i++){
        for(p = set->buckets[i]; p; p = p->link){
            //调用函数指针
            apply(p->member, cl);
            assert(stamp == set->timestamp);   //检查stamp是否发生了变化
        }
    }
}

void **setToArray(Set set, void *end){
    assert(set);
    void **array = ALLOC((2 * set->length + 1) * sizeof(*array));
    struct member *p;
    int j = 0;
    for(int i = 0; i < set->size; i++){
        for(p = set->buckets[i]; p; p = p->link){
            array[j++] = (void *)p->member;
        }
    }
    array[j] = end;
    return array;
}