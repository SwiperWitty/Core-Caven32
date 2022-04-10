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
#include "Data_Handle.h"

struct _Base_Init
{
	void (*IIC_Software_Init)(FunctionalState SET); // FunctionalState 是一个枚举，你可以看成bit,只有0/1.做使能位
	void (*SPI_Software_Init)(FunctionalState SET);
	void (*UART_x_Init)(char UART_x, unsigned int Baud, FunctionalState SET);
	void (*CAN)(FunctionalState SET); //	很明显没写

	void (*ADC_x_Init)(char ADC_x, FunctionalState SET);
	void (*DAC_x_Init)(char DAC_x, FunctionalState SET);

	void (*Encoderx_Init)(char Timex, FunctionalState SET);
	void (*PWM_x_Init)(char PWM_x, u16 arr, u16 psc, FunctionalState SET);
	void (*SYS_Init)(FunctionalState SET);
	
	void (*Flash_Init)(FunctionalState SET);
};

struct _Base_User
{
	struct _IIC *IIC; //如果你企图看懂代码内容，请参考IIC.h
	struct _SPI *SPI;
	struct _UART *UART;
	// CAN没写，谁爱写谁写

	struct _ADC *ADC;
	struct _DAC *DACx;

	struct _Delay *Delay;
	//
	//
	
	struct _Flash *Flash;
};

void Base_Init_index(void); //初始化Base索引,真正功能的初始化请调用结构体中函数指针(Base_Init)

#endif
