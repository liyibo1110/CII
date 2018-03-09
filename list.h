#ifndef LIST_H_
#define LIST_H_

struct List_T{
    void *first;
    struct List_T *rest;
};
typedef struct List_T *List;

List listList(void *x, ...);
List listPush(List list, void *x);
List listPop(List list, void *x);
List listReverse(List list);
List listCopy(List list);
List listAppend(List list, List tail);
int listLength(List list);
void listFree(List list);
void listMap(List list, 
    void *apply(void *x, void *cl), void *cl);
void **listToArray(List list, void *end);

#endif