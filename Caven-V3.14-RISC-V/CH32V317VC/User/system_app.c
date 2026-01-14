#include"system_app.h"

static const char *TAG = "SYS app";

SYS_cfg_Type g_SYS_Config = {
    .Board_ID = 0,
    .RS232_UART_Cfg = 115200,
    .RS485_UART_Cfg = 115200,
    .SYS_UART_Cfg = 115200,
    .Addr = 1,
    .debug = 0,

    .wifi_En = 0,
    .eth_En = 1,
    .NetCardCfg = 0,
    .tcp_server_enable = 1,
    .tcp_client_enable = 1,
    .tcp_http_enable = 1,
    .tcp_udp_enable = 0,
    .tcp_mqtt_enable = 0,
    
    .wifi_mode = 1,
    .eth_mode = 0,
    .eth_ip_str = "192.168.1.168",
    .eth_gw_str = "192.168.1.1",
    .eth_netmask_str = "255.255.255.0",

    .TCPHBT_En = 1,
    .Server_break_off = 0,              //
    .TCPServer_port = "8160",
    .TCPClient_url = "192.168.1.128:9090",   //

    .HTTPHBT_En = 1,
    .HTTP_cycle = 10000,
    .HTTP_url = "http://192.168.1.128:8080/post",
    
    .Net_falg = 0,
    .Now_time.SYS_Sec = 0,
    .Now_time.SYS_Us = 0,
    .Work_sec = 0,
    //
    .Connect_passage = 0,
    //
    .Heartbeat_num = 0,
    .Heartbeat_Run = 0,
    .Heartbeat_MAX = 5,
};
Caven_event_Type g_SYS_events;

Task_Overtime_Type led_task;
void system_cfg_memory_init (void)
{

}

void System_app_Restore (void)
{

}

int System_app_SYS_Config_Save (void)
{
    int retval = 0;

    return retval;
}

int System_app_SYS_Config_Gain (void)
{
    int retval = 0;

    return retval;
}


void system_app_init(void)
{
    // Allow other core to finish initialization
    int temp_num,temp_rtc = 0;
    char array_ip[100],array_port[20];
    //
    SystemCoreClockUpdate();
    Mode_Init.TIME(ENABLE);
    Mode_Init.UART(DEBUG_CH,g_SYS_Config.SYS_UART_Cfg,ENABLE);

    stb_printf("MCU Init,MCU_SYS_FREQ: %d Hz \n",MCU_SYS_FREQ);

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
    User_GPIO_config(2,0,1);
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
