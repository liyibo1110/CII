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

#define pad(n, c) do {  \
                    int nn = (n);   \
                    while(nn-- > 0){    \
                        put((c), cl);   \
                    }   \
                } while(0)

const Except_T Fmt_Overflow = {"Formatting Overflow"};

char *Fmt_flags = "-+ 0";

struct buf{
    char *buf;
    char *bp;
    int size;
};

static int outc(int c, void *cl){
    FILE *f = cl;   //要封装成特定的FILE类型
    return putc(c, f);  //实际调用库函数putc
}

static int insert(int c, void *cl){
    //cl是个buf类型的结构体
    struct buf *p = cl;
    //在往内部的bp逐一赋值过程中，每次都要检查实际存的是否已超出了最早的限制
    if(p->bp >= p->buf + p->size){
        RAISE(Fmt_Overflow);
    }
    *p->bp++ = c;   //实际是存到了bp变量里
    return c;
}

static int append(int c, void *cl){
    struct buf *p = cl;
    /**
     * 在往内部的bp逐一赋值过程中，每次都要检查实际存的是否已超出了最早的限制
     * 和insert不同的是，如果超过了，这里会扩容2倍
     */ 
    if(p->bp >= p->buf + p->size){
        RESIZE(p->buf, 2*p->size);  //空间加倍，记住是buf记录了字符串起始地址，bp是会变的
        p->bp = p->buf + p->size;   //还要重新定位bp，因为RESIZE以后，buf地址可能完全变了
        p->size *= 2;   //移动完了bp，size才能真正加倍，不然bp就可能指丢了
    }
    *p->bp++ = c;   //实际是存到了bp变量里
    return c;
}

/**
 * 处理s描述符（字符串）
 */ 
static void cvt_s(int code, va_list *app, 
                int put(int c, void *cl), void *cl, 
                unsigned char flags[], int width, int precision){
    
    //从app里取出实际的字符串值，传进来的app是要每次迭代一下共用的，所以必须传指针
    char *str = va_arg(*app, char *);
    assert(str);
    fmtPuts(str, strlen(str), put, cl, flags, width, precision);
}

/**
 * 处理d描述符（整数）
 */ 
static void cvt_d(int code, va_list *app, 
                int put(int c, void *cl), void *cl, 
                unsigned char flags[], int width, int precision){
    
    //从app里取出实际的字符串值，传进来的app是要每次迭代一下共用的，所以必须传指针
    int val = va_arg(*app, int);
    unsigned m; //用来另存为val，val要留着最后判断符号
    char buf[43];
    char *p = buf + sizeof(buf);    //指向buf的尾部
    if(val == INT_MIN){ //处理极值
        m = INT_MAX + 1U;
    }else if(val < 0){
        m = -val;   //如果是负的，先改成正的
    }else{
        m = val;
    }
    do{
        *--p = m % 10 + '0';
    } while ((m /= 10) > 0);
    if(val < 0){
        *--p = '-';
    }
    //以上就是把int类型的变量val，转换成char *类型的过程
    fmtPutd(p, buf + sizeof(buf) - p, put, cl, flags, width, precision);
}

/**
 * 处理u描述符（无符号整数）
 */ 
static void cvt_u(int code, va_list *app, 
                int put(int c, void *cl), void *cl, 
                unsigned char flags[], int width, int precision){
    
    //从app里取出实际的字符串值，传进来的app是要每次迭代一下共用的，所以必须传指针
    unsigned m = va_arg(*app, unsigned);
   
    char buf[43];
    char *p = buf + sizeof(buf);    //指向buf的尾部
    do{
        *--p = m % 10 + '0';
    } while ((m /= 10) > 0);
    //以上就是把uint类型的变量m，转换成char *类型的过程，不需要处理符号了，省了不少代码
    fmtPutd(p, buf + sizeof(buf) - p, put, cl, flags, width, precision);
}

