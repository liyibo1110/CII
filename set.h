#ifndef SET_H_
#define SET_H_

struct Set_T{
    int size;   //多少个桶，构建好以后就是固定不变的数目了
    int (*cmp)(const void *x, const void *y);
    unsigned (*hash)(const void *x);
    int length; //多少个存储实体,新增则加1，删除则减1，会动态变化
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