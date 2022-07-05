#ifndef _BASE_H__
#define _BASE_H__

#include "iic.h"
#include "uart.h"
#include "spi.h"

#include "dac.h"
#include "adc.h"

#include "sys_time.h"
#include "encoder.h"
#include "PWM.h"

#include "stmflash.h"

/*
    底层（Base）中有不少东西让人迷惑，他们大致分布在 system_stm32xxx.c 以及 stm32xxxx_it.h 和 stm32xxxx.h
    如果查阅这里，问题都没有解决，那么这就是一个不该由你解决的问题。
*/

extern struct Base_Init_ Base_Init;
extern struct Base_User_ Base_User;


struct Base_Init_
{
	void (*IIC_Software_Init)(FunctionalState SET); // FunctionalState 是一个枚举，你可以看成bit,只有0/1.做使能位
	void (*SPI_Software_Init)(FunctionalState SET);	//不全，没有硬件SPI
	void (*Uart)(char Channel, int Baud,FunctionalState SET);
	void (*CAN)(FunctionalState SET); //	很明显没写

	void (*ADC_x_Init)(char ADC_x, FunctionalState SET);	//OK
	void (*DAC_x_Init)(char DAC_x, FunctionalState SET);	//OK

	void (*Encoderx_Init)(char Timex, FunctionalState SET);	//OK
	void (*PWM_x_Init)(char PWM_x, u16 arr, u16 psc, FunctionalState SET);	//OK
	void (*SYS_Time_Init)(FunctionalState SET);	//OK
	
	void (*Flash_Init)(FunctionalState SET);	//OK
};

struct Base_User_
{
	struct IIC_ IIC; //如果你企图看懂代码内容，请参考IIC.h
	struct SPI_ SPI;
    struct Uart_ UART;
	// CAN没写，谁爱写谁写

	struct ADC_ ADC;
	struct DAC_ DACx;

	struct Sys_Time_ Delay;
	//
	//
	
	struct Flash_ Flash;
};

void Base_Index(void); //初始化Base索引,真正功能的初始化请调用结构体中函数指针(Base_Init)

#endif
