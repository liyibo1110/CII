#ifndef MEM_H_
#define MEM_H_

#include "except.h"
//引用全局变量
extern const Except_T Mem_failed;

void *memAlloc(long nbytes, const char *file, int line);
void *memCalloc(long count, long nbytes, const char *file, int line);
void memFree(void *ptr, const char *file, int line);
void *memResize(void *ptr, long nbytes, const char *file, int line);

#define ALLOC(nbytes) memAlloc((nbytes), __FILE__, __LINE__)
#define CALLOC(count, nbytes) memAlloc((count), (nbytes), __FILE__, __LINE__)
#define NEW(p) ((p) = ALLOC((long)sizeof *(p)))
#define NEWO(p) ((p) = CALLOC(1, (long)sizeof *(p)))
#define FREE(ptr) ((void)memFree((ptr), __FILE__, __LINE__), (ptr) = 0)
#define RESIZE(ptr, nbytes) ((ptr) = memResize((ptr), (nbytes), __FILE__, __LINE__))
#endif