#include "center_app.h"

Caven_BaseTIME_Type Center_time = {0};
Task_Overtime_Type oled_task;
TIM_Capture_Type time2_Capture_val;
void Capture2_pwm_handle (void *data);

int tim2_free = 0;
Caven_BaseTIME_Type free_time = {0};
/*

主线程，代码主要逻辑在这里
author	cavendish
date	2025.7.4
*/
int Center_State_machine(Caven_BaseTIME_Type time)
{
	int retval = 0,get_State = 0,temp_num = 0;
	float temp_ft = 0;
	char array_str[200];
	struct tm Center_tm;
	// 把UTC时间转换成日期，再把日期时间放到数组里面
	Center_time = time;
	Center_tm = Mode_Use.TIME.Get_Date_pFun (8*60*60);
	memset(array_str,0,sizeof(array_str));
	sprintf(array_str,"time %d:%02d:%02d ",Center_tm.tm_hour,Center_tm.tm_min,Center_tm.tm_sec);
	API_Task_Timer (&oled_task,Center_time);
	

	Mode_Use.OLED.Show_String_pFun (2,0,"oled show",0,0,16);
	Mode_Use.OLED.Show_String_pFun (0,1,array_str,0,0,16);
	
	// tim 捕获到信号
	if(time2_Capture_val.finish_flag)
	{
		tim2_free = time2_Capture_val.period_val;
		memset(&time2_Capture_val,0,sizeof(Caven_BaseTIME_Type));
	}
	else	// 500ms没信号就清零
	{
		temp_num = (Center_time.SYS_Sec - free_time.SYS_Sec) * 1000000 + (Center_time.SYS_Us - free_time.SYS_Us);
		if(temp_num > 500000 || free_time.SYS_Sec == 0)
		{
			tim2_free = 0;
			free_time = Center_time;
		}
	}
	if (oled_task.Trigger_Flag)		// 每隔 50ms刷新一次屏幕
	{
		temp_num = User_GPIO_get(1,8);
		sprintf(array_str,"free %d Hz %d ",tim2_free,temp_num);
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
	/* 
	Capture中断的回调函数
	*/
	TIMx_Capture_Callback_pFunBind(1,Capture2_pwm_handle);
}

void Capture2_pwm_handle (void *data)
{
	TIM_Capture_Type temp_Capture_val;
	if(data != NULL)
	{
		memcpy(&temp_Capture_val,data,sizeof(TIM_Capture_Type));
		memcpy(&time2_Capture_val,data,sizeof(TIM_Capture_Type));

	}
}
