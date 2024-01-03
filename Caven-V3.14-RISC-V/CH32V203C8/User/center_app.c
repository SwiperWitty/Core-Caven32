/*
 * center_app.c
 *
 *  Created on: 2023年11月29日
 */

#include "center_app.h"

static int GX_TO_Caven_info_Make(GX_info_packet_Type source, Caven_info_packet_Type *target);
static int Center_send_packet(Caven_info_packet_Type data);

/*
 * Caven-info
 */
Caven_info_packet_Type SYS_input_packet;       // 给接收中断
Caven_info_packet_Type SYS_temp_packet;
Caven_info_packet_Type SYS_output_packet;
Caven_info_packet_Type SYS_data_packet_buff[10];

Caven_info_packet_Type standard = {
    .Head = 0xFA8A,
    .Versions = 0x01, // 版本1
    .Type = 0x03,     // 混合IO
    .Addr = 0x0A,     // 地址0x0A，其实设置00也可以
    .dSize = BUFF_MAX,     // 最大长度
};
/*
 * GX-info
 */
GX_info_packet_Type RS232_input_packet;       // 给接收中断
GX_info_packet_Type RS232_output_packet;
GX_info_packet_Type RS232_gx_data_packet_buff[5];

GX_info_packet_Type RS485_input_packet;       // 给接收中断
GX_info_packet_Type RS485_output_packet;
GX_info_packet_Type RS485_gx_data_packet_buff[3];

GX_info_packet_Type gx_standard = {
    .Head = 0x5A,
    .Prot_W_Versions = 0x01,    // 版本
    .dSize = BUFF_MAX,          // 最大长度
};

CAVEN_Status_Event_Type MCU_Status_Event;

static int Center_order_handle(const Caven_info_packet_Type data)
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
        temp_data.Result = m_Result_Back_CMD; // CMD
        break;
    }

    retval = Center_send_packet(temp_data);
    return retval;
}

u8 temp_array[100] = {0x5A,0x00,0x01,0x12,0x00,0x00,0x0E,0x00,0x02,0x43,0x21,0x08,0x00,0x01,0x01,0xE4,0x08,0x00,0x0D,0xF7,0x32,0xE9,0xFC};//
int temp_num = 0;
int get_sys_num = 0;
int get_sys_pack_num = 0;
int out_sys_num = 0;
int out_sys_pack_num = 0;

int get_232_data = 0;
int get_232_num = 0;
int get_232_pack_num = 0;
int out_232_pack_num = 0;


