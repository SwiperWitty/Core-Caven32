#include "center_app.h"

Caven_BaseTIME_Type Center_time;
Caven_BaseTIME_Type show_time;

TIM_Capture_Type time1_Capture_val;
TIM_Capture_Type time2_Capture_val;

void debug_info_handle (void *data);
void Capture1_pwm_handle (void *data);
void Capture2_pwm_handle (void *data);

Task_Overtime_Type Vofa_JustFloat_Show_Task = {
		.Switch = 1,
		.Begin_time = {0},
		.Set_time.SYS_Sec = 0,
		.Set_time.SYS_Us = 50000,
		.Flip_falg = 0,
};

char Capture_flag = 0;
int period = 0,temp_val = 0,Freq = 0,Freq_last = 0xff;
float period_t,temp_f = 0.00f,cycle = 0;
float show_buff[10];
char str_array[100];
int time_one = 0;
int Center_State_machine(Caven_BaseTIME_Type time)
{
	int retval = 0,get_State = 0;
	
    Center_time = time;
	get_State |= Caven_app_State_machine (Center_time);
	get_State |= GX_app_State_machine (Center_time);
	get_State |= System_app_State_machine (Center_time);
	
	if(g_SYS_Config.Reset_falg)
	{
		retval |= 1;
	}
	return retval;
}

void Center_app_Init (void)
{
	Mode_Use.UART.Receive_Bind_pFun (DEBUG_OUT,debug_info_handle);

	Caven_app_Init ();
	GX_app_Init ();
}

int get_debug_data_num = 0,get_debug_pack_num = 0;
void debug_info_handle (void *data)
{
	uint8_t temp_data = *(uint8_t *)data;
	int temp_num = 0;
    temp_num |= Caven_app_Make_pack (temp_data,SYS_Link,Center_time);
	if (temp_num <= 0)
	{
		temp_num |= GX_app_Make_pack (temp_data,SYS_Link,Center_time);
	}
	if(temp_num == 0xff)
	{
		get_debug_pack_num ++;
	}
	get_debug_data_num ++;
}