/**
 * 处理o描述符（八进制无符号整数）
 */ 
static void cvt_o(int code, va_list *app, 
                int put(int c, void *cl), void *cl, 
                unsigned char flags[], int width, int precision){
    
    //从app里取出实际的字符串值，传进来的app是要每次迭代一下共用的，所以必须传指针
    unsigned m = va_arg(*app, unsigned);
    char buf[43];
    char *p = buf + sizeof(buf);    //指向buf的尾部
    do{
        *--p = (m&0x7) + '0';
    } while ((m >>= 3) != 0);
    //以上就是把八进制uint类型的变量m，转换成char *类型的过程
    fmtPutd(p, buf + sizeof(buf) - p, put, cl, flags, width, precision);
}

/**
 * 处理x描述符（十六进制无符号整数）
 */ 
static void cvt_x(int code, va_list *app, 
                int put(int c, void *cl), void *cl, 
                unsigned char flags[], int width, int precision){
    
    //从app里取出实际的字符串值，传进来的app是要每次迭代一下共用的，所以必须传指针
    unsigned m = va_arg(*app, unsigned);
    char buf[43];
    char *p = buf + sizeof(buf);    //指向buf的尾部
    do{
        //等价于static char digits[] = "0123456789abcdef";  *p++ = digits[m&0xf]
        *--p = "0123456789abcdef"[m&0xf];
    } while ((m >>= 4) != 0);
    //以上就是把十六进制uint类型的变量m，转换成char *类型的过程
    fmtPutd(p, buf + sizeof(buf) - p, put, cl, flags, width, precision);
}

/**
 * 处理p描述符（十六进制指针地址）
 */ 
static void cvt_p(int code, va_list *app, 
                int put(int c, void *cl), void *cl, 
                unsigned char flags[], int width, int precision){
    
    //从app里取出实际的字符串值，传进来的app是要每次迭代一下共用的，所以必须传指针
    unsigned long m = (unsigned long)va_arg(*app, void *);
    char buf[43];
    char *p = buf + sizeof(buf);    //指向buf的尾部
    do{
        *--p = "0123456789abcdef"[m&0xf];
    } while ((m >>= 4) != 0);
    //以上就是把十六进制指针类型的变量m，转换成char *类型的过程
    precision = INT_MIN;    //精度强制改成默认，p格式不支持传来的精度值
    fmtPutd(p, buf + sizeof(buf) - p, put, cl, flags, width, precision);
}

/**
 * 处理c描述符（单个字符）
 */ 
static void cvt_c(int code, va_list *app, 
                int put(int c, void *cl), void *cl, 
                unsigned char flags[], int width, int precision){
    
    //和别的不太一样，要先处理宽度
    if(width == INT_MIN){   //没传就为0
        width = 0;
    }
    if(width < 0){  //如果传来的宽度为负，则转正，但是要记录
        flags['-'] = 1;
        width = -width;
    }
    if(!flags['-']){    //如果没有负号标记，则先填充空格
        pad(width - 1, ' ');    //单个字符只有1位，定死的
    }
    //从app里取出实际的字符串值，传进来的app是要每次迭代一下共用的，所以必须传指针
    put((unsigned char)va_arg(*app, int), cl);  //直接输出单个字符
    if(flags['-']){     //如果有符号标记，则在后面填充空格
        pad(width - 1, ' ');
    }
}

/**
 * 处理f描述符（浮点数）
 */ 
static void cvt_f(int code, va_list *app, 
                int put(int c, void *cl), void *cl, 
                unsigned char flags[], int width, int precision){

    char buf[DBL_MAX_10_EXP+1+1+99+1];  //最长的形态为%.99f
    if(precision < 0){  //为负给默认值6
        precision = 6;  
    }
    if(code == 'g' && precision == 0){
        precision = 1;
    }

    static char fmt[] = "%.dd?";
    assert(precision <= 99);
    fmt[4] = code;
    fmt[3] = precision % 10 + '0';
    fmt[2] = (precision / 10) % 10 + '0';
    sprintf(buf, fmt, va_arg(*app, double));

    fmtPutd(buf, strlen(buf), put, cl, flags, width, precision);
}

