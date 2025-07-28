#include "center_app.h"

Caven_BaseTIME_Type Center_time = {0};
Task_Overtime_Type oled_task;

void get_uart2_data (void *data);
int uart2_num = 0;
Caven_BaseTIME_Type uart2_time;
u8 uart2_array[200];
int face_id = 0;
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
	
	if(uart2_num)
	{
		int temp_time = Center_time.SYS_Sec - uart2_time.SYS_Sec;
		char get_array[100];
		temp_time *= 1000000;
		temp_time += Center_time.SYS_Us - uart2_time.SYS_Us;
		if(uart2_num > 100 || temp_time > 100000)		// 满足处理数据条件
		{
			get_State = Caven_gain_str_by_sign((char *)uart2_array,uart2_num,get_array,"face id:",0);
			face_id = atoi(get_array);
			uart2_num = 0;
			memset(uart2_array,0,sizeof(uart2_array));
			memset(get_array,0,sizeof(get_array));
		}
	}
	/*
	遇到 face_id：1 2 3都开门，其他不开，关门信号由视觉发送{v831:face id:0}
	*/
	switch(face_id)
	{
		case 1:
		{
			Mode_Use.Steering_Engine.Set_Angle (1,90);
		}
		break;
		case 2:
		{
			Mode_Use.Steering_Engine.Set_Angle (1,90);
		}
		break;
		case 3:
		{
			Mode_Use.Steering_Engine.Set_Angle (1,90);
		}
		break;
		default :
			Mode_Use.Steering_Engine.Set_Angle (1,0);
		break;
	}

	Mode_Use.OLED.Show_String_pFun (2,0,"oled show",0,0,16);
	if (oled_task.Trigger_Flag)					// 每隔 50ms刷新一次
	{
		sprintf(array_str,"face id:%4d ",face_id);
		Mode_Use.OLED.Show_String_pFun (0,1,array_str,0,0,16);
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
	
	Mode_Use.UART.Receive_Bind_pFun (2,get_uart2_data);		// 视觉数据
}

void get_uart2_data (void *data)
{
	if(data != 	NULL && (uart2_num < sizeof(uart2_array)))
	{
		uart2_array[uart2_num++] = *(u8 *)data;
		uart2_time = Center_time;
	}
}
