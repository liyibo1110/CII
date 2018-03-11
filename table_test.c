#include "table.h"
#include <stdio.h>

void apply(const void *key, void **value, void *cl){
    printf("key: %s\n", key);
    printf("value: %s\n", *value);
}

int main(void){

    Table table = tableNew(1024, NULL, NULL);
    printf("table ok\n");

    printf("table length: %d\n", tableLength(table));

    //开始遍历
    tableMap(table, apply, NULL);

    //插入键值对
    printf("\n");
    void *result = tablePut(table, "aaa", "111");
    printf("result: %s\n", result);
    result = tablePut(table, "bbb", "333");
    tableMap(table, apply, NULL);
    result = tablePut(table, "aaa", "222");
    printf("result: %s\n", result);
    tableMap(table, apply, NULL);

    printf("table length: %d\n", tableLength(table));
    
    //获取值
    /* void *v = tableGet(table, "aaa");
    printf("v: %s\n", (char *)v); */

    //删除key
    void *v = tableRemove(table, "aaa");
    printf("v: %s\n", (char *)v);
    printf("table length: %d\n", tableLength(table));
}