#ifndef EXCEPT_H_
#define EXCEPT_H_

#include <setjmp.h>
    
    //定义异常对象，只能放到全局环境
    typedef struct Except_T{
        char *reason;
    } Except_T;

    struct Except_Frame{
        Except_Frame *prev;
        jmp_buf env;
        const char *file;
        int line;
        const Except_T *exception;
    };
    typedef struct Except_Frame Except_Frame;

    enum{
        exceptEntered = 0,  //迎合setjmp第一次调用的返回值（也为0）
        exceptRaised,
        exceptHandled,
        exceptFinalized
    };

    //全局变量，代表当前栈顶部的异常帧
    extern Except_Frame *exceptStack;
    //函数
    void exceptRaise(const Except_T *e, const char *file, int line);
    //宏
    #define RAISE(e) exceptRaise(&(e), __FILE__, __LINE__)
    #define RERAISE exceptRaise(exceptFrame.exception, exceptFrame.file, exceptFrame.line)

    #define TRY do {    \
        volatile int exceptFlag;    \
        Except_Frame exceptFrame;   \
        exceptFrame.prev = &exceptStack;    \
        exceptStack = &exceptFrame; \
        exceptFlag = setjmp(exceptFrame.env);   \
        if(exceptFlag == exceptEntered) {

    #define EXCEPT(e) if(exceptFlag == exceptEntered){  \
                           exceptStack = exceptStack->prev;  \
                        }   \
        } else if(exceptFrame.exception == &(e)) {  \
            exceptFlag = exceptHandled;
    
    #define ELSE if(exceptFlag == exceptEntered){  \
                           exceptStack = exceptStack->prev;  \
                        }   \
        } else {  \
            exceptFlag = exceptHandled;

    #define END_TRY if(exceptFlag == exceptEntered) {   \
                        exceptStack = exceptStack->prev; \
                    }   \
        }   \
        if(exceptFlag == exceptRaise) {  \
           RERAISE;  \
        }   \
    } while(0)

    #define FINALLY if(exceptFlag == exceptEntered){  \
                           exceptStack = exceptStack->prev;  \
                        }   \
        }   \
            {   \
                if(exceptFlag == exceptEntered){    \
                    exceptFlag = exceptFinalized;   \
                }   

    //如果在TRY-END_TRY结构中return，则必须先弹出TRY弄进去的异常帧
    #define RETURN switch(exceptStack = exceptStack->prev,0) default: return
            
           
#endif