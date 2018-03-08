#include "list.h"
#include "assert.h"
#include "mem.h"
#include <stdarg.h>

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

List listPop(List list, void *x){
    if(list){
        List head = list->rest;
        x = list->first;
        FREE(list);
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