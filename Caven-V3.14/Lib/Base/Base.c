#include "base.h"

/*	本系统的索引结构体	*/			//本地boy
struct _Init Base_Init;
struct _User Base_User;

/*	基础功能的索引结构体	*/		//外来boy
extern struct _IIC	IIC;
extern struct _SPI	SPI;

void Base_User_index(void)		//索引 功能函数 本体
{
	Base_User.IIC.Soft_ReadByte = IIC.Soft_ReadByte;		//绑定各个函数,(没写的后面 + //*)
	Base_User.IIC.Soft_SendByte = IIC.Soft_SendByte;
	Base_User.IIC.Auto_ReadByte = IIC.Auto_ReadByte;	//*
	Base_User.IIC.Auto_SendByte = IIC.Auto_SendByte;	//*
	
	Base_User.SPI.Soft_Write_And_Read_Byte = SPI.Soft_Write_And_Read_Byte;
	Base_User.SPI.Auto_Write_And_Read_Byte = SPI.Auto_Write_And_Read_Byte;	//*
	
	
}

void Base_Init_index(void)		//索引 初始化函 数本体
{
	Base_Init.IIC_Software_Init = IIC_Software_Init;
	Base_Init.SPI_Software_Init = SPI_Software_Init;
	
	Base_Init.UARTx_Init = UARTx_Init;
	
	Base_Init.SYS_Init = Sys_time_Init;
	
	Base_User_index();			//初始化 功能索引
}

