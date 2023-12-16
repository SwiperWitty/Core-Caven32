/*
 * system_app.c
 *
 *  Created on: 2023年11月29日
 *
 */

#include "system_app.h"

system_cfg_Type Sys_cfg;
static u8 buff_array[BUFF_MAX]; // buff缓冲区
static void Heartbeat_Set(int *run_num);

static void Flash_verify(system_cfg_Type *system_cfg)
{
    system_cfg_Type default_Sys_cfg = {
        .Verify_Head = 0x00A8,
        .Verify_End = 0x005F,

        .Info_packet_Type = 0xFA8A, // caven
        .Addr = 0X01,
        .Versions = {0, 0, 1, 0},
        .Equipment = "WCH-GPIO-000001",
        .Maketime = "2023.11.29",

        .Modbus = 0,
        .RS232_Baud = 115200,
        .RS485_Baud = 115200,
        .SYS_COM_Baud = 115200,
        .RS232_SET = 1,
        .RS485_SET = 1,
        .SYS_COM_SET = 1,
        .Last_Comm = UART_RS232,

        .Run_TIME = {0},
        .Heartbeat_NUM = 20,
        .Heartbeat_Run = 0,
        .MCU_Status = 0,

    };
    system_cfg_Type temp_Sys_cfg = {0};
    if ((temp_Sys_cfg.Verify_Head == default_Sys_cfg.Verify_Head) && (temp_Sys_cfg.Verify_End == default_Sys_cfg.Verify_End))
    {
        temp_Sys_cfg.Heartbeat_Run = 0;
        temp_Sys_cfg.MCU_Status = 0;
        *system_cfg = temp_Sys_cfg;
    }
    else
    {
        *system_cfg = default_Sys_cfg;
        // save
    }
}

Caven_info_packet_Type SYS_Versions_Get(Caven_info_packet_Type data)
{
    Caven_info_packet_Type retval = data;
    int run_num = 0;
    u8 Open_ID;

    DESTROY_DATA(buff_array, sizeof(buff_array));
    if (data.dSize > 0)
    {
        memcpy(buff_array, retval.p_Data, data.dSize);
        Open_ID = buff_array[run_num];
        switch (Open_ID)
        {
        case 1:
            buff_array[(++run_num)] = Sys_cfg.Versions[run_num] + '0';
            buff_array[(++run_num)] = Sys_cfg.Versions[run_num] + '0';
            buff_array[(++run_num)] = Sys_cfg.Versions[run_num] + '0';
            buff_array[(++run_num)] = Sys_cfg.Versions[run_num] + '0';
            memcpy(retval.p_Data, buff_array, run_num);
            retval.dSize = run_num;
            retval.Result = 0; // 问版本是要回的
            break;
        default:
            retval.dSize = 0;
            retval.Result = 9;
            break;
        }
    }
    return retval;
}

/*
 * 消息的完全转发
 */
Caven_info_packet_Type Data_TRANSPOND_Order(Caven_info_packet_Type data)
{
    Caven_info_packet_Type retval = data;
    u8 Open_ID;
    DESTROY_DATA(buff_array, sizeof(buff_array));
    if (data.dSize > 0)
    {
        memcpy(buff_array, data.p_Data, data.dSize);
        Open_ID = buff_array[0];
        switch (Open_ID)
        {
        case 0: // 自动
            Mode_Use.UART.Send_Data_pFun(Sys_cfg.Last_Comm, &buff_array[1], (data.dSize - 1));
            retval.Result = RESULT_DEFAULT;
            break;
        case 1: // RS232
            Mode_Use.UART.Send_Data_pFun(UART_RS232, &buff_array[1], (data.dSize - 1));
            retval.Result = RESULT_DEFAULT;
            break;
        case 2: // RS485
            Mode_Use.UART.Send_Data_pFun(UART_RS485, &buff_array[1], (data.dSize - 1));
            retval.Result = RESULT_DEFAULT;
            break;
        case 3: // weigen

            retval.Result = RESULT_DEFAULT;
            break;
        default:
            retval.dSize = 0;
            retval.Result = 9;
            break;
        }
    }
    return retval;
}