static Fmt_T cvt[256] = {
 /*   0-  7 */ 0,     0, 0,     0,     0,     0,     0,     0,
 /*   8- 15 */ 0,     0, 0,     0,     0,     0,     0,     0,
 /*  16- 23 */ 0,     0, 0,     0,     0,     0,     0,     0,
 /*  24- 31 */ 0,     0, 0,     0,     0,     0,     0,     0,
 /*  32- 39 */ 0,     0, 0,     0,     0,     0,     0,     0,
 /*  40- 47 */ 0,     0, 0,     0,     0,     0,     0,     0,
 /*  48- 55 */ 0,     0, 0,     0,     0,     0,     0,     0,
 /*  56- 63 */ 0,     0, 0,     0,     0,     0,     0,     0,
 /*  64- 71 */ 0,     0, 0,     0,     0,     0,     0,     0,
 /*  72- 79 */ 0,     0, 0,     0,     0,     0,     0,     0,
 /*  80- 87 */ 0,     0, 0,     0,     0,     0,     0,     0,
 /*  88- 95 */ 0,     0, 0,     0,     0,     0,     0,     0,
 /*  96-103 */ 0,     0, 0, cvt_c, cvt_d, cvt_f, cvt_f, cvt_f,
 /* 104-111 */ 0,     0, 0,     0,     0,     0,     0, cvt_o,
 /* 112-119 */ cvt_p, 0, 0, cvt_s,     0, cvt_u,     0,     0,
 /* 120-127 */ cvt_x, 0, 0,     0,     0,     0,     0,     0
};

void fmtPutd(const char *str, int len, 
                int put(int c, void *cl), void *cl,
                unsigned char flags[256], int width, int precision){
    assert(str);
    assert(len >= 0);
    assert(flags);
    //处理width和flags
    if(width == INT_MIN){   //没传就为0
        width = 0;
    }
    if(width < 0){  //如果传来的宽度为负，则转正，但是要记录
        flags['-'] = 1;
        width = -width;
    }
    //处理precision
    if(precision >= 0){
        flags['0'] = 0; //如果传了有效精度值，flags里的0标记直接失效
    }
    //开始计算sign（正负号）
    int sign;
    if(len > 0 && (*str == '-' || *str == '+')){
        sign = *str++;
        len--;
    }else if(flags['+']){
        sign = '+';
    }else if(flags[' ']){
        sign = ' ';
    }else{
        sign = 0;
    }
    //开始计算最终输出位数n
    int n;
    if(precision < 0){  //精度为负
        precision = 1;
    }
    if(len < precision){    //实际不够精度长度，则扩展成精度长度
        len = precision;
    }else if(precision == 0 && len == 1 && str[0] == '0'){  //如果传入为数字0，精度也为0，则不输出
        n = 0;
    }else{
        n = len;
    }
    if(sign){   //如果符号位
        n++;
    }
    //开始输出宽度相关
    if(flags['-']){ //是否输出符号位
        if(sign){
            put(sign, cl);
        }
        
    }else if(flags['0']){   //如果定义了0占位
        if(sign){
            put(sign, cl);
        }
        pad(width - len, '0');
    }else{
        pad(width - len, ' ');  //如果定义了宽度，则尝试
        if(sign){   //如果有符号则输出，注意sign始终只有1个，上面if到这里也只会输出1组数据
            put(sign, cl);
        }
    }
    //如果精度大于实际的长度，则先输出0占位
    pad(precision - len, '0');
    //输出正式数据
    for(int i=0; i<len; i++){   //输出正式的数字字符
        put((unsigned char)*str++, cl);
    }
    if(flags['-']){
        pad(width - n, ' ');
    }
}

