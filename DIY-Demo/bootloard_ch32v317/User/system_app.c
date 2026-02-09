#include "system_app.h"

/*
    系统外设数据保存
    ————26.2.6

*/
 
static const char *TAG = "SYS app";

Caven_event_Type g_SYS_events;
SYS_cfg_Type g_SYS_Config;
static SYS_val_Type s_SYS_val = {0};
Caven_BaseTIME_Type System_start_Time = {0};

int sys_bzz_event = 0;
int sys_gpo_event = 0;
int sys_rs232_event = 0;

void bzz_event_fun (void *data);
void gpo_event_fun (void *data);

Caven_BaseTIME_Type bzz_TIME = {0};
void bzz_event_fun (void *data)
{
	int temp = *(int *)data;
	if(temp == 0)
	{
		return;
	}
	if (bzz_TIME.SYS_Sec == 0 && bzz_TIME.SYS_Us == 0)
	{
		bzz_TIME = g_SYS_Config.temp_val->Now_time;
		sys_set_bzz_fun (1);
	}
	else
	{
		int temp_time = g_SYS_Config.temp_val->Now_time.SYS_Sec - bzz_TIME.SYS_Sec;
		if (temp_time >= 0)
		{
			temp_time *= 1000000;
			temp_time += g_SYS_Config.temp_val->Now_time.SYS_Us - bzz_TIME.SYS_Us;
			if (temp_time > 500000)
			{
				*(int *)data = 0;
			}
		}
		else
		{
			*(int *)data = 0;
		}
		if (*(int *)data == 0)		// 结束 
		{
			sys_set_bzz_fun (0);
			bzz_TIME.SYS_Sec = 0;
			bzz_TIME.SYS_Us = 0;
		}
	}
}

Caven_BaseTIME_Type gpo_TIME;
void gpo_event_fun (void *data)
{
	
}

int sys_set_time_fun (Caven_BaseTIME_Type time)
{
	int retval = 0;
	g_SYS_Config.temp_val->Now_time = time;
	return retval;
}

void line_gpo_set(int num,int val)
{
	switch (num)
	{
		case 1:
			User_GPIO_set(1,(3+num),val);
			break;
		case 2:
			User_GPIO_set(1,(3+num),val);
			break;
		case 3:
			User_GPIO_set(1,(3+num),val);
			break;
		case 4:
			break;
		case 5:
			break;
		default:
			
			break;
	}
}

static int gpos_state = -1;
int sys_set_gpo_fun (int gpos,int state)
{
	int retval = 0;
	int temp_num = 0,temp_val = 0,make_gpos_state = 0;
	
	temp_num = ~ gpos;
	temp_val = gpos_state & temp_num;
	make_gpos_state = temp_val | state;
	
	if (gpos_state != make_gpos_state)
	{
		for (int n = 0;n < 30;n++)
		{
			temp_num = (uint32_t)0x01 << n;
			temp_val = n + 1;
			if (make_gpos_state & temp_num)
			{
				line_gpo_set(temp_val,1);
			}
			else
			{
				line_gpo_set(temp_val,0);
			}
		}
		gpos_state = make_gpos_state;
	}
	return retval;
}

int bzz_state = 0x00;
int bzz_old = 0x00;
int sys_set_bzz_fun (int state)
{
	int retval = 0;
	bzz_state = state;
	if (bzz_old != bzz_state)
	{
		bzz_old = bzz_state;
		if (bzz_state == 0)
		{
			User_GPIO_set(1,8,1);		// BZZ
		}
		else
		{
			User_GPIO_set(1,8,0);		// BZZ
		}
	}
	return retval;
}

