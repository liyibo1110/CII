#include "seq.h"
#include "array.h"
#include "arrayrep.h"
#include <stdio.h>

int main(void){

    //开始写入数字
    int intArray[5] = {1, 2, 3, 4, 5};

    Seq seq = seqSeq(&intArray[0], &intArray[1], &intArray[2], &intArray[3], &intArray[4], NULL);
    printf("seq ok\n");

    printf("seq length: %d\n", seqLength(seq));

    //取出某元素
    void *v = ((void **)seq->array.array)[1];
    printf("v: %p\n", (int *)v);

    return 0;
}
