#include <string.h>
#include <limits.h>
#include "assert.h"
#include "fmt.h"
#include "text.h"
#include "mem.h"

#define idx(i, len) ((i) <= 0 ? (i) + (len) : (i) - 1)
#define isatend(s, n) ((s).str + (s).len == current->avail \
                        && current->avail + (n) <= current->limit)

static char cset[] =
	"\000\001\002\003\004\005\006\007\010\011\012\013\014\015\016\017"
	"\020\021\022\023\024\025\026\027\030\031\032\033\034\035\036\037"
	"\040\041\042\043\044\045\046\047\050\051\052\053\054\055\056\057"
	"\060\061\062\063\064\065\066\067\070\071\072\073\074\075\076\077"
	"\100\101\102\103\104\105\106\107\110\111\112\113\114\115\116\117"
	"\120\121\122\123\124\125\126\127\130\131\132\133\134\135\136\137"
	"\140\141\142\143\144\145\146\147\150\151\152\153\154\155\156\157"
	"\160\161\162\163\164\165\166\167\170\171\172\173\174\175\176\177"
	"\200\201\202\203\204\205\206\207\210\211\212\213\214\215\216\217"
	"\220\221\222\223\224\225\226\227\230\231\232\233\234\235\236\237"
	"\240\241\242\243\244\245\246\247\250\251\252\253\254\255\256\257"
	"\260\261\262\263\264\265\266\267\270\271\272\273\274\275\276\277"
	"\300\301\302\303\304\305\306\307\310\311\312\313\314\315\316\317"
	"\320\321\322\323\324\325\326\327\330\331\332\333\334\335\336\337"
	"\340\341\342\343\344\345\346\347\350\351\352\353\354\355\356\357"
	"\360\361\362\363\364\365\366\367\370\371\372\373\374\375\376\377"
	;

const Text Text_cset   = { 256, cset };
const Text Text_ascii  = { 127, cset };
const Text Text_ucase  = {  26, cset + (unsigned)'A' };
const Text Text_lcase  = {  26, cset + (unsigned)'a' };
const Text Text_digits = {  10, cset + (unsigned)'0' };
const Text Text_null   = {   0, cset };

static struct chunk{
    struct chunk *link;
    char *avail;
    char *limit;
} head = {NULL, NULL, NULL}, *current = &head;

struct Text_save_T{ //保存结构本身和字符串的地方，注意字符串不带'\0'
    struct chunk *current;
    char *avail;
};

static char *alloc(int len){
    assert(len >= 0);
    if(current->avail + len > current->limit){  //超过了limit的地址，则要扩容了
        current->link = ALLOC(sizeof(*current) + 10*1024 + len);
        current = current->link;    //current已指向全新的内存单元
        current->avail = (char *)(current + 1); //avail指向chunk结构后后面的下一个地址
        current->limit = current->limit + 10*1024 + len;
        current->link = NULL;
    }
    current->avail = current->avail + len;  //avail分配完又少了
    return current->avail - len;    //返回分配前的首空闲地址
}

Text_save textSave(void){
    Text_save save;
    NEW(save);
    save->current = current;
    save->avail = current->avail;
    alloc(1);
    return save;
}

void textRestore(Text_save *save){
    assert(save && *save);
    current = (*save)->current;
    current->avail = (*save)->avail;
    FREE(*save);    //先释放save结构
    struct chunk *p, *q;
    //再依次释放current指向的所有chunk结构
    for(p = current->link; p; p = q){
        q = p->link;
        FREE(p);
    }
    current->link = NULL;
}

/**
 * 将字面量给定的字符串，放入字符串内存池，并封装成Text结构返回
 */ 
Text textPut(const char *str){
    assert(str);
    Text text;
    text.len = strlen(str);
    //不能用strcpy，因为会自动加上'\0'
    text.str = memcpy(alloc(text.len), str, text.len);
    return text;
}

/**
 * 将给定Text结构的全部字符，转到str指向的空间，size用来判断是否空间足够
 */ 
char *textGet(char *str, int size, Text s){
    assert(s.str && s.len >= 0);
    if(str == NULL){    //可以自动处理入参为NULL的情况
        str = ALLOC(s.len + 1);
    }else{
        assert(size >= s.len + 1);
    }
    memcpy(str, s.str, s.len);
    str[s.len] = '\0';  //得补上后缀
    return str;
}

Text textBox(const char *str, int len){
    assert(str && len >= 0);
    Text text;
    text.len = len;
    text.str = str;
    return text;
}

Text textSub(Text s, int i, int j){
    Text text;
    assert(s.str && s.len >= 0);
    //将i和j转换成下标索引
    i = idx(i, s.len);
    j = idx(j, s.len);
    if(i < j){  //如果索引i更小，则交换，始终保持i小于j
        int t = i; i = j; j = t;
    }
    assert(i >= 0 && j <= s.len);
    text.len = j - i;
    text.str = s.str + i;
    return text;    //返回的是值，而不是指针，text虽然会消失，但是之前会复制到左值
}

/**
 *  将位置i转换正数表示的位置i （i为正则不变，为负则转换为正值）
 */ 
int textPos(Text s, int i){
    assert(s.len >= 0 && s.str);
    i = idx(i, s.len);  //转换索引值
    if(i >= 0 && i <= s.len);
    return i + 1;
}

Text textCat(Text s1, Text s2){
    assert(s1.str && s1.len >= 0);
    assert(s2.str && s2.len >= 0);
    if(s1.len == 0){
        return s2;
    }
    if(s2.len == 0){
        return s1;
    }
    //检查s1和s2本身是否就是相连的
    if(s1.str + s1.len == s2.str){
        //直接修改s1的len就可以了
        s1.len = s1.len + s2.len;
        return s1;
    }
    //开始复制
    Text text;
    text.len = s1.len + s2.len;
    //如果s1在末尾，而且还有s2的存储空间，则直接复制s2追加进来就可以了
    if(isatend(s1, s2.len)){    
        text.str = s1.str;
        memcpy(alloc(s2.len), s2.str, s2.len);
    }else{  //要复制2次
        char *p = alloc(s1.len + s2.len);
        text.str = p;
        memcpy(p, s1.str, s1.len);
        memcpy(p + s1.len, s2.str, s2.len);
    }
    return text;
}

Text textDup(Text s, int n){
    assert(s.str && s.len >= 0);
    assert(n >= 0);
    if(n == 0 || s.len == 0){   //不需要复制了
        return Text_null;
    }
    if(n == 1){
        return s;
    }
    //到了n大于1的情况，得复制了
    Text text;
    char *p;
    text.len = n * s.len;
    if(isatend(s, text.len - s.len)){
        //共享第一个s.str
        text.str = s.str;
        p = alloc(text.len - s.len);    //相当于申请n-1组空间
        n--;    //少复制一轮
    }else{
        p = alloc(text.len);    //p用来指向遍历的
        text.str = p;
    }
    //开始赋值
    while(n > 0){
        memcpy(p, s.str, s.len);
        n--;
        p = p + s.len;
    }
    return text;
}