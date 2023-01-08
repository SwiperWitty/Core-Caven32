#ifndef _CAVEN__H_
#define _CAVEN__H_

/*
    这是一个【纯C】的【.h】文件，是各个底层【.h】的 “共同语言”，上层管理函数的【.h】不需要包含
    仅用于32位系统
    #include "Caven.h"
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
    SDK->Items->GPIO(Exist_GPIO)->BASE->
                                         \\
                                          -->[XXX]->MODE
                                         //
                    C(Lib)->Caven->API->
*/

#define U8 unsigned char
#define S8 signed char
#define U16 unsigned short
#define S16 signed short
#define U32 unsigned int
#define S32 signed int
#define U64 unsigned long long

#define Buff_Length 200
    
#ifndef ENABLE
    #define ENABLE 1
    #define DISABLE 0
#endif

#define Destroy(X,N) memset((X),0,(N))        //销毁 的地址 (X)  长度 (N)
#define MAX(a,b)    ((a)>(b))?(a):(b)               //比较函数返回最大值，防止过小
#define MIN(a,b)    ((a)<(b))?(a):(b)               //比较函数返回最小值，防止过大
/*
*   temp = MIN(temp,100)        //temp永远取小于100的值
*/

//日期
struct Caven_Date
{
    int year;
    U8 month;
    U8 day;
    U8 week;
};

//时间
struct Caven_Watch
{
    U8 hour;
    U8 minutes;
    U8 second;
    U32 time_us;          //这里 1000 000为1S （us）
};

//颜色
struct Caven_Color
{
    U8 REG;
    U8 GREEN;
    U8 BULE;
};

//DATA
//让 Run_num 去追 Length，如果 (Length - Run_num)为0，且过了很长的时间，那么这个数据就该结束了
//将不在这个DATA存放数据，因为MCU内存不一样，规划的空间也不同，所以空间占用大小应该由MCU文件决定，而不是Caven文件。

struct Caven_Data           //这个数据是动态的
{
    U16 Length;                     //目前接收到的数据长度
    volatile U16 Run_num;           //目前运行/处理到的数据个数

    char index;                     //哪个指针会存放数据
    U8 *Poit_U8;
    U16 *Poit_U16;
    int *Poit_U32;
};



#endif