Caven_BaseTIME_Type sys_get_time_fun (void)
{
	return g_SYS_Config.temp_val->Now_time;
}
// 
void System_app_Restore (void)
{
	int app_crc = g_SYS_Config.app_crc;
	Debug_OutStr("System_app_Restore ...\n");
	memset(&g_SYS_Config,0,sizeof(g_SYS_Config));
	g_SYS_Config.temp_val = &s_SYS_val;
#if SYS_BTLD == 1
	// 在bt重置会删app
	g_SYS_Config.Bt_mode = 0;
#else
	g_SYS_Config.Bt_mode = 1;
#endif
    g_SYS_Config.Board_ID = 0;
    g_SYS_Config.debug = 1;
	memcpy(g_SYS_Config.Hostname,DEMO_Name_str,strlen(DEMO_Name_str));
	g_SYS_Config.Bddate = DEMO_Build_str;
	g_SYS_Config.Version[0] = DEMO_VER;
	g_SYS_Config.Version[1] = DEMO_VER_sub;
	g_SYS_Config.Version[2] = DEMO_VER_sub_bit;
	
    g_SYS_Config.Addr = 1;
	g_SYS_Config.SYS_UART_Cfg = 115200;
    g_SYS_Config.RS232_UART_Cfg = 115200;
    g_SYS_Config.RS485_UART_Cfg = 115200;
    g_SYS_Config.CANCfg = 0;
    g_SYS_Config.BLECfg = 0;
#if NETWORK
    g_SYS_Config.eth_mode = 0;		// 静态
    g_SYS_Config.wifi_mode = 1;		// 自动
	g_SYS_Config.eth_En = 1;
    g_SYS_Config.wifi_En = 0;
    g_SYS_Config.NetCardCfg = 0;	// 物联网卡
	
	strcpy(g_SYS_Config.eth_ip_str,"192.168.1.168");
	strcpy(g_SYS_Config.eth_gw_str,"192.168.1.1");
	strcpy(g_SYS_Config.eth_netmask_str,"255.255.255.0");
	strcpy(g_SYS_Config.eth_DNS1_str,"114.114.114.114");
	strcpy(g_SYS_Config.eth_DNS2_str,"8.8.8.8");
	
    g_SYS_Config.tcp_server_enable = 1;
    g_SYS_Config.tcp_client_enable = 1;
    g_SYS_Config.tcp_http_enable = 0;
    g_SYS_Config.tcp_mqtt_enable = 0;
    g_SYS_Config.tcp_udp_enable = 0;

    g_SYS_Config.Heartbeat_num = 0;
    g_SYS_Config.Heartbeat_Run = 0;
	g_SYS_Config.Heartbeat_cycle = 3;
    g_SYS_Config.Heartbeat_MAX = 5;

    g_SYS_Config.TCPHBT_En = 1;
	g_SYS_Config.Server_break_off = 0;
	strcpy(g_SYS_Config.TCPServer_port,"8160");
	strcpy(g_SYS_Config.TCPClient_url,"192.168.1.128:9090");

    g_SYS_Config.HTTPHBT_En = 1;
    g_SYS_Config.HTTP_cycle = 5;
	strcpy(g_SYS_Config.HTTP_url,"http://192.168.1.128:8080");
	strcpy(g_SYS_Config.MQTTCfg,"url<dianjixz.online:1883>U<nihao>P<nihao3.14>Ptopic<send>Stopic<reace>Id<caven_test>");
	strcpy(g_SYS_Config.UDPCfg,"null");
	strcpy(g_SYS_Config.UDP_multicast_str,"null");
#endif
	g_SYS_Config.temp_val->Connect_passage = SYS_Link;

	g_SYS_Config.app_crc = app_crc;
	System_app_SYS_Config_Save ();
}

/*
	save SYS_Config
*/
int System_app_SYS_Config_Save (void)
{
	int retval = -1;
	retval = Base_Flash_Erase (SYS_CFG_ADDR,sizeof(g_SYS_Config));
	if(retval == 0)
	{
		retval = Base_Flash_Write (&g_SYS_Config,SYS_CFG_ADDR,sizeof(g_SYS_Config));
	}
	return retval;
}

int System_app_save_UTCtime (void)
{
	int retval = System_app_SYS_Config_Save();
	return retval;
}
int System_app_save_Addr (void)
{
	int retval = System_app_SYS_Config_Save();
	return retval;
}
int System_app_save_RS232Cfg (void)
{
	int retval = System_app_SYS_Config_Save();
	return retval;
}
int System_app_save_RS485Cfg (void)
{
	int retval = System_app_SYS_Config_Save();
	return retval;
}
int System_app_save_IPv4Cfg (void)
{
	int retval = System_app_SYS_Config_Save();
	return retval;
}
int System_app_save_TCPHBT (void)
{
	int retval = System_app_SYS_Config_Save();
	return retval;
}
int System_app_save_TCPServer (void)
{
	int retval = System_app_SYS_Config_Save();
	return retval;
}
int System_app_save_TCPClient (void)
{
	int retval = System_app_SYS_Config_Save();
	return retval;
}
int System_app_save_HTTPHBT (void)
{
	int retval = System_app_SYS_Config_Save();
	return retval;
}
int System_app_save_HTTPCfg (void)
{
	int retval = System_app_SYS_Config_Save();
	return retval;
}
int System_app_save_MQTTCfg (void)
{
	int retval = System_app_SYS_Config_Save();
	return retval;
}
int System_app_save_boot (void)
{
	int retval = System_app_SYS_Config_Save();
	return retval;
}
int System_app_save_debug (void)
{
	int retval = System_app_SYS_Config_Save();
	return retval;
}
int System_app_save_Serial (void)
{
	int retval = System_app_SYS_Config_Save();
	return retval;
}
int System_app_save_MACCfg (void)
{
	int retval = System_app_SYS_Config_Save();
	return retval;
}

