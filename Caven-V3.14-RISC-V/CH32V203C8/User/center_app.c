/*
 * center_app.c
 *
 *  Created on: 2023年11月29日
 */

#include "center_app.h"

static int GX_TO_Caven_info_Make(GX_info_packet_Type source, Caven_info_packet_Type *target);
static int Center_send_packet(Caven_info_packet_Type data);

Caven_info_packet_Type data_packet;
Caven_info_packet_Type standard = {
    .Head = 0xFA8A,
    .Versions = 0x01, // 版本1
    .Type = 0x03,     // 混合IO
    .Addr = 0x0A,     // 地址0x0A，其实设置00也可以
    .dSize = 300,     // 最大长度
};

GX_info_packet_Type RS232_gx_data_packet;
GX_info_packet_Type RS485_gx_data_packet;
GX_info_packet_Type gx_standard = {
    .Head = 0x5A,
    .Prot_W_Versions = 0x01, // 版本
    .dSize = 300,            // 最大长度
};

CAVEN_Status_Event_Type MCU_Status_Event;

static int Center_order_handle(Caven_info_packet_Type data)
{
    int retval = 0;
    Caven_info_packet_Type temp_data = data;

    switch (data.Cmd)
    {
    case m_CAVEN_TEST_Order:
        //            temp_data = data;
        break;
    case m_CAVEN_SYS_Order:
        temp_data = system_handle(data);
        break;
    case m_CAVEN_BOOT_Order:
        temp_data = bootloader_handle(data);
        break;
    case m_CAVEN_FEATURE_Order:
        temp_data = gpio_handle(data);
        break;
    default:
        temp_data.dSize = 0;
        temp_data.Result = 4; // CMD
        break;
    }
    retval = Center_send_packet(temp_data);
    return retval;
}

int Center_State_machine(Caven_Watch_Type time)
{
    int retval = 0;
    u8 Buff_array[300];
    Caven_info_packet_Type temp_packet;

    if ((data_packet.Result & 0x80) > 0)
    {
        //        printf("packet Head : 0x%x \n",data_packet.Head);
        //        printf("packet Versions : 0x%x \n",data_packet.Versions);
        //        printf("packet Type : 0x%x \n",data_packet.Type);
        //        printf("packet Addr : 0x%x \n",data_packet.Addr);
        //        printf("packet Cmd : 0x%x \n",data_packet.Cmd);
        //        printf("packet Cmd_sub : 0x%x \n",data_packet.Cmd_sub);
        //        printf("packet dSize : 0x%x \n",data_packet.dSize);
        //        printf("packet data[0] : 0x%x \n",*(data_packet.p_Data + 0));
        //        printf("packet Result : 0x%x \n",data_packet.Result);
        //        printf("packet comm_way : 0x%x \n",data_packet.Comm_way);
        //        printf("packet Get_num : 0x%x \n",data_packet.Get_num);
        //        printf(" \n");

        Center_order_handle(data_packet); // 处理pack
        Caven_info_packet_clean_Fun(&data_packet);
        MCU_Status_Event.Status_flag = m_CAVEN_IDLE_Stat;
    }
    else if ((RS232_gx_data_packet.Result & 0x80) > 0)
    {
        //        printf("packet Head : 0x%x \n",RS232_gx_data_packet.Head);
        //        printf("packet Prot_W_Type : 0x%x \n",RS232_gx_data_packet.Prot_W_Type);
        //        printf("packet Prot_W_Versions : 0x%x \n",RS232_gx_data_packet.Prot_W_Versions);
        //        printf("packet 485Type : 0x%x \n",RS232_gx_data_packet.Prot_W_485Type);
        //        printf("packet DFlag : 0x%x \n",RS232_gx_data_packet.Prot_W_DFlag);
        //        printf("packet Class : 0x%x \n",RS232_gx_data_packet.Prot_W_Class);
        //        printf("packet MID : 0x%x \n",RS232_gx_data_packet.Prot_W_MID);
        //        printf("packet Addr : 0x%x \n",RS232_gx_data_packet.Addr);
        //        printf("packet dSize : 0x%x \n",RS232_gx_data_packet.dSize);
        //        printf("packet data[0] : 0x%x \n",*(RS232_gx_data_packet.p_Data + 0));
        //        printf("packet comm_way : 0x%x \n",RS232_gx_data_packet.Comm_way);
        //        printf("packet Get_num : 0x%x \n",RS232_gx_data_packet.Get_num);
        //        printf(" \n");

        Caven_info_packet_index_Fun(&temp_packet, Buff_array);

        GX_TO_Caven_info_Make(RS232_gx_data_packet, &temp_packet); // 只需要转发
        Center_send_packet(temp_packet);
        GX_info_packet_clean_Fun(&RS232_gx_data_packet);
    }
    else if ((RS485_gx_data_packet.Result & 0x80) > 0)
    {
        Caven_info_packet_index_Fun(&temp_packet, Buff_array);

        GX_TO_Caven_info_Make(RS485_gx_data_packet, &temp_packet); // 只需要转发
        Center_send_packet(temp_packet);
        GX_info_packet_clean_Fun(&RS485_gx_data_packet);
    }

    if (MCU_Status_Event.Status_flag == m_CAVEN_IDLE_Stat)
    {
    }
    Heartbeat_Check(time); // 检测心跳
    return retval;
}

