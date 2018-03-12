#include "arith.h"

int arithMax(int x, int y){
    return x > y ? x : y;
}

int arithMin(int x, int y){
    return x > y ? y : x;
}

/**
 * 针对不同平台做的强制统一
 * 只是为了处理操作符号相反的特例
 */ 
int arithDiv(int x, int y){
    //操作数相反，而且不能整除的2个int相除，商值要偏向于左侧
    if(-13/5 == -2 && (x < 0) != (y < 0) && x%y != 0){
        return x/y - 1;
    }else{  //否则按照默认的除法计算
        return x/y;
    }
}

int arithMod(int x, int y){
    //操作数相反，而且不能整除的2个int相除，商值要偏向数轴左侧
    if(-13/5 == -2 && (x < 0) != (y < 0) && x%y != 0){
        return x%y + y;
    }else{  //否则按照默认的除法计算
        return x%y;
    }
}

/**
 * 即除完以后，往大了取整
 */ 
int arithCeiling(int x, int y){
    return arithDiv(x, y) + (x%y != 0);
}

/**
 * 和除法一样，如符号相同则去掉尾数，符号不同要偏向数轴左侧
 */ 
int arithFloor(int x, int y){
    return arithDiv(x, y);
}