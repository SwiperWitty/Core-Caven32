#ifndef SYSTEM_APP_H_
#define SYSTEM_APP_H_


#include "Mode.h"
#include "API.h"

#include "GX_info_frame.h"

#define UART_SYS    m_UART_CH1
#define UART_RFID   m_UART_CH2
#define UART_RS232  m_UART_CH3
#define UART_RS485  m_UART_CH4

typedef struct
{
    int RS232_Baud;
    int RS485_Baud;
    int SYS_Baud;
    int RFID_Baud;
    char RS232_Baud_Type;
    char RS485_Baud_Type;
    int RS232_Mode;         // 0透传\外挂模式
    int RS485_Mode;         // 0透传\外挂模式
    int RS485_Addr;         // 0透传\外挂模式
    int RS232_Flag;         // 根据步骤操作UART
    char RFID_version[50];
    char SYS_version[30];
    char RFID_version_len;
    char SYS_version_len;
    char combination_version_Flag;
    int Connect_passage;    // 连接管理,从SYS来的[GPI、GPO、BZZ、UART]操作不会变更此数据
    int SYS_Run_Status;
    int Heartbeat_Run;
    int Heartbeat_MAX;
    char SYS_Rst;
}SYS_cfg_Type;

extern SYS_cfg_Type s_SYS_Config;


void system_init(void);

void MCU_Combination_version_Updata_Handle (void);

void RS232_Baud_event_task_Fun (void * data);

void MCU_query_SYS_version (void);
int Heartbeat_Set(void);
void Heartbeat_Check(Caven_Watch_Type const time);
int GX_send_packet(GX_info_packet_Type const source);
void GX_force_Send_packet (u8 W_Class, u8 W_MID, u8 Comm_way, u8 *data, u16 dSize, u8 DFlag);
#endif
