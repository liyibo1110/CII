#include "str.h"
#include "mem.h"
#include "assert.h"
#include "fmt.h"
#include <string.h>
#include <limits.h>

#define idx(i, len) ((i) <= 0 ? (i) + (len) : (i) - 1)
#define convert(s, i, j) do {   \
                            assert(s);  \
                            int len = strlen(s);    \
                            i = idx(i, len);    \
                            j = idx(j, len);    \
                            if(i > j) {int t = i; i = j; j = t;}    \
                            assert(i >= 0 && j <= len);   \
                        } while(0)

char *strSub(const char *s, int i, int j){
    convert(s, i, j);   //转换成正常索引值，并且i小于j
    char *str = ALLOC(j - i + 1);
    char *p = str;  //str是用来最终返回的，p是用来移动赋值的
    while(i < j){
        *p++ = s[i++];
    }
    *p = '\0';
    return str;
}

/**
 * 和strSub不同之处，只是最后区间反向字符串
 */ 
char *reverse(const char *s, int i, int j){
    convert(s, i, j);   //转换成正常索引值，并且i小于j
    char *str = ALLOC(j - i + 1);
    char *p = str;  //str是用来最终返回的，p是用来移动赋值的
    while(i < j){
        *p++ = s[j--];  //只有这里不同
    }
    *p = '\0';
    return str;
}

/**
 * 将区间字符串重复n次然后返回
 */ 
char *strDup(const char *s, int i, int j, int n){
    convert(s, i, j);   //转换成正常索引值，并且i小于j
    char *str = ALLOC(n * (j - i) + 1);
    char *p = str;  //str是用来最终返回的，p是用来移动赋值的
    if(i < j){
        while(n-- > 0){
            for(int k = i; k < j; k++){
                *p++ = s[i++];
            }
        } 
    }
    *p = '\0';
    return str;
}

char *strCat(const char *s1, int i1, int j1,
             const char *s2, int i2, int j2){
    convert(s1, i1, j1);
    convert(s2, i2, j2);
    char *str = ALLOC(j1 - i1 + j2 - i2 + 1);
    char *p = str;
    while(i1 < j1){
        *p++ = s1[i1++];
    }
    while(i2 < j2){
        *p++ = s2[i2++];
    }
    *p = '\0';
    return str;
}

/**
 * 即strCat多参数版本，strCat只能有2套参数，这个可以更多
 */ 
char *strCatv(const char *s, ...){
    const char *save = s;
    int i, j;
    int len = 0;
    va_list ap;
    va_start(ap, s);
    //计算len值
    while(s){
        i = va_arg(ap, int);
        j = va_arg(ap, int);
        convert(s, i, j);
        len = len + j - i;
        s = va_arg(ap, const char *); //尝试取下一组开头的字符串参数
    }
    va_end(ap);
    char *str = ALLOC(len + 1); //上一轮遍历参数，只是为了算出所需空间长度
    char *p = str;
    s = save;   //将最初的s参数还回去，重新开始

    //第二轮遍历
    va_start(ap, s);
    while(s){
        i = va_arg(ap, int);
        j = va_arg(ap, int);
        convert(s, i, j);
        while(i < j){
            *p++ = s[i++];
        }
        s = va_arg(ap, const char *);
    }
    va_end(ap);
    *p = '\0';
    return str;    
}