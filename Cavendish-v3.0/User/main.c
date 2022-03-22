#include "stm32f10x.h"
#include "sys_time.h"
#include "lcd.h"
#include "pic.h"
#include "Data_Handle.h"

#include "key.h"
#include "BZZ.h"
#include "Tracing.h"

#include "Steering_engine.h"
#include "motor.h"
#include "encoder.h"

#include "Ultrasonic.h"
#include "mlx90614.h"

#include "uart.h"
#include "dac.h"
#include "adc.h"
#include "stmflash.h"

#define GD 		0
#define ST  	1
#define MCU		GD

char Free_Array[16];

//要写入到STM32 FLASH的字符串数组
#define SIZE sizeof(Free_Array)			//数组长度
#define FLASH_SAVE_ADDR  0X08012000		//FLASH_SAVE_ADDR >= 本代码所占用FLASH的大小(16进制) + 0X08000000 + 0X800)

float far,temp,vol;
char array_watch[16];
char clo = 0;
int CL = RED;
int X_axis_num = 0;

float a,b;

void Mian_Init(void);
int main(void)
{
	Mian_Init();
	while(1)
	{
		vol = Read_MCU_Temp();
		sprintf(Free_Array,"MCU: %.2f C ",vol);
		LCD_ShowString(2,2,Free_Array,WHITE,BLACK,16); Data_Replace ("123",Free_Array,0,sizeof(Free_Array));
		
		sprintf(Free_Array,"DAC: %.2f V ",DACx_Set_Vol(DAC_1,1.8));			//设置 DAC 电压
		LCD_ShowString(0 ,4,Free_Array,BLUE,BLACK,16); Data_Replace ("123",Free_Array,0,sizeof(Free_Array));
		
		vol = ADCx_Read_Vol (ADC_2);										//读 ADC 电压
		sprintf(Free_Array,"ADC: %.2f V ",vol);
		LCD_ShowString(19 ,4,Free_Array,RED,BLACK,16); Data_Replace ("123",Free_Array,0,sizeof(Free_Array));
		
		
		
		temp = Mlx90614_ReadTemp();
//		far = Distance();													//读 超声波距离
		sprintf(Free_Array,"  %.2f c   ",temp);
		LCD_ShowString(10,5,Free_Array,WHITE,BLACK,16); Data_Replace ("123",Free_Array,0,sizeof(Free_Array));

		sprintf(Free_Array,"{*HX%dY%.2fH*} \r\n",X_axis_num ++,temp);			//把距离数据传给蓝牙
		UARTx_Send_Data(UART_3,Free_Array,16);									// 蓝牙与上位机通信协议
		Data_Replace ("123",Free_Array,0,sizeof(Free_Array));
		
		LCD_ShowString(12,6,Tracing_Find (),WHITE,BLACK,16);					// 循迹读取 + 显示

		sprintf(array_watch, "Time: %2d:%2d:%2d  ", Timewatch.hour, Timewatch.minutes, Timewatch.second);
		LCD_ShowString(8,13,array_watch,BLUE,BLACK,16); Data_Replace("123", array_watch, 0, sizeof(array_watch));
		
		if(CV_UART.Read_Flag[3])																								// 串口收到数据
		{
			CV_UART.Read_Flag[3] = 0;
			if(CV_UART.UARTx_Array[3][0] == '{')																				// 协议格式正确
			{
				if(Data_Judge("{a:}",CV_UART.UARTx_Array[3],sizeof("{a:}")))							// 校验（比较函数）
					a = Data_Chang_NUM (CV_UART.UARTx_Array[3],sizeof(CV_UART.UARTx_Array[3]));
				
				else if(Data_Judge("{b:}",CV_UART.UARTx_Array[3],sizeof("{b:}")))							// 校验（比较函数）
					b = Data_Chang_NUM (CV_UART.UARTx_Array[3],sizeof(CV_UART.UARTx_Array[3]));
				else											
				{
					temp = Data_Chang_NUM (CV_UART.UARTx_Array[3],sizeof(CV_UART.UARTx_Array[3]));
					sprintf(Free_Array,"Str_num: %.1f  ",temp);
					LCD_ShowString(8 ,7,Free_Array,BLUE,BLACK,16); Data_Replace ("123",Free_Array,0,sizeof(Free_Array));		// 显示 串口发送的 字符串类型数字
				}
			}
			else
				UARTx_Send_String(UART_3,"{Error !}\r\n");																		// 协议格式错误
		}
		
		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 0)
		{
			temp = 0;
//			BZZ_Control (BZZ_ON);
			do{
				Delay_ms(20);
				temp++;
				if(temp > 50)
				{
					Timewatch.sys_time = 0;
				}
			}
			while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 0);
			BZZ_Control (BZZ_OF);
			Free_Array[0] = Timewatch.sys_time;
			STMFLASH_Write(FLASH_SAVE_ADDR, (u16*)Free_Array, 2);
			clo++;
			if(clo > 4) clo = 1;
		}
		sprintf((char *)Free_Array, "%2d h",clo);
		LCD_ShowString(18,2,Free_Array,CL,BLACK,16);
		
		if(clo == 1)
		{
			Motorx_Control(Motor_1,800);
			Motorx_Control(Motor_2,0);
			Motorx_Control(Motor_3,0);
			Motorx_Control(Motor_4,0);
		}
		else if(clo == 2)
		{
			Motorx_Control(Motor_1,-100);
			Motorx_Control(Motor_2,-800);
			Motorx_Control(Motor_3,0);
			Motorx_Control(Motor_4,0);
		}
		else if(clo == 3)
		{
			Motorx_Control(Motor_1,0);
			Motorx_Control(Motor_2,0);
			Motorx_Control(Motor_3,800);
			Motorx_Control(Motor_4,0);
		}
		else if(clo == 4)
		{
			Motorx_Control(Motor_1,0);
			Motorx_Control(Motor_2,0);
			Motorx_Control(Motor_3,0);
			Motorx_Control(Motor_4,-800);
		}
		if(clo % 2) CL = BLUE;
		else CL = RED; 
		Delay_ms(20);
	}
}


