#ifndef SET_H_
#define SET_H_

struct Set_T{
    int size;
    int (*cmp)(const void *x, const void *y);
    unsigned (*hash)(const void *x);
    int length;
    unsigned timestamp;
    struct member{
        struct member *link;
        const void *member;
    } **buckets;
};
typedef struct Set_T *Set;

Set setNew(int hint,
               int (*cmp)(const void *x, const void *y),
               unsigned (*hash)(const void *x));
void setFree(Set *set);
int setLength(Set set);
int setMember(Set set, const void *member);
void setPut(Set set, const void *member);
void *setRemove(Set set, const void *member);
void setMap(Set set,
            void apply(const void *member, void *cl),
            void *cl);
void **setToArray(Set set, void *end);

Set setUnion(Set s, Set t);
Set setInter(Set s, Set t);
Set setMinus(Set s, Set t);
Set setDiff(Set s, Set t);
#endif