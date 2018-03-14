#ifndef RING_H_
#define RING_H_

typedef struct Ring_T *Ring;
struct Ring_T{
    struct node{
        struct node *llink; //前驱
        struct node *rlink; //后驱
        void *value;
    } *head;
    int length;
};

Ring ringNew(void);
Ring ringRing(void *x, ...);
void ringFree(Ring *ring);
int ringLength(Ring ring);

void *ringGet(Ring ring, int i);
void *ringPut(Ring ring, int i, void *x);
void *ringAdd(Ring ring, int pos, void *x);
void *ringAddlo(Ring ring, void *x);
void *ringAddhi(Ring ring, void *x);
void *ringRemove(Ring ring, int i);
void *ringRemlo(Ring ring);
void *ringRemhi(Ring ring);
void ringRotate(Ring ring, int n);

#endif