#include "center_app.h"

Caven_BaseTIME_Type Center_time = {0};
Task_Overtime_Type oled_task;
int drive_num = 0;
u16 Moisture = 0;
u16 Light = 0;

void ADC_CallBack_Fun (void * data);
void Motor_Drive (int num);
/*

主线程，代码主要逻辑在这里
author	cavendish
date	2025.7.4
*/
int Center_State_machine(Caven_BaseTIME_Type time)
{
	int retval = 0,get_State = 0;
	float temp_ft = 0;
	char array_str[200];
	struct tm Center_tm;
	
	Center_time = time;
	Center_tm = Mode_Use.TIME.Get_Date_pFun (8*60*60);
	memset(array_str,0,sizeof(array_str));
	sprintf(array_str,"time %d:%02d:%02d ",Center_tm.tm_hour,Center_tm.tm_min,Center_tm.tm_sec);
	API_Task_Timer (&oled_task,Center_time);
	
	/* 
	每2s检测一次
	*/
	if ((Center_time.SYS_Sec % 2) == 0)
	{
		if (Moisture > 2000)
		{
			Motor_Drive (1);
			Mode_Use.TIME.Delay_Ms (600);
			Motor_Drive (0);
		}
	}
	Mode_Use.OLED.Show_String_pFun (2,0,"oled show",0,0,16);
	Mode_Use.OLED.Show_String_pFun (0,1,array_str,0,0,16);
	
	if (oled_task.Trigger_Flag)					// 每隔 50ms刷新一次
	{
		sprintf(array_str,"Moisture %d   ",Moisture);
		Mode_Use.OLED.Show_String_pFun (0,2,array_str,0,0,16);
		sprintf(array_str,"Light %d   ",Light);
		Mode_Use.OLED.Show_String_pFun (0,3,array_str,0,0,16);
		Mode_Use.OLED.Refresh();
	}
	return retval;
}

void Center_app_Init (void)
{
	oled_task.Begin_time = Center_time;
	oled_task.Set_time.SYS_Sec = 0;
	oled_task.Set_time.SYS_Us = 50000;		// 50ms触发一次任务
	oled_task.Switch = 1;
	
	Mode_Use.Base_ADC.Receive_Bind_pFun (ADC_CallBack_Fun);
}

void ADC_CallBack_Fun (void * data)
{
	if(data != NULL)
	{
		Moisture = ((u16 *)data)[0];
		Light = ((u16 *)data)[1];
	}
}

/*
num:
1 正转
2 反转
other 停止
*/
void Motor_Drive (int num)
{
	switch(num)
	{
	case (1):
		User_GPIO_set (1,0,1);
		User_GPIO_set (1,1,1);
		User_GPIO_set (1,1,0);
		break;
	case (2):
		User_GPIO_set (1,0,1);
		User_GPIO_set (1,1,1);
		User_GPIO_set (1,0,0);
		break;
	default:
		User_GPIO_set (1,0,1);
		User_GPIO_set (1,1,1);
		break;
	}
} 