int Center_State_machine(Caven_Watch_Type time)
{
    int retval = 0;

//    Mode_Use.TIME.Delay_Us(10);

    Caven_Circular_queue_output (&SYS_output_packet ,SYS_data_packet_buff,9);   // 从队列中提取
    GX_Circular_queue_output (&RS232_output_packet,RS232_gx_data_packet_buff,5);   // 从队列中提取
    GX_Circular_queue_output (&RS485_output_packet,RS485_gx_data_packet_buff,3);   // 从队列中提取

    if (SYS_output_packet.Result & 0x50)
    {
//        printf("Circular_queue num : %d \n",temp_data);
//        printf("packet Head : 0x%x \n",temp_packet.Head);
//        printf("packet Versions : 0x%x \n",temp_packet.Versions);
//        printf("packet Type : 0x%x \n",temp_packet.Type);
//        printf("packet Addr : 0x%x \n",temp_packet.Addr);
//        printf("packet Cmd : 0x%x \n",temp_packet.Cmd);
//        printf("packet Cmd_sub : 0x%x \n",temp_packet.Cmd_sub);
//        printf("packet dSize : 0x%x \n",temp_packet.dSize);
//        printf("packet data[0] : 0x%x \n",*(temp_packet.p_Data + 0));
//        printf("packet Result : 0x%x \n",temp_packet.Result);
//        printf("packet comm_way : 0x%x \n",temp_packet.Comm_way);
//        printf("packet Get_num : 0x%x \n",temp_packet.Get_num);
//        printf(" \n");

        out_sys_pack_num ++;
        out_sys_num += SYS_output_packet.Get_num;
        if (SYS_output_packet.Cmd == 2)
        {
            temp_num = 0;
        }

        Center_order_handle(SYS_output_packet); // 处理pack
        Caven_info_packet_clean_Fun(&SYS_output_packet);
    }

    if (RS232_output_packet.Result & 0x50)
    {

//        printf("Circular_queue num : %d \n",temp_num);
//        printf("packet Head : 0x%x \n",RS232_gx_temp_packet.Head);
//        printf("packet Prot_W_Type : 0x%x \n",RS232_gx_temp_packet.Prot_W_Type);
//        printf("packet Prot_W_Versions : 0x%x \n",RS232_gx_temp_packet.Prot_W_Versions);
//        printf("packet 485Type : 0x%x \n",RS232_gx_temp_packet.Prot_W_485Type);
//        printf("packet DFlag : 0x%x \n",RS232_gx_temp_packet.Prot_W_DFlag);
//        printf("packet Class : 0x%x \n",RS232_gx_temp_packet.Prot_W_Class);
//        printf("packet MID : 0x%x \n",RS232_gx_temp_packet.Prot_W_MID);
//        printf("packet Addr : 0x%x \n",RS232_gx_temp_packet.Addr);
//        printf("packet dSize : 0x%x \n",RS232_gx_temp_packet.dSize);
//        printf("packet data[0] : 0x%x \n",*(RS232_gx_temp_packet.p_Data + 0));
//        printf("packet comm_way : 0x%x \n",RS232_gx_temp_packet.Comm_way);
//        printf("packet Get_num : 0x%x \n",RS232_gx_temp_packet.Get_num);
//        printf(" \n");
        out_232_pack_num++;
        GX_TO_Caven_info_Make(RS232_output_packet, &SYS_temp_packet); // 只需要转发，转成Caven
        GX_info_packet_clean_Fun(&RS232_output_packet);
        Center_send_packet(SYS_temp_packet);
        Caven_info_packet_clean_Fun(&SYS_temp_packet);
    }
    if (RS485_output_packet.Result & 0x50)
    {
        GX_TO_Caven_info_Make(RS485_output_packet, &SYS_temp_packet); // 只需要转发，转成Caven
        GX_info_packet_clean_Fun(&RS485_output_packet);
        Center_send_packet(SYS_temp_packet);
        Caven_info_packet_clean_Fun(&SYS_temp_packet);
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
    temp_num++;
    Caven_info_Make_packet_Fun(standard, &SYS_input_packet, temp);
    if (SYS_input_packet.Result & 0x50)     // 加入队列
    {
        get_sys_pack_num ++;
        get_sys_num += SYS_input_packet.Get_num;
        SYS_input_packet.Comm_way = 0;
        Caven_Circular_queue_input (&SYS_input_packet,SYS_data_packet_buff,9);
    }
}
/*
 * UART2
 */
void UART_RS485_Getrx_Fun(void *data)
{
    u8 temp = *(u8 *)data;

    MCU_Status_Event.Status_flag = m_CAVEN_COMM_Stat;
//    Base_UART_Send_Byte_Fast(UART_SYS,temp);    // 中断里面发东西还是用快的吧
    GX_info_Make_packet_Fun(gx_standard, &RS485_input_packet, temp);
    if (RS485_input_packet.Result & 0x50)     // 加入队列
    {
        RS485_input_packet.Comm_way = 2;
        GX_Circular_queue_input (&RS485_input_packet,RS485_gx_data_packet_buff,3);
    }
}
/*
 * UART3
 */
void UART_RS232_Getrx_Fun(void *data)
{
    u8 temp = *(u8 *)data;

//    Base_UART_Send_Byte_Fast(UART_SYS,temp);    // 中断里面发东西还是用快的吧   UART_SYS UART_RS232
    GX_info_Make_packet_Fun(gx_standard, &RS232_input_packet, temp);
    get_232_data ++;
    if (RS232_input_packet.Result & 0x50)         // 加入队列
    {
        get_232_pack_num ++;
        get_232_num += RS232_input_packet.Get_num;
        RS232_input_packet.Comm_way = 1;
        GX_Circular_queue_input (&RS232_input_packet,RS232_gx_data_packet_buff,5);
    }
}

u8 g_SYS_Buff_array[12][BUFF_MAX];       // buff缓冲区
u8 g_RS232_gx_Buff_array[7][BUFF_MAX];  // buff缓冲区
u8 g_RS485_gx_Buff_array[5][BUFF_MAX];  // buff缓冲区

int Center_Init(void)
{
    int retval = 0;
    DESTROY_DATA(&MCU_Status_Event, sizeof(MCU_Status_Event));

    // UART_SYS
    Caven_info_packet_index_Fun(&SYS_input_packet, g_SYS_Buff_array[0]);
    Caven_info_packet_clean_Fun(&SYS_input_packet);
    Caven_info_packet_index_Fun(&SYS_output_packet, g_SYS_Buff_array[1]);
    Caven_info_packet_clean_Fun(&SYS_output_packet);
    Caven_info_packet_index_Fun(&SYS_temp_packet, g_SYS_Buff_array[2]);
    Caven_info_packet_clean_Fun(&SYS_temp_packet);

    Caven_info_packet_index_Fun(&SYS_data_packet_buff[0], g_SYS_Buff_array[3]);
    Caven_info_packet_index_Fun(&SYS_data_packet_buff[1], g_SYS_Buff_array[4]);
    Caven_info_packet_index_Fun(&SYS_data_packet_buff[2], g_SYS_Buff_array[5]);
    Caven_info_packet_index_Fun(&SYS_data_packet_buff[3], g_SYS_Buff_array[6]);
    Caven_info_packet_index_Fun(&SYS_data_packet_buff[4], g_SYS_Buff_array[7]);
    Caven_info_packet_index_Fun(&SYS_data_packet_buff[5], g_SYS_Buff_array[8]);
    Caven_info_packet_index_Fun(&SYS_data_packet_buff[6], g_SYS_Buff_array[9]);
    Caven_info_packet_index_Fun(&SYS_data_packet_buff[7], g_SYS_Buff_array[10]);
    Caven_info_packet_index_Fun(&SYS_data_packet_buff[8], g_SYS_Buff_array[11]);

    // UART_RS232
    GX_info_packet_index_Fun(&RS232_gx_data_packet_buff[0], g_RS232_gx_Buff_array[0]);
    GX_info_packet_index_Fun(&RS232_gx_data_packet_buff[1], g_RS232_gx_Buff_array[1]);
    GX_info_packet_index_Fun(&RS232_gx_data_packet_buff[2], g_RS232_gx_Buff_array[2]);
    GX_info_packet_index_Fun(&RS232_gx_data_packet_buff[3], g_RS232_gx_Buff_array[3]);
    GX_info_packet_index_Fun(&RS232_gx_data_packet_buff[4], g_RS232_gx_Buff_array[4]);
    GX_info_packet_index_Fun(&RS232_input_packet, g_RS232_gx_Buff_array[5]);
    GX_info_packet_clean_Fun(&RS232_input_packet);
    GX_info_packet_index_Fun(&RS232_output_packet, g_RS232_gx_Buff_array[6]);
    GX_info_packet_clean_Fun(&RS232_output_packet);
    // UART_RS485
    GX_info_packet_index_Fun(&RS485_gx_data_packet_buff[0], g_RS485_gx_Buff_array[0]);
    GX_info_packet_index_Fun(&RS485_gx_data_packet_buff[1], g_RS485_gx_Buff_array[1]);
    GX_info_packet_index_Fun(&RS485_gx_data_packet_buff[2], g_RS485_gx_Buff_array[2]);
    GX_info_packet_index_Fun(&RS485_input_packet, g_RS485_gx_Buff_array[3]);
    GX_info_packet_clean_Fun(&RS485_input_packet);
    GX_info_packet_index_Fun(&RS485_output_packet, g_RS485_gx_Buff_array[4]);
    GX_info_packet_clean_Fun(&RS485_output_packet);

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
        temp_packet.Cmd_sub = m_SYS_TRANSPOND_Order;
        temp_packet.Comm_way = 0;           // 发给系统串口

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
        case 0:
            Mode_Use.UART.Send_Data_pFun(UART_SYS, temp_buff, buff_size);
            break;
        case 2:
            Mode_Use.UART.Send_Data_pFun(UART_RS485, temp_buff, buff_size);
            break;
        case 1:
            Mode_Use.UART.Send_Data_pFun(UART_RS232, temp_buff, buff_size);
            break;
        default:
            retval = -1;
            break;
        }
    }

    return retval;
}
