#include "base.h"

/*	本系统的索引结构体	*/			//本地boy
struct Base_Init_ Base_Init;
struct Base_User_ Base_User;        //结构体实体


void Base_User_index(void)		//索引 功能函数 本体		（这是强制绑定函数本体）
{
	Base_User.IIC.Soft_ReadByte = IICs_ReadByte;		//绑定各个函数
	Base_User.IIC.Soft_SendByte = IICs_SendByte;		//其实你可以结构体指针对向索引结构体，但是结构体指针->在MDK没有代码补全，还是一直按住.不撒手来的爽。

	Base_User.SPI.Soft_Write_And_Read_Byte = SPIs_Write_And_Read_Byte;		

	Base_User.UART.Send_Data= UART_Send_Data;
	Base_User.UART.Send_String = UART_Send_String;

	Base_User.ADC.Read_MCU_Temp = Read_MCU_Temp;
	Base_User.ADC.ADC_x_Read_Vol = ADC_x_Read_Vol;

	Base_User.DACx.DAC_x_Set_Vol = DAC_x_Set_Vol;

	Base_User.Delay.Delay_10us = Delay_10us;
	Base_User.Delay.Delay_ms = Delay_ms;
	Base_User.Delay.Delay_S = Delay_S;

	Base_User.Flash.STMFLASH_Read = STMFLASH_Read;
	Base_User.Flash.STMFLASH_ReadHalfWord = STMFLASH_ReadHalfWord;
	Base_User.Flash.STMFLASH_Write = STMFLASH_Write;
	Base_User.Flash.Test_Write = Test_Write;
}

void Base_Index(void)		//索引 初始化函数 和 功能函数
{
	Base_Init.IIC_Software_Init = IIC_Software_Init;	//其实这里结构体内的函数指针直接绑定函数就可以了
	Base_Init.SPI_Software_Init = SPI_Software_Init;	//这样在这就可以直接追踪到.c文件
	Base_Init.Uart = Uart_Init;

	Base_Init.ADC_x_Init = ADC_x_Init;
	Base_Init.DAC_x_Init = DAC_x_Init;
	
	Base_Init.PWM_x_Init = PWM_x_Init;
	Base_Init.SYS_Time_Init = Sys_Time_Init;
		
	Base_Init.Flash_Init = Flash_Init;		//这些内容的初始化也有索引
	
	Base_User_index();						//初始化 功能索引
}

