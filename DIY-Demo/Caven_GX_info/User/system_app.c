#include "system_app.h"

/*
 * SYS
 */
SYS_cfg_Type g_SYS_Config = {
    .Board_ID = 0,
    .debug = 1,
    .Hostname = "Link Board",
    .Addr = 1,
    .MAC = {0x00,0x00,0x00,0x00,0x00,0x00},
#if NETWORK
    .eth_mode = 1,
    .wifi_mode = 0,
    .wifi_En = 1,
    .NetCardCfg = 0,

    .tcp_server_enable = 1,
    .tcp_client_enable = 0,
    .tcp_http_enable = 0,
    .tcp_mqtt_enable = 0,
    .tcp_udp_enable = 0,

    .Heartbeat_nun = 0,
    .Heartbeat_Run = 0;
    .Heartbeat_MAX = 5,

    .TCPHBT_En = 1,
    .TCPServer = "192.168.1.168:8160"
    .TCPClient = "192.168.1.128:8160"

    .HTTPHBT_En = 1,
    .HTTP_cycle = 5,
    .HTTPCfg = "http://192.168.1.128:8080",
    .MQTTCfg = " ",
    .UDPCfg = " ",
    .tcp_udp_multicast_str = " ",
#endif
    .Reset_falg = 0,
    .Work_falg = 0,

    .init_finish_state = 0,
    .Connect_passage = 0,
    .Work_sec = 0,
    .Now_time = {0},

    .SYS_UART_Cfg = 115200,
    .RS232_UART_Cfg = 115200,
    .RS485_UART_Cfg = 115200,
    .CANCfg = 0,
    .BLECfg = 0,
};

Caven_BaseTIME_Type System_start_Time = {0};
int System_app_State_machine (Caven_BaseTIME_Type time)
{
	int retval = 0;
    
    g_SYS_Config.Now_time = time;
    if (System_start_Time.SYS_Sec != g_SYS_Config.Now_time.SYS_Sec)
    {
        System_start_Time = g_SYS_Config.Now_time;
        g_SYS_Config.Work_sec ++;
    }
	if(g_SYS_Config.init_finish_state != 1)
	{
		
	}
    
    return retval;
}

void System_app_Init (void)
{
    Mode_Init.TIME(ENABLE);
    Mode_Use.TIME.Delay_Ms(10);
	Mode_Init.UART(DEBUG_OUT,115200,ENABLE);
	
	Mode_Use.OLED.Set_Direction_pFun(0,0x3c);
	Mode_Init.OLED(ENABLE);
	
	User_GPIO_config(1,11,1);
	User_GPIO_config(1,12,1);
    User_GPIO_config(3,13,0);
	User_GPIO_set(1,11,1);
	User_GPIO_set(1,12,0);
	
    Mode_Use.UART.Send_String_pFun(DEBUG_OUT,"hello 2!\n");
    
    TIM1_Capture_Start_Init(0xffff,72-1,0x01|0x02|0x04|0x08,0,ENABLE);				// a8,a9
    TIM2_Capture_Start_Init(0xffff,72-1,0x01|0x02,0,ENABLE);
//	Vofa_JustFloat_Init_Fun (Debug_Out);
#ifdef MCU_SYS_FREQ 
	printf("MCU Init,MCU_SYS_FREQ: %d Hz \n",MCU_SYS_FREQ);
#endif
}
