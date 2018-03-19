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

/**
 * 在strSub的基础上，将结果集再次过滤，将from里面出现的特定字符，转换成to中同样位置的特定字符
 */ 
char *strMap(const char *s, int i, int j,
             const char *from, const char *to){
    
    static char map[256] = {0}; //每次进来也会重建的，只是免了每次声明

    if(from && to){ //如果传了from和to参数，则开始重建map表元素
        for(unsigned c = 0; c < sizeof(map); c++){
            map[c] = c; //相当于生成了ascii码表
        }
        //开始覆盖特定的替换值
        while(*from && *to){
            map[(unsigned char)*from++] = *to++;
        }
    }else{
        assert(from == NULL && to == NULL && s);
        assert(map['a']);
    }
    //构造完map了开始正常截取
    convert(s, i, j);   //转换成正常索引值，并且i小于j
    char *str = ALLOC(j - i + 1);
    char *p = str;  //str是用来最终返回的，p是用来移动赋值的
    while(i < j){
        *p++ = map[(unsigned char)s[i++]];
    }
    *p = '\0';
    return str;
}

/**
 * 就是将负的i值，转换为正的i值（如果i为正数，则不变）
 */ 
int strPos(const char *s, int i){
    assert(s);
    int len = strlen(s);
    i = idx(i, len);
    assert(i >= 0 && i <= len);
    return i + 1;
}

/**
 * 返回截取后字符串的长度（只算长度，并不真截取）
 */ 
int strLen(const char *s, int i, int j){
    convert(s, i, j);   //先得转换成标准的i小于j的值
    return j - i;
}

int strCmp(const char *s1, int i1, int j1,
           const char *s2, int i2, int j2){
    //先转换成标准的i和j值
    convert(s1, i1, j1);
    convert(s2, i2, j2);
    //将s1和s2移到区间开头，即i的位置，因为之前的不需要比较
    s1 = s1 + i1;
    s2 = s2 + i2;
    //取2组比较索引，较短的来比较
    if(j1 - i1 < j2 - i2){  //如果第一组更短，按第一组的个数比较
        int cond = strncmp(s1, s2, j1 - i1);
        return cond == 0 ? -1 : cond;   //找到则返回-1
    }else if(j1 - i1 > j2 - i2){    //如果第一组更长，按第二组的个数比较
        int cond = strncmp(s1, s2, j2 - i2);
        return cond == 0 ? +1 : cond;   //找到则返回正1
    }else{
        return strncmp(s1, s2, j1 - i1);    //找到则返回0
    }
}

/**
 * 在s的区间里找字符c
 */ 
int strChr(const char *s, int i, int j, int c){
    convert(s, i, j);
    while(i < j){
        if(s[i] == c){
            return i + 1;   //返回的是原始定义的i，而不是索引位
        }
        i++;
    }
    return 0;   //没找到最后返回0
}

int strRchr(const char *s, int i, int j, int c){
    convert(s, i, j);
    while(i < j){
        if(s[--j] == c){    //注意如果反着找，区间是不包括j元素的，所以要先减1
            return j + 1;   //返回的是原始定义的j，而不是索引位
        }
    }
    return 0;   //没找到最后返回0
}

/**
 * 和strChr类似，不同的是要搜索的参照物不是字符，而是字符串，其中每个字符遍历寻找，找到第1个即可
 */ 
int strUpto(const char *s, int i, int j, const char *set){
    assert(set);
    convert(s, i, j);
    while(i < j){
        if(strchr(set, s[i])){  //调的是string库的函数，而不是上面自定义的
            return i + 1;   //返回的是原始定义的i，而不是索引位
        }
        i++;
    }
    return 0;
}

int strRupto(const char *s, int i, int j, const char *set){
    assert(set);
    convert(s, i, j);
    while(i < j){
        if(strchr(set, s[--j])){  //调的是string库的函数，而不是上面自定义的
            return j + 1;   //返回的是原始定义的i，而不是索引位
        }
    }
    return 0;
}

