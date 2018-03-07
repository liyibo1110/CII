#ifndef ARENA_H_
#define ARENA_H_

#include "except.h"


typedef struct Arena_T *Arena;
struct Arena_T{
    Arena prev;
    char *avail;
    char *limit;
};

extern const Except_T Arena_NewFailed;
extern const Except_T Arena_Failed;

Arena arenaNew(void);
void arenaDispose(Arena ap);
void *arenaAlloc(Arena arena, long nbytes, const char *file, int line);
void *arenaCalloc(Arena arena, int count, long nbytes, const char *file, int line);
void arenaFree(Arena arena);
#endif