/*
 * center_app.c
 *
 *  Created on: 2023年11月29日
 */

#include "center_app.h"

Caven_info_packet_Type data_packet;
Caven_info_packet_Type standard = {
        .Head = 0xFA8A,
        .Versions = 0x01,   // 版本1
        .Type = 0x03,       // 混合IO
        .Addr = 0x01,       // 地址1，其实设置00也可以
        .dSize = 300,       // 最大长度
};

CAVEN_Status_Event_Type MCU_Status_Event;

static int center_send_packet (Caven_info_packet_Type data)
{
    int retval = 0;
    u8 temp_buff[BUFF_MAX];
    int buff_size;
    buff_size = Caven_info_Split_packet_Fun(data,temp_buff);
    if ((data.Result == 2) || (data.Result == 3) || (data.Result == 7) || (data.Result == 8))
    {
        return retval;
    }

    if (buff_size > 0)
    {
        switch (data.comm_way)
        {
            case UART_SYS:
                Mode_Use.UART.Send_Data_pFun(UART_SYS,temp_buff,buff_size);
                break;
            case UART_RS232:
                Mode_Use.UART.Send_Data_pFun(UART_RS232,temp_buff,buff_size);
                break;
            default:
                retval = -1;
                break;
        }
    }

    return retval;
}

int center_order_handle(Caven_info_packet_Type data)
{
    int retval = 0;
    int data_size;
    Caven_info_packet_Type temp_data;
    data_size = sizeof(temp_data);
    DESTROY_DATA(&temp_data,data_size);

    switch (data.Cmd) {
        case m_CAVEN_TEST_Order:
            temp_data = data;
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
            temp_data.Result = 4;       // CMD
            break;
    }
    retval = center_send_packet(temp_data);
    return retval;
}



int center_State_machine (void)
{
    int retval = 0;
    if ((data_packet.Result & 0x80) > 0)
    {
//        printf("packet Head : %x \n",data_packet.Head);
//        printf("packet Versions : %x \n",data_packet.Versions);
//        printf("packet Type : %x \n",data_packet.Type);
//        printf("packet Addr : %x \n",data_packet.Addr);
//        printf("packet Cmd : %x \n",data_packet.Cmd);
//        printf("packet Cmd_sub : %x \n",data_packet.Cmd_sub);
//        printf("packet dSize : %x \n",data_packet.dSize);
//        printf("packet data[0] : %x \n",*(data_packet.p_Data + 0));
//        printf("packet Result : %x \n",data_packet.Result);
//        printf("packet comm_way : %x \n",data_packet.comm_way);
//        printf(" \n");

        center_order_handle(data_packet);
        Caven_info_packet_clean_Fun(&data_packet);
        MCU_Status_Event.Status_flag = m_CAVEN_IDLE_Stat;
    }

    if(MCU_Status_Event.Status_flag == m_CAVEN_IDLE_Stat)
    {

    }
    return retval;
}



/*
 * UART1
 */
void UART_SYS_Getrx_Fun (u8 data)
{
    MCU_Status_Event.Status_flag = m_CAVEN_COMM_Stat;
    data_packet.comm_way = UART_SYS;
    Caven_info_Make_packet_Fun(standard,&data_packet,data);
}
/*
 * UART2
 */
void UART_RS485_Getrx_Fun (u8 data)
{
    MCU_Status_Event.Status_flag = m_CAVEN_COMM_Stat;
    data_packet.comm_way = UART_RS485;

    Base_UART_Send_Byte(UART_SYS,data);

}
/*
 * UART3
 */
void UART_RS232_Getrx_Fun (u8 data)
{
    MCU_Status_Event.Status_flag = m_CAVEN_COMM_Stat;
    data_packet.comm_way = UART_RS232;
    Caven_info_Make_packet_Fun(standard,&data_packet,data);
//    Base_UART_Send_Byte(UART_SYS,data);
}

u8 Buff_array[BUFF_MAX];

int center_Init(void)
{
    int retval = 0;
    DESTROY_DATA(&MCU_Status_Event,sizeof(MCU_Status_Event));
    DESTROY_DATA(&data_packet,sizeof(data_packet));
    Caven_info_packet_index_Fun(&data_packet,Buff_array);

    Mode_Use.UART.Receive_Bind_pFun(UART_SYS,UART_SYS_Getrx_Fun);
    Mode_Use.UART.Receive_Bind_pFun(UART_RS485,UART_RS485_Getrx_Fun);
    Mode_Use.UART.Receive_Bind_pFun(UART_RS232,UART_RS232_Getrx_Fun);
    MCU_Status_Event.Status_flag = m_CAVEN_IDLE_Stat;
    return retval;
}