void Mian_Init(void)
{
	if(SysTick_Config(SystemCoreClock/100000))         //10us定时器
	{
		while(1);
	}
	
	LCD_Init();											// LCD显示器 初始化
	
	ADCx_Init(MCU_Temp);							// MCU内部温度 初始化
	ADCx_Init(ADC_2);								// ADC_2 初始化
	DACx_Init(DAC_1);                             // DCA_2 初始化                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
	DACx_Set_Vol(DAC_1,2.5);							// DCA 设置电压输出为 2.5V
//	Tracing_Init ();									// 循迹
//	Ultrasonic_Init();									// 超声波
	Mlx90614_Init();									// 红外测温
	KEY_Init();											// 触摸按键
	BZZ_Init();											// 蜂鸣器
	UARTx_Init (UART_3,9600);
	Init_Steering_Engine_T4();							// 舵机初始化
	Motorx_Init(Motor_1);
	Motorx_Init(Motor_2);
	Motorx_Init(Motor_3);
	Motorx_Init(Motor_4);								// 电机初始化
	
	Motorx_Control(Motor_1,800);
	Motorx_Control(Motor_2,400);
	Motorx_Control(Motor_3,400);
	Motorx_Control(Motor_4,500);							// 电机初始转速
			
	Steering_Engine_Angle(1,0);
	Steering_Engine_Angle(2,0);
	Steering_Engine_Angle(3,0);
	Steering_Engine_Angle(4,0);							// 舵机全部归零
	
	LCD_Fill(0,0,LCD_W,LCD_H,BLACK);
	
	LCD_ShowPicture(180,0,60,60,gImage_am_60);			// 显示AM_60这个图片
	if(MCU == ST)	LCD_ShowString(20,0,"ST ",BLUE,BLACK,16);
	else	LCD_ShowString(21,0,"GD  ",BLUE,BLACK,16);				// X = 21、Y = 0 为起点 显示 "GD  "	字体为 BLUE，背景色为 BLACK，字体16
	LCD_ShowString(5,0,"One Day",BLUE,BLACK,24);
	LCD_ShowString(7,14,"Fight, young man!",BLUE,BLACK,16);
	LCD_ShowString(29,14,"1",RED,BLACK,16);							//这里是16字 的极限 0-29（30行） 0-14(15列)
	
	STMFLASH_Read(FLASH_SAVE_ADDR, (u16*)Free_Array, 2);
	Timewatch.sys_time = Free_Array[0];
	Timewatch.hour = (Timewatch.sys_time / 3600) % 24;
	Timewatch.minutes = (Timewatch.sys_time / 60) % 60;
	Timewatch.second = Timewatch.sys_time % 60;
	
//	Delay_S(1);
	
}


