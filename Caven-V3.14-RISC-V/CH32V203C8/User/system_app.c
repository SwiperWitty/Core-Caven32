#include "system_app.h"

/*
 * SYS
 */
SYS_cfg_Type s_SYS_Config = {
        .SYS_Baud = 460800,
        .RFID_Baud = 115200,
        .RS232_Baud = 115200,
        .RS485_Baud = 115200,
        .RS232_Mode = 0,
        .RS485_Baud = 0,

        .Connect_passage = UART_RS232,
        .SYS_Run_Status = 0,
        .Heartbeat_MAX = 30,
        .SYS_Rst = 0,
        .RS232_Flag = 0,
        .combination_version_Flag = 0,
        .SYS_version_len = 0,
        .RFID_version_len = 0,
};
unsigned char sys_pack_array[BUFF_MAX];
unsigned char send_buff[BUFF_MAX];

void system_init(void)
{
    // UART
    s_SYS_Config.RFID_Baud = s_SYS_Config.RS232_Baud;
    Mode_Init.UART(UART_SYS, s_SYS_Config.SYS_Baud, ENABLE);
    Mode_Use.UART.Send_String_pFun(UART_SYS, "UART_SYS:hello world ! \n");

    Mode_Init.UART(UART_RFID, s_SYS_Config.RFID_Baud, ENABLE);
    Mode_Use.UART.Send_String_pFun(UART_RFID, "RFID:hello world ! \n");

    Mode_Init.UART(UART_RS232, s_SYS_Config.RS232_Baud, ENABLE);
    Mode_Use.UART.Send_String_pFun(UART_RS232, "RS232:hello world ! \n");
    Base_UART_DMA_Send_Data(UART_RS232,"this is dma send data \n",sizeof("this is dma send data \n"));

    // gpio
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
    Heartbeat_Set();

    s_SYS_Config.SYS_version[0] = 0x01;
    s_SYS_Config.SYS_version[1] = 0x00;s_SYS_Config.SYS_version[2] = 0x02;s_SYS_Config.SYS_version[3] = 0x00;s_SYS_Config.SYS_version[4] = 0x01;
    s_SYS_Config.SYS_version[5] = 0x02;
    s_SYS_Config.SYS_version[6] = 0x00;s_SYS_Config.SYS_version[7] = 0x09;
    s_SYS_Config.SYS_version_len += 8;
    memcpy(&s_SYS_Config.SYS_version[8],"E110X_MCU",s_SYS_Config.SYS_version[7]);
    s_SYS_Config.SYS_version_len += s_SYS_Config.SYS_version[7];
}

/*
 * 0.最耗时的函数
 */
int GX_send_packet(GX_info_packet_Type const source)
{
    int retval = 0;

    if (source.p_Data != NULL)
    {
        memcpy(send_buff,source.p_Data,source.Get_num);

        switch (source.Comm_way) {
            case UART_SYS:
                Mode_Use.UART.Send_Data_pFun(source.Comm_way,send_buff,source.Get_num);
                break;
            case UART_RFID:
                Mode_Use.UART.Send_Data_pFun(source.Comm_way,send_buff,source.Get_num);
                break;
            case UART_RS232:
                if (s_SYS_Config.RS232_Mode == 0) {
                    Mode_Use.UART.Send_Data_pFun(source.Comm_way,send_buff,source.Get_num);
                }
                break;
            case UART_RS485:
                if (s_SYS_Config.RS485_Mode == 0) {
                    Mode_Use.UART.Send_Data_pFun(source.Comm_way,send_buff,source.Get_num);
                }
                break;
            default:
                break;
        }
    }
    else {
        retval = -1;
    }

    return retval;
}

/*
 * 0 sys通道有效
 * Heartbeat
 */
int Heartbeat_Set(void)
{
    int retval = 0;

    Mode_Use.LED.SET_pFun(1, DISABLE);
    s_SYS_Config.Heartbeat_Run = 0;
    return retval;
}

static Task_Overtime_Type Heartbeat_Task = {
    .Switch = 0,            // 任务开关
    .Begin_time = {0},
    .Set_time.second = 1,
    .Set_time.time_us = 0,
};
void Heartbeat_Check(Caven_Watch_Type const time)
{
    API_Task_Timer(&Heartbeat_Task, time);
    if (Heartbeat_Task.Trigger_Flag)
    {
        Heartbeat_Task.Trigger_Flag = 0;
        s_SYS_Config.Heartbeat_Run++;
        if (s_SYS_Config.Heartbeat_Run > s_SYS_Config.Heartbeat_MAX)
        {
            s_SYS_Config.Heartbeat_Run = 0;
            RSTIC_H();
            printf("RST IC ! \n");
            Mode_Use.LED.SET_pFun(1, DISABLE);
            Mode_Use.TIME.Delay_Ms(1000);
            RSTIC_L();
            Mode_Use.TIME.Delay_Ms(5000);
        }
    }
}

