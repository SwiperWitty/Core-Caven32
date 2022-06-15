#include "Ultrasonic.h"
#include "sys_time.h"

void Ultrasonic_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	SysTick_Config(SystemCoreClock/100000);								//SystemCoreClock
	RCC_APB2PeriphClockCmd(CSB_T_GPIO_TIM | CSB_E_GPIO_TIM, ENABLE);	//使能PB端口时钟

	GPIO_InitStructure.GPIO_Pin = Trig;//Trig
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;					//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
	GPIO_Init(CSB_T_GPIO, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = Echo;//Echo
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;						//下拉输入	 
	GPIO_Init(CSB_E_GPIO, &GPIO_InitStructure);
}



float Distance(void)
{
	int count=0;
	u32 num;
	float distance=0;
	
	GPIO_ResetBits(CSB_T_GPIO, Trig);//预先拉低Trig引脚
	while(Timewatch.time_unm > 70000);
	
	GPIO_SetBits(CSB_T_GPIO, Trig);					//start
	num = Timewatch.time_unm;
	while(Timewatch.time_unm - num < 2);
	GPIO_ResetBits(CSB_T_GPIO, Trig);//发出20us的脉冲

	num = Timewatch.time_unm;
	do{
		if(Timewatch.time_unm - num > 2000)
		{
			return 0;
		}
	}
	while(GPIO_ReadInputDataBit(CSB_E_GPIO, Echo) == 0);	//等待信号发出，信号发出，计数器置0,信号发出，变为高电平，引脚置1

	num = Timewatch.time_unm;
	do{
		if(Timewatch.time_unm - num > 19000)
		{
			return 0;
		}
	}
	while(GPIO_ReadInputDataBit(CSB_E_GPIO, Echo) == 1); //等待信号接受，信号发出过程中，引脚一直置位1 
	count = Timewatch.time_unm - num;
	//v = 340m/s = 34000cm/s = 34000cm/10^6us = 0.034cm/us
	distance=(float)count*0.17;
	return distance;
}

