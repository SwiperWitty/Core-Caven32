#ifndef _SYS_TIME_H__
#define _SYS_TIME_H__

//	无法跨芯片移植
/*
	依赖于	SysTick_Config(SystemCoreClock/100000)
		2022.2.28
	底层
*/

#include "stm32f10x.h"

struct _SYS_Watch
{
	char hour;
	char minutes;
	char second;
	u32 sys_time;			//这是系统秒数（hour*3600 + minutes * 60 + second）
	u32 time_num;			//这是中断溢出次数，达到 100000 就是1S
};

struct _Delay
{
	void (*Delay_10us)(int num);
	void (*Delay_ms)(int num);
	void (*Delay_S)(char num);
};

extern struct _SYS_Watch SYS_Watch;

void Delay_10us(int num);
void Delay_ms(int num);
void Delay_S(char num);
void Sys_time_Init(FunctionalState Set);

#endif
