#include "set.h"
#include <stdio.h>

void apply(const void *member, void *cl){
    printf("member: %s\n", (char *)member);
}

int main(void){

    Set set = setNew(1024, NULL, NULL);
    printf("set ok\n");

    printf("set length: %d\n", setLength(set));

    //开始遍历
    setMap(set, apply, NULL);

    //插入新值
    printf("\n");
    setPut(set, "aaa");
    setPut(set, "bbb");
    setMap(set, apply, NULL);
    setPut(set, "ccc");
    printf("\n");
    setMap(set, apply, NULL);
    printf("table length: %d\n", setLength(set));
    
    //删除元素
    void *member = setRemove(set, "bbb");
    printf("remove: %s\n", (char *)member);
    printf("set1: \n");
    setMap(set, apply, NULL);

    Set set2 = setNew(1024, NULL, NULL); 
    //构造第二个集合
    setPut(set2, "aaa");
    setPut(set2, "bbb");

    printf("set2: \n");
    setMap(set2, apply, NULL);
    
    //取并集    
    printf("sum: \n");
    Set sumSet = setUnion(set, set2);
    setMap(sumSet, apply, NULL);

    //取交集
    printf("inter: \n");
    Set interSet = setInter(set, set2);
    setMap(interSet, apply, NULL);

    //取差集
    printf("minus: \n");
    Set minusSet = setMinus(set, set2);
    setMap(minusSet, apply, NULL);

    //取差分集
    printf("diff: \n");
    Set diffSet = setDiff(set, set2);
    setMap(diffSet, apply, NULL);
    
}