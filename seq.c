#include "seq.h"
#include "array.h"
#include "arrayrep.h"
#include "assert.h"
#include "mem.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/**
 * 将原序列内部的array扩容为2倍
 */ 
static void expand(Seq seq){
    int n = seq->array.length;
    arrayResize(&seq->array, 2*n);  //先利用realloc重置array的实际空间
    if(seq->head > 0){  //如果head位移过，则要平移数据
        //记住head元素的位置，而且old还得是指向void *的指针
        void **old = &((void **)seq->array.array)[seq->head];  
        //只往前移动head和后面的元素，不包括回环的，等于是空出中间
        memcpy(old + n, old, (n - seq->head) * sizeof(void *)); 
    }
}

Seq seqNew(int hint){
    assert(hint >= 0);
    Seq seq;
    NEWO(seq);  //所有字段值全设置为0
    if(hint == 0){
        hint = 16;  //hint必须要有一个合适的初始值，默认为16个元素空间
    }
    //初始化内部的Array，size参数只能给void *的大小，不然你咋知道客户端具体要多大
    arrayRepInit(&seq->array, hint, sizeof(void *), 
                    ALLOC(hint*sizeof(void *)));
    return seq;
}

Seq seqSeq(void *x, ...){
    va_list ap;
    Seq seq = seqNew(0);
    va_start(ap, x);
    while(x){
        //先处理第一个x元素
        seqAddhi(seq, x);
        x = va_arg(ap, void *); //把下一个给x
    }
    va_end(ap);
    return seq;
}

void seqFree(Seq *seq){
    assert(seq && *seq);
    assert((void *)*seq == (void *)&(*seq)->array);
    /**
     * 依赖于结构体内存结构的free方式
     * 因为Array是Seq结构的第一个元素，所以内存地址都一样
     * arrayFree接收的是结构的指针地址，说白了函数里面也是只认地址的
     * 最好不要这么做吧
     */ 
    arrayFree((Array *)seq);
}

int seqLength(Seq seq){
    assert(seq);
    return seq->length;
}

void *seqGet(Seq seq, int i){
    assert(seq);
    assert(i >= 0 && i < seq->length);
    //seq->array.array是个char *类型，可以用这种技巧来直接转换成显式数组，就可以使用[]操作符了
    return ((void **)seq->array.array)[(seq->head + i) % seq->array.length];
}

void *seqAddhi(Seq seq, void *x){
    assert(seq);
    //动态扩容
    if(seq->length == seq->array.length){
        expand(seq);
    }
    int i = seq->length++;
    return ((void **)seq->array.array)[(seq->head + i) % seq->array.length] = x;
}

void *seqAddlo(Seq seq, void *x){
    assert(seq);
    //动态扩容
    if(seq->length == seq->array.length){
        expand(seq);
    }
    //无论head在哪儿，都会往回移动一格，如果在最顶端，说明没数据，直接移到最后
    if(--seq->head < 0){    
        seq->head = seq->array.length - 1;
    }
    //插入数据
    seq->length++;
    //等于是head往回走一格，然后把新数据直接塞进head里
    return ((void **)seq->array.array)[(seq->head) % seq->array.length] = x;
}

void *seqRemhi(Seq seq){
    assert(seq);
    assert(seq->length > 0);
    int i = --seq->length;  //直接减少长度就可以了，不用真删除
    //直接返回上一个值
    return ((void **)seq->array.array)[(seq->head + i) % seq->array.length];
}

void *seqRemlo(Seq seq){
    assert(seq);
    assert(seq->length > 0);
    //直接返回head指向的元素
    void *x = ((void **)seq->array.array)[(seq->head) % seq->array.length];
    //直接将head后移一格，如果是最后了，则移到0位，也是不用真删除
    seq->head = (seq->head + 1) % seq->array.length;
    seq->length--;
    return x;
}