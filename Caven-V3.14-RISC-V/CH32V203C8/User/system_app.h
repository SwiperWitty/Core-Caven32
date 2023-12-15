/*
 * sys_app.h
 *
 *  Created on: 2023年11月29日
 *
 */

#ifndef USER_SYSTEM_APP_H_
#define USER_SYSTEM_APP_H_

#include "Mode.h"


#define UART_SYS    m_UART_CH1
#define UART_RS485  m_UART_CH2
#define UART_RS232  m_UART_CH3

/*
 * SYS_CFG_CMD_SUB
 */
enum SYS_CFG_CMD_SUB
{
    m_SYS_TEST_Order = 0x00,
    m_SYS_Versions_Order,
    m_SYS_Equipment_Order,
    m_SYS_Addr_Order,
    m_SYS_Maketime_Order,
    m_SYS_UART_Order,
    m_SYS_TRANSPOND_Order,
    m_SYS_TIME_Order,
    m_SYS_Heartbeat_Order,

};

typedef struct
{
    u16 Verify_Head;

    u16 Info_packet_Type;
    char Versions[4];
    char Equipment[20];
    u8 Addr;
    char Maketime[10];

    u8 Modbus;
    u8 SYS_COM_SET;
    u8 RS232_SET;
    u8 RS485_SET;
    int SYS_COM_Baud;
    int RS232_Baud;
    int RS485_Baud;
    u8 Last_Comm;

    u8 Wiegand_BIT;
    u8 Wiegand_SET;
    u8 Addr_IIC;

    Caven_TIME_Type Run_TIME;
    int Heartbeat_NUM;
    int Heartbeat_Run;
    int MCU_Status;

    u16 Verify_End;
}system_cfg_Type;


/*
 * BOOT_CMD_SUB
 */
enum BOOT_CMD_SUB
{
    m_BOOT_TEST_Order = 0x00,
    m_BOOT_Start_Order,
    m_BOOT_Data_Order,
    m_BOOT_End_Order,
    m_BOOT_Get_Order,
    m_BOOT_RST_Order,
    m_BOOT_Debug_Order,
};

void system_init(void);
Caven_info_packet_Type system_handle(Caven_info_packet_Type data);
Caven_info_packet_Type bootloader_handle(Caven_info_packet_Type data);

void Heartbeat_Check (Caven_Watch_Type time);

#endif /* USER_SYSTEM_APP_H_ */
