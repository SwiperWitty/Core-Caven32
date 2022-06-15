#include "sys_time.h"
/*
		SYS_Time
1.Sys_Watch
2.Delay_10us
3.Delay_ms
4.Delay_S
				
				2021.10.07
*/

SYS_Watch Timewatch = {23,59,55,0,0};
u32 TimingDelay = 0;

void Sys_Watch (void)
{
	Timewatch.time_unm++;
	if(Timewatch.time_unm > 100000)
	{
		Timewatch.time_unm = 1;
		Timewatch.sys_time++;
		Timewatch.second++;
		if(Timewatch.second > 59)
		{
			Timewatch.second = 0;
			Timewatch.minutes++;
			if(Timewatch.minutes > 59)
			{
				Timewatch.minutes = 0;
				Timewatch.hour++;
				if(Timewatch.hour > 23)
				{
					Timewatch.hour = 0;
				}
			}
		}
	}
}

void SysTick_Handler(void)
{
	TimingDelay++;
	Sys_Watch ();
}

void Delay_10us(int num)
{
	int n = num;
	TimingDelay = 0;
	while(TimingDelay < n);
}

void Delay_ms(int num)
{
	int n = num;
	while(n--)
	{
		TimingDelay = 0;
		while(TimingDelay < 100);
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