/*
	gain SYS_Config
*/
int System_app_SYS_Config_Gain (void)
{
	int retval = 0;
	Base_Flash_Read (&g_SYS_Config,SYS_CFG_ADDR,sizeof(g_SYS_Config));
	// g_SYS_Config.debug = 0;
	g_SYS_Config.Bddate = DEMO_Build_str;
	g_SYS_Config.Version[0] = DEMO_VER;
	g_SYS_Config.Version[1] = DEMO_VER_sub;
	g_SYS_Config.Version[2] = DEMO_VER_sub_bit;
	g_SYS_Config.Version[3] = 0;
	memcpy(g_SYS_Config.Hostname,DEMO_Name_str,sizeof(DEMO_Name_str));
	if(g_SYS_Config.debug > 0X0F || g_SYS_Config.debug == 0)
	{
		System_app_Restore ();
	}
	g_SYS_Config.temp_val->Reset_falg = 0;
#if SYS_BTLD == 0
	// 在app层发现bt不在app，需要重置bt
	if (g_SYS_Config.Bt_mode == 0)
	{
		System_app_Restore ();
	}
#endif
	return retval;
}

int System_app_save_ip (void)
{
	int retval = 0;
	retval = System_app_SYS_Config_Save ();
	return retval;
}

int cg_rs232_cfg = 0,cg_rs485_cfg = 0,cg_rj45_cfg = 0;
int System_app_State_machine (Caven_BaseTIME_Type time)
{
	int retval = 0;
    int	temp_val = 0;
    g_SYS_Config.temp_val->Now_time = time;
	/*
	工作时长统计
	*/
    if (System_start_Time.SYS_Sec != g_SYS_Config.temp_val->Now_time.SYS_Sec)
    {
        System_start_Time = g_SYS_Config.temp_val->Now_time;
        g_SYS_Config.temp_val->Work_sec ++;
		User_GPIO_set(2,15,1);	// rfid
		User_GPIO_set(1,1,1);	// info
		User_GPIO_set(2,0,System_start_Time.SYS_Sec % 2);
    }
#if NETWORK == 1
	Base_ETH_Task ();
	g_SYS_Config.temp_val->Net_falg = Base_ETH_get_status ();
	User_GPIO_set(1,0,!g_SYS_Config.temp_val->Net_falg);
#endif
	/*
	ota自动升级
	*/
	if (1)
	{
		
	}
	if (g_SYS_Config.temp_val->Reset_falg)
	{
		Mode_Use.TIME.Delay_Ms (100);
		// stb_printf("RST SYS UTC %ds,work %ds \n",g_SYS_Config.Now_time.SYS_Sec,g_SYS_Config.Work_sec);
	}
	if(cg_rs232_cfg == 0)
	{
		cg_rs232_cfg = g_SYS_Config.RS232_UART_Cfg;
	}
	else if (cg_rs232_cfg != g_SYS_Config.RS232_UART_Cfg)
	{
		temp_val = 1;
		cg_rs232_cfg = g_SYS_Config.RS232_UART_Cfg;
		Mode_Use.TIME.Delay_Ms(100);
		Mode_Init.UART(DEBUG_CH,g_SYS_Config.RS232_UART_Cfg,ENABLE);
	}
	if(cg_rs485_cfg == 0)
	{
		cg_rs485_cfg = g_SYS_Config.RS485_UART_Cfg;
	}
	else if (cg_rs485_cfg != g_SYS_Config.RS485_UART_Cfg)
	{
		temp_val = 1;
		cg_rs485_cfg = g_SYS_Config.RS485_UART_Cfg;
	}
	if (temp_val)
	{
		System_app_SYS_Config_Save ();
	}
	/*
	初始化失败
	*/
	if(g_SYS_Config.temp_val->init_finish_state != 1)
	{
		
	}
    Caven_handle_event_Fun(&g_SYS_events);
    return retval;
}

