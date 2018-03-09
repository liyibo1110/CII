#include "list.h"
#include "assert.h"
#include "mem.h"
#include <stdarg.h>
#include <stdio.h>



List listList(void *x, ...){
    va_list ap;
    List list;
    List *p = &list;    //指针的指针，指向当前要赋值的那项

    va_start(ap, x);
    //开始向list里添加元素
    while(x){
        NEW(*p);    //注意要加星号，传的还是指针
        (*p)->first = x;
        p = &((*p)->rest);  //将p指向下一个元素，不一定会赋值了
        x = va_arg(ap, void *); //将下一个参数给x  
    }
    *p = NULL;
    va_end(ap);

    return list;
} 

List listPush(List list, void *x){
    List l;
    NEW(l);
    l->first = x;
    l->rest = list;
    return l;
}

/**
 * x参数必须是指针的指针，void *是结构体的内部元素，要当做一个整体来看待
 * 外界需要用&x这样的实参传进来，才能把void *里的内容带回去，因此形参需要void **x
 */ 
List listPop(List list, void **x){
    if(list){
        List head = list->rest;
        if(x){
            *x = list->first;
            FREE(list);
        }
        return head;
    }else{
        return list;
    }
}

List listAppend(List list, List tail){
    List *p = &list;    //指针的指针，指向原链表的头元素
    while(*p){  //一直向前，直到指向了最后一个元素的rest里面
        p = &((*p)->rest);
    }
    *p = tail;
    return list;
}

List listCopy(List list){
    List head;
    List *p = &head;    //指针的指针，指向当前要赋值的那项
    while(list){
        NEW(*p);
        (*p)->first = list->first;
        //移到rest里面
        p = &(*p)->rest; 
        list = list->rest; 
    }
    *p = NULL;
    return head;
}

List listReverse(List list){
    List head = NULL;
    List next;  //永远指向下一个，当为NULL说明遍历完了
    while(list){
        next = list->rest;
        list->rest = head;  //处理新list的后续，将断开list的头元素，后续指向已逆序出来的新headlist
        head = list;    //将新断开的头元素，放到headlist的最前面
        list = next;
    }
    return head;
}

int listLength(List list){
    int count = 0;
    while(list){
        count++;
        list = list->rest;
    }
    return count;
}

void listFree(List list){
    assert(list);
    List next;
    while(list){
        next = list->rest;  //指名下一个元素位置，这样就可以清理首元素了
        FREE(list);
    }
}

void listMap(List list, 
    void *apply(void **x, void *cl), void *cl){
        assert(apply);
        while(list){
            //调用传入的函数指针，参数是当前遍历的first值和客户端调用传入的cl参数
            apply(&list->first, cl);
            list = list->rest;
        }
}

void **listToArray(List list, void *end){
    int i;
    int n = listLength(list);   //如果list为NULL则返回0
    void **array = ALLOC((n + 1)*sizeof(array)); //分配n+1组内存，注意array是指向指针的数组
    //开始遍历赋值
    for(i = 0; i < n; i++){
        array[i] = list->first;
        list = list->rest;
    }
    //最后把传进来的end给数组额外的最后1组
    array[i] = end;
    return array;   
}