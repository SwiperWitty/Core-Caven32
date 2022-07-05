#include "SPI.h"

struct SPI_	SPI;

char SPIs_Write_And_Read_Byte(char Byte_8,int Speed)					//Master
{
	char read_Byte_8a = 0;
	char n, m = 0x80;

	SPI_MOSI_H();
	SPI_SCLK_H();
	SPI_CS_L();
	Delay_10us(Speed);
	for (n = 0; n < 8; n++)
	{
		SPI_SCLK_L();
		if (((Byte_8 << n) & m) == m)
			SPI_MOSI_H();
		else
			SPI_MOSI_L();

		Delay_10us(Speed);

		SPI_SCLK_H();
		if (SPI_MISO_IN())												//开始读
			read_Byte_8a |= (m >> n);
		else
			read_Byte_8a |= (0 >> n);
		Delay_10us(Speed);
	}
	SPI_MOSI_H();
	SPI_SCLK_H();
	SPI_CS_H();

	return read_Byte_8a;
}

void SPI_Software_Init(FunctionalState SET)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);			//禁用 jtag

	if(SET)
	{
		SPI_GPIO_Init();
		SPI_MOSI_H();
		SPI_SCLK_H();
		SPI_CS_H();
	}
	else
	{
		SPI_GPIO_Exit();
	}
	SPI.Soft_Write_And_Read_Byte = SPIs_Write_And_Read_Byte;
}
