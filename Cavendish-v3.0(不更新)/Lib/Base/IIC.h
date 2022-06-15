#ifndef _IIC_H_
#define _IIC_H_

//	可以跨芯片移植      
/*
    以 Xxx_Software_Init 作为初始化的库都是软件模拟对应功能（IO可改）
	依赖	"sys_time.h"
    例如：
        SPI_Software_Init
        IIC_Software_Init
    底层
*/

#include "stm32f10x.h"
#include "sys_time.h"

#define ACK	 	0 	//应答
#define	NACK 	1 	//无应答

#define IIC_GPIO		GPIOB                                   //IO

#define IIC_SDA		GPIO_Pin_11                                 //数据脉冲线
#define IIC_SCK		GPIO_Pin_10                                 //时钟脉冲线

#define IIC_RCC_GPIO_TIM		RCC_APB2Periph_GPIOB        //时钟

void IIC_StartBit(void);
void IIC_StopBit(void);

char IIC_SendByte(char Byte_8,int Speed);
char IIC_ReadByte(char ack_nack,int Speed);

void IIC_Software_Init(void);

#endif
