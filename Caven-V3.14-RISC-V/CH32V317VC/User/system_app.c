#include"system_app.h"

static const char *TAG = "SYS app";

SYS_cfg_Type g_SYS_Config;
Caven_event_Type g_SYS_events;

Task_Overtime_Type led_task;
void system_cfg_memory_init (void)
{

}

void System_app_Restore (void)
{
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
	
	memset(g_SYS_Config.eth_ip_str,0,sizeof(g_SYS_Config.eth_ip_str));
	memset(g_SYS_Config.eth_gw_str,0,sizeof(g_SYS_Config.eth_gw_str));
	memset(g_SYS_Config.eth_netmask_str,0,sizeof(g_SYS_Config.eth_netmask_str));
	memset(g_SYS_Config.eth_DNS1_str,0,sizeof(g_SYS_Config.eth_DNS1_str));
	memset(g_SYS_Config.eth_DNS2_str,0,sizeof(g_SYS_Config.eth_DNS2_str));
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
    g_SYS_Config.Heartbeat_MAX = 5;

    g_SYS_Config.TCPHBT_En = 1;
    g_SYS_Config.Server_break_off = 0;
	memset(g_SYS_Config.TCPServer_port,0,sizeof(g_SYS_Config.TCPServer_port));
	strcpy(g_SYS_Config.TCPServer_port,"8160");
	memset(g_SYS_Config.TCPClient_url,0,sizeof(g_SYS_Config.TCPClient_url));
	strcpy(g_SYS_Config.TCPClient_url,"192.168.1.128:9090");

    g_SYS_Config.HTTPHBT_En = 1;
    g_SYS_Config.HTTP_cycle = 5;
	memset(g_SYS_Config.HTTP_url,0,sizeof(g_SYS_Config.HTTP_url));
	strcpy(g_SYS_Config.HTTP_url,"http://192.168.1.128:8080");

#endif
    g_SYS_Config.Reset_falg = 0;
    g_SYS_Config.Work_falg = 0;
	g_SYS_Config.Net_falg = 0;
 
    g_SYS_Config.init_finish_state = 0;
    g_SYS_Config.Connect_passage = SYS_Link;
    g_SYS_Config.Work_sec = 0;
	memset(&g_SYS_Config.Now_time,0,sizeof(g_SYS_Config.Now_time));
	// System_app_SYS_Config_Save ();
}

/*
	save SYS_Config
*/
int System_app_SYS_Config_Save (void)
{
	int retval = 0;
	Base_Flash_Erase (SYS_CFG_ADDR,sizeof(g_SYS_Config));
	Base_Flash_Write (&g_SYS_Config,SYS_CFG_ADDR,sizeof(g_SYS_Config));
	return retval;
}

/*
	gain SYS_Config
*/
int System_app_SYS_Config_Gain (void)
{
	int retval = 0;
	Base_Flash_Read (&g_SYS_Config,SYS_CFG_ADDR,sizeof(g_SYS_Config));
	if(g_SYS_Config.debug > 0X0F || g_SYS_Config.debug == 0)
	{
		System_app_Restore ();
	}
	g_SYS_Config.Reset_falg = 0;
#if SYS_BTLD == 0
	// 在app层发现bt不在app，需要重置bt
	if (g_SYS_Config.Bt_mode == 0)
	{
		System_app_Restore ();
	}
#endif
	return retval;
}

void System_app_Init (void)
{
	int temp_num;
	char array_ip[100],array_port[20];
	System_app_SYS_Config_Gain ();

#if SYS_BTLD == 1
	NVIC_VECTOR_SET(SYS_STR_ADDR);
	if (g_SYS_Config.Bt_mode)
	{
		int temp_num = g_SYS_Config.AppEnd_addr - SYS_APP_ADDR;
		if (temp_num > 0)
		{
			uint8_t * addr_p = (uint8_t *)SYS_APP_ADDR;
			int temp_sum;
			temp_sum = Encrypt_XMODEM_CRC16_Fun(addr_p, temp_num);
			if (temp_sum == g_SYS_Config.app_crc)
			{
				GO_TO_APP(SYS_APP_ADDR);
			}
		}
	}
	else
	{
		__enable_irq();
		Mode_Init.TIME(ENABLE);
		Mode_Use.TIME.Delay_Ms(10);
		Mode_Init.UART(DEBUG_CH,115200,ENABLE);
		Mode_Init.USB(ENABLE);
	}
#else
	// NVIC_VECTOR_SET(SYS_APP_ADDR);
	// __enable_irq();
	
	Mode_Init.TIME(ENABLE);
    Mode_Use.TIME.Delay_Ms(10);
	Mode_Init.UART(DEBUG_CH,115200,ENABLE);
	Mode_Init.UART(m_UART_CH2,g_SYS_Config.RS232_UART_Cfg,ENABLE);
	// Mode_Init.UART(m_UART_CH3,g_SYS_Config.RS232_UART_Cfg,ENABLE);

	#ifdef MCU_SYS_FREQ 
	stb_printf("MCU Init,MCU_SYS_FREQ: %d Hz \n",MCU_SYS_FREQ);
	#endif
	stb_printf("MCU build date %s \n",g_SYS_Config.Bddate);

    Base_ETH_config_local_ip (g_SYS_Config.eth_mode,
                    g_SYS_Config.eth_ip_str,
                    g_SYS_Config.eth_gw_str,
                    g_SYS_Config.eth_netmask_str);
    Base_ETH_Init(0x02,g_SYS_Config.eth_En);

    Mode_Use.TIME.Delay_S(3);
    Base_TCP_Server_Config (g_SYS_Config.TCPServer_port,g_SYS_Config.Server_break_off,g_SYS_Config.tcp_server_enable);
    memset(array_ip,0,sizeof(array_ip));
    memset(array_port,0,sizeof(array_port));
    temp_num = Base_ETH_IPprot (g_SYS_Config.TCPClient_url,array_ip,array_port);
    if (temp_num >= 0)
    {
        stb_printf("TCPClient_url to ip: %s,port:%s \n",array_ip,array_port);
        Base_TCP_Client_Config (array_ip,array_port,g_SYS_Config.tcp_client_enable);
    }
	// Mode_Init.USB(ENABLE);
	User_GPIO_config(2,0,1);
	
	User_GPIO_set(2,0,1);		// run
	
#endif
	if(g_SYS_Config.Bt_mode == 0)
	{
		stb_printf("MCU running bootloard ...\n");
	}
	else 
	{
		stb_printf("MCU running app ...\n");
	}
    led_task.Set_time.SYS_Sec = 1;
    led_task.Set_time.SYS_Us = 500000;
    led_task.Switch = 1;
}

void system_rst(void)
{

}

int sys_gpio_State_machine (Caven_BaseTIME_Type time)
{
    int retval = 0;
    g_SYS_Config.Now_time = time;
    API_Task_Timer (&led_task,time);
    if(led_task.Trigger_Flag)
    {
        User_GPIO_set(2,0,led_task.Flip_falg);
    }
    return retval;
}
