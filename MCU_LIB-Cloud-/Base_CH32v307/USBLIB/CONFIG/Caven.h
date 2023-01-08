#ifndef _CAVEN__H_
#define _CAVEN__H_

/*
    这是一个【纯C】的【.h】文件，是各个底层【.h】的 “共同语言”，上层管理函数的【.h】不需要包含
    仅用于32位系统
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

//#define DISABLE 0
//#define ENABLE (!DISABLE)

#define U8 unsigned char
#define S8 signed char
#define U16 unsigned short
#define S16 signed short
#define U32 unsigned int
#define S32 signed int

#define Buff_Length 500
    
#define Destroy(X,N) memset((X),0,N)        //销毁 的地址 (X)  长度 (N)

struct Caven_Date
{
    int year;
    char month;
    char day;
    char week;
};

struct Caven_Watch
{
    char date;              //此位只有0/1提醒系统已经是下一天了
    char hour;
    char minutes;
    char second;
    volatile int time_num;          //这是中断溢出次数 10 0000为1S
};

struct Caven_Color
{
    char REG;
    char GREEN;
    char BULE;
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
