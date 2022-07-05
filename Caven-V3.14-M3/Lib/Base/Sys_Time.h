#ifndef _SYS_TIME_H__
#define _SYS_TIME_H__

//  跨芯片移植
/*
    依赖于
    滴答定时器    SysTick_Config(SystemCoreClock/ Frequency )
                                            2022.02.28
    或者任意16位及以上定时器
                                            2022.07.04
    底层需要更改时钟初始化 & 中断
*/

#include "stm32f10x.h"
#include "Caven.h"

//#define Base_Time 1
#define Base_SysTick    1     //在此区分 使用通用定时器/滴答

#define Frequency   100000      //目前是 10us

struct _SYS_Time
{
    struct Caven_Date Date;
    struct Caven_Watch Watch;
    u32 sys_time;           //这是系统秒数（hour*3600 + minutes * 60 + second）
};

struct Sys_Time_
{
    void (*Delay_10us)(int num);
    void (*Delay_ms)(int num);
    void (*Delay_S)(char num);
};

extern struct _SYS_Time SYS_Time;
extern struct Sys_Time_ Delay;

void Delay_10us(int num);
void Delay_ms(int num);
void Delay_S(char num);
void Sys_Time_Init(FunctionalState Set);

#endif