Caven_info_packet_Type system_handle(Caven_info_packet_Type data)
{
    Caven_info_packet_Type retval = data;

    switch (data.Cmd_sub)
    {
    case m_SYS_TEST_Order:
        //            retval = data;
        retval.Result = 0;
        break;
    case m_SYS_Versions_Order:
        retval = SYS_Versions_Get(data);
        break;
    case m_SYS_Equipment_Order:
        break;
    case m_SYS_Addr_Order:
        break;
    case m_SYS_Maketime_Order:
        break;
    case m_SYS_UART_Order:
        break;
    case m_SYS_TRANSPOND_Order:
        retval = Data_TRANSPOND_Order(data);
        break;
    case m_SYS_Heartbeat_Order:
        Heartbeat_Set(&Sys_cfg.Heartbeat_Run);
        retval.dSize = 0;
        retval.Result = 10;
        break;
    default:
        retval.dSize = 0;
        retval.Result = 5;
        break;
    }

    return retval;
}

Caven_info_packet_Type bootloader_handle(Caven_info_packet_Type data)
{
    Caven_info_packet_Type retval = data;

    switch (data.Cmd_sub)
    {
    case m_BOOT_TEST_Order:
        //            retval = data;
        retval.Result = 0;
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
        Mode_Use.LED.SET_pFun(1, DISABLE);
        SYS_RESET();
        break;
    case m_BOOT_Debug_Order:
        break;
    default:
        retval.dSize = 0;
        retval.Result = 5;
        break;
    }

    return retval;
}

void system_init(void)
{
    // Flash
    Flash_verify(&Sys_cfg);
    // GPIO

    // UART
    if (Sys_cfg.SYS_COM_SET == 1)
    {
        Mode_Init.UART(UART_SYS, Sys_cfg.SYS_COM_Baud, ENABLE);
        Mode_Use.UART.Send_String_pFun(UART_SYS, "UART_SYS:hello world ! \n");
    }
    if (Sys_cfg.RS485_SET == 1)
    {
        Mode_Init.UART(UART_RS485, Sys_cfg.RS485_Baud, ENABLE);
        Mode_Use.UART.Send_String_pFun(UART_RS485, "RS485:hello world ! \n");
    }
    if (Sys_cfg.RS232_SET == 1)
    {
        Mode_Init.UART(UART_RS232, Sys_cfg.RS232_Baud, ENABLE);
        Mode_Use.UART.Send_String_pFun(UART_RS232, "RS232:hello world ! \n");
    }

    // Wiegand
    Custom_GPIO_Init(ENABLE);
    // BZZ
    Mode_Init.BZZ(ENABLE);
    // LED
    Mode_Init.LED(ENABLE);

    // 载入date & Watch -> time
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

void Heartbeat_Set(int *run_num)
{
    int temp_num = 0;
    Mode_Use.LED.SET_pFun(1, DISABLE);
    *run_num = temp_num;
}

static Task_Overtime_Type Heartbeat_Task = {
    .Switch = 1,
    .Begin_time = {0},
    .Set_time.second = 1,
    .Set_time.time_us = 0,
};
void Heartbeat_Check(Caven_Watch_Type time)
{
    API_Task_Timer(&Heartbeat_Task, time);
    if (Heartbeat_Task.Trigger_Flag)
    {
        Sys_cfg.Heartbeat_Run++;
        if ((Sys_cfg.Heartbeat_Run > Sys_cfg.Heartbeat_NUM) && (Sys_cfg.Heartbeat_NUM != 0))
        {
            Sys_cfg.Heartbeat_Run = 0;
            RSTIC_H();
            printf("RST IC ! \n");
            Mode_Use.LED.SET_pFun(1, DISABLE);
            Mode_Use.TIME.Delay_Ms(1000);
            RSTIC_L();
            Mode_Use.TIME.Delay_Ms(5000);
        }
    }
}
