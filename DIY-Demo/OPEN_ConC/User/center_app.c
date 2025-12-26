#include "center_app.h"

Caven_BaseTIME_Type Center_time;

void debug_info_handle (void *data);
void RFID_info_handle (void *data);
void usb_info_handle (void *data);

char JSON_array[500];
int JSON_len = 0;
uint8_t RFID_array[500];
int RFIDBK_len = 0;
Caven_BaseTIME_Type JSON_time,RFIDBK_time;

int time_one = 0;
int Center_State_machine(Caven_BaseTIME_Type time)
{
	int retval = 0,get_State = 0,temp_num = 0;
    Center_time = time;
	if (JSON_len)
	{
		temp_num = Center_time.SYS_Sec - JSON_time.SYS_Sec;
		if (temp_num >= 0)
		{
			temp_num *= 1000000;
			temp_num += Center_time.SYS_Us - JSON_time.SYS_Us;
		}
		if (JSON_len >= (sizeof(JSON_array) - 10))
		{
			Caven_app_JSON_Make_pack (JSON_array,TCP_HTTP_Link);
			Mode_Use.UART.Send_Data_pFun (m_UART_CH2,(uint8_t *)JSON_array,JSON_len);
			JSON_len = 0;
		}
		else if(temp_num > 3000)
		{
			Caven_app_JSON_Make_pack (JSON_array,TCP_HTTP_Link);
			Mode_Use.UART.Send_Data_pFun (m_UART_CH2,(uint8_t *)JSON_array,JSON_len);
			JSON_len = 0;
		}
	}
	if (RFIDBK_len)
	{
		temp_num = Center_time.SYS_Sec - RFIDBK_time.SYS_Sec;
		if (temp_num >= 0)
		{
			temp_num *= 1000000;
			temp_num += Center_time.SYS_Us - RFIDBK_time.SYS_Us;
		}
		if (RFIDBK_len >= (sizeof(RFID_array) - 10))
		{
			Mode_Use.UART.Send_Data_pFun (DEBUG_CH,(uint8_t *)RFID_array,RFIDBK_len);
			RFIDBK_len = 0;
		}
		else if(temp_num > 3000)
		{
			Mode_Use.UART.Send_Data_pFun (DEBUG_CH,(uint8_t *)RFID_array,RFIDBK_len);
			RFIDBK_len = 0;
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
	Mode_Use.UART.Receive_Bind_pFun (m_UART_CH2,RFID_info_handle);
	Mode_Use.USB_HID.RX_Callback_Bind(usb_info_handle);
	Caven_app_Init ();
#if SYS_BTLD == 0
	GX_app_Init ();
#endif
}

int get_debug_data_num = 0,get_debug_pack_num = 0,get_debug_pack_error1 = 0;
void debug_info_handle (void *data)
{
	uint8_t temp_data = *(uint8_t *)data;
	int temp_num = 0;
    temp_num = Caven_app_Make_pack (temp_data,SYS_Link,Center_time);
#if SYS_BTLD == 0
	if (temp_num <= 0)
	{
		temp_num = GX_app_Make_pack (temp_data,SYS_Link,Center_time);
	}
#endif
	if (temp_num <= 0 && JSON_len < sizeof(JSON_array))
	{
		JSON_array[JSON_len++] = temp_data;
		JSON_time = Center_time;
	}
	if(temp_num == 0xff)
	{
		g_SYS_Config.Connect_passage = SYS_Link;
		get_debug_pack_num ++;
	}
	else if (temp_num == -1)
	{
		get_debug_pack_error1 ++;
	}
	
	get_debug_data_num ++;
}

void usb_info_handle (void *data)
{
	uint8_t temp_data = *(uint8_t *)data;
	int temp_num = 0;
    temp_num = Caven_app_Make_pack (temp_data,USB_Link,Center_time);
#if SYS_BTLD == 0
	if (temp_num <= 0)
	{
		temp_num = GX_app_Make_pack (temp_data,USB_Link,Center_time);
	}
	if(temp_num == 0xff)
	{
		g_SYS_Config.Connect_passage = USB_Link;
	}

#endif
}

int get_RFID_data_num = 0,get_RFID_pack_num = 0,get_RFID_pack_error1 = 0;
void RFID_info_handle (void *data)
{
#if SYS_BTLD == 0
	uint8_t temp_data = *(uint8_t *)data;
	int temp_num = 0;
	
	temp_num = GX_app_Make_pack (temp_data,RS232_Link,Center_time);
	if (temp_num <= 0 && RFIDBK_len < sizeof(RFID_array))
	{
		RFID_array[RFIDBK_len++] = temp_data;
		RFIDBK_time = Center_time;
	}
	if(temp_num == 0xff)
	{
		get_RFID_pack_num ++;
	}
	else if (temp_num == -1)
	{
		get_RFID_pack_error1 ++;
	}
	get_RFID_data_num ++;
#endif
}
