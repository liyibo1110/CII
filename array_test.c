#include "array.h"
#include "arrayrep.h"
#include "assert.h"
#include <stdio.h>
#include <stdlib.h>

static void arrayIntPrint(Array array){
    assert(array);
    for(int i=0; i<array->length; i++){
        int result = *(int*)arrayGet(array, i);
        printf("%d\n", result);
    }
} 

int main(void){

   /*  Array array = arrayNew(5, sizeof(int));
    //开始写入数字
    int intArray[5] = {1, 2, 3, 4, 5};
    for(int i = 0; i < 5; i++){
        arrayPut(array, i, &intArray[i]);
    }  */

    //arrayIntPrint(array);

    /* printf("length: %d\n", array->length);
    printf("size: %d\n", array->size); */

    /* void *v1 = array->array + 4;
    printf("v1: %d\n", *(int *)v1);
    void *v2 = ((void **)array->array)[2];
    printf("v2: %d\n", *(int *)v2); */ 
    // intPArray[i] = &intArray[i];
    

    /* for(int i = 0; i < 5; i++){
        printf("%p\n", intPArray[i]);
    } */

    //开始遍历
    

    //开始复制
    /* printf("\n");
    Array array2 = arrayCopy(array, 3);
    arrayIntPrint(array2); */  

    /* int intArray[5] = {1, 2, 3, 4, 5};
    for(int i = 0; i < 5; i++){
        printf("%d: ", intArray[i]);
        printf("%p\n", &intArray[i]);
    } */
    
    char *array = malloc(sizeof(4 * sizeof(void *)));

    //当做数组来搞
    ((void **)array)[0] = "abc";
    ((void **)array)[1] = "xyz";
    //再取出来
    void *v = ((void **)array)[0];
    printf("v1: %s\n", (char *)v);
    v = ((void **)array)[1];
    printf("v2: %s\n", (char *)v);

    return 0;
}