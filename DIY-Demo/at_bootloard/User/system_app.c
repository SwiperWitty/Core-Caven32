#include "system_app.h"

/*
 * SYS
 */
SYS_cfg_Type g_SYS_Config;
Caven_BaseTIME_Type System_start_Time = {0};

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
	g_SYS_Config.Serial = DEMO_Serial;
	g_SYS_Config.Version[0] = DEMO_VER;
	g_SYS_Config.Version[1] = DEMO_VER_sub;
	g_SYS_Config.Version[2] = DEMO_VER_sub_bit;
	
    g_SYS_Config.Addr = 1;
    g_SYS_Config.Bdtime = DEMO_Build_UTC;
	
    g_SYS_Config.SYS_UART_Cfg = 115200;
    g_SYS_Config.RS232_UART_Cfg = 115200;
    g_SYS_Config.RS485_UART_Cfg = 115200;
    g_SYS_Config.CANCfg = 0;
    g_SYS_Config.BLECfg = 0;
	
#if NETWORK
	memset(g_SYS_Config.MAC,0,sizeof(g_SYS_Config.MAC));
    g_SYS_Config.eth_mode = 1;		// 静态
    g_SYS_Config.wifi_mode = 0;		// 自动
    g_SYS_Config.wifi_En = 1;
    g_SYS_Config.NetCardCfg = 0;	// 物联网卡
	
	memset(g_SYS_Config.eth_ip_str,0,sizeof(g_SYS_Config.eth_ip_str));
	memset(g_SYS_Config.eth_gw_str,0,sizeof(g_SYS_Config.eth_gw_str));
	memset(g_SYS_Config.eth_netmask_str,0,sizeof(g_SYS_Config.eth_netmask_str));
	memset(g_SYS_Config.eth_DNS1_str,0,sizeof(g_SYS_Config.eth_DNS1_str));
	memset(g_SYS_Config.eth_DNS2_str,0,sizeof(g_SYS_Config.eth_DNS2_str));
	strcpy(g_SYS_Config.eth_ip_str,"192.168.1.100");
	strcpy(g_SYS_Config.eth_gw_str,"192.168.1.1");
	strcpy(g_SYS_Config.eth_netmask_str,"255.255.255.0");
	strcpy(g_SYS_Config.eth_DNS1_str,"114.114.114.114");
	strcpy(g_SYS_Config.eth_DNS2_str,"8.8.8.8");
	
    g_SYS_Config.tcp_server_enable = 1;
    g_SYS_Config.tcp_client_enable = 0;
    g_SYS_Config.tcp_http_enable = 0;
    g_SYS_Config.tcp_mqtt_enable = 0;
    g_SYS_Config.tcp_udp_enable = 0;

    g_SYS_Config.Heartbeat_nun = 0;
    g_SYS_Config.Heartbeat_Run = 0;
    g_SYS_Config.Heartbeat_MAX = 5;
	
    g_SYS_Config.TCPHBT_En = 1;
	memset(g_SYS_Config.TCPServer,0,sizeof(g_SYS_Config.TCPServer));
	strcpy(g_SYS_Config.TCPServer,"8160");
	memset(g_SYS_Config.TCPClient,0,sizeof(g_SYS_Config.TCPServer));
	strcpy(g_SYS_Config.TCPClient,"192.168.1.128:8000");

    g_SYS_Config.HTTPHBT_En = 1;
    g_SYS_Config.HTTP_cycle = 5;
	memset(g_SYS_Config.HTTPCfg,0,sizeof(g_SYS_Config.HTTPCfg));
	strcpy(g_SYS_Config.HTTPCfg,"http://192.168.1.128:8080");
	memset(g_SYS_Config.MQTTCfg,0,sizeof(g_SYS_Config.HTTPCfg));
	strcpy(g_SYS_Config.MQTTCfg,"null");
	memset(g_SYS_Config.UDPCfg,0,sizeof(g_SYS_Config.HTTPCfg));
	strcpy(g_SYS_Config.UDPCfg,"null");
	memset(g_SYS_Config.tcp_udp_multicast_str,0,sizeof(g_SYS_Config.tcp_udp_multicast_str));
	strcpy(g_SYS_Config.tcp_udp_multicast_str,"null");
#endif
    g_SYS_Config.Reset_falg = 0;
    g_SYS_Config.Work_falg = 0;
	g_SYS_Config.Net_falg = 0;
 
    g_SYS_Config.init_finish_state = 0;
    g_SYS_Config.Connect_passage = 0;
    g_SYS_Config.Work_sec = 0;
	memset(&g_SYS_Config.Now_time,0,sizeof(g_SYS_Config.Now_time));
	System_app_SYS_Config_Save ();
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
#if SYS_BTLD == 0
	// 在app层发现bt不在app，需要重置bt
	if (g_SYS_Config.Bt_mode == 0)
	{
		System_app_Restore ();
	}
#endif
	return retval;
}

int System_app_State_machine (Caven_BaseTIME_Type time)
{
	int retval = 0;
    
    g_SYS_Config.Now_time = time;
	/*
	工作时长统计
	*/
    if (System_start_Time.SYS_Sec != g_SYS_Config.Now_time.SYS_Sec)
    {
        System_start_Time = g_SYS_Config.Now_time;
        g_SYS_Config.Work_sec ++;
    }
	/*
	ota自动升级
	*/
	if (1)
	{
		
	}
	if (g_SYS_Config.Reset_falg)
	{
		Mode_Use.TIME.Delay_Ms (100);
		printf("RST SYS UTC %ds,work %ds \n",g_SYS_Config.Now_time.SYS_Sec,g_SYS_Config.Work_sec);
	}
	/*
	初始化失败
	*/
	if(g_SYS_Config.init_finish_state != 1)
	{
		
	}
    
    return retval;
}

void System_app_Init (void)
{
	System_app_SYS_Config_Gain ();
#if SYS_BTLD == 1
	NVIC_VECTOR_SET(SYS_STR_ADDR);
	if (g_SYS_Config.Bt_mode)
	{
		GO_TO_APP(SYS_APP_ADDR);
	}
#else
	NVIC_VECTOR_SET(SYS_APP_ADDR);
#endif
	__enable_irq();
    Mode_Init.TIME(ENABLE);
    Mode_Use.TIME.Delay_Ms(10);
	Mode_Init.UART(DEBUG_CH,115200,ENABLE);
	
    Mode_Use.UART.Send_String_pFun(DEBUG_CH,"hello !\n");

#ifdef MCU_SYS_FREQ 
	printf("MCU Init,MCU_SYS_FREQ: %d Hz \n",MCU_SYS_FREQ);
#endif
	if(g_SYS_Config.Bt_mode == 0)
	{
		printf("MCU running bootloard ...\n");
	}
	else 
	{
		printf("MCU running app ...\n");
	}
}

int sys_set_time_fun (Caven_BaseTIME_Type time)
{
	int retval = 0;
	g_SYS_Config.Now_time = time;
	return retval;
}

Caven_BaseTIME_Type sys_get_time_fun (void)
{
	return g_SYS_Config.Now_time;
}


