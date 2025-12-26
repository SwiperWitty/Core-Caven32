#include "center_app.h"

Caven_BaseTIME_Type Center_time;

void debug_info_handle (void *data);

char JSON_array[500];
int JSON_len = 0;
Caven_BaseTIME_Type JSON_time;

int time_one = 0;
int Center_State_machine(Caven_BaseTIME_Type time)
{
	int retval = 0,get_State = 0;
    Center_time = time;
	if (JSON_len)
	{
		if (JSON_len >= (sizeof(JSON_array) - 10))
		{
			Caven_app_JSON_Make_pack (JSON_array,TCP_HTTP_Link);
			JSON_len = 0;
		}
		else if((Center_time.SYS_Sec - JSON_time.SYS_Sec) > 1)
		{
			Caven_app_JSON_Make_pack (JSON_array,TCP_HTTP_Link);
			JSON_len = 0;
		}
	}
	
	get_State |= Caven_app_State_machine (Center_time);
#if SYS_BTLD == 0
	get_State |= GX_app_State_machine (Center_time);
#endif
	get_State |= System_app_State_machine (Center_time);
	if(g_SYS_Config.Reset_falg)
	{
		retval |= 1;
	}
	return retval;
}

void Center_app_Init (void)
{
	Mode_Use.UART.Receive_Bind_pFun (DEBUG_CH,debug_info_handle);

	Caven_app_Init ();
#if SYS_BTLD == 0
	GX_app_Init ();
#endif
}

int get_debug_data_num = 0,get_debug_pack_num = 0;
void debug_info_handle (void *data)
{
	uint8_t temp_data = *(uint8_t *)data;
	int temp_num = 0;
    temp_num |= Caven_app_Make_pack (temp_data,SYS_Link,Center_time);
#if SYS_BTLD == 0
	if (temp_num <= 0)
	{
		temp_num |= GX_app_Make_pack (temp_data,SYS_Link,Center_time);
	}
#endif
	if (temp_num <= 0 && JSON_len < sizeof(JSON_array))
	{
		JSON_array[JSON_len++] = temp_data;
		JSON_time = Center_time;
	}
	if(temp_num == 0xff)
	{
		get_debug_pack_num ++;
	}
	
	get_debug_data_num ++;
}