/*
 * UART1
 */
void UART_SYS_Getrx_Fun(void *data)
{
    u8 temp = *(u8 *)data;
    MCU_Status_Event.Status_flag = m_CAVEN_COMM_Stat;
    data_packet.Comm_way = UART_SYS;
    Caven_info_Make_packet_Fun(standard, &data_packet, temp);
}
/*
 * UART2
 */
void UART_RS485_Getrx_Fun(void *data)
{
    u8 temp = *(u8 *)data;
    MCU_Status_Event.Status_flag = m_CAVEN_COMM_Stat;
    RS485_gx_data_packet.Comm_way = UART_RS485;

    //    Base_UART_Send_Byte_Fast(UART_SYS,temp);    // 中断里面发东西还是用快的吧
    GX_info_Make_packet_Fun(gx_standard, &RS485_gx_data_packet, temp);
}
/*
 * UART3
 */
void UART_RS232_Getrx_Fun(void *data)
{
    u8 temp = *(u8 *)data;
    MCU_Status_Event.Status_flag = m_CAVEN_COMM_Stat;
    RS232_gx_data_packet.Comm_way = UART_RS232;

    //    Base_UART_Send_Byte_Fast(UART_SYS,temp);    // 中断里面发东西还是用快的吧
    GX_info_Make_packet_Fun(gx_standard, &RS232_gx_data_packet, temp);
}

u8 g_Buff_array[BUFF_MAX];          // buff缓冲区
u8 g_RS232_gx_Buff_array[BUFF_MAX]; // buff缓冲区
u8 g_RS485_gx_Buff_array[BUFF_MAX]; // buff缓冲区

int Center_Init(void)
{
    int retval = 0;
    DESTROY_DATA(&MCU_Status_Event, sizeof(MCU_Status_Event));

    Caven_info_packet_index_Fun(&data_packet, g_Buff_array);
    Caven_info_packet_clean_Fun(&data_packet);

    GX_info_packet_index_Fun(&RS232_gx_data_packet, g_RS232_gx_Buff_array);
    GX_info_packet_clean_Fun(&RS232_gx_data_packet);

    GX_info_packet_index_Fun(&RS485_gx_data_packet, g_RS485_gx_Buff_array);
    GX_info_packet_clean_Fun(&RS485_gx_data_packet);

    Mode_Use.UART.Receive_Bind_pFun(UART_SYS, UART_SYS_Getrx_Fun);
    Mode_Use.UART.Receive_Bind_pFun(UART_RS485, UART_RS485_Getrx_Fun);
    Mode_Use.UART.Receive_Bind_pFun(UART_RS232, UART_RS232_Getrx_Fun);
    MCU_Status_Event.Status_flag = m_CAVEN_IDLE_Stat;
    return retval;
}

int GX_TO_Caven_info_Make(GX_info_packet_Type source, Caven_info_packet_Type *target)
{
    int retval = 0;
    Caven_info_packet_Type temp_packet = *target;
    if (source.p_Data == NULL || temp_packet.p_Data == NULL)
    {
        retval = -1;
    }
    else
    {
        temp_packet.Head = 0xFA8A;
        temp_packet.Versions = 1;
        temp_packet.Type = 3;
        temp_packet.Addr = 0;
        temp_packet.Cmd = 1;
        temp_packet.Cmd_sub = 6;
        temp_packet.Comm_way = UART_SYS;

        temp_packet.Result = 0;
        temp_packet.p_Data[0] = source.Comm_way;
        temp_packet.dSize = source.Get_num + 1;
        memcpy((temp_packet.p_Data + 1), source.p_Data, source.Get_num);
    }
    if (retval == 0)
    {
        *target = temp_packet;
    }

    return retval;
}

int Center_send_packet(Caven_info_packet_Type data)
{
    int retval = 0;
    u8 temp_buff[BUFF_MAX];
    int buff_size = 0;

    if ((data.Result == 0) || (data.Result == 1) || (data.Result == 4) || (data.Result == 5) || (data.Result == 6) || (data.Result == 9))
    {
        buff_size = Caven_info_Split_packet_Fun(data, temp_buff);
    }
    else
    {
    }

    if (buff_size > 0)
    {
        switch (data.Comm_way)
        {
        case UART_SYS:
            Mode_Use.UART.Send_Data_pFun(UART_SYS, temp_buff, buff_size);
            break;
        case UART_RS485:
            Mode_Use.UART.Send_Data_pFun(UART_RS485, temp_buff, buff_size);
            break;
        case UART_RS232:
            Mode_Use.UART.Send_Data_pFun(UART_RS232, temp_buff, buff_size);
            break;
        default:
            retval = -1;
            break;
        }
    }

    return retval;
}
