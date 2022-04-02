#ifndef _SPI__H_
#define _SPI__H_

//	可以跨芯片移植
/*
    以 Xxx_Software_Init 作为初始化的库都是软件模拟对应功能（IO可改）
	依赖	"sys_time.h"
    尽量使 SPI_GPIO 在同一组GPIO上（A/B/C/D），这样就无需更改 SPI_RCC_GPIO_TIM 等等
    否则需要在 SPI.C 大改   
        2022.2.26
    底层
*/
#include "stm32f10x.h"
#include "sys_time.h"

#define SPI_GPIO		GPIOB                                //IO

#define SPI_MISO        GPIO_Pin_14
#define SPI_MOSI        GPIO_Pin_15
#define SPI_SCLK		GPIO_Pin_13
#define SPI_CS  		GPIO_Pin_12

#define SPI_RCC_GPIO_TIM		RCC_APB2Periph_GPIOB        //时钟

extern char SPI_array_s[20];

void SPI_Software_Init(void);                               //软件初始化
void SPI_SetSpeed(unsigned int SPI_BaudRatePrescaler_x);    //速度分频

//char SPI_Read_And_Write_Byte(char dat);               //Slave
//char *SPI_Read_And_Write(const char *array);

char SPI_Write_And_Read_Byte(char dat); //Master
char *SPI_Write_And_Read(const char *array, char Len);

#endif
