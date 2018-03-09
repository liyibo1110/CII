#include "list.h"
#include <stdio.h>

void *apply(void **x, void *cl){
    printf("value: %s\n", *x);
    return NULL;
}

int main(void){

    List list = listList("aaa", "bbb", "ccc", "ddd", NULL);
    printf("listList ok\n");

    printf("list length: %d\n", listLength(list));

    //开始遍历
    listMap(list, apply, NULL);

    //插入头元素
    printf("\n");
    list = listPush(list, "111");
    listMap(list, apply, NULL);

    //弹出元素
    printf("\n");
    void *v = NULL;
    list = listPop(list, &v);
    printf("pop: %s\n", (char *)v);

    //尾部插入新链表
    printf("\n");
    list = listAppend(list, listList("222", NULL));
    listMap(list, apply, NULL);

    //翻转
    printf("\n");
    list = listReverse(list);
    listMap(list, apply, NULL);

}