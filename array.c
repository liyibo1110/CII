#include "array.h"
#include "arrayrep.h"
#include "assert.h"
#include "mem.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Array arrayNew(int length, int size){
    Array array;
    NEW(array);
    if(length > 0){
        arrayRepInit(array, length, size, CALLOC(length, size));
    }else{
        arrayRepInit(array, length, size, NULL);
    }
    return array;
}

void arrayRepInit(Array array, int length,
                  int size, void *ary){
    //做好各种入参检查
    assert(array);
    assert((ary && length > 0) || (length == 0 && ary == NULL));
    assert(size > 0);
    array->length = length;
    array->size = size;
    if(length > 0){
        array->array = ary;
    }else{
        array->array = NULL;
    }
}

void arrayFree(Array *array){
    assert(array);
    assert(*array);
    FREE((*array)->array);  //里面的array可以为NULL，不需要管
    FREE(*array);
}

void *arrayGet(Array array, int i){
    assert(array);
    assert(i >= 0 && i < array->length);
    //直接计算内存位置并返回
    return array->array + i * array->size;
}

void *arrayPut(Array array, int i, void *elem){
    assert(array);
    assert(i >= 0 && i < array->length);
    assert(elem);
    //memcpy(array->array + i * array->size, elem, array->size);
    ((void **)array->array)[i] = elem;
    return elem;
}

int arrayLength(Array array){
    assert(array);
    return array->length;
}

int arraySize(Array array){
    assert(array);
    return array->size;
}

void arrayResize(Array array, int length){
    assert(array);
    assert(length >= 0);
    if(array->length == 0){ //如果原来数组长度为0
        array->array = ALLOC(length * array->size);
    }else if(length > 0){   //如果是要调整为其他长度
        RESIZE(array->array, length * array->size);
    }else{  //如果传的length为0
        FREE(array->array);
    }
    array->length = length;
}

Array arrayCopy(Array array, int length){
    assert(array);
    assert(length >= 0);
    Array copy = arrayNew(length, array->size);
    if((copy->length >= array->length) && (array->length > 0)){
        memcpy(copy->array, array->array, array->length*array->size);   //原书代码第3个参数竟然写错了。。。
    }else if((array->length >= copy->length) && (copy->length > 0)){
        memcpy(copy->array, array->array, copy->length*copy->size);    //原书代码第3个参数竟然写错了。。。
    }
    return copy;
}