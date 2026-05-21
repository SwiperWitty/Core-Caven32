#include "center_app.h"

/*
	写到这里的时候发现不知道写什么内容可以概况这个系统，但是又想写点什么来增加后面的可读性，增加思维加载速度
	1)系统info入口：其实无论是Rtos的轮询，还是本系统奉行的回调，本质上都在回答同一个问题，那就是如何避免消息阻塞/增加实时性，
	但是最佳的实时性基本上是在中断中调接收状态机，通过不断优化状态机运行效率来优化中断效率。这种办法在低速通信时没什么问题（实测144m的460800没什么问题），
	但这并不符合理论上的最优解，于是有了第二种方案，双缓存方案。
	双缓存方案就是在访问并处理a缓存时，让b缓存来接收（这要求系统的主循环不能有太长的停顿，至少在b缓存放满之前主循环要完成一次）。
	2)系统info出口：这个交给dma或者某一个缓存即可了。
	3)系统info处理：基本上的方案都大同小异，交给执行函数，如果一次执行不完，那就激活事件状态机/定时任务。
	4)主业务:基本是一个大的状态机，不断循环和创造或激活事件状，受info消息控制，需要注意单次循环的时间，不能影响1)的逻辑。
*/

Caven_BaseTIME_Type Center_time;

void debug_info_handle (void *data);
void RFID_info_handle (void *data);
void usb_info_handle (void *data);
void server_info_handle (void *data);
void client_info_handle (void *data);
void Other_info_handle (void *data);
int Center_app_Dual_cache_Make_pack 
(uint8_t *cache_a,int *p_len_a,uint8_t *cache_b,int *p_len_b,char *p_flag,uint8_t *p_Collect_d,int *p_Collect_n,int Collect_max,int way,Caven_BaseTIME_Type time);

char JSON_array[0x500];
int JSON_len = 0,http_json = 0;
int JSON_way = SYS_Link;
uint8_t RFID_array[0x200];
int RFIDBK_len = 0;
Caven_BaseTIME_Type JSON_time = {0},RFIDBK_time = {0};

int Center_State_machine(Caven_BaseTIME_Type time)
{
	int retval = 0,get_State = 0;

    Center_time = time;
#if NETWORK == 1
	http_json = Base_TCP_HTTP_cache_Read_Fun (JSON_array,sizeof(JSON_array));
#endif                                        

	if(JSON_len)
	{
		int diff_time = 0;
		diff_time = Caven_BaseTIME_Diff (Center_time,JSON_time);
		if(diff_time < 0)
		{
			diff_time = 0xffff;
		}
		else if(diff_time > 5000)
		{
			diff_time = 0xffff;
		}
		if(JSON_len > (sizeof(JSON_array) >> 1) || diff_time >= 0xffff)
		{
			Mode_Use.UART.Send_Data_pFun(m_UART_CH1,(uint8_t *)JSON_array,JSON_len);
			JSON_len = 0;
		}
	}

	get_State |= Caven_app_State_machine (Center_time);		// 5000 b
#if SYS_BTLD == 0
	get_State |= GX_app_State_machine (Center_time);
#endif
	get_State |= System_app_State_machine (Center_time);
	if(g_SYS_Config.temp_val->Reset_falg)
	{
		retval |= 1;
	}
	return retval;
}

/*
Center_app_Dual_cache_Make_pack
p_flag:接收缓存区0->a,1->b;
p_Collect_d: break Collect data;
p_Collect_n: break Collect num;
Collect_max: Collect max num;
way：link way;
retval:0    nop;
retval:1    pack succ;
retval:2    Collect succ;
*/
int Center_app_Dual_cache_Make_pack 
(uint8_t *cache_a,int *p_len_a,uint8_t *cache_b,int *p_len_b,char *p_flag,uint8_t *p_Collect_d,int *p_Collect_n,int Collect_max,int way,Caven_BaseTIME_Type time)
{
    int retval = 0;
    uint8_t *cache_p = NULL;
    int *len_p = NULL;
	int get_len = 0,temp_num = 0,temp_run = 0;
    if(cache_a == NULL || cache_b == NULL || p_flag == NULL)
    {
        return retval;
    }
	if(*p_flag == 0)
	{
		get_len = *p_len_a;
		if(get_len > 0)
		{
			*p_flag = 1;
			cache_p = cache_a;
            len_p = p_len_a;
            temp_run = 1;
		}
	}
	if(temp_run == 0)
	{
		get_len = *p_len_b;
		if(get_len > 0)
		{
			*p_flag = 0;
			cache_p = cache_b;
            len_p = p_len_b;
            temp_run = 2;
		}
	}
    if(temp_run)
	{
		uint8_t temp_data;
		for(int i = 0; i < get_len; i++)
		{
			temp_data = *(cache_p + i);
			temp_num = Caven_app_Make_pack (temp_data,way,time);
			if(temp_num <= 0)	
			{
				// other info

			}
			if(temp_num == 0xff)
			{
                retval = 1;
				*p_Collect_n = 0;
			}
		}
        if (retval == 0 && p_Collect_d != NULL) 
        {
            if ((*p_Collect_n + get_len) < Collect_max)
			{
                memcpy(&p_Collect_d[*p_Collect_n],cache_p,get_len);
                *p_Collect_n += get_len;
                retval = 2;
			}
        }
        *len_p = 0;
	}
    return retval;
}

