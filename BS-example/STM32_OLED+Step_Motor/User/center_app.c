#include "center_app.h"

Caven_BaseTIME_Type Center_time = {0};
Task_Overtime_Type oled_task;
int drive_num = 0;

/*

主线程，代码主要逻辑在这里
author	cavendish
date	2025.7.4
*/
int Center_State_machine(Caven_BaseTIME_Type time)
{
	int retval = 0,get_State = 0;
	char array_str[200];
	struct tm Center_tm;
	
	Center_time = time;
	Center_tm = Mode_Use.TIME.Get_Date_pFun (8*60*60);
	memset(array_str,0,sizeof(array_str));
	sprintf(array_str,"time %d:%02d:%02d ",Center_tm.tm_hour,Center_tm.tm_min,Center_tm.tm_sec);
	API_Task_Timer (&oled_task,Center_time);
	
	/* 
	每三秒转两圈
	*/
	if (Center_time.SYS_Sec % 3)
	{
		Mode_Use.Motor_BYJ.Drive(1,1,2);
	}
	Mode_Use.OLED.Show_String_pFun (2,0,"oled show",0,0,16);
	Mode_Use.OLED.Show_String_pFun (0,1,array_str,0,0,16);
	
	if (oled_task.Trigger_Flag)					// 每隔 50ms刷新一次
	{
		sprintf(array_str,"drive_num %d ",drive_num);
		Mode_Use.OLED.Show_String_pFun (0,2,array_str,0,0,16);
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
}

