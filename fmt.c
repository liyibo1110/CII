#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <ctype.h>
#include <math.h>
#include "mem.h"
#include "assert.h"
#include "except.h"
#include "fmt.h"

const Except_T Fmt_Overflow = {"Formatting Overflow"};

char *Fmt_flags = "-+ 0";

void fmtFfmt(int put(int c, void *cl), void *cl, 
                    const char *fmt, va_list ap){
    assert(put);
    assert(fmt);
    while(*fmt){    //开始遍历fmt字符串，返回每一个char字符
        if(*fmt != '%' || *++fmt == '%'){   //不为单独的%才直接输出，%%这样的字符串片段会输出1个%
            //调用传入的put，用来输出
            put((unsigned char)*fmt++, cl);
        }else{  //开始处理格式
            unsigned char c;
            unsigned char flags[256];   //ascii位图，记录每个字符出现的次数
            int width = INT_MIN;
            int precision = INT_MIN;
            memset(flags, '\0', sizeof(flags)); //填充为空字符串
            //开始处理flags
            if(Fmt_flags){
                unsigned char c = *fmt; //就是%了
                while(c && strchr(Fmt_flags, c)){
                    //说明是Fmt_flags里面的字符，需要和%连接在一起来使用
                    assert(flags[c] < 255);
                    flags[c]++;
                    c = *++fmt; //读下一个字符
                }
            }
        }
    }
}