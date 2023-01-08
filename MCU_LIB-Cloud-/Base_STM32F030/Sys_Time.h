#ifndef _SYS_TIME_H__
#define _SYS_TIME_H__

/*
本文件属于MCU专属文件，每个MCU不同，这里的内容就会不同 ———— 需要系统提供时钟
这个文件是给Time的，也就是说你得找一个能满足以下条件时钟来作为动力  （选一个幸运儿做苦力）。
    滴答定时器（首选）
                                            2022.02.28
    或者任意16位及以上定时器
                                            2022.07.04
    有中断
    
为了方便上层统一调用，本文件需要提供一个 【宏函数群】上层使用
也就是说，无论是什么MCU【宏函数群】的内容可以不一样，但是【宏】必须保持一致
    底层需要提供
    1、时钟中断函数宏 SYS_Time_Interrupt （10us进入一次）
    2、中断标志位（可以没有）
    3、中断标志位清除（可以没有）
    
    总结：提供 初始化 & 控制【函数群】
*/

#include "Items.h"

#define Base_SysTick        //使用 滴答定时器 作为【系统时钟】

/* 【宏函数群】   */



#define	Tick_Frequency	MCU_SYS_Freq		//滴答分频
#define	Tick_Set_CMP	Tick_Frequency/10  	//设置滴答初始值
#define	Tick_OverfLow	0.10  	            //定时器溢出时间（24位滴答才有）

#define Freq_us     (Tick_Frequency/1000000)   //微秒
#define Freq_ms     (Tick_Frequency/1000)      //毫秒


//很长的时间戳
struct _SYS_Ticktime
{
    volatile uint32_t SYS_Tick_H;         //每Frequency进1 
    volatile uint32_t SYS_Tick_L;         //24bit 的
};

uint64_t GET_SysTick (void);
void SET_SysTick (uint64_t time);

void Sys_Time_Init (int Set);

void SYS_Delay_us (int n);
void SYS_Delay_ms (int n);
void SYS_Delay_S (int n);

#endif
