#ifndef _SYS_TIME_H__
#define _SYS_TIME_H__

//	无法跨芯片移植
/*
	依赖于	SysTick_Config(SystemCoreClock/100000)		//10us
		2022.2.28
	底层
*/

//取消 结构体函数在此实体化

#include "at32f4xx.h"
#include "Caven.h"

#define Frequency	100000		//目前是 10us

struct _SYS_Watch
{
	struct  Caven_Watch Watch;
	int sys_time;			//这是系统秒数（hour*3600 + minutes * 60 + second）
};

struct Sys_Time_
{
	void (*Delay_10us)(int num);
	void (*Delay_ms)(int num);
	void (*Delay_S)(char num);
};

extern struct _SYS_Watch SYS_Watch;

//函数目录
void Delay_10us(int num);
void Delay_ms(int num);
void Delay_S(char num);

void Sys_time_Init(FunctionalState Set);

//#undef	Frequency
#endif
