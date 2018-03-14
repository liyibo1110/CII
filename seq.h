#ifndef SEQ_H_
#define SEQ_H_

#include "array.h"

typedef struct Seq_T *Seq;
struct Seq_T{
    struct Array_T array;   //注意并不是指针，是实体
    int length;
    int head;
};

Seq seqNew(int hint);
Seq seqSeq(void *x, ...);
void seqFree(Seq *seq);
int seqLength(Seq seq);

void *seqGet(Seq seq, int i);
void *seqPut(Seq seq, int i, void *x);
void *seqAddlo(Seq seq, void *x);
void *seqAddhi(Seq seq, void *x);
void *seqRemlo(Seq seq);
void *seqRemhi(Seq seq);

#endif