int strFind(const char *s, int i, int j, const char *str){
    assert(str);
    convert(s, i, j);
    int len = strlen(str);
    if(len == 0){   //str为'\0'
        return i + 1;   //如果str为空，则直接返回入参i的位置
    }else if(len == 1){ //str只有1个有效字符，则当做strChr一样的方式
        while(i < j){
            if(s[i] == *str){
                return i + 1;   //返回的是原始定义的i，而不是索引位
            }
            i++;
        }
    }else{  //str大于1个有效字符，则要循环遍历
        while(i + len < j){ //要加上len，因为剩下的字符串不够len位，也就不用找了
            if(strncmp(&s[i], str, len) == 0){  //比较前len个，找到则返回，找不到则推进1位
                return i + 1;
            }
        }
    }   
    return 0;   //没找到则返回0
}

int strRfind(const char *s, int i, int j, const char *str){
    assert(str);
    convert(s, i, j);
    int len = strlen(str);
    if(len == 0){   //str为'\0'
        return j + 1;   //如果str为空，则直接返回入参j的位置
    }else if(len == 1){ //str只有1个有效字符，则当做strRchr一样的方式
        while(i < j){
            if(s[--j] == *str){
                return j + 1;   //返回的是原始定义的j，而不是索引位
            }
        }
    }else{  //str大于1个有效字符，则要循环遍历
        while(i <= j - len){ //j要减去len，因为剩下的字符串不够len位，也就不用找了
            if(strncmp(&s[j-len], str, len) == 0){  //比较前len个，找到则返回，找不到则推进1位
                return j - len + 1;
            }
            j--;
        }
    }   
    return 0;   //没找到则返回0
}

int strAny(const char *s, int i, const char *set){
    assert(s);
    assert(set);
    int len = strlen(s);
    assert(i >= 0 && i <= len);
    if(i < len && strchr(set, s[i])){
        return i + 2;   //如果找到，则返回i的原始值，然后再加1（就是一共加2了）
    }
    return 0;   //找不到或者i值过大，则返回0
}

int strMany(const char *s, int i, int j, const char *set){
    assert(set);
    convert(s, i, j);
    if(i < j && strchr(set, s[i])){ //先确定区间首字符在set中是否存在，存在才会继续
        do{
            i++;
        }while(i < j && strchr(set, s[i]));
        return i + 1;
    }
    return 0;
}

int strRmany(const char *s, int i, int j, const char *set){
    assert(set);
    convert(s, i, j);
    if(i < j && strchr(set, s[j-1])){ //先确定区间尾字符在set中是否存在，存在才会继续
        do{
            --j;
        }while(i <= j && strchr(set, s[j]));
        return i + 2;
    }
    return 0;
}

int strMatch(const char *s, int i, int j, const char *str){
    assert(str);
    convert(s, i, j);
    int len = strlen(s);
    if(len == 0){
        return i + 1;
    }else if(len == 1){
        if(i < j && s[i] == *str){
            return i + 2;
        }
    }else if(i + len <= j && strncmp(&s[i], str, len) == 0){
        return i + len + 1;
    }
    return 0;
}

int strRmatch(const char *s, int i, int j, const char *str){
    assert(str);
    convert(s, i, j);
    int len = strlen(s);
    if(len == 0){
        return j + 1;
    }else if(len == 1){
        if(i < j && s[j-1] == *str){
            return j;
        }
    }else if(i <= j - len && strncmp(&s[j-len], str, len) == 0){
        return j - len + 1;
    }
    return 0;
}

void strFmt(int code, va_list *app, 
            int put(int c, void *cl), void *cl,
            unsigned char flags[], int width, int precision){
    char *s;
    int i, j;
    assert(app && flags);
    s = va_arg(*app, char *);
    i = va_arg(*app, int);
    j = va_arg(*app, int);
    convert(s, i, j);
    fmtPuts(s + i, j - i, put, cl, flags, width, precision);
}