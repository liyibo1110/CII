#ifndef ARRAY_H_
#define ARRAY_H_

typedef struct Array_T *Array;   

Array arrayNew(int length, int size);
void arrayFree(Array *array);
int arrayLength(Array array);
int arraySize(Array array);
void *arrayGet(Array array, int i);
void *arrayPut(Array array, int i, void *elem);
void arrayResize(Array array, int length);
Array arrayCopy(Array array, int length);
#endif