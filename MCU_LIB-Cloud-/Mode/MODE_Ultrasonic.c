#include "MODE_Ultrasonic.h"

void Ultrasonic_Init(int SET)
{
#ifdef Exist_Ultrasonic
    Ultrasonic_GPIO_Init(SET);
#endif
}

float Distance(void)
{
    float distance=0;
#ifdef Exist_Ultrasonic
	int count=0;
	u32 num;
	while(SYS_Time.Watch.time_num > 70000);

	Trig_Clr();
	num = SYS_Time.Watch.time_num;
	while(SYS_Time.Watch.time_num - num < 2);
	Trig_Set();

	num = SYS_Time.Watch.time_num;
	do{
		if(SYS_Time.Watch.time_num - num > 2000)
		{
			return 0;
		}
	}
	while(Echo_Read() == 0);	//等待信号发出，信号发出，计数器置0,信号发出，变为高电平，引脚置1

	num = SYS_Time.Watch.time_num;
	do{
		if(SYS_Time.Watch.time_num - num > 19000)
		{
			return 0;
		}
	}
	while(Echo_Read() == 1); //等待信号接受，信号发出过程中，引脚一直置位1
	count = SYS_Time.Watch.time_num - num;
	//v = 340m/s = 34000cm/s = 34000cm/10^6us = 0.034cm/us
	distance=(float)count*0.17;
#endif
	return distance;
}

