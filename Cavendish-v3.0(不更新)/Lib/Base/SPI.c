#include "SPI.h"

#define SPI_SCLK_H() SPI_GPIO->BSRR = SPI_SCLK		//置高电平
#define SPI_SCLK_L() SPI_GPIO->BRR = SPI_SCLK 		//置低电平
#define SPI_MOSI_H() SPI_GPIO->BSRR = SPI_MOSI
#define SPI_MOSI_L() SPI_GPIO->BRR = SPI_MOSI
#define SPI_CS_H() SPI_GPIO->BSRR = SPI_CS
#define SPI_CS_L() SPI_GPIO->BRR = SPI_CS

#define SPI_MISO_IN() SPI_GPIO->IDR &SPI_MISO 		//读取引脚电平

char SPI_Flag = 0;
char SPI_array_s[20];
unsigned int Time = 10;

void SPI_Software_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStrue;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);					//禁用 jtag

	RCC_APB2PeriphClockCmd(SPI_RCC_GPIO_TIM,ENABLE);

	GPIO_InitStrue.GPIO_Pin = SPI_CS | SPI_SCLK| SPI_MOSI;				//OUT
	GPIO_InitStrue.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStrue.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_GPIO,&GPIO_InitStrue);

	GPIO_InitStrue.GPIO_Pin = SPI_MISO;									//IN 
	GPIO_InitStrue.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(SPI_GPIO,&GPIO_InitStrue);

	GPIO_SetBits(SPI_GPIO, SPI_CS | SPI_SCLK| SPI_MOSI);
}

char SPI_Write_And_Read_Byte(char dat)							//Master
{
	char read_data = 0;
	char n, m = 0x80;

	SPI_MOSI_H();
	SPI_SCLK_H();
	SPI_CS_L();
	Delay_10us(Time);
	for (n = 0; n < 8; n++)
	{
		SPI_SCLK_L();
		if (((dat << n) & m) == m)
			SPI_MOSI_H();
		else
			SPI_MOSI_L();

		Delay_10us(Time);

		SPI_SCLK_H();
		if (SPI_MISO_IN())										//开始读
			read_data |= (m >> n);
		else
			read_data |= (0 >> n);
		Delay_10us(Time);
	}
	SPI_MOSI_H();
	SPI_SCLK_H();
	SPI_CS_H();

	return read_data;
}

char *SPI_Write_And_Read(const char *array, char Len)
{
	char n;

	for (n = 0; n < 20; n++)
	{
		SPI_array_s[n] = 0;
	}

	for (n = 0; n < Len; n++)
	{
		SPI_array_s[n] = SPI_Write_And_Read_Byte(*(array + n));
		Delay_10us(Time);
	}
	return SPI_array_s;
}


void SPI_SetSpeed(unsigned int SPI_BaudRatePrescaler_x) //
{
	Time = (SPI_BaudRatePrescaler_x + 1) * 2;
}
