#ifndef _IIC_H_
#define _IIC_H_
   
/*
    以 Xxx_Software_Init 作为初始化的库都是软件模拟对应功能（IO可改）
	依赖	"sys_time.h"
    例如：
        SPI_Software_Init
        IIC_Software_Init
    底层
	
	Speed		(0,1000)
	降低耦合	可以跨芯片移植 		2022.04.06
*/

#include "stm32f10x.h"
#include "sys_time.h"

#define ACK	 	0 	//应答
#define	NACK 	1 	//无应答

#define IIC_GPIO		GPIOB                                   //IO

#define IIC_SDA		GPIO_Pin_11                                 //数据脉冲线
#define IIC_SCK		GPIO_Pin_10                                 //时钟脉冲线

//以下是移植F4需要更改内容	不管软硬下面IO都是这么初始化的
#define IIC_PeriphClockCmd(SET)		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, SET)	//时钟(F1)
//#define IIC_PeriphClockCmd(SET)	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,SET); //时钟(F4)

#define IIC_GPIO_Init()									\
do														\
{														\
	GPIO_InitTypeDef GPIO_InitStructure;				\
	IIC_PeriphClockCmd(SET);							\
    GPIO_InitStructure.GPIO_Pin = IIC_SCK | IIC_SDA;	\
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	\
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		\
														\
	GPIO_Init(IIC_GPIO, &GPIO_InitStructure);			\
}while(0);

#define IIC_GPIO_Exit()									\
do														\
{														\
	GPIO_InitTypeDef GPIO_InitStructure;				\
    GPIO_InitStructure.GPIO_Pin = IIC_SCK | IIC_SDA;	\
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		\
	GPIO_Init(IIC_GPIO, &GPIO_InitStructure);			\
}while(0);
#define IIC_SCK_H() IIC_GPIO->BSRR = IIC_SCK //置高电平
#define IIC_SCK_L() IIC_GPIO->BRR = IIC_SCK  //置低电平
#define IIC_SDA_H() IIC_GPIO->BSRR = IIC_SDA
#define IIC_SDA_L() IIC_GPIO->BRR = IIC_SDA

#define IIC_SDA_IN() IIC_GPIO->IDR &IIC_SDA //读取引脚电平

struct IIC_
{
	char (*Soft_SendByte) (char Byte_8,int Speed);
	char (*Soft_ReadByte) (char ack_nack,int Speed);
	char (*Auto_SendByte) (char Byte_8,int Speed);
	char (*Auto_ReadByte) (char ack_nack,int Speed);
};

void IIC_StartBit(void);
void IIC_StopBit(void);

char IICs_SendByte(char Byte_8,int Speed);		//Software SendByte
char IICs_ReadByte(char ack_nack,int Speed);	//Software ReadByte
void IIC_Software_Init(FunctionalState SET);

#endif
