#ifndef _SYSTEM_APP_H_
#define _SYSTEM_APP_H_

#include "Mode.h"
#include "API.h"

#include "debug_log.h"
//
#define SYS_Link        0
#define RS232_Link      1
#define RS485_Link      2
#define CAN_Link        3		// CAN转译为Caven_packet
#define NET4G_Link      4		// 本质是TCP_Client,也可以以HTTP/MQTT方式出现（HTTP可以做在线升级）
#define TCP_Server_Link 5
#define TCP_Client_Link 6
#define TCP_HTTP_Link   7		// JSON转译为Caven_packet
#define TCP_MQTT_Link   8		// JSON转译为Caven_packet
#define TCP_UDP_Link    9
#define BLE_Link        10

//
#define DEMO_VER          1L
#define DEMO_VER_sub      0L
#define DEMO_VER_sub_bit  1L
//
#define DEMO_Build_UTC 1752226212
//
#define DEMO_Serial 0x0101011900123456
//
#define DEMO_Name_str		"E1205_Linux_4G"
//
#define NETWORK     1
//
#define	SYS_BTLD	1
#define	SYS_STR_ADDR		0x08000000
#define	SYS_APP_ADDR		0x08008000
#define	SYS_CFG_ADDR		0x08038000
#define	SYS_APP_SIZE		(SYS_CFG_ADDR - SYS_APP_ADDR)
//

typedef struct
{
	uint16_t Bt_mode;
	uint16_t Addr;
    int Board_ID;     // 0(default),13(E1103),17(E1107)
    uint16_t debug;
    uint8_t Version[10];		// 固件版本
    uint64_t Serial;			// 设备序号
    char Hostname[30];			// 设备名称
    uint32_t Bdtime;			// 固件日期
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
int System_app_State_machine (Caven_BaseTIME_Type time);
void System_app_Init (void);
void System_app_Restore (void);
int System_app_SYS_Config_Save (void);
int System_app_SYS_Config_Gain (void);

#endif 
