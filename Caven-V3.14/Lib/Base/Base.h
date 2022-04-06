#ifndef _BASE_H__
#define _BASE_H__

#include "sys_time.h"
#include "Data_Handle.h"

#include "iic.h"
#include "uart.h"
#include "spi.h"

#include "dac.h"
#include "adc.h"

#include "encoder.h"
#include "PWM.h"

#include "stmflash.h"

struct _Init
{
	void (*IIC_Software_Init) (FunctionalState SET);		// FunctionalState 是一个枚举，你可以看成bit,只有0/1.做使能位
	void (*SPI_Software_Init) (FunctionalState SET);
	void (*UARTx_Init) (char UARTx, unsigned int Baud, FunctionalState SET);
	void (*CAN) (FunctionalState SET);		//	很明显没写
	
	void (*ADCx_Init) (char ADCx,FunctionalState SET);
	void (*DACx_Init) (char DACx,FunctionalState SET);
	
	void (*Encoderx_Init) (char Timex, FunctionalState SET);
	void (*PWMx_Init) (char Timex, FunctionalState SET);
	void (*SYS_Init) (FunctionalState SET);

};

struct _User
{
	struct _IIC IIC;						//如果你企图看懂代码内容，请参考IIC.h
	struct _SPI SPI;
	
	
};

void Base_Init_index(void);					//初始化Base索引,真正功能的初始化请调用结构体中函数指针(Base_Init)

#endif
