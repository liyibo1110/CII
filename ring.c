#include "ring.h"
#include "assert.h"
#include "mem.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

Ring ringNew(void){
    Ring ring;
    NEWO(ring); //length变成0了就
    ring->head = NULL;  //不能是野指针
    return ring;
}

Ring ringRing(void *x, ...){
    va_list ap;
    Ring ring = ringNew();
    va_start(ap, x);
    while(x){
        //add
        x = va_arg(ap, void *);
    }
    va_end(ap);
    return ring;
}

int ringLength(Ring ring){
    assert(ring);
    return ring->length;
}

void *ringGet(Ring ring, int i){
    assert(ring);
    assert(i >= 0 && i < ring->length);
    //开始寻找索引i对应的node元素
    struct node *q = ring->head;
    int n;
    if(i <= ring->length/2){    //如果索引i不过半，则往后找
        for(n = i; n > 0; n--){
            q = q->rlink;
        }
    }else{  //如果索引i已过半，则反着往前找
        for(n = ring->length - 1; n > 0; n--){
            q = q->llink;
        }
    }
    return q->value;
}

void *ringPut(Ring ring, int i, void *x){
    assert(ring);
    assert(i >= 0 && i < ring->length);
    //开始寻找索引i对应的node元素
    struct node *q = ring->head;
    int n;
    if(i <= ring->length/2){    //如果索引i不过半，则往后找
        for(n = i; n > 0; n--){
            q = q->rlink;
        }
    }else{  //如果索引i已过半，则反着往前找
        for(n = ring->length - 1; n > 0; n--){
            q = q->llink;
        }
    }
    void *prev = q->value;  //取出旧值
    q->value = x;   //覆盖新值
    return prev;
}

void *ringAddhi(Ring ring, void *x){
    assert(ring);
    struct node *p; //要新增的节点
    NEW(p);
    struct node *q = ring->head;    //要参与遍历的节点
    if(q == NULL){  //说明是个空环，全都指向p就好了
        ring->head = p;
        //注意环内部如果只有一个元素，那么也是自己指向自己的，不会指向NULL，这和head不一样
        p->llink = p;
        p->rlink = p;
    }else{  //如果不为空，则插到head的左侧，相当于环的最后一个元素的位置
        p->llink = q->llink;    //处理新元素的前驱
        p->rlink = q;   //处理新元素的后驱
        q->llink->rlink = p;    //处理原来尾元素的后驱
        q->llink = p;   //最后处理head元素的前驱
    }
    ring->length++;
    p->value = x;
    return p->value;
}

void *ringAddlo(Ring ring, void *x){
    assert(ring);
    ringAddhi(ring, x);
    //然后右旋一次即可，直接将head改成新插入的尾元素即可
    ring->head = ring->head->llink;
    return x;
}

void *ringAdd(Ring ring, int pos, void *x){
    assert(ring);
    assert(pos >= -ring->length && pos <= ring->length + 1);    //正负都不能出界
    if(pos == 1 || pos == -ring->length){   //相当于插入头部
        return ringAddlo(ring, x);
    }else if(pos == 0 || pos == ring->length + 1){  //相当于插入尾部
        return ringAddhi(ring, x);
    }else{
        struct node *p; //要新增的节点
        NEW(p);
        struct node *q = ring->head;
        //将i设置成正常的索引值，pos只是为了扩展函数的功能性
        int i = pos < 0 ? pos + ring->length : pos - 1;
        int n;
        if(i <= ring->length/2){    //如果索引i不过半，则往后找
            for(n = i; n > 0; n--){
                q = q->rlink;
            }
        }else{  //如果索引i已过半，则反着往前找
            for(n = ring->length - 1; n > 0; n--){
                q = q->llink;
            }
        }
        //插入p的右侧
        p->llink = q->llink;    //处理新元素的前驱
        p->rlink = q;   //处理新元素的后驱
        q->llink->rlink = p;    //处理左侧元素的后驱
        q->llink = p;   //最后处理后元素的前驱
        ring->length++;
        p->value = x;
        return p->value;
    }
}

void *ringRemove(Ring ring, int i){
    assert(ring);
    assert(ring->length > 0);
    assert(i >= 0 && i < ring->length);
    struct node *q = ring->head;
    int n;
    //还是要用老代码寻找i对应的元素
    if(i <= ring->length/2){    //如果索引i不过半，则往后找
        for(n = i; n > 0; n--){
            q = q->rlink;
        }
    }else{  //如果索引i已过半，则反着往前找
        for(n = ring->length - 1; n > 0; n--){
            q = q->llink;
        }
    }
    //开始删除
    if(i == 0){ //如果要删除的是首元素，则一定要多一步，即将head的指向改成后面的
        ring->head = ring->head->rlink;
    }
    void *x = q->value; //保存旧值
    q->llink->rlink = q->rlink;
    q->rlink->llink = q->llink;
    FREE(q);
    ring->length--;
    if(ring->length == 0){  //如果是最后一个元素，则也要修改head指向
        ring->head = NULL;
    }
    return x;
}

void *ringRemhi(Ring ring){
    assert(ring);
    assert(ring->length > 0);
    struct node *q = ring->head->llink; //直接就找到了
    void *x = q->value; //保存旧值
    q->llink->rlink = q->rlink;
    q->rlink->llink = q->llink;
    FREE(q);
    ring->length--;
    if(ring->length == 0){  //如果是最后一个元素，则也要修改head指向
        ring->head = NULL;
    }
    return x;
}

void *ringRemlo(Ring ring){
    assert(ring);
    assert(ring->length > 0);
    //直接head右移一个，然后复用ringRemhi函数，可以这样做是因为当只有1个元素时，前后驱都指向了自己而不是NULL
    ring->head = ring->head->rlink;
    return ringRemhi(ring);
}

void ringRotate(Ring ring, int n){
    assert(ring);
    //正负不能超过length即可,否则是转过一整圈了，也可以改成取模操作，减少空转次数
    assert(n >= -ring->length && n <= ring->length);
    int i;
    if(n >= 0){ //顺时针或不转
        i = n;
    }else{  //逆时针
        i = n + ring->length;   //改成顺时针计算方式
    }
    struct node *q = ring->head;
    if(i <= ring->length/2){    //如果索引i不过半，则往后找
        for(n = i; n > 0; n--){
            q = q->rlink;
        }
    }else{  //如果索引i已过半，则反着往前找
        for(n = ring->length - 1; n > 0; n--){
            q = q->llink;
        }
    }
    ring->head = q; //直接当头就可以了，当做旋转完毕
}