#ifndef BASE_SYS_TIME_H__
#define BASE_SYS_TIME_H__

#ifdef DEFAULT
#include "Items.h"              //默认功能
#endif

#ifndef DEFAULT
#include "User_items.h"         //自行设置功能，一般出现在本地文件的User中
#endif

/****************/

#define Tick_Frequency (MCU_SYS_Freq/8)   //滴答分频（8分频）
#define Tick_Set_CMP (Tick_Frequency / 2)   //设置滴答初始值(/2 就是 1/2 s)
#define Tick_Over_IT (0.50)                 //定时器溢出时间（24位滴答才有）


/* 【宏函数群】 会给到 mode  */

//很长的时间戳（一天）
typedef struct _SYS_Ticktime
{
    unsigned int SYS_Tick_H; //每Frequency进1
    unsigned int SYS_Tick_L; // 24bit 的
} SYS_Tick_type;

//typedef struct curtime_s{
//	unsigned int s_tenth;
//	unsigned int ticks;
//}curtime_t;

// source
uint64_t SysTick_type_Change_NUM (SYS_Tick_type stamp);
uint64_t GET_SysTick (SYS_Tick_type *stamp);
void SET_SysTick(uint64_t time);

// Init (systick(Set = 1/2)  dog(Set = 2))
void Sys_Time_Init(int Set);        //enable & reboot

// Delay
void Base_NOP_Delay (int time,int Speed);

void SYS_Delay_us(int n);
void SYS_Delay_ms(int n);
void SYS_Delay_S(int n);

//dog
void IWDG_Configuration(void);
void Feed_Watchdog(void);

#endif
