#ifndef TEXT_H_
#define TEXT_H_
#include <stdarg.h>
#include <stdlib.h>

typedef struct Text_T{
    int len;
    const char *str;    //并不包含'\0'，而且内容是不可变的
} Text;

typedef struct Text_save_T *Text_save;

//引用各种常量Text，分别保存相应的字符串集合
extern const Text Text_cset;
extern const Text Text_ascii;
extern const Text Text_ucase;
extern const Text Text_lcase;
extern const Text Text_digits;
extern const Text Text_null;

Text textPut(const char *str);
char *textGet(char *str, int size, Text s);
Text textBox(const char *str, int len);

Text textSub(Text s, int i, int j);
int textPos(Text s, int i);
Text textCat(Text s1, Text s2);
Text textDup(Text s, int n);
Text textReverse(Text s);
Text textMap(Text s, const Text *from, const Text *to);
int textCmp(Text s1, Text s2);

int textChr(Text s, int i, int j, int c);
int textRchr(Text s, int i, int j, int c);
int textUpto(Text s, int i, int j, Text set);
int textRupto(Text s, int i, int j, Text set);
int textAny(Text s, int i, Text set);
int textMany(Text s, int i, int j, Text set);
int textRmany(Text s, int i, int j, Text set);

int textFind(Text s, int i, int j, Text str);
int textRfind(Text s, int i, int j, Text str);
int textMatch(Text s, int i, int j, Text str);
int textRmatch(Text s, int i, int j, Text str);

void textFmt(int code, va_list *app, 
            int put(int c, void *cl), void *cl,
            unsigned char flags[], int width, int precision);

Text_save textSave(void);
void textRestore(Text_save *save);
        
#endif