#ifndef TABLE_H_
#define TABLE_H_

struct Table_T{
    int size;
    int (*cmp)(const void *x, const void *y);
    unsigned (*hash)(const void *key);
    struct bindings{
        struct bindings *link;
        const void *key;
        void *value;
    } **buckets;
};
typedef struct Table_T *Table;

Table tableNew(int hint,
               int (*cmp)(const void *x, const void *y),
               unsigned (*hash)(const void *key));
void tableFree(Table table);
int tableLength(Table table);
void *tablePut(Table table, const void *key, void *value);
void *tableGet(Table table, const void *key);
void *tableRemove(Table table, const void *key);
void tableMap(Table table,
              void apply(const void *key, const void **value, void *cl),
              void *cl);
void **tableToArray(Table table, void *end);
#endif