#ifndef _SYS__TIME_
#define _SYS__TIME_

#include "gd32f4xx.h"
#include <stdio.h>

/*SYS_Time.h*/

#define Tick_Frequency  (SystemCoreClock/8)
#define Tick_Set_CMP (Tick_Frequency / 10)   //设置滴答初始值(/10 就是 1/10 s)
#define Tick_Over_IT (0.10)                 //定时器溢出时间（24位滴答才有）


/* 【宏函数群】 会给到 mode  */

//很长的时间戳（一天）
struct _SYS_Ticktime
{
    volatile uint32_t SYS_Tick_H; //每Frequency进1
    volatile uint32_t SYS_Tick_L; // 24bit 的
};

//定义系统时间格式，与具体实现相关
typedef struct curtime_s{
	unsigned int s_tenth;
	unsigned int ticks;
}curtime_t;


// source
uint64_t GET_SysTick(void); //请在空闲时尽可能调用它
void SET_SysTick(uint64_t time);

// Init
void Sys_Time_Init(int Set);

// Delay
void SYS_Delay_us(int n);
void SYS_Delay_ms(int n);
void SYS_Delay_S(int n);

void Base_Delay (int time,int Speed);

int cpu_get_lltimer(curtime_t* timestamp);
unsigned int cpu_diff_lltimer(curtime_t lltimer_stamp);
unsigned int cpu_diff_us(curtime_t lltimer_stamp);
unsigned int cpu_diff_ms(curtime_t lltimer_stamp);

#endif