void GX_force_Send_packet (u8 W_Class, u8 W_MID, u8 Comm_way, u8 *data, u16 dSize, u8 DFlag)
{
    int run_num = 0;
    u8 temp_array[350];
    GX_info_packet_Type temp_packet = {
        .Head = 0x5A,
        .Prot_W_Type = 0,
        .Prot_W_Versions = 1,
        .Prot_W_485Type = 0,
        .Prot_W_DFlag = DFlag,
        .Prot_W_Class = W_Class,
        .Prot_W_MID = W_MID,
        .dSize = dSize,
        .Addr = s_SYS_Config.RS485_Addr,
        .Comm_way = Comm_way,
    };
    if (Comm_way == UART_RS485) {
        temp_packet.Prot_W_485Type = 1;
    }
    temp_array[run_num++] = temp_packet.Head;
    temp_array[run_num++] = temp_packet.Prot_W_Type;
    temp_array[run_num++] = temp_packet.Prot_W_Versions;
    temp_array[run_num++] = (0x20 & temp_packet.Prot_W_485Type << 5) | (0x10 & temp_packet.Prot_W_DFlag << 4) | (0x0f & temp_packet.Prot_W_Class);
    temp_array[run_num++] = temp_packet.Prot_W_MID;
    if (temp_packet.Prot_W_485Type) {
        temp_array[run_num++] = temp_packet.Addr;
    }
    temp_array[run_num++] = (temp_packet.dSize >> 8) & 0xff;
    temp_array[run_num++] = (temp_packet.dSize >> 0) & 0xff;
    if (dSize > 0) {
        memcpy(&temp_array[run_num],data,dSize);
        run_num += dSize;
    }
    temp_packet.End_crc = CRC16_CCITT_CalculateBuf(&temp_array[1], run_num - 1);
    temp_array[run_num++] = (temp_packet.End_crc >> 8) & 0xff;
    temp_array[run_num++] = (temp_packet.End_crc >> 0) & 0xff;
    temp_packet.Get_num = run_num;

    GX_info_packet_index_Fun(&temp_packet,temp_array);
    GX_send_packet(temp_packet);
}

void RS232_Baud_event_task_Fun (void * data)
{
    int temp_data = *(int *)data;
    u16 run_num = 0;

    if (temp_data)
    {
        switch (temp_data) {
            case 1:             // 返回强制修改

                break;
            case 2:             // 返回成功
                sys_pack_array[run_num++] = 0;
                GX_force_Send_packet (1, config_reader_serial_params_order, s_SYS_Config.Connect_passage, sys_pack_array, run_num, 0);
                run_num = 0;
                sys_pack_array[run_num++] = s_SYS_Config.RS232_Baud_Type;
                GX_force_Send_packet (1, config_reader_serial_params_order, UART_SYS, sys_pack_array, run_num, 1);      // 向ESP32备份
                break;
            case 3:             // 返回失败
                sys_pack_array[run_num++] = 1;
                GX_force_Send_packet (1, config_reader_serial_params_order, s_SYS_Config.Connect_passage, sys_pack_array, run_num, 0);
                break;
            default:
                break;
        }
        if (temp_data != 3) {
            Mode_Use.TIME.Delay_Ms(10);
            s_SYS_Config.RFID_Baud = s_SYS_Config.RS232_Baud;
            Mode_Init.UART(UART_RFID, s_SYS_Config.RFID_Baud, ENABLE);
            Mode_Init.UART(UART_RS232, s_SYS_Config.RS232_Baud, ENABLE);
        }
        s_SYS_Config.RS232_Flag = 0;
    }
    *(int *)data = 0;
}

void MCU_query_SYS_version (void)
{
    GX_force_Send_packet (1, query_reader_params_order, UART_SYS, "0", 0, 0);
}

void MCU_Combination_version_Updata_Handle (void)
{
    int temp_num;
    int run_num = 0;

    if (s_SYS_Config.combination_version_Flag == 2)
    {
        temp_num = s_SYS_Config.RFID_version_len;
        memcpy(&sys_pack_array[run_num],s_SYS_Config.RFID_version,temp_num);
        run_num += temp_num;
        temp_num = s_SYS_Config.SYS_version_len;
        memcpy(&sys_pack_array[run_num],s_SYS_Config.SYS_version,temp_num);
        run_num += temp_num;

        GX_force_Send_packet (1, query_reader_params_order, s_SYS_Config.Connect_passage, sys_pack_array, run_num, 0);
        s_SYS_Config.combination_version_Flag = 0;
    }
}


