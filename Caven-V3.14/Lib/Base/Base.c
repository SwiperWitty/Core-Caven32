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

void Base_User_index(void)		//索引 功能函数 本体
{
	Base_User.IIC = &IIC;		//绑定各个函数	
	Base_User.SPI = &SPI;		//其实你可以结构体指针对向索引结构体，但是结构体指针->在MDK没有代码补全，还是一直按住.不撒手来的爽。
	Base_User.UART = &UART;		//这是绑定在别的文件定义好的函数指针。

	Base_User.ADC = &ADC;		//这样在这里只能追踪到.h文件		用心看看效果是一样的
	Base_User.DACx = &DACx;

	Base_User.Delay = &Delay;
	Base_User.Flash = &Flash;
}

void Base_Init_index(void)		//索引 初始化函 数本体
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

