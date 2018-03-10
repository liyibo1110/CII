#include "table.h"
#include "mem.h"
#include "assert.h"
#include <limits.h>

//x和y是原子性的，因此可以直接比较，如相等则返回0，不相等则返回1
static int cmpAtom(const void *x, const void *y){
    return x != y;
}

static unsigned hashAtom(const void *key){
    return (unsigned long)key >> 2;
}

Table tableNew(int hint,
               int (*cmp)(const void *x, const void *y),
               unsigned (*hash)(const void *key)){

    assert(hint >= 0);
    Table table;
    int i;
    static int primes[] = {509, 509, 1021, 2053, 4093, 
                        8191, 16381, 32771, 65521, INT_MAX};
    for (i = 1; primes[i] < hint; i++){
        //只是为了给i指定最接近hint的值，桶的大小将为以上这些素数
        //此结构并不支持动态rehash扩容，而且hash算法也可以用外界自定义，因此能做的就是将桶大小强制为一个合适的素数了
    }
    table = ALLOC(sizeof(*table) + (primes[i - 1]) * sizeof(table->buckets[0]));
    table->size = primes[i - 1];
    table->cmp = (cmp ? cmp : cmpAtom); //传了则用，否则用默认的实现
    table->hash = (hash ? hash : hashAtom); //传了则用，否则用默认的实现
    table->length = 0;
    table->timestamp = 0;
    for (int j = 0; j < table->size; j++){
        table->buckets[j] = NULL;
    }
    return table;
}

void *tableGet(Table table, const void *key){
    struct bindings *p;
    assert(table);
    assert(key);
    //使用指针调用table的hash函数，然后和桶大小取模，生成桶索引
    int i = (*table->hash)(key) % table->size;
    //开始在特定桶里遍历寻找相应的key
    for (p = table->buckets[i]; p; p = p->link){
        if((*table->cmp)(key, p->key) == 0){
            break;  //找到则跳出
        }
    }
    return p ? p->value : NULL;
}

void *tablePut(Table table, const void *key, void *value){
    struct bindings *p;
    assert(table);
    assert(key);
    void *prev; //保存被替换之前的value
    //使用指针调用table的hash函数，然后和桶大小取模，生成桶索引
    int i = (*table->hash)(key) % table->size;
    //开始在特定桶里遍历寻找相应的key
    for (p = table->buckets[i]; p; p = p->link){
        if((*table->cmp)(key, p->key) == 0){
            break;  //找到则跳出
        }
    }
    if(p == NULL){  //没有则新增
        NEW(p);
        p->link = table->buckets[i];    //加入链表
        table->buckets[i] = p;
        p->key = key;
        table->length++;
        prev = NULL;
    }else{  //已存在则修改value
        prev = p->value; 
    }
    p->value = value;
    table->timestamp++;
    return prev;
}

//length即为key的总数
int tableLength(Table table){
    assert(table);
    return table->length;
}