void fmtPuts(const char *str, int len, 
                int put(int c, void *cl), void *cl,
                unsigned char flags[256], int width, int precision){
    assert(str);
    assert(len >= 0);
    assert(flags);
    //处理width和flags
    if(width == INT_MIN){   //没传就为0
        width = 0;
    }
    if(width < 0){  //如果传来的宽度为负，则转正，但是要记录
        flags['-'] = 1;
        width = -width;
    }
    //处理precision
    if(precision >= 0){
        flags['0'] = 0; //如果传了有效精度值，flags里的0标记直接失效
    }
    if(precision >= 0 && precision < len){
        len = precision;    //输出参数不得大于precision（如果给定了精度）
    }
    if(!flags['-']){    //如果宽度非负，则需要填充
        pad(width - len, ' ');
    }
    for(int i=0; i<len; i++){   //输出正式的字符串
        put((unsigned char)*str++, cl);
    }
    if(flags['-']){    //如果宽度是负的，则需要填充后面的空间
        pad(width - len, ' ');
    }
}

void fmtFmt(int put(int c, void *cl), void *cl, 
                    const char *fmt, ...){
    //把可变参数封装成va_list就可以了，在fmtFfmt里面再去assert
    va_list ap;
    va_start(ap, fmt);
    fmtVfmt(put, cl, fmt, ap);
    va_end(ap);
}

void fmtPrint(const char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    fmtVfmt(outc, stdout, fmt, ap);
    va_end(ap);
}

void fmtFprint(FILE *stream, const char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    fmtVfmt(outc, stream, fmt, ap);
    va_end(ap);
}

int fmtSfmt(char *buf, int size, const char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    int len = fmtVsfmt(buf, size, fmt, ap);
    va_end(ap);
    return len;
}

int fmtVsfmt(char *buf, int size, const char *fmt, va_list ap){
    assert(buf);
    assert(size > 0);
    assert(fmt);
    struct buf cl;
    //给结构体赋值，用结构体只是为了能检查是否越界，因为put函数定死了形参，cl必须得是个复合类型
    cl.buf = buf;
    cl.bp = buf;
    cl.size = size;
    fmtVfmt(insert, &cl, fmt, ap);
    insert(0, &cl); //还必须要追加空字符串结尾'/0'
    return cl.bp - cl.buf - 1;  //返回的是放进去的字符串长度（相当于strlen）
}

char *fmtString(const char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    char *str = fmtVstring(fmt, ap);
    va_end(ap);
    return str;
}

/**
 * 和fmtVsfmt相比，就是不传要存的地方，内部自己动态生成一个新空间存放
 */ 
char *fmtVstring(const char *fmt, va_list ap){
    assert(fmt);
    struct buf cl;
    cl.size = 256;  //初始给256个字节
    cl.buf = ALLOC(cl.size);
    cl.bp = ALLOC(cl.size);
    fmtVfmt(append, &cl, fmt, ap);
    append(0, &cl); //还必须要追加空字符串结尾'/0'
    return RESIZE(cl.buf, cl.bp - cl.buf);  //返回新生成动态字符串，注意最后还有一步修剪，没有浪费空间
}

void fmtVfmt(int put(int c, void *cl), void *cl, 
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
            //在字符->处理函数数据表里中寻找相应的处理函数
            assert(cvt[c]); //必须存在
            //最后调用处理函数
            (*cvt[c])(c, &ap, put, cl, flags, width, precision);
        }
    }
}

Fmt_T fmtRegister(int code, Fmt_T newcvt){
    Fmt_T old;
    assert(0 < code && code < (int)(sizeof(cvt)/sizeof(cvt[0])));
    old = cvt[code];    //原来旧的转换函数给old，用来返回
    cvt[code] = newcvt; //新的转换函数塞到函数表里  
    return old;
}