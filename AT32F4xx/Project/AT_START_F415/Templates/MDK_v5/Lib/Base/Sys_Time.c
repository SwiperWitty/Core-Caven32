#include "sys_time.h"
/*
		SYS_Time
1.Sys_Watch
2.Delay_10us
3.Delay_ms
4.Delay_S
				
				2021.10.07
*/
// 取消 结构体函数在此实体化

struct _SYS_Watch  SYS_Watch = {0};			//全局地、全文件可调用，但只读
u32 TimingDelay = 0;

void Sys_Watch_Handle (void)
{
	SYS_Watch.Watch.time_num++;
	if(SYS_Watch.Watch.time_num > Frequency)
	{
		SYS_Watch.Watch.time_num = 1;
		SYS_Watch.sys_time++;
		SYS_Watch.Watch.second++;
		if(SYS_Watch.Watch.second > 59)
		{
			SYS_Watch.Watch.second = 0;
			SYS_Watch.Watch.minutes++;
			if(SYS_Watch.Watch.minutes > 59)
			{
				SYS_Watch.Watch.minutes = 0;
				SYS_Watch.Watch.hour ++;
				if(SYS_Watch.Watch.hour > 23)
				{
					SYS_Watch.Watch.hour = 0;
				}
			}
		}
	}
}

void SysTick_Handler(void)			//SYS_TIME中断
{
	TimingDelay++;
	Sys_Watch_Handle ();
}

void Delay_10us(int num)
{
	TimingDelay = 0;
	while(TimingDelay < num);
}

void Delay_ms(int num)
{
	int n = num;
	while(n--)
	{
		TimingDelay = 0;
		while(TimingDelay <= 100);
	};
}

void Delay_S(char num)
{
	int n = num;
	while(n--)
	{
		Delay_ms(1000);
	};
}

void Sys_time_Init (FunctionalState Set)
{
	if(Set)
	{
		if(SysTick_Config(SystemCoreClock/Frequency))         //系统使用滴答定时器，因为RTC定时器的最小细分不足以用于一些场景（例如超声波）
			while(1);
	}
	else
		NVIC_SystemReset();
}