void System_app_Init (void)
{
	int temp_num;
	NVIC_VECTOR_SET(SYS_RUN_ADDR);
	g_SYS_Config.temp_val = &s_SYS_val;
	System_app_SYS_Config_Gain ();

#if SYS_BTLD == 1
	if (g_SYS_Config.Bt_mode)
	{
		uint8_t * addr_p = (uint8_t *)SYS_APP_ADDR;
		int temp_sum;
		temp_sum = Encrypt_XMODEM_CRC16_Fun(addr_p, SYS_APP_SIZE);
		if (temp_sum == g_SYS_Config.app_crc)
		{
			Debug_OutStr("GO_TO_APP ...\n");
			Mode_Use.TIME.Delay_Ms(20);
			GO_TO_APP(SYS_APP_ADDR);
		}
	}
	Mode_Init.TIME(ENABLE);
	Mode_Init.UART(DEBUG_CH,115200,ENABLE);
	Debug_OutStr("GO_TO_APP fail \n");

#else
	Mode_Init.TIME(ENABLE);
    Mode_Use.TIME.Delay_Ms(1);
	Mode_Init.UART(DEBUG_CH,g_SYS_Config.RS232_UART_Cfg,ENABLE);
	Mode_Init.UART(m_UART_CH2,g_SYS_Config.RS232_UART_Cfg,ENABLE);

	#ifdef MCU_SYS_FREQ 
	stb_printf("MCU Init,MCU_SYS_FREQ: %d Hz \n",MCU_SYS_FREQ);
	#endif
	stb_printf("MCU build date %s \n",g_SYS_Config.Bddate);
	
	User_GPIO_config(1,4,1);
	User_GPIO_config(1,5,1);
	User_GPIO_config(1,6,1);
	User_GPIO_config(1,8,1);

	
	User_GPIO_set(1,4,1);		// GPOA
	User_GPIO_set(1,5,1);		// GPOB
	User_GPIO_set(1,6,1);		// GPOC
	User_GPIO_set(1,8,1);		// BZZ

	Caven_new_event_Fun(&g_SYS_events,bzz_event_fun,&sys_bzz_event);
	Caven_new_event_Fun(&g_SYS_events,gpo_event_fun,&sys_gpo_event);
#endif
	User_GPIO_config(2,15,1);
	User_GPIO_config(2,0,1);
	User_GPIO_config(1,0,1);
	User_GPIO_config(1,1,1);

	User_GPIO_set(2,15,1);		// rfid_LED
	User_GPIO_set(2,0,1);		// run
	User_GPIO_set(1,0,1);		// net
	User_GPIO_set(1,1,1);		// info
#if Exist_USB
	Mode_Init.USB(ENABLE);
#endif
#if (NETWORK == 1)
	char array_ip[100],array_port[20];
    Base_ETH_config_local_ip (g_SYS_Config.eth_mode,
                    g_SYS_Config.eth_ip_str,
                    g_SYS_Config.eth_gw_str,
                    g_SYS_Config.eth_netmask_str);
    Base_ETH_Init(0x02,g_SYS_Config.eth_En);

    Mode_Use.TIME.Delay_S(2);
    Base_TCP_Server_Config (g_SYS_Config.TCPServer_port,g_SYS_Config.Server_break_off,g_SYS_Config.tcp_server_enable);
    memset(array_ip,0,sizeof(array_ip));
    memset(array_port,0,sizeof(array_port));
    temp_num = Base_ETH_IPprot (g_SYS_Config.TCPClient_url,array_ip,array_port);
    if (temp_num >= 0)
    {
        stb_printf("TCPClient_url to ip: %s,port:%s \n",array_ip,array_port);
        Base_TCP_Client_Config (array_ip,array_port,g_SYS_Config.tcp_client_enable);
    }
#endif
	if(g_SYS_Config.Bt_mode == 0)
	{
		Debug_OutStr("MCU running bootloard ...\n");
	}
	else 
	{
		Debug_OutStr("MCU running app ...\n");
	}
	(void)temp_num;
}
