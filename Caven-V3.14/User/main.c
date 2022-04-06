#ifdef Cavendish
	#include "stm32f10x.h"
	#include "base.h"
	#include "Mode.h"
	#include "pic.h"		//图片

	#ifdef ST
		#define MCU 1		//STM32
	#else
		#define MCU 0		//GD 32
	#endif
#endif

extern struct _Init Base_Init;
extern struct _User Base_User;

char Free_Array[16];

//要写入到STM32 FLASH的字符串数组
#define SIZE sizeof(Free_Array)			//数组长度
#define FLASH_SAVE_ADDR  0X08012000		//FLASH_SAVE_ADDR >= 本代码所占用FLASH的大小(16进制) + 0X08000000 + 0X800)

float far,temp,vol;
char Free_Show[30];


void Mian_Init(void);
int main(void)
{
	Mian_Init();
	while(1)
	{
		vol = Read_MCU_Temp();
		sprintf(Free_Array,"MCU: %.2f C ",vol);
		LCD_ShowString(2,2,Free_Array,WHITE,BLACK,16); Data_Replace ("123",Free_Array,0,sizeof(Free_Array));
		
		sprintf(Free_Show, "Time: %2d:%2d:%2d  ", Timewatch.hour, Timewatch.minutes, Timewatch.second);
		LCD_ShowString(8,14,Free_Show,BLUE,BLACK,16); Data_Replace("123", Free_Show, 0, sizeof(Free_Show));
	}
}


void Mian_Init(void)
{
	Base_Init_index();
	Base_Init.IIC_Software_Init(ENABLE);
	Base_User.IIC.Soft_SendByte('a',12);
//	Delay_S(1);
	
}


