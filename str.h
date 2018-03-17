#ifndef STR_H_
#define STR_H_
#include <stdarg.h>

char *strSub(const char *s, int i, int j);
char *strDup(const char *s, int i, int j, int n);
char *strCat(const char *s1, int i1, int j1,
             const char *s2, int i2, int j2);
char *strCatv(const char *s, ...);
char *reverse(const char *s, int i, int j);
char *strMap(const char *s, int i, int j,
             const char *from, const char *to);

int strPos(const char *s, int i);
int strLen(const char *s, int i, int j);
int strCmp(const char *s1, int i1, int j1,
           const char *s2, int i2, int j2);

int strChr(const char *s, int i, int j, int c);
int strRchr(const char *s, int i, int j, int c);
int strUpto(const char *s, int i, int j, const char *set);
int strRupto(const char *s, int i, int j, const char *set);
int strFind(const char *s, int i, int j, const char *str);
int strRfind(const char *s, int i, int j, const char *str);

int strAny(const char *s, int i, const char *set);
int strMany(const char *s, int i, int j, const char *set);
int strRmany(const char *s, int i, int j, const char *set);
int strMatch(const char *s, int i, int j, const char *str);
int strRmatch(const char *s, int i, int j, const char *str);

void strFmt(int code, va_list *app, 
            int put(int c, void *cl), void *cl,
            unsigned char flags[], int width, int precision);
#endif