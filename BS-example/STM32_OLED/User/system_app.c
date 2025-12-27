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
	/* 
	初始化系统时钟，这是必要的。
	*/
    Mode_Init.TIME(ENABLE);
    Mode_Use.TIME.Delay_Ms(10);
	/* 
	初始化debug串口
	*/
	Mode_Init.UART(DEBUG_CH,115200,ENABLE);
	/* LED PA11 ,初始化配置
	第一个参数1，代表的是PA
	第二个参数11代表的是11pin,组合起来就是选择PA11,如果是PB12,那么他的初始化配置应该是 User_GPIO_config(2,12,1);
	第三个参数1代表的是 输出模式，这里要注意一个误区，不管是把这个管脚变成高电平（3.3v），还是低电平（0v），它都是输出模式。如果这个参数是0，那么就是输入模式。
	*/
	User_GPIO_config(1,11,1);
	/* LED PA11 ,初始化配置
	第一个参数1，代表的是PA
	第二个参数11代表的是11pin,组合起来就是选择PA11
	第三个参数1代表的是 输出高电平，如果是0代表的是 输出低电平。
	根据LED电路可知，PA11输出低电平，那么灯就会亮，初始化我们设置它不亮，那么就是 User_GPIO_set (1,11,1);
	*/
	User_GPIO_set (1,11,1);
	/* 
	初始化OLED,其中的IIC会自动初始化
	*/
	OLED_Set_Horizontal_addr(0,0x3c);
	Mode_Init.OLED (ENABLE);
	
#ifdef MCU_SYS_FREQ 
	printf("MCU Init,MCU_SYS_FREQ: %d Hz \n",MCU_SYS_FREQ);
#endif
}
