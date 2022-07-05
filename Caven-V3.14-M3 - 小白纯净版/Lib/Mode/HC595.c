#include "HC595.h"

void HC595_Init (void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(HC595_GPIO_TIM, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(HC595_GPIO,&GPIO_InitStructure);
	GPIO_ResetBits(HC595_GPIO,RCLK | SRCLK | SER_Data);
}

void Send_HC595 (const char Dat)
{
	char n;
	GPIO_ResetBits(HC595_GPIO,RCLK);
	GPIO_ResetBits(HC595_GPIO,SRCLK);
    for(n = 0;n < 8;n++)								//ÉÏÉýÑØÓÐÐ§
    {
        if((Dat << n) & 0x80) GPIO_SetBits(HC595_GPIO,SER_Data);
        else       			  GPIO_ResetBits(HC595_GPIO,SER_Data);
        GPIO_ResetBits(HC595_GPIO,SRCLK);
		GPIO_SetBits(HC595_GPIO,SRCLK);
    }
}

void HC595 (const char *Dat,char num)
{
	char m;
	GPIO_ResetBits(HC595_GPIO,RCLK);
	GPIO_ResetBits(HC595_GPIO,SRCLK);
	for(m = 0; m < num;m++)
	{
		Send_HC595 (*(Dat+m));
	}
	GPIO_ResetBits(HC595_GPIO,RCLK);
	GPIO_SetBits(HC595_GPIO,RCLK);
}

void HC595_Byte (const char Dat)
{
	Send_HC595 (Dat);
	GPIO_ResetBits(HC595_GPIO,RCLK);
	GPIO_SetBits(HC595_GPIO,RCLK);
}
