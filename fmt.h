#ifndef FMT_H_
#define FMT_H_

#include <stdarg.h>
#include <stdio.h>
#include "except.h"

typedef void (*Fmt_T)(int code, va_list *app,
                    int put(int c, void *cl), void *cl,
                    unsigned char flags[256], int width, int precision);

extern char *Fmt_flags;
extern const Except_T Fmt_Overflow;

void fmtFmt(int put(int c, void *cl), void *cl, 
                    const char *fmt, ...);

void fmtVfmt(int put(int c, void *cl), void *cl, 
                    const char *fmt, va_list ap);

void fmtPrint(const char *fmt, ...);
void fmtFprint(FILE *stream, const char *fmt, ...);
int fmtSfmt(char *buf, int size, const char *fmt, ...);
int fmtVsfmt(char *buf, int size, const char *fmt, va_list ap);
char *fmtString(const char *fmt, ...);
char *fmtVstring(const char *fmt, va_list ap);

Fmt_T fmtRegister(int code, Fmt_T cvt);
void fmtPutd(const char *str, int len, 
                int put(int c, void *cl), void *cl,
                unsigned char flags[256], int width, int precision);
void fmtPuts(const char *str, int len, 
                int put(int c, void *cl), void *cl,
                unsigned char flags[256], int width, int precision);

#endif