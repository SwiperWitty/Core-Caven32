#ifdef Cavendish
#include "stm32f10x.h"
#include "base.h"
#include "Mode.h"
#include "pic.h" //图片

#ifdef ST
#define MCU 1 // STM32
#else
#define MCU 0 // GD 32
#endif
#endif

extern struct _Base_Init Base_Init;
extern struct _Base_User Base_User;
extern struct _Mode_Init Mode_Init;
extern struct _Mode_User Mode_User;

//要写入到STM32 FLASH的字符串数组
#define SIZE sizeof(Free_Array)	   //数组长度
#define FLASH_SAVE_ADDR 0X08012000 // FLASH_SAVE_ADDR >= 本代码所占用FLASH的大小(16进制) + 0X08000000 + 0X800)

float far, temp, vol;
char Free_Show[30];
extern struct _CV_UART CV_UART;
unsigned int Overflow_Time,Now_Time;
char UART_Overflow_num = 0;
int abd;
void Mian_Init(void);
int main(void)
{
	Mian_Init();
	
	while (1)
	{
		
		if(CV_UART.Rxd_Num[3] > 0)
		{
			if(Data_Overflow_Time(CV_UART.Rxd_Num[3],100) == (-1))
			{
				Base_User.UART.UART_x_Send_String(3,"time over !");
			}
		}
		
		if(CV_UART.Read_Flag[3])
		{
			CV_UART.Read_Flag[3] = 0;
			Base_User.UART.UART_x_Send_String(3,CV_UART.UART_x_Array[3]);
			Data_Replace ("123",CV_UART.UART_x_Array[3],0,sizeof(CV_UART.UART_x_Array[3]));
			Base_User.Delay.Delay_ms(10);
		}
		sprintf(Free_Show,"-> TIME: %2d:%2d:%2d S  ",SYS_Watch.hour,SYS_Watch.minutes,SYS_Watch.second);
		Mode_User.LCD.Show_String(0, 3, Free_Show, GBLUE, BLACK, 16);
		Data_Replace("123", Free_Show, 0, sizeof(Free_Show));
		abd = KEY_Touch_IN();
		if(abd == 0)
		{
			BZZ_Control (BZZ_ON);
			Mode_User.LCD.Show_String(21, 3, "0 ", GBLUE, BLACK, 16);
		}
		else
		{
			BZZ_Control (BZZ_OF);
			Mode_User.LCD.Show_String(21, 3, "1 ", GBLUE, BLACK, 16);
		}
		

	}
}

void Mian_Init(void)
{
	Base_Init_index();
	Mode_Init_index();

	Base_Init.SYS_Init(ENABLE);
	Base_Init.UART_x_Init(UART_3, 115200, ENABLE);
	Base_User.UART.UART_x_Send_String(UART_3, "{Ready !}\n");

	Base_Init.ADC_x_Init(MCU_Temp, ENABLE);

	Mode_Init.LCD_Init(ENABLE);
	Mode_User.LCD.Show_Picture(180, 0, 60, 60, gImage_am_60);

	if (MCU)
		Mode_User.LCD.Show_String(22, 0, "ST", BLUE, BLACK, 16);
	else
		Mode_User.LCD.Show_String(22, 0, "GD", MAGENTA, BLACK, 16); // X = 21、Y = 0 为起点 显示 "GD  "	字体为 BLUE，背景色为 BLACK，字体16
	Mode_User.LCD.Show_String(5, 0, "Caven Pro", RED, BLACK, 24);
	Mode_User.LCD.Show_String(29, 14, "1", RED, BLACK, 16); //这里是16字 的极限 0-29（30行） 0-14(15列)
	Data_Replace("123", Free_Show, '-', 26);				//分割区
	Mode_User.LCD.Show_String(0, 2, "-> MCU : 26.10 C ", BLUE, BLACK, 16);
	Mode_User.LCD.Show_String(0, 3, "-> TIME: 00:00:00 S ", GBLUE, BLACK, 16);
	Mode_User.LCD.Show_String(2, 4, Free_Show, WHITE, BLACK, 16);
	Data_Replace("123", Free_Show, 0, sizeof(Free_Show));

	KEY_Init(ENABLE);
	BZZ_Init(ENABLE);
	Base_User.Delay.Delay_S(1);
}
