#include "base.h"

/*	本系统的索引结构体	*/			//本地boy
struct _Base_Init Base_Init;
struct _Base_User Base_User;

/*	基础功能的索引结构体	*/		//外来boy
extern struct _IIC	IIC;
extern struct _SPI	SPI;
extern struct _UART UART;

extern struct _ADC	ADC;
extern struct _DAC DACx;

extern struct _Delay Delay;

extern struct _Flash Flash;

extern struct _Data	Data;
// 以上的内容，是实体，在对应的.C文件中调用。但是 Base_User_index 强行绑定了，所以以上内容没有意义（没调用过他们）。

void Base_User_index(void)		//索引 功能函数 本体		（这是强制绑定函数本体）
{
	Base_User.IIC.Soft_ReadByte = IICs_ReadByte;		//绑定各个函数
	Base_User.IIC.Soft_SendByte = IICs_SendByte;		//其实你可以结构体指针对向索引结构体，但是结构体指针->在MDK没有代码补全，还是一直按住.不撒手来的爽。

	Base_User.SPI.Soft_Write_And_Read_Byte = SPIs_Write_And_Read_Byte;		

	Base_User.UART.UART_x_Send_Data = UART_x_Send_Data;
	Base_User.UART.UART_x_Send_String = UART_x_Send_String;

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

	Base_User.Data.Chang_NUM = Data_Chang_NUM;
	Base_User.Data.Judge = Data_Judge;
	Base_User.Data.Replace = Data_Replace;
}

void Base_Init_index(void)		//索引 初始化函数 本体
{
	Base_Init.IIC_Software_Init = IIC_Software_Init;	//其实这里结构体内的函数指针直接绑定函数就可以了
	Base_Init.SPI_Software_Init = SPI_Software_Init;	//这样在这就可以直接追踪到.c文件
	Base_Init.UART_x_Init = UART_x_Init;

	Base_Init.ADC_x_Init = ADC_x_Init;
	Base_Init.DAC_x_Init = DAC_x_Init;
	
	Base_Init.PWM_x_Init = PWM_x_Init;
	Base_Init.SYS_Init = Sys_time_Init;
		
	Base_Init.Flash_Init = Flash_Init;		//这些内容的初始化也有索引
	
	Base_User_index();						//初始化 功能索引
}

