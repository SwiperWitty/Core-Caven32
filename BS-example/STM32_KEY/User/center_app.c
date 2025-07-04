#include "center_app.h"

Caven_BaseTIME_Type Center_time;
Caven_BaseTIME_Type show_time;

int key_flag = 0;
Caven_BaseTIME_Type key_time = {0};
/*

主线程，代码主要逻辑在这里
author	cavendish
date	2025.7.4
*/
int Center_State_machine(Caven_BaseTIME_Type time)
{
	int retval = 0,get_State = 0;
	
	if(User_GPIO_get(3,13) == 0)	// 按下key
	{
		if(key_flag == 0)
		{
			key_time = time;
			key_flag = 1;
		}
		else
		{
		}
	}
	else		// 常规状态
	{
		if(key_flag == 1)	// 如果之前有按下，现在回归常规，说明按下信号结束
		{
			key_flag = 2;
		}
		else
		{
			key_flag = 0;
		}
	}
	
	if(key_flag == 2)		// 按键逻辑完成
	{
		if((time.SYS_Sec - key_time.SYS_Sec) >= 2)	// 超过2s，算长按,长按蜂鸣器响2次
		{
			User_GPIO_set (1,12,1);
			Mode_Use.TIME.Delay_Ms(200);
			User_GPIO_set (1,12,0);
			Mode_Use.TIME.Delay_Ms(200);
			User_GPIO_set (1,12,1);
			Mode_Use.TIME.Delay_Ms(200);
			User_GPIO_set (1,12,0);
		}
		else										// 短按，蜂鸣器响1次
		{
			User_GPIO_set (1,12,1);
			Mode_Use.TIME.Delay_Ms(200);
			User_GPIO_set (1,12,0);
		}
	}
	return retval;
}

void Center_app_Init (void)
{
	
}