void Center_app_Init (void)
{
	Mode_Use.UART.Receive_Bind_pFun (m_UART_CH1,debug_info_handle);
	Mode_Use.UART.Receive_Bind_pFun (m_UART_CH2,RFID_info_handle);
	Mode_Use.UART.Receive_Bind_pFun (m_UART_CH3,Other_info_handle);
#if NETWORK == 1
	Base_TCP_Server_Receive_Bind_Fun (server_info_handle);
	Base_TCP_Client_Receive_Bind_Fun (client_info_handle);
#endif
#if Exist_USB
	Mode_Use.USB_HID.Receive_Bind_pFun(usb_info_handle);
#endif
	Caven_app_Init ();
#if SYS_BTLD == 0
	GX_app_Init ();
#endif
}

// uint64_t tim_a,tim_b,tim_c = 0;
// call brEAK
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
	if (temp_num != 0XFF && JSON_len < sizeof(JSON_array))
	{
		JSON_array[JSON_len++] = temp_data;
		JSON_time = Center_time;
		JSON_way = SYS_Link;
	}
	if(temp_num == 0xff)
	{
		g_SYS_Config.temp_val->Connect_passage = SYS_Link;
		JSON_len = 0;
	}
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
#endif
	if (temp_num != 0XFF && JSON_len < sizeof(JSON_array))
	{
		JSON_array[JSON_len++] = temp_data;
		JSON_time = Center_time;
		JSON_way = USB_Link;
	}
	if(temp_num == 0xff)
	{
		g_SYS_Config.temp_val->Connect_passage = USB_Link;
		JSON_len = 0;
	}
}

void server_info_handle (void *data)
{
	uint8_t temp_data = *(uint8_t *)data;
	int temp_num = 0;
    temp_num = Caven_app_Make_pack (temp_data,TCP_Server_Link,Center_time);
#if SYS_BTLD == 0
	if (temp_num <= 0)
	{
		temp_num = GX_app_Make_pack (temp_data,TCP_Server_Link,Center_time);
	}

#endif
	if (temp_num != 0XFF && JSON_len < sizeof(JSON_array))
	{
		JSON_array[JSON_len++] = temp_data;
		JSON_time = Center_time;
		JSON_way = TCP_Server_Link;
	}
	if(temp_num == 0xff)
	{
		g_SYS_Config.temp_val->Connect_passage = TCP_Server_Link;
		JSON_len = 0;
	}
}

void client_info_handle (void *data)
{
	uint8_t temp_data = *(uint8_t *)data;
	int temp_num = 0;
    temp_num = Caven_app_Make_pack (temp_data,TCP_Client_Link,Center_time);
#if SYS_BTLD == 0
	if (temp_num <= 0)
	{
		temp_num = GX_app_Make_pack (temp_data,TCP_Client_Link,Center_time);
	}

#endif
	if (temp_num != 0XFF && JSON_len < sizeof(JSON_array))
	{
		JSON_array[JSON_len++] = temp_data;
		JSON_time = Center_time;
		JSON_way = TCP_Client_Link;
	}
	if(temp_num == 0xff)
	{
		g_SYS_Config.temp_val->Connect_passage = TCP_Client_Link;
		JSON_len = 0;
	}
}

void Other_info_handle (void *data)
{
	uint8_t temp_data = *(uint8_t *)data;

	int temp_num = 0;
    temp_num = Caven_app_Make_pack (temp_data,Other_Link,Center_time);

#if SYS_BTLD == 0
	if (temp_num <= 0)
	{
		temp_num = GX_app_Make_pack (temp_data,Other_Link,Center_time);
		if(temp_num <= 0)
		{
			// at
		}
	}
#endif
	if (temp_num != 0XFF && JSON_len < sizeof(JSON_array))
	{
		JSON_array[JSON_len++] = temp_data;
		JSON_time = Center_time;
		JSON_way = Other_Link;
	}
	if(temp_num == 0xff)
	{
		g_SYS_Config.temp_val->Connect_passage = Other_Link;
		JSON_len = 0;
	}
}

void RFID_info_handle (void *data)
{
#if SYS_BTLD == 0
	uint8_t temp_data = *(uint8_t *)data;
	int temp_num = 0;
	
	temp_num = GX_app_Make_pack (temp_data,RS232_Link,Center_time);
	if (temp_num != 0XFF 0 && RFIDBK_len < sizeof(RFID_array))
	{
		RFID_array[RFIDBK_len++] = temp_data;
		RFIDBK_time = Center_time;
	}
	if(temp_num == 0xff)
	{
		RFIDBK_len = 0;
	}
#endif
}
