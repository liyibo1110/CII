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
            int width = INT_MIN;    //默认为最小值
            int precision = INT_MIN;    //默认为最小值
            memset(flags, '\0', sizeof(flags)); //填充为空字符串
            //开始处理flags
            if(Fmt_flags){
                unsigned char c = *fmt; //就是%了
                while(c && strchr(Fmt_flags, c)){
                    //说明是Fmt_flags里面的字符，需要和%连接在一起来使用
                    assert(flags[c] < 255);
                    flags[c]++; //只是加1
                    c = *++fmt; //读下一个字符
                }
            }
            //开始处理宽度，查看是否跟着*或者数字
            if(*fmt == '*'){    //*代表宽度定义结束
                width = va_arg(ap, int);    //如果定义了宽度描述符，第一个变参肯定是宽度值
                assert(width != INT_MIN);
                fmt++;  //移到下一个字符
            }
            if(isdigit(*fmt)){
                int n = 0;
                while(isdigit(*fmt)){  //数字不止给定1位，所以要循环按位累加
                    int d = *fmt - '0'; //将char数字转换成int（即当前数字的ascii - 数字0的ascii）
                    assert(n <= (INT_MAX - d) / 10);    //其实就是最终的n不能大于INT_MAX，只是不能真去算下一步而已，因为此时已经溢出
                    n = 10 * n + d; //累加技巧，利用局部变量n
                    fmt++;
                }
                if(n != 0){
                    width = n;
                }
            }
            //开始处理精度，查看是否跟着.（点号）同时后面是*号或者数字
            if(*fmt == '.' && (*++fmt == '*' || isdigit(*fmt))){
                int n = 0;
                if(*fmt == '*'){
                    n = va_arg(ap, int);    //如果定义了宽度描述符，第一个变参肯定是宽度值
                    assert(n != INT_MIN);
                    fmt++;  //移到下一个字符
                }else{  //到这里肯定就是数字了
                    while(isdigit(*fmt)){
                        int d = *fmt - '0';
                        assert(n <= (INT_MAX - d) / 10);
                        n = 10 * n + d;
                        fmt++;
                    }
                }
                precision = n;
            }
            //继续处理，该处理变量类型的字符了，例如d、f、s、p之类的
            c = *fmt++;
        }
    }
}