#include "set.h"
#include "mem.h"
#include "assert.h"
#include "arith.h"
#include <limits.h>
#include <stdio.h>

//x和y是原子性的，因此可以直接比较，如相等则返回0，不相等则返回1
static int cmpAtom(const void *x, const void *y){
    return x != y;
}

static unsigned hashAtom(const void *x){
    return (unsigned long)x >> 2;
}

static Set copy(Set t, int hint){
    assert(t);
    Set set;
    set = setNew(hint, t->cmp, t->hash);
    //遍历赋值
    struct member *q;
    for(int i = 0; i < t->size; i++){
        for(q = t->buckets[i]; q; q = q->link){
            const void *member = q->member; //新建一个指针，因为q是临时的，注意没有复制内容
            //不使用setPut函数，而是直接计算hash，因为新集合肯定是空的，不需要先寻找
            int j = (*set->hash)(member)%set->size; 
            //开始赋值
            struct member *p;
            NEW(p);
            p->member = member;
            //加入到链表
            p->link = set->buckets[j];
            set->buckets[j] = p;
            set->length++;
        }
    }
    return set;
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
    set->buckets = (struct member **)(set + 1);    //alloc是申请了2块，所以要把buckets人工指向
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

Set setUnion(Set s, Set t){
    if(s == NULL){  //s为空，直接复制一套t的
        assert(t);
        return copy(t, t->size);
    }else if(t == NULL){    //t为空，直接复制一套s的
        return copy(s, s->size);
    }else{
        Set set = copy(s, arithMax(s->size, t->size));  //先把s复制出去，桶数为2者更大的那个
        //得确保2个Set的cmp和hash函数都一致，不然算不到一起去
        assert(s->cmp == t->cmp && s->hash == t->hash);
        //遍历t，尝试存入新的set
        struct member *q;
        for(int i = 0; i < t->size; i++){
            for(q = t->buckets[i]; q; q = q->link){
                setPut(set, q->member);
            }
        }
        return set;
    }
}

Set setInter(Set s, Set t){
    if(s == NULL){  //s为空，直接根据t建一个空的set返回
        assert(t);
        return setNew(t->size, t->cmp, t->hash);
    }else if(t == NULL){    //t为空，直接根据s建一个空的set返回
        return setNew(s->size, s->cmp, s->hash);
    }else{
        //得确保2个Set的cmp和hash函数都一致，不然算不到一起去
        assert(s->cmp == t->cmp && s->hash == t->hash);
        //建立空的set，桶数为2者更小的那个
        Set set = setNew(arithMin(s->size, t->size), s->cmp, s->hash);
        //遍历s，然后看t中是否存在，存在则写入新的set
        struct member *q;
        for(int i=0; i<s->size; i++){
            for(q = s->buckets[i]; q; q = q->link){
                if(setMember(t, q->member)){
                    //直接计算hash写入，因为set是空的
                    const void *member = q->member; //新建一个指针，因为q是临时的，注意没有复制内容
                    //不使用setPut函数，而是直接计算hash，因为新集合肯定是空的，不需要先寻找
                    int j = (*set->hash)(member)%set->size; 
                    //开始赋值
                    struct member *p;
                    NEW(p);
                    p->member = member;
                    //加入到链表
                    p->link = set->buckets[j];
                    set->buckets[j] = p;
                    set->length++;
                }
            }
        }
        return set;
    }
}

/**
 * 返回集合s存在，但集合t不存在的元素
 */ 
Set setMinus(Set s, Set t){
    if(s == NULL){  //s为空，直接根据t建一个空的set返回
        assert(t);
        return setNew(t->size, t->cmp, t->hash);
    }else if(t == NULL){    //t为空，直接复制一套s的
        return copy(s, s->size);
    }else{
        //得确保2个Set的cmp和hash函数都一致，不然算不到一起去
        assert(s->cmp == t->cmp && s->hash == t->hash);
        //建立空的set，桶数为2者更小的那个
        Set set = setNew(arithMin(s->size, t->size), s->cmp, s->hash);
        //遍历s，然后看t中是否存在，存在则写入新的set
        struct member *q;
        for(int i=0; i<s->size; i++){
            for(q = s->buckets[i]; q; q = q->link){
                if(!setMember(t, q->member)){   //在t中不存在则新建元素过去
                    //直接计算hash写入，因为set是空的
                    const void *member = q->member; //新建一个指针，因为q是临时的，注意没有复制内容
                    //不使用setPut函数，而是直接计算hash，因为新集合肯定是空的，不需要先寻找
                    int j = (*set->hash)(member)%set->size; 
                    //开始赋值
                    struct member *p;
                    NEW(p);
                    p->member = member;
                    //加入到链表
                    p->link = set->buckets[j];
                    set->buckets[j] = p;
                    set->length++;
                }
            }
        }
        return set;
    }
}

Set setDiff(Set s, Set t){
    if(s == NULL){  //s为空，直接复制一套t的
        assert(t);
        return copy(t, t->size);
    }else if(t == NULL){    //t为空，直接复制一套s的
        return copy(s, s->size);
    }else{
        //得确保2个Set的cmp和hash函数都一致，不然算不到一起去
        assert(s->cmp == t->cmp && s->hash == t->hash);
        //建立空的set，桶数为2者更小的那个
        Set set = setNew(arithMin(s->size, t->size), s->cmp, s->hash);
         //先遍历s，然后看t中是否存在，不存在则写入新的set
        struct member *q;
        for(int i=0; i<s->size; i++){
            for(q = s->buckets[i]; q; q = q->link){
                if(!setMember(t, q->member)){   //在t中不存在则新建元素过去
                    //直接计算hash写入，因为set是空的
                    const void *member = q->member; //新建一个指针，因为q是临时的，注意没有复制内容
                    //不使用setPut函数，而是直接计算hash，因为新集合肯定是空的，不需要先寻找
                    int j = (*set->hash)(member)%set->size; 
                    //开始赋值
                    struct member *p;
                    NEW(p);
                    p->member = member;
                    //加入到链表
                    p->link = set->buckets[j];
                    set->buckets[j] = p;
                    set->length++;
                }
            }
        }
        //再遍历t,然后看s中是否存在，不存在则写入新的set
        for(int i=0; i<t->size; i++){
            for(q = t->buckets[i]; q; q = q->link){
                if(!setMember(s, q->member)){   //在t中不存在则新建元素过去
                    //直接计算hash写入，因为set是空的
                    const void *member = q->member; //新建一个指针，因为q是临时的，注意没有复制内容
                    //不使用setPut函数，而是直接计算hash，因为新集合肯定是空的，不需要先寻找
                    int j = (*set->hash)(member)%set->size; 
                    //开始赋值
                    struct member *p;
                    NEW(p);
                    p->member = member;
                    //加入到链表
                    p->link = set->buckets[j];
                    set->buckets[j] = p;
                    set->length++;
                }
            }
        }
        return set;
    }
}