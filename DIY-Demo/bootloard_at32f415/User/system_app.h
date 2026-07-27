#ifndef _SYSTEM_APP__H_
#define _SYSTEM_APP__H_

#include"Items.h"
#include"API.h"
#include "Mode.h"

/* 
-试一下
2023.11.8
2026.5.7
2026.7.16       // ota
2026.7.21       // 全面支持外部flash操作 v2050
*/

typedef enum {
    m_Connect_SYS = 0,
    m_RS232_Link,
    m_RS485_Link,
    m_CAN_Link,
    m_NET4G_Link,
    m_Server_Link,
    m_Client_Link,
    m_HTTP_Link,
    m_MQTT_Link,
    m_UDP_Link,
    m_BLE_Link,
    m_USB_Link,
	m_Other_Link,
} Link_mType;

//
#define Device_VER      1
#define Device_TYPE     1
//
#define DEMO_Build_str __DATE__
#define DEMO_Serial     0x0101011900123456
#if SYS_BTLD == 1
#define DEMO_Name_str   "bootld\0"
#define DEMO_VER          2L
#define DEMO_VER_sub      0L
#define DEMO_VER_sub_bit  5L
#else
#define DEMO_Name_str   "L1004 v2.0\0"
#define DEMO_VER          2L
#define DEMO_VER_sub      0L
#define DEMO_VER_sub_bit  5L
#endif
//
#if Exist_ETH && SYS_BTLD != 1
#define NETWORK     1       // 1 使用功能
#else 
#define NETWORK     0
#endif
//

/*-----------------------------------*/

/*  [SYS_val]     */
typedef struct
{
    char Reset_falg;
    char Work_falg;
    char Net_falg;
    
    char Net_HBT_max;
    int TCPHBT_num;     // 编号
    int TCPHBT_Run;
    int HTTPHBT_num;    // 编号
    int HTTPHBT_Run;
    //
    int init_finish_state;
    int Connect_passage;    // 连接管理,从SYS来的回答消息不会变更此数据
    uint32_t Work_sec;
    uint32_t u32_val;
    Caven_BaseTIME_Type Now_time;

}SYS_val_Type;

typedef struct
{
    uint16_t app_crc;
    uint32_t app_size;
    uint16_t Bt_mode;       // 0:bootld,1:app,2:Load flash a;3:Load flash b;
    uint8_t Encrypt[16];
    uint32_t Load_Default;  // addr
    uint32_t Load_Latest;   // addr
}SYS_boot_Type;

/*  [SYS_config]     */
typedef struct
{
	SYS_boot_Type Boot;

    uint8_t Version[10];		// 固件版本
    uint64_t Serial;			// 设备序号
    uint8_t MAC[6];
    char* Bddate;			    // 固件日期
    char Hostname[24];			// 设备名称

	uint16_t Addr;
    int Board_ID;     // 0(default)
    uint16_t debug;
    int SYS_UART_Cfg;
    int RS232_UART_Cfg;
    int RS485_UART_Cfg;
    int BLE_Cfg;
    int CAN_Cfg;    // en + Bps[0-3]
    int CAN_Id;     // 0-2024
    int CAN_Range[2];     // 0-2024
    int CAN_Filter;
    uint16_t CAN_Filter_len;

#if SYS_ETH_Config
    char eth_mode;           // 1:dhcp   0:static
    char eth_En;
    char eth_ip_str[30];
    char eth_gw_str[30];
    char eth_netmask_str[30];
    char eth_DNS1_str[30];
    char eth_DNS2_str[30];
    char wifi_mode;           // 1:dhcp   0:static
    char wifi_En;
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

    char tcp_server_enable;
    char tcp_client_enable;
    char tcp_http_enable;
    char tcp_mqtt_enable;
    char tcp_udp_enable;

    char TCPHBT_En;      // DEMO_Serial + UTC + Run
    int TCPHBT_cycle;	// s
    char Server_break_off;
    char TCPServer_port[10];
    char TCPClient_url[100];
    
    char HTTPHBT_En;    // DEMO_Serial + UTC + Run
    int HTTP_cycle;     // s
    char HTTP_url[160];
    char MQTTCfg[160];

    char UDPCfg[160];
    char UDP_multicast_str[160];
#endif
	// 以下无需保存
    SYS_val_Type *temp_val;

}SYS_cfg_Type;

extern SYS_cfg_Type g_SYS_Config;
extern Caven_event_Type g_SYS_events;

extern int sys_bzz_event;
extern int sys_gpo_event;
extern int sys_rs232_event;
/*-----------------------------------*/

void System_app_Init (void);
void system_rst(void);

void System_app_Restore (void);
int System_app_SYS_Config_Save (void);
int System_app_SYS_Config_Gain (void);
int System_app_State_machine (Caven_BaseTIME_Type time);

void System_app_Gain_ICID (u8* bk_data);
void System_Send_data (void *data,int len,int way);
void line_gpo_set(int num,int val);
int sys_set_gpo_fun (int gpo,int state);
int sys_set_bzz_fun (int state);
int sys_get_mac_fun (uint8_t *mac);

void Sys_TCP_send_Heartbeat_Bind_Fun (iD_pFun Fun);

int System_app_save_UTCtime (void);
int System_app_save_Addr (void);
int System_app_save_RS232Cfg (void);
int System_app_save_RS485Cfg (void);
int System_app_save_IPv4Cfg (void);
int System_app_save_TCPHBT (void);
int System_app_save_TCPServer (void);
int System_app_save_TCPClient (void);
int System_app_save_HTTPHBT (void);
int System_app_save_HTTPCfg (void);
int System_app_save_MQTTCfg (void);
int System_app_save_boot (void);
int System_app_save_debug (void);
int System_app_save_Serial (void);
int System_app_save_MACCfg (void);

#endif 
