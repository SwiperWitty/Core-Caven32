#include "sys_time.h"
/*
		SYS_Time
1.Sys_Watch
2.Delay_10us
3.Delay_ms
4.Delay_S
				
				2021.10.07
*/

struct _Delay Delay;
struct _SYS_Watch  SYS_Watch = {0,0,0,0,0};
u32 TimingDelay = 0;

void Sys_Watch_Handle (void)
{
	SYS_Watch.time_num++;
	if(SYS_Watch.time_num > 100000)
	{
		SYS_Watch.time_num = 1;
		SYS_Watch.sys_time++;
		SYS_Watch.second++;
		if(SYS_Watch.second > 59)
		{
			SYS_Watch.second = 0;
			SYS_Watch.minutes++;
			if(SYS_Watch.minutes > 59)
			{
				SYS_Watch.minutes = 0;
				SYS_Watch.hour++;
				if(SYS_Watch.hour > 23)
				{
					SYS_Watch.hour = 0;
				}
			}
		}
	}
}

char Over_Time(struct _Over_time *Item)
{
    int tmp = (*Item->Now_Time) - Item->Last_Time;  //求时差
    Item->Flag = 0;
    if ((*Item->Now_data) != Item->Last_data)       //他不一定是数据内容，可以是接收到数据的【个数】 例如有效的内容长度str_sise()
    {
        Item->Last_data = (*Item->Now_data);
        Item->Last_Time = (*Item->Now_Time);
    }
    else if (tmp > Item->Set_Time)                  //不满足上面的条件，且超时
    {
        Item->Flag = 'p';
    }

    return Item->Flag;
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
		if(SysTick_Config(SystemCoreClock/100000))         //10us定时器(无论时钟频率多少)
			while(1);
		Delay.Delay_10us = Delay_10us;
		Delay.Delay_ms = Delay_ms;
		Delay.Delay_S = Delay_S;
	}
	else
		NVIC_SystemReset();
}
