#ifndef _SYSTEM_APP_H_
#define _SYSTEM_APP_H_

#include "Mode.h"
#include "API.h"

//
#define SYS_Link        0
#define RS232_Link      1
#define RS485_Link      2
#define CAN_Link        3
#define NET4G_Link      4
#define TCP_Server_Link 5
#define TCP_Client_Link 6
#define TCP_HTTP_Link   7
#define TCP_MQTT_Link   8
#define TCP_UDP_Link    9
#define BLE_Link        10

//
#define DEMO_VER          1L
#define DEMO_VER_sub      0L
#define DEMO_VER_sub_bit  1L
//
#define DEMO_Serial 0x0101011900123456
//
#define DEMO_VER_str      "E1205_Linux_4G"
//

//
#define NETWORK     0

typedef struct
{
    int Board_ID;     // 0(default),13(E1103),17(E1107)
    int debug;
    uint8_t Version[10];
    uint8_t Serial[20];
    char Hostname[30];
    uint32_t Bdtime;
    uint16_t Addr;
    uint8_t MAC[6];
#if NETWORK
    int eth_mode;           // 0:dhcp   1:static
    char eth_ip_str[30];
    char eth_gw_str[30];
    char eth_netmask_str[30];
    char eth_DNS1_str[30];
    char eth_DNS2_str[30];
    int wifi_mode;           // 0:dhcp   1:static
    int wifi_En;
    char wifi_ip_str[30];
    char wifi_gw_str[30];
    char wifi_netmask_str[30];
    char wifi_DNS1_str[30];
    char wifi_DNS2_str[30];
    int NetCardCfg;
    int NetCard_ICCID;
    char NetCard_ip_str[30];
    char NetCard_gw_str[30];
    char NetCard_netmask_str[30];
    char NetCard_DNS1_str[30];
    char NetCard_DNS2_str[30];

    int tcp_server_enable;
    int tcp_client_enable;
    int tcp_http_enable;
    int tcp_mqtt_enable;
    int tcp_udp_enable;

    int Heartbeat_nun;
    int Heartbeat_Run;
    int Heartbeat_MAX;

    int TCPHBT_En;
    char TCPServer[100];
    char TCPClient[100];

    int HTTPHBT_En;
    int HTTP_cycle;
    char HTTPCfg[160];
    char MQTTCfg[160];

    char UDPCfg[160];
    char tcp_udp_multicast_str[160];
#endif
    char Reset_falg;
    char Work_falg;
    char Net_falg;
    //
    int init_finish_state;
    int Connect_passage;    // 连接管理,从SYS来的回答消息不会变更此数据
    uint32_t Work_sec;
    Caven_BaseTIME_Type Now_time;
    //
    int SYS_UART_Cfg;
    int RS232_UART_Cfg;
    int RS485_UART_Cfg;
    int CANCfg;
    int BLECfg;

}SYS_cfg_Type;

extern SYS_cfg_Type g_SYS_Config;
extern Caven_event_Type g_Events_buff;

//
void System_app_Init (void);

#endif 
