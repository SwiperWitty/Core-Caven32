#ifndef _SPI__H_
#define _SPI__H_

#include "stm32f10x.h"
#include "sys_time.h"

/*
    以 Xxx_Software_Init 作为初始化的库都是软件模拟对应功能（IO可改）
	依赖	"sys_time.h"
    尽量使 SPI_GPIO 在同一组GPIO上（A/B/C/D），这样就无需更改 SPI_RCC_GPIO_TIM 等等
																					2022.02.26
    底层
	对于	GPIO_Speed_50MHz 可不能写 0 哦，不然会报 /enumerated type mixed with another type/
																					2022.04.06	优化
*/

#define SPI_GPIO		GPIOB

#define SPI_MISO        GPIO_Pin_14
#define SPI_MOSI        GPIO_Pin_15
#define SPI_SCLK		GPIO_Pin_13
#define SPI_CS  		GPIO_Pin_12

//移植修改
#define SPI_PeriphClockCmd(SET)	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,SET)

#define SPI_GPIO_Init()											\
do																\
{																\
	GPIO_InitTypeDef GPIO_InitStructure;						\
	SPI_PeriphClockCmd(SET);									\
    GPIO_InitStructure.GPIO_Pin = SPI_MOSI | SPI_SCLK | SPI_CS;	\
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			\
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			\
	GPIO_Init(SPI_GPIO, &GPIO_InitStructure);					\
																\
	GPIO_InitStructure.GPIO_Pin = SPI_MISO;						\
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;				\
	GPIO_Init(SPI_GPIO, &GPIO_InitStructure);					\
}while(0);		
		
#define SPI_GPIO_Exit()											\
do																\
{																\
	GPIO_InitTypeDef GPIO_InitStructure;						\
    GPIO_InitStructure.GPIO_Pin = SPI_MOSI | SPI_SCLK | 		\
	SPI_CS | SPI_MISO;											\
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;				\
	GPIO_Init(SPI_GPIO, &GPIO_InitStructure);					\
}while(0);

#define SPI_SCLK_H() SPI_GPIO->BSRR = SPI_SCLK		//置高电平
#define SPI_SCLK_L() SPI_GPIO->BRR = SPI_SCLK 		//置低电平
#define SPI_MOSI_H() SPI_GPIO->BSRR = SPI_MOSI
#define SPI_MOSI_L() SPI_GPIO->BRR = SPI_MOSI
#define SPI_CS_H() SPI_GPIO->BSRR = SPI_CS
#define SPI_CS_L() SPI_GPIO->BRR = SPI_CS

#define SPI_MISO_IN() SPI_GPIO->IDR &SPI_MISO 		//读取引脚电平

struct SPI_
{
	char (*Soft_Write_And_Read_Byte) (char Byte_8,int Speed);
	char (*Auto_Write_And_Read_Byte) (char Byte_8,int Speed);
};


char SPIs_Write_And_Read_Byte(char Byte_8,int Speed);	//Master

void SPI_Software_Init(FunctionalState SET);			//软件初始化


#endif
