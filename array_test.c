#include "array.h"
#include "arrayrep.h"
#include "assert.h"
#include <stdio.h>

static void arrayIntPrint(Array array){
    assert(array);
    for(int i=0; i<array->length; i++){
        int result = *(int*)arrayGet(array, i);
        printf("%d\n", result);
    }
}

int main(void){
    Array array = arrayNew(5, sizeof(int));
    //开始写入数字
    int i1 = 1, i2 = 2, i3 = 3, i4 = 4, i5 = 5;
    arrayPut(array, 0, &i1);
    arrayPut(array, 1, &i2);
    arrayPut(array, 2, &i3);
    arrayPut(array, 3, &i4);
    arrayPut(array, 4, &i5);
    //开始遍历
    arrayIntPrint(array);

    //开始复制
    printf("\n");
    Array array2 = arrayCopy(array, 3);
    arrayIntPrint(array2);

    return 0;
}