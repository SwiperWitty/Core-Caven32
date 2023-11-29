/*
 * system_app.c
 *
 *  Created on: 2023年11月29日
 *
 */


#include "system_app.h"

system_cfg_Type Sys_cfg;

static void Flash_verify(system_cfg_Type * system_cfg)
{
    system_cfg_Type default_Sys_cfg = {
        .Verify_Head = 0x00A8,
        .Verify_End = 0x005F,

        .Info_packet_Type = 0xFA8A,     // caven
        .Addr = 0X01,
        .Versions = {0,0,1,0},
        .Equipment = "WCH-GPIO-000001",
        .Maketime = "2023.11.29",

        .Heartbeat_NUM = 10,
        .Modbus = 0,
        .RS232_Baud = 115200,
        .RS485_Baud = 115200,
        .SYS_COM_Baud = 115200,
        .RS232_SET = 1,
        .RS485_SET = 1,
        .SYS_COM_SET = 1,
        .Run_TIME = 0,
    };
    system_cfg_Type temp_Sys_cfg = {0};
    if ((temp_Sys_cfg.Verify_Head == default_Sys_cfg.Verify_Head) && (temp_Sys_cfg.Verify_End == default_Sys_cfg.Verify_End))
    {
        *system_cfg = temp_Sys_cfg;
    }
    else
    {
        *system_cfg = default_Sys_cfg;
        // save
    }
}

Caven_info_packet_Type system_handle(Caven_info_packet_Type data)
{
    Caven_info_packet_Type retval;
    Caven_info_packet_Type temp_data = data;

    switch (temp_data.Cmd_sub)
    {
        case m_SYS_TEST_Order:
            retval = temp_data;
            break;
        case m_SYS_Versions_Order:
            break;
        case m_SYS_Equipment_Order:
            break;
        case m_SYS_Addr_Order:
            break;
        case m_SYS_Maketime_Order:
            break;
        case m_SYS_UART_Order:
            break;
        default:
            retval.Result = 5;
            break;
    }

    return retval;
}

Caven_info_packet_Type bootloader_handle(Caven_info_packet_Type data)
{
    Caven_info_packet_Type retval;

    switch (data.Cmd_sub)
    {
        case m_BOOT_TEST_Order:
            retval = data;
            break;
        case m_BOOT_Start_Order:
            break;
        case m_BOOT_Data_Order:
            break;
        case m_BOOT_End_Order:
            break;
        case m_BOOT_Get_Order:
            break;
        case m_BOOT_RST_Order:
            SYS_RESET();
            break;
        case m_BOOT_Debug_Order:
            break;
        default:
            break;
    }

    return retval;
}

void system_init (void)
{
    // Flash
    Flash_verify(&Sys_cfg);
    // GPIO

    // UART
    if(Sys_cfg.SYS_COM_SET == 1)
    {
        Mode_Init.UART(UART_SYS,Sys_cfg.SYS_COM_Baud,ENABLE);
        Mode_Use.UART.Send_String_pFun(UART_SYS,"u1:hello world ! \n");
    }
    if(Sys_cfg.RS485_SET == 1)
    {
        Mode_Init.UART(UART_RS485,Sys_cfg.RS485_Baud,ENABLE);
        Mode_Use.UART.Send_String_pFun(UART_RS485,"RS485:hello world ! \n");
    }
    if(Sys_cfg.RS232_SET == 1)
    {
        Mode_Init.UART(UART_RS232,Sys_cfg.RS232_Baud,ENABLE);
        Mode_Use.UART.Send_String_pFun(UART_RS232,"RS232:hello world ! \n");
    }


    // Wiegand
    Custom_GPIO_Init(ENABLE);
    // BZZ
    Mode_Init.BZZ(ENABLE);
    // LED
    Mode_Init.LED(ENABLE);


    Caven_Watch_Type set_time = {
            .hour = 8,
            .minutes = 58,
            .second = 10,
            .time_us = 5000,
    };
    Caven_Date_Type set_date = {
            .Days = 5,
    };
    Mode_Use.TIME.Set_Date_pFun(set_date);
    Mode_Use.TIME.Set_Watch_pFun(set_time);

}




