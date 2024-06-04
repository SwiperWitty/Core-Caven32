/*
 * center_app.c
 *
 *  Created on: 2023年11月29日
 */

#include "center_app.h"

void BZZ_event_task_Fun (void * data);
void GPO_event_task_Fun (void * data);

int GPI_Change_Updata_Handle (void);
int GPI_GET_Order(GX_info_packet_Type const data);
int GPO_SET_Order(GX_info_packet_Type const data);
int BZZ_Status_Order(GX_info_packet_Type const data);
int GPO_Sync_Status (unsigned char *data,int len);
void GPO_Open_lock_ALL (void);

int RS232_SET_Order(GX_info_packet_Type const data);
int RS485_SET_Order(GX_info_packet_Type const data);
int RS232_GET_Order(GX_info_packet_Type const data);
int RS485_GET_Order(GX_info_packet_Type const data);

int RST_SYS_Order(GX_info_packet_Type const data);
int query_params_order(GX_info_packet_Type const data);
int Get_SYS_version (GX_info_packet_Type data);
int Get_RFID_version (GX_info_packet_Type data);
int WG_Handle_Order(GX_info_packet_Type const data);

static int Center_RFID_order_handle(GX_info_packet_Type data);
static int Center_SYS_order_handle(GX_info_packet_Type data);
static int Center_order_handle(GX_info_packet_Type data);

static Caven_Watch_Type center_time;
static SYS_BaseTIME_Type center_BaseTIME;
static GPIO_cfg_Type s_GPIO_Config = {
        .GPO_num = 50,
        .GPI_num = 2,
        .BZZ_Keep_Time = 0,
        .GPO_Keep_Time = 0,
};

/*
 * Caven-event
 */
Caven_event_Type SYS_events;
int LED_event_Handle = 0;
int BZZ_event_Handle = 0;
int GPO_event_Handle = 0;
int RS232_event_Handle = 0;

/*
 * GX-info
 */
GX_info_packet_Type SYS_input_packet;           // 给接收中断
GX_info_packet_Type SYS_temp_packet;
GX_info_packet_Type SYS_output_packet;
GX_info_packet_Type SYS_data_packet_buff[5];

GX_info_packet_Type RFID_input_packet;           // 给接收中断
GX_info_packet_Type RFID_output_packet;
GX_info_packet_Type RFID_data_packet_buff[5];

GX_info_packet_Type RS232_input_packet;         // 给接收中断
GX_info_packet_Type RS232_output_packet;
GX_info_packet_Type RS232_data_packet_buff[3];

GX_info_packet_Type RS485_input_packet;         // 给接收中断
GX_info_packet_Type RS485_output_packet;
GX_info_packet_Type RS485_data_packet_buff[3];

GX_info_packet_Type gx_standard = {
    .Head = 0x5A,
    .Addr = 0x01,
    .Prot_W_Versions = 0x01,    // 版本
    .dSize = BUFF_MAX,          // 最大长度
};

unsigned char pack_array[BUFF_MAX];
unsigned char temp_array[BUFF_MAX];

int get_temp_num = 0;

int get_PC_data = 0;
int get_PC_num = 0;
int get_PC_pack_num = 0;
int out_PC_num = 0;
int out_PC_pack_num = 0;

int get_RFID_data = 0;
int get_RFID_num = 0;
int get_RFID_pack_num = 0;
int out_RFID_num = 0;
int out_RFID_pack_num = 0;

//volatile u64 test_cen_a,test_cen_b,test_cen_c,test_cen_d;
/*
 *  1.如果一条消息要发给RFID和ESP32，那么就要考虑是否会有消息返回，如果同时有消息返回就会有中断被占用。
 *
 */
int Center_State_machine(Caven_Watch_Type time)
{
    int retval = 0;

    center_time = time;
    GX_Circular_queue_output (&RFID_output_packet,RFID_data_packet_buff,5);     // 从队列中提取
    GX_Circular_queue_output (&SYS_output_packet ,SYS_data_packet_buff,5);      // 从队列中提取
    GX_Circular_queue_output (&RS232_output_packet,RS232_data_packet_buff,3);   // 从队列中提取
    GX_Circular_queue_output (&RS485_output_packet,RS485_data_packet_buff,3);   // 从队列中提取

    if (RFID_output_packet.Result & 0x50)           // RFID来的消息
    {
        out_RFID_pack_num ++;
        out_RFID_num += RFID_output_packet.Get_num;
        Center_RFID_order_handle(RFID_output_packet);
        GX_info_packet_clean_Fun(&RFID_output_packet);
    }

    if (SYS_output_packet.Result & 0x50)
    {
        out_PC_pack_num ++;
        out_PC_num += SYS_output_packet.Get_num;
        //
//        s_SYS_Config.Connect_passage = SYS_output_packet.Comm_way;
        Center_SYS_order_handle(SYS_output_packet);
        GX_info_packet_clean_Fun(&SYS_output_packet);
    }
    if (RS232_output_packet.Result & 0x50)
    {
        out_PC_pack_num++;
        out_PC_num += RS232_output_packet.Get_num;

        s_SYS_Config.Connect_passage = RS232_output_packet.Comm_way;
        Center_order_handle(RS232_output_packet);
        GX_info_packet_clean_Fun(&RS232_output_packet);
    }
    if (RS485_output_packet.Result & 0x50)
    {
        out_PC_pack_num++;
        out_PC_num += RS485_output_packet.Get_num;
        GX_info_remove_addr(&RS485_output_packet);
        s_SYS_Config.Connect_passage = RS485_output_packet.Comm_way;
        Center_order_handle(RS485_output_packet);
        GX_info_packet_clean_Fun(&RS485_output_packet);
    }
    Caven_handle_event_Fun(&SYS_events);        // event
    GPI_Change_Updata_Handle();                 // 检测GPI
    MCU_Combination_version_Updata_Handle ();   // 收集版本消息

    Heartbeat_Check(time);                      // 检测心跳
    SYS_Time_Get(&center_BaseTIME);

    if (BUTTON_STATE() == 0) {
        GPO_Open_lock_ALL ();
    }
    if (s_SYS_Config.SYS_Rst) {
        Mode_Use.TIME.Delay_Ms(10);
        retval = 1;
    }
    return retval;
}

/*
 * doing
 */

int Center_RFID_order_handle(GX_info_packet_Type data)
{
    int retval = 1;                 // 默认都会转发

    if (data.Prot_W_Class == 1 && data.Prot_W_DFlag == 0)     // 读写器配置与管理消息
    {
        switch (data.Prot_W_MID)
        {
            case query_reader_params_order:
                if (s_SYS_Config.combination_version_Flag) {
                    s_SYS_Config.combination_version_Flag++;
                    retval = Get_RFID_version (data);       // 这个是uart问的，需要保存用来合成，不直接转发
                }
                else {
                    Get_RFID_version (data);                // 这个是esp32问的，下面直接转发
                }
                break;
            case query_reader_software_version_order:
                retval = 1;
                break;
            case config_reader_serial_params_order:
                retval = 0;
                if (s_SYS_Config.RS232_Flag)
                {
                    if (data.Prot_W_485Type) {
                        memcpy(temp_array, data.p_Data + 8, data.dSize);
                    }
                    else {
                        memcpy(temp_array, data.p_Data + 7, data.dSize);
                    }
                    if (temp_array[0] == 0) {
                        Caven_trigger_event_Fun(&SYS_events,RS232_event_Handle,0x02);   // 成功
                    }
                    else {
                        Caven_trigger_event_Fun(&SYS_events,RS232_event_Handle,0x03);   // 失败
                    }
                }
                break;
            case config_reader_rs485_address_order:
                retval = 1;
                break;
            default:
                retval = 1;
                break;
        }
    }
    else if (data.Prot_W_Class == 2 && data.Prot_W_DFlag == 1)
    {
        switch (data.Prot_W_MID)
        {
            case 0:
            {
                data.Comm_way = UART_SYS;
                GX_send_packet(data);               // 主动上报的需要给一份到SYS

                if(s_SYS_Config.Connect_passage != UART_RS485)
                {
                    data.Comm_way = UART_RS232;
                    GX_send_packet(data);           //
                }
                else
                {
                    data.Comm_way = UART_RS485;
                    GX_send_packet(data);           //
                }
                RFID_LED_H();
                retval = 0;
            }
                break;
            case 1:
            {
                if (s_SYS_Config.ESP32_board_ID == 0) {
                    memset(&s_GPIO_Config.GPO_Status_Set[9],0,8);
                    GPO_Sync_Status (s_GPIO_Config.GPO_Status_Set,s_GPIO_Config.GPO_num);
                }
                retval = 1;
            }
                break;
            case 2:
            {
                if (data.Prot_W_485Type) {
                    memcpy(temp_array, data.p_Data + 8, data.dSize);
                }
                else {
                    memcpy(temp_array, data.p_Data + 7, data.dSize);
                }
                if(temp_array[0] <= 8)
                {
                    if (s_SYS_Config.ESP32_board_ID == 0) {
                        memset(&s_GPIO_Config.GPO_Status_Set[9],0,8);
                        s_GPIO_Config.GPO_Status_Set[8+temp_array[0]] = 1;
                        GPO_Sync_Status (s_GPIO_Config.GPO_Status_Set,s_GPIO_Config.GPO_num);
                    }
                }
                retval = 1;
            }
                break;
            default:
                retval = 1;
                break;
        }
    }
    else if (data.Prot_W_Class == 2)
    {
        switch (data.Prot_W_MID)
        {
            case 0xff:
                if (s_SYS_Config.ESP32_board_ID == 0) {
                    memset(&s_GPIO_Config.GPO_Status_Set[9],0,8);
                    GPO_Sync_Status (s_GPIO_Config.GPO_Status_Set,s_GPIO_Config.GPO_num);
                }
                else if (s_SYS_Config.ESP32_board_ID == 21)  //
                {
                    s_GPIO_Config.GPO_Status_Set[9] = 0;
                    GPO_Sync_Status (s_GPIO_Config.GPO_Status_Set,s_GPIO_Config.GPO_num);
                }
                retval = 1;
                break;
            case 0x10:  // 读卡
                if (s_SYS_Config.ESP32_board_ID == 21)  // N21X天线
                {
                    s_GPIO_Config.GPO_Status_Set[9] = 1;
                    GPO_Sync_Status (s_GPIO_Config.GPO_Status_Set,s_GPIO_Config.GPO_num);
                }
                retval = 1;
                break;
            default:
                retval = 1;
                break;
        }
    }

    if (retval == 1)
    {
        data.Comm_way = s_SYS_Config.Connect_passage;
        GX_send_packet(data);               // 转发给活动的接口
    }

    return retval;
}

// 初始化成功之后，Connect_passage = UART_SYS;
int Center_SYS_order_handle(GX_info_packet_Type data)
{
    int retval = 1;
    if (data.Prot_W_Type == PROTOCOL_UHF_READER && data.Prot_W_Class == 1 && data.Prot_W_DFlag == 0)    // 读写器配置与管理消息
    {
        switch (data.Prot_W_MID)
        {
            case query_reader_params_order:
                retval = Get_SYS_version (data);    // 给ESP32或者给RFID
                if (retval) {
                    s_SYS_Config.Connect_passage = UART_SYS;
                }
                break;
            case query_reader_software_version_order:
                retval = 1;                         // 给RFID
                s_SYS_Config.Connect_passage = UART_SYS;
                break;
            case config_reader_serial_params_order:
                retval = RS232_SET_Order(data);     // 给MCU,以及RFID
                s_SYS_Config.Connect_passage = UART_SYS;
                break;
            case config_reader_rs485_address_order:
                retval = RS485_SET_Order(data);     //!!!! 给MCU
                s_SYS_Config.Connect_passage = UART_SYS;
                break;
            case config_reader_GPO_state_order:
                retval = GPO_SET_Order(data);       // 给MCU
                s_SYS_Config.Connect_passage = UART_SYS;
                break;
            case query_reader_GPI_state_order:
                retval = GPI_GET_Order(data);       // 给MCU
                if (retval == 0) {
                    s_SYS_Config.Connect_passage = UART_SYS;
                }
                break;
            case beep_device_control_order:
                retval = BZZ_Status_Order(data);    // 给MCU
                s_SYS_Config.Connect_passage = UART_SYS;
                break;
            case restart_reader_order:
                retval = RST_SYS_Order(data);       // 给MCU,以及RFID
                RSTIC_H();
                break;
            case reader_restore_default_params_order:
                retval = 1;                         // 给RFID
                s_SYS_Config.Connect_passage = UART_SYS;
                break;

            default:
                retval = 2;
                break;
        }
    }
    else if (data.Prot_W_Type == PROTOCOL_ICIO_CMD && data.Prot_W_Class == 1)   // 特殊cmd,只给IO板
    {
        switch (data.Prot_W_MID)
        {
            case config_reader_rs485_address_order:
                retval = RS485_SET_Order(data);     //!!!! 给MCU
                break;
            case config_reader_GPO_state_order:
                retval = GPO_SET_Order(data);       // 给MCU
                break;
            case query_reader_GPI_state_order:
                retval = GPI_GET_Order(data);       // 给MCU
                break;
            case beep_device_control_order:
                retval = BZZ_Status_Order(data);    // 给MCU
                break;
            case config_reader_WG_params_order:
                retval = WG_Handle_Order(data);     // 给MCU
                break;
            case reader_connect_state_confirm_order:
                retval = Heartbeat_Set();           // 给MCU
                break;
            default:
                break;
        }
        retval = 0;
    }
    else if (data.Prot_W_Type == PROTOCOL_ICIO_CMD && data.Prot_W_Class == 2)   // 特殊cmd,只给IO板
    {
        switch (data.Prot_W_MID)
        {
            case 0:
                if (data.dSize == 1)
                {
                    s_SYS_Config.ESP32_board_ID = 0;
                    s_SYS_Config.ESP32_board_ID = data.p_Data[7];

                }
                break;
            case 1:
                if (data.p_Data[7] == 0) {
                    s_SYS_Config.RFID_Mode = 0;
                }
                else {
                    s_SYS_Config.RFID_Mode = 1;
                }
                break;
            case 2:
                if (data.p_Data[7] == 0) {
                    s_SYS_Config.RS232_Mode = 0;
                }
                else {
                    s_SYS_Config.RS232_Mode = 1;
                }
                break;
            case 3:
                if (data.p_Data[7] == 0) {
                    s_SYS_Config.RS485_Mode = 0;
                }
                else {
                    s_SYS_Config.RS485_Mode = 1;
                }
                break;
            case 0x11:
                Mode_Use.UART.Send_Data_pFun(UART_RFID,data.p_Data+7,data.dSize);
                break;
            case 0x12:
                Base_UART_DMA_Send_Data(UART_RS232,data.p_Data+7,data.dSize);
                break;
            case 0x13:
                Base_UART_DMA_Send_Data(UART_RS485,data.p_Data+7,data.dSize);
                break;
            default:
                break;
        }
        retval = 0;
    }
    else if(data.Prot_W_Type == PROTOCOL_UHF_READER && data.Prot_W_Class == 2)
    {
        s_SYS_Config.Connect_passage = UART_SYS;
    }
    else if(data.Prot_W_DFlag)                      // EPS32触发的主动上报
    {
        retval = 2;
    }

    if (retval == 1){
        data.Comm_way = UART_RFID;
        GX_send_packet(data);             // 转发给RFID
    }
    else if (retval == 2){
        if(s_SYS_Config.Connect_passage != UART_RS485)
        {
            data.Comm_way = UART_RS232;
            GX_send_packet(data);
        }
        else
        {
            data.Comm_way = UART_RS485;
            GX_send_packet(data);
        }
    }
    return retval;
}

/*
 * 1备份给ESP32
 * 2会忽略RS485位
 */
void SYS_Backups (GX_info_packet_Type data)
{
    if (data.Prot_W_485Type) {
        memcpy(temp_array, data.p_Data + 8, data.dSize);
    }
    else {
        memcpy(temp_array, data.p_Data + 7, data.dSize);
    }

    GX_force_Send_packet (data.Prot_W_Class, data.Prot_W_MID, UART_SYS, temp_array, data.dSize, 1);
}

int Center_order_handle(GX_info_packet_Type data)
{
    int retval = 1;
    if (data.Prot_W_Class == 1)    // 读写器配置与管理消息
    {
        switch (data.Prot_W_MID)    //
        {
            case query_reader_params_order:
                retval = query_params_order(data);
                break;
            case query_reader_software_version_order:
                retval = 1;                         // 直接给RFID
                break;
            case config_reader_serial_params_order:
                retval = RS232_SET_Order(data);     // 修改成功才会备份
                break;
            case config_reader_rs485_address_order:
                retval = RS485_SET_Order(data);
                SYS_Backups (data);                 // 向ESP32备份
                break;
            case query_reader_serial_params_order:
                retval = RS232_GET_Order(data);
                break;
            case query_reader_rs485_addresss_order:
                retval = RS485_GET_Order(data);
                break;
            case config_reader_GPO_state_order:
                retval = GPO_SET_Order(data);
                break;
            case query_reader_GPI_state_order:
                retval = GPI_GET_Order(data);
                break;
            case beep_device_control_order:
                retval = BZZ_Status_Order(data);
                break;
            case restart_reader_order:
                retval = RST_SYS_Order(data);
                SYS_Backups (data);
                RSTIC_H();                      // 复位ESP32(uart通道有效)，强制。
                break;
            case reader_restore_default_params_order:
                retval = 2;                     // 给ESP32
                break;
            default:
                retval = 2;                     // 其他消息都给ESP32
                break;
        }
    }
    if (retval == 1){
        data.Comm_way = UART_RFID;
        GX_send_packet(data);             // 转发给RFID
    }
    else if (retval == 2){
        data.Comm_way = UART_SYS;
        GX_send_packet(data);             // 转发给ESP32
    }
    return retval;
}

/*
 * 0 主动上传到sys
 */
int GPI_Change_Updata_Handle (void)
{
    int run_num = 0;
    int retval = 0;

    for (int i = 1; i <= s_GPIO_Config.GPI_num; ++i)
    {
        switch (i)
        {
            case 1:
                s_GPIO_Config.GPI_Status_Get[i] = GPI1_STATE();
                break;
            case 2:
                s_GPIO_Config.GPI_Status_Get[i] = GPI2_STATE();
                break;
            default:
                break;
        }
        if (s_GPIO_Config.GPI_Status_Get[i] != s_GPIO_Config.GPI_Statls_Last[i])    // 发现差异
        {
            s_GPIO_Config.GPI_Statls_Last[i] = s_GPIO_Config.GPI_Status_Get[i];
            temp_array[run_num ++] = i;
            temp_array[run_num ++] = s_GPIO_Config.GPI_Status_Get[i];
        }
    }
    if (run_num > 0)
    {
        if (s_SYS_Config.ESP32_board_ID != 13) // E1103 不用MCU的GPI,所以主动上传关闭
        {
            GX_force_Send_packet (1, query_reader_GPI_state_order, UART_SYS, temp_array, run_num, 1);
        }
    }
    return retval;
}

/*
 * 0 返回包
 * 1 处理后，不透传
 */
int GPI_GET_Order(GX_info_packet_Type data)
{
    int retval = 0;
    int run_num = 0;
    int GPI_state;

    DESTROY_DATA(temp_array, sizeof(temp_array));

    if (data.Comm_way == UART_SYS)
    {
        if (data.dSize == 0)
        {
            for (int i = 1;i <= s_GPIO_Config.GPI_num;i ++)
            {
                GPI_state = s_GPIO_Config.GPI_Status_Get[i];

                temp_array[run_num++] = i;
                temp_array[run_num++] = GPI_state & 0xFF;
            }
            GX_force_Send_packet (data.Prot_W_Class,data.Prot_W_MID, data.Comm_way,temp_array,run_num,0);
        }
        else        // 活动其他通道
        {
            retval = 2;
        }
    }
    else            // RS232 & RS485
    {
        if (s_SYS_Config.ESP32_board_ID != 13)       // 上传
        {
            for (int i = 1;i <= s_GPIO_Config.GPI_num;i ++)
            {
                GPI_state = s_GPIO_Config.GPI_Status_Get[i];

                temp_array[run_num++] = i;
                temp_array[run_num++] = GPI_state & 0xFF;
            }
            GX_force_Send_packet (data.Prot_W_Class,data.Prot_W_MID, data.Comm_way,temp_array,run_num,0);
        }
        else {
            GX_force_Send_packet (data.Prot_W_Class,data.Prot_W_MID, UART_SYS,data.p_Data,data.dSize,0);
        }
    }
//    GX_info_rest_data_packet_Fun(&temp_packet,temp_array,run_num);
//    GX_send_packet(temp_packet);
    return retval;
}

int GPO_Sync_Status (unsigned char *data,int len)
{
    int retval = 0;
    int GPO_state;
    int temp_num;
    unsigned char temp_data[50];
    unsigned char hc595[10];
    if (data == NULL) {
        return retval;
    }
    memcpy(temp_data,data,len);
    memset(hc595,0,sizeof(hc595));
    if (s_SYS_Config.ESP32_board_ID == 13)      // E1103 的GPO[1-16]都是锁，并且是第二个HC595开始
    {
        memcpy(&temp_data[1],&data[17],8);
        memcpy(&temp_data[9],&data[1],16);
        temp_num = 1;      // lock
        for (int hc595_num = 4; hc595_num >= 0; hc595_num--)
        {
            for (int j = 0; j < 8; j++)
            {
                GPO_state = temp_data[temp_num++];
                hc595[hc595_num] |= GPO_state << j;
            }
        }
        HC595_Set_DATA_Fun (hc595,5);
        retval = 1;
        return retval;
    }
    else
    {
        for (int i = 1; i <= len; ++i)
        {
            GPO_state = temp_data[i];
            switch (i)
            {
            case 1:
                if (GPO_state)  {GPO1_H();}
                else            {GPO1_L();}
                break;
            case 2:
                if (GPO_state)  {GPO2_H();}
                else            {GPO2_L();}
                break;
            case 3:
                if (GPO_state)  {GPO3_H();}
                else            {GPO3_L();}
                break;
            case 4:
                if (GPO_state)  {GPO4_H();}
                else            {GPO4_L();}
                break;
            case 5:
                break;
            case 6:
                break;
            case 7:
                break;
            case 8:
                break;
            default:
                // HC595,直接给int,长度，然后直接退出
                // dat是50位的,HC595有5个，6固定是天线灯，前面的是天线/锁；0是最后一组数据。
                temp_num = 9;
                for (int hc595_num = 4; hc595_num >= 0; hc595_num--)
                {
                    for (int j = 0; j < 8; j++)
                    {
                        GPO_state = temp_data[temp_num++];
                        hc595[hc595_num] |= GPO_state << j;
                    }
                }
                HC595_Set_DATA_Fun (hc595,5);
                retval = 1;
                return retval;
            }
        }
    }
    retval = 0;
    return retval;
}

void GPO_Open_lock_ALL (void)
{
    if (s_SYS_Config.ESP32_board_ID == 13) {
        for (int var = 0; var < 16; ++var)
        {
            s_GPIO_Config.GPO_Status_Set[1 + var] = 1;
            GPO_Sync_Status (s_GPIO_Config.GPO_Status_Set,s_GPIO_Config.GPO_num);
            Mode_Use.TIME.Delay_Ms(600);
            memset(s_GPIO_Config.GPO_Status_Set,0,sizeof(s_GPIO_Config.GPO_Status_Set));
            GPO_Sync_Status (s_GPIO_Config.GPO_Status_Set,s_GPIO_Config.GPO_num);
            Mode_Use.TIME.Delay_Ms(400);
        }
    }
    else {
        memset(s_GPIO_Config.GPO_Status_Set,1,sizeof(s_GPIO_Config.GPO_Status_Set));
        GPO_Sync_Status (s_GPIO_Config.GPO_Status_Set,s_GPIO_Config.GPO_num);
        Mode_Use.TIME.Delay_Ms(600);
        memset(s_GPIO_Config.GPO_Status_Set,0,sizeof(s_GPIO_Config.GPO_Status_Set));
        GPO_Sync_Status (s_GPIO_Config.GPO_Status_Set,s_GPIO_Config.GPO_num);
        Mode_Use.TIME.Delay_Ms(400);
    }
}

int s_once_GPO = 0;
void GPO_event_task_Fun (void * data)
{
    int temp_data = *(int *)data;
    int set_time = 0;

    static Task_Overtime_Type GPO_once_Task = {
        .Switch = 1,            // 任务开关
        .Begin_time = {0},
        .Set_time.second = 0,
    };
    if (temp_data && s_GPIO_Config.GPO_Keep_Time > 0)
    {
        if (s_once_GPO == 0) {
            s_once_GPO = 1;
            GPO_once_Task.Begin_time = center_time;
            set_time = s_GPIO_Config.GPO_Keep_Time;    // ms
            GPO_once_Task.Set_time.minutes = ((set_time / 1000) / 60) % 60;
            GPO_once_Task.Set_time.second = (set_time / 1000) % 60;
            GPO_once_Task.Set_time.time_us = (set_time % 1000) * 1000;  // ms * 1000 = us
            GPO_once_Task.Set_time.hour = 0;
            GPO_Sync_Status (s_GPIO_Config.GPO_Status_Set,s_GPIO_Config.GPO_num);
        }
        else {
            API_Task_Timer(&GPO_once_Task, center_time);
        }

        if (GPO_once_Task.Trigger_Flag)
        {
            GPO_once_Task.Trigger_Flag = 0;
            s_once_GPO = 0;
            // 来到上一个操作的相反态
            GPO_Sync_Status (s_GPIO_Config.GPO_Status_Last,s_GPIO_Config.GPO_num);
            memcpy(s_GPIO_Config.GPO_Status_Set, s_GPIO_Config.GPO_Status_Last, sizeof(s_GPIO_Config.GPO_Status_Last));
            *(int *)data = 0;
        }
    }
    else {
        *(int *)data = 0;
    }
}
/*
 * 0 无
 * 1 转发给RFID
 * 2 转发给ESP32
 */
int GPO_SET_Order(GX_info_packet_Type const data)
{
    int retval = 0;
    int run_num;
    int GPO_data,GPO_state,GPO_Time;
    GX_info_packet_Type temp_packet = data;
    if (data.dSize > 0 && data.dSize < sizeof(temp_array))
    {
        if (data.Prot_W_485Type) {
            memcpy(temp_array, data.p_Data + 8, data.dSize);
        }
        else {
            memcpy(temp_array, data.p_Data + 7, data.dSize);
        }
        // 保存上一次的状态
        memcpy(s_GPIO_Config.GPO_Status_Last, s_GPIO_Config.GPO_Status_Set, sizeof(s_GPIO_Config.GPO_Status_Last));
//        retval.Result = RESULT_DEFAULT;

        for (run_num = 0;run_num < data.dSize;run_num += 2)
        {
            GPO_data = temp_array[run_num] & 0xFF;
            GPO_state = temp_array[run_num + 1];
            if ((GPO_data & 0x40) && GPO_data != 0xFF)      // 给状态灯[ 0100 0000] + STATE_id
            {
                switch (GPO_data & 0x0f)
                {
                    case (0x01):
                        if (GPO_state) {
                            RUN_LED_H();
                        }
                        else{
                            RUN_LED_L();
                        }
                        break;
                    case (0x02):
                        if (GPO_state) {
                            ERROR_LED_H();
                        }
                        else{
                            ERROR_LED_L();
                        }
                        break;
                    default:
                        break;
                }
                break;
            }
            else
            {
                switch (GPO_data)
                {
                case (0xFF):
                    GPO_Time = temp_array[run_num + 1];
                    GPO_Time <<= 8;
                    GPO_Time += temp_array[run_num + 2];
                    s_once_GPO = 0;                                         // 更新GPO的event

                    s_GPIO_Config.GPO_Keep_Time = GPO_Time;
                    Caven_trigger_event_Fun(&SYS_events,GPO_event_Handle,0x01);     // 触发一次事件
                    run_num ++;
                    temp_array[0] = 0;
                    GX_info_rest_data_packet_Fun(&temp_packet,temp_array,1);
                    GX_send_packet(temp_packet);
                    return retval;
                default:
                    s_GPIO_Config.GPO_Status_Set[GPO_data] = GPO_state;             //载入
                    if (GPO_state) {                                                //对操作对象镜像
                        s_GPIO_Config.GPO_Status_Last[GPO_data] = 0;
                    }
                    else {
                        s_GPIO_Config.GPO_Status_Last[GPO_data] = 1;
                    }

                    break;
                }
            }
        }
        GPO_Sync_Status (s_GPIO_Config.GPO_Status_Set,s_GPIO_Config.GPO_num);
        temp_array[0] = 0;
        GX_info_rest_data_packet_Fun(&temp_packet,temp_array,1);

        GX_send_packet(temp_packet);
    }
    return retval;
}

int s_once_bzz = 0;
void BZZ_event_task_Fun (void * data)
{
    int temp_data = *(int *)data;
    static char temp_run;
    static Task_Overtime_Type BZZ_once_Task;
    if (temp_data)
    {
        if (s_once_bzz == 0) {
            s_once_bzz = 1;
            BZZ_once_Task.Begin_time = center_time;
            BZZ_once_Task.Switch = 1;       // 开任务
            if (s_SYS_Config.BZZ_times == 0) {
                BZZ_once_Task.Set_time.time_us = 200 * 1000;
            }
            else {
                if (s_SYS_Config.BZZ_ON_time == 0) {
                    BZZ_once_Task.Set_time.time_us = 5 * 1000;
                }
                else {
                    BZZ_once_Task.Set_time.time_us = (s_SYS_Config.BZZ_ON_time * 10000) % 1000000;
                    BZZ_once_Task.Set_time.second = (s_SYS_Config.BZZ_ON_time * 10000) / 1000000;
                }
            }
            temp_run = 0;
            BZZ_H();
        }
        else {
            API_Task_Timer(&BZZ_once_Task, center_time);
        }

        if (BZZ_once_Task.Trigger_Flag)                 // 时间激活
        {
            BZZ_once_Task.Trigger_Flag = 0;
            temp_run ++;
            if (s_SYS_Config.BZZ_times > 0) {
                if (temp_run % 2)                       // 触发响的定时，说明要不响了
                {
                    s_SYS_Config.BZZ_times--;
                    BZZ_once_Task.Begin_time = center_time;

                    if (s_SYS_Config.BZZ_ON_time == 0) {
                        BZZ_once_Task.Set_time.time_us = 5 * 1000;
                    }
                    else {
                        BZZ_once_Task.Set_time.time_us = (s_SYS_Config.BZZ_OFF_time * 10000) % 1000000;
                        BZZ_once_Task.Set_time.second = (s_SYS_Config.BZZ_OFF_time * 10000) / 1000000;      // 设置不响的时间
                    }
                    BZZ_L();
                }
                else                                    // 不响的时间到了,接下来要响了
                {
                    BZZ_once_Task.Begin_time = center_time;
                    if (s_SYS_Config.BZZ_ON_time == 0) {
                        BZZ_once_Task.Set_time.time_us = 5 * 1000;
                    }
                    else {
                        BZZ_once_Task.Set_time.time_us = (s_SYS_Config.BZZ_ON_time * 10000) % 1000000;
                        BZZ_once_Task.Set_time.second = (s_SYS_Config.BZZ_ON_time * 10000) / 1000000;      // 设置响的时间
                    }
                    BZZ_H();
                }
            }
            else {
                memset(&BZZ_once_Task,0,sizeof(BZZ_once_Task));     // 删除时钟任务
                BZZ_L();
                temp_run = 0;
                s_once_bzz = 0;
                *(int *)data = 0;           // end
            }
        }
    }
    else {
        *(int *)data = 0;
    }
}
/*
 * 0 返回包
 * 1 处理后，不透传
 * 2024.3.19 新增可选参数，实现在BZZ_event_task_Fun，由Caven_trigger_event_Fun触发
 */
int BZZ_Status_Order(GX_info_packet_Type const data)
{
    int retval = 0;
    int temp_run = 0;
    GX_info_packet_Type temp_packet = data;

    if (data.dSize > 0 && data.Get_num < sizeof(temp_array))
    {
        DESTROY_DATA(temp_array, sizeof(temp_array));
        if (data.Prot_W_485Type) {
            memcpy(temp_array, data.p_Data + 8, data.dSize);
        }
        else {
            memcpy(temp_array, data.p_Data + 7, data.dSize);
        }
        if (temp_array[temp_run++] == 0)
        {
            BZZ_L();
        }
        else
        {
            if (temp_array[temp_run++])
            {
                //触发BZZ响
                BZZ_H();
            }
            else
            {
                //触发BZZ响一次事件
                s_once_bzz = 0;
                if (temp_array[temp_run++] == 0x01)        // 可选
                {
                    s_SYS_Config.BZZ_ON_time = temp_array[temp_run++];
                    s_SYS_Config.BZZ_ON_time <<= 8;
                    s_SYS_Config.BZZ_ON_time |= temp_array[temp_run++];
                    if (temp_array[temp_run++] == 0x02)        // 可选
                    {
                        s_SYS_Config.BZZ_OFF_time = temp_array[temp_run++];
                        s_SYS_Config.BZZ_OFF_time <<= 8;
                        s_SYS_Config.BZZ_OFF_time |= temp_array[temp_run++];
                    }
                    if (temp_array[temp_run++] == 0x03)        // 可选
                    {
                        s_SYS_Config.BZZ_times = temp_array[temp_run++];
                        if (s_SYS_Config.BZZ_times == 0)
                        {
                            s_SYS_Config.BZZ_times = 1;
                        }
                    }
                }
                else {
                    s_SYS_Config.BZZ_times = 0;
                }
                Caven_trigger_event_Fun(&SYS_events,BZZ_event_Handle,0x01);

            }
        }
        temp_array[0] = 0;
        GX_info_rest_data_packet_Fun(&temp_packet,temp_array,1);

        GX_send_packet(temp_packet);

//        run_num = GX_info_return_Fun (temp_packet.Prot_W_Class,temp_packet.Prot_W_MID,temp_packet.Addr,&temp_data,1,temp_array);
//        Base_UART_DMA_Send_Data(temp_packet.Comm_way,temp_array,run_num);
    }
    return retval;
}

/*
 * Wiegand 执行
 */
int WG_Handle_Order(GX_info_packet_Type const data)
{
    int retval = 2;
    int run_num = 0;
    int WG_temp_num;
    char temp_byte;
    if (data.Prot_W_485Type) {
        memcpy(temp_array, data.p_Data + 8, data.dSize);
    }
    else {
        memcpy(temp_array, data.p_Data + 7, data.dSize);
    }

    retval = 0;
    temp_byte = temp_array[run_num++];
    if (temp_byte == 1 && s_SYS_Config.WG_config.WG_Switch == 0)
    {
        s_SYS_Config.WG_config.WG_Switch = 1;
        temp_byte = temp_array[run_num++];
        if (temp_byte == 0)
        {
            s_SYS_Config.WG_config.WG_mode = 26;
        }
        else if (temp_byte == 1)
        {
            s_SYS_Config.WG_config.WG_mode = 34;
        }
        else if (temp_byte == 2)
        {
            s_SYS_Config.WG_config.WG_mode = 66;
        }
        WG_temp_num = temp_array[run_num++] << 8;
        WG_temp_num |= temp_array[run_num++];
        s_SYS_Config.WG_config.WG_with = WG_temp_num;
        WG_temp_num = temp_array[run_num++] << 8;
        WG_temp_num |= temp_array[run_num++];
        s_SYS_Config.WG_config.WG_cycle = WG_temp_num;

        WG_temp_num = temp_array[run_num++];
        memcpy(s_SYS_Config.WG_config.WG_data,&temp_array[run_num],WG_temp_num);
    }

    return retval;
}

/*
 * 0 延迟执行,所有通道透传
 */
int RST_SYS_Order(GX_info_packet_Type const data)
{
    int retval = 1;

    Mode_Use.LED.SET_pFun(1, DISABLE);
    s_SYS_Config.SYS_Rst = 1;
    return retval;
}
/*
 * 0 无返回包
 * 1 查询设备型号，串口数据需要透传给RFID，并且发一份给ESP32；如果是网口数据，那就直接给RFID
 * 2 接收到两个设备消息，合成一个统一返回
 */
int query_params_order(GX_info_packet_Type const data)
{
    int retval = 1;
    if (data.Comm_way == UART_SYS) {
        retval = 1;                     // 给RFID
    }
    else {
        s_SYS_Config.combination_version_Flag = 1;
        MCU_query_SYS_version ();       // 查询ESP32
        retval = 1;                     // 给RFID
    }

    return retval;
}

/*
 * 0 获取RFID的版本并保存
 */
int Get_RFID_version (GX_info_packet_Type data)
{
    int retval = 0;
    if (data.p_Data != NULL)
    {
        s_SYS_Config.RFID_version_len = data.dSize;
        if (data.dSize > 0)
        {
            if (data.Prot_W_485Type) {
                memcpy(s_SYS_Config.RFID_version, data.p_Data + 8, data.dSize);
            }
            else {
                memcpy(s_SYS_Config.RFID_version, data.p_Data + 7, data.dSize);
            }
        }
    }
    else {
        retval = 1;
    }
    return retval;
}

/*
 * 0[00 01 00][00 00 00 01][00 01 00]-10
 * 1 获取ESP32版本号
 */
int Get_SYS_version (GX_info_packet_Type data)
{
    int retval = 0;
    if (data.p_Data != NULL)
    {
        if (data.dSize > 10)
        {
            if (data.Prot_W_485Type) {
                memcpy(s_SYS_Config.SYS_version, data.p_Data +8+10, data.dSize-10);
            }
            else {
                memcpy(s_SYS_Config.SYS_version, data.p_Data +7+10, data.dSize-10);
            }
            s_SYS_Config.SYS_version_len = data.dSize - 10;
            if (s_SYS_Config.combination_version_Flag) {
                s_SYS_Config.combination_version_Flag++;
            }
            retval = 0;         // 这个是返回，给MCU合成
        }
        else {
            retval = 1;         // 这个是询问，查RFID
        }
    }

    return retval;
}

int RS232_GET_Order(GX_info_packet_Type const data)
{
    int retval = 1;
    u8 Uart_type;
    int run_num = 0;
    GX_info_packet_Type temp_packet = data;
    if (data.dSize == 0)
    {
        switch (s_SYS_Config.RS232_Baud)
        {
            case 9600:
                Uart_type = 0;
                break;
            case 19200:
                Uart_type = 1;
                break;
            case 230400:
                Uart_type = 3;
                break;
            case 460800:
                Uart_type = 4;
                break;
            default:
                Uart_type = 2;
                break;
        }
        temp_array[run_num++] = Uart_type;
        GX_info_rest_data_packet_Fun(&temp_packet,temp_array,run_num);

        GX_send_packet(temp_packet);
        retval = 0;
    }
    return retval;
}

int RS485_GET_Order(GX_info_packet_Type const data)
{
    int retval = 1;
    u8 Uart_type;
    int run_num = 0;
    GX_info_packet_Type temp_packet = data;
    if (data.dSize == 0)
    {
        switch (s_SYS_Config.RS485_Baud)
        {
            case 9600:
                Uart_type = 0;
                break;
            case 19200:
                Uart_type = 1;
                break;
            case 230400:
                Uart_type = 3;
                break;
            case 460800:
                Uart_type = 4;
                break;
            default:
                Uart_type = 2;
                break;
        }
        temp_array[run_num++] = s_SYS_Config.RS485_Addr;
        temp_array[run_num++] = Uart_type;
        GX_info_rest_data_packet_Fun(&temp_packet,temp_array,run_num);

        GX_send_packet(temp_packet);
        retval = 0;
    }
    return retval;
}
/*
 * 0 无返回包
 * 1 自身处理后，透传RFID，然后成功之后备份一份给ESP32
 */
int RS232_SET_Order(GX_info_packet_Type const data)
{
    int retval = 1;
    char Set_Status;
    int Uart_Baud;

    if (data.dSize > 0 && data.dSize < sizeof(temp_array))
    {
        if (data.Prot_W_485Type) {
            memcpy(temp_array, data.p_Data + 8, data.dSize);
        }
        else {
            memcpy(temp_array, data.p_Data + 7, data.dSize);
        }
        Set_Status = temp_array[0];

        switch (Set_Status) {
            case 0:             // 9600
                Uart_Baud = 9600;
                break;
            case 1:             // 19200
                Uart_Baud = 19200;
                break;
            case 2:             // 115200
                Uart_Baud = 115200;
                break;
            case 3:             // 230400
                Uart_Baud = 230400;
                break;
            case 4:             // 460800
                Uart_Baud = 460800;
                break;
            default:            // 失败
                pack_array[0] = 1;
                if (data.Comm_way != UART_SYS)
                {
                    GX_force_Send_packet (data.Prot_W_Class, data.Prot_W_MID, s_SYS_Config.Connect_passage, pack_array, 1, 0);
                }
                retval = 0;
                return retval;
                break;
        }

        s_SYS_Config.RS232_Baud_Type = Set_Status;
        s_SYS_Config.temp_Baud = Uart_Baud;

        if (data.Comm_way == UART_SYS) {
            Caven_trigger_event_Fun(&SYS_events,RS232_event_Handle,0x01);   // root
        }
        else {
//            Caven_trigger_event_Fun(&SYS_events,RS232_event_Handle,0x01); // 别开
            s_SYS_Config.RS232_Flag = 1;
        }
        retval = 1;                     // 透传
    }
    return retval;
}

/*
 * 0 无返回包
 * 1 自身处理后，不透传RFID，然后在RS485_SET_Order函数之后备份一份给ESP32（sys来的命令本身就不用备份）
 */
int RS485_SET_Order(GX_info_packet_Type const data)
{
    int retval = 0;
    u8 Set_ID,Set_Status = 0;
    int Uart_Baud = 0;
    int run_num = 0;
    if (data.dSize > 0 && data.dSize < sizeof(temp_array))
    {
        if (data.Prot_W_485Type) {
            memcpy(temp_array, data.p_Data + 8, data.dSize);
        }
        else {
            memcpy(temp_array, data.p_Data + 7, data.dSize);
        }
        Set_ID = temp_array[0];
        pack_array[run_num++] = 0;      // 成功

        if (temp_array[1] == 1) {
            Set_Status = temp_array[2];

            switch (Set_Status)
            {
                case 0:             // 9600
                    Uart_Baud = 9600;
                    break;
                case 1:             // 19200
                    Uart_Baud = 19200;
                    break;
                case 2:             // 115200
                    Uart_Baud = 115200;
                    break;
                case 3:             // 230400
                    Uart_Baud = 230400;
                    break;
                case 4:             // 460800
                    Uart_Baud = 460800;
                    break;
                default:            // 失败
                    Uart_Baud = 0;
                    pack_array[0] = 1;
                    break;
            }
        }
        if (data.Comm_way != UART_SYS)      // 返回消息
        {
            GX_force_Send_packet (data.Prot_W_Class, data.Prot_W_MID, s_SYS_Config.Connect_passage, pack_array, run_num, 0);
            Mode_Use.TIME.Delay_Ms(100);
        }
        if (Uart_Baud)
        {
            s_SYS_Config.RS485_Baud_Type = Set_Status;
            s_SYS_Config.RS485_Baud = Uart_Baud;
            Mode_Init.UART(UART_RS485, s_SYS_Config.RS485_Baud, ENABLE);   //
        }
        gx_standard.Addr = Set_ID & 0xff;
        s_SYS_Config.RS485_Addr = gx_standard.Addr;

        retval = 0;                     // 485不透传给RFID
    }
    return retval;
}


/*
 * UART1
 */
SYS_BaseTIME_Type SYS_Get_last_time;
void UART_SYS_Getrx_Fun(void *data)
{
    u8 temp = *(u8 *)data;
    int temp_num;
    get_PC_data++;
    temp_num = center_BaseTIME.SYS_Sec - SYS_Get_last_time.SYS_Sec;
    if (temp_num > 1)       // 间隔超过1s
    {
        GX_info_packet_fast_clean_Fun(&SYS_input_packet);
        SYS_Get_last_time = center_BaseTIME;
    }
    GX_info_Make_packet_Fun(gx_standard, &SYS_input_packet, temp);
    if (SYS_input_packet.Result & 0x50)     // 加入队列
    {
        if (SYS_input_packet.Prot_W_485Type) {      // rs485自动读卡,算rs485命令
            SYS_input_packet.Comm_way = UART_RS485;
            s_SYS_Config.Connect_passage = UART_RS485;
            GX_Circular_queue_input (&SYS_input_packet,RS485_data_packet_buff,0,3);
        }
        else {
            SYS_input_packet.Comm_way = UART_SYS;
            temp_num = GX_Circular_queue_input (&SYS_input_packet,SYS_data_packet_buff,0,5);
        }
        if (temp_num == 0) {
            get_PC_pack_num ++;
            get_PC_num += SYS_input_packet.Get_num;
        }
    }
}
/*
 * UART4
 */
char input_num = 0;
SYS_BaseTIME_Type RFID_Get_last_time;
void UART_RFID_Getrx_Fun(void *data)
{
    u8 temp = *(u8 *)data;
    int temp_num;
    if(s_SYS_Config.RFID_Mode == 0)
    {
        get_RFID_data ++;
        temp_num = center_BaseTIME.SYS_Sec - RFID_Get_last_time.SYS_Sec;
        if (temp_num > 1)       // 间隔超过1s
        {
            GX_info_packet_fast_clean_Fun(&RFID_input_packet);
            RFID_Get_last_time = center_BaseTIME;
        }
        GX_info_Make_packet_Fun(gx_standard, &RFID_input_packet, temp);

        if (RFID_input_packet.Result & 0x50)     // 加入队列
        {
            RFID_input_packet.Comm_way = UART_RFID;
            temp_num = GX_Circular_queue_input (&RFID_input_packet,RFID_data_packet_buff,input_num,5);
            input_num++;
            if (input_num >= 5) {
                input_num = 0;
            }
            if (temp_num == 0) {
                get_RFID_pack_num ++;
                get_RFID_num += RFID_input_packet.Get_num;
            }
        }
    }
    else {
        Mode_Use.UART.Send_Data_pFun(UART_SYS,&temp,1);
    }
}
/*
 * UART3
 */
SYS_BaseTIME_Type RS232_Get_last_time;
void UART_RS232_Getrx_Fun(void *data)
{
    u8 temp = *(u8 *)data;
    int temp_num;
    if(s_SYS_Config.RS232_Mode == 0)
    {
        get_PC_data ++;
        temp_num = center_BaseTIME.SYS_Sec - RS232_Get_last_time.SYS_Sec;
        if (temp_num > 1)       // 间隔超过1s
        {
            GX_info_packet_fast_clean_Fun(&RS232_input_packet);
            RS232_Get_last_time = center_BaseTIME;
        }
        GX_info_Make_packet_Fun(gx_standard, &RS232_input_packet, temp);
        if (RS232_input_packet.Result & 0x50)         // 加入队列
        {
            if (RS232_input_packet.Prot_W_485Type == 0) {
                RS232_input_packet.Comm_way = UART_RS232;
                temp_num = GX_Circular_queue_input (&RS232_input_packet,RS232_data_packet_buff,0,3);
                if (temp_num == 0) {
                    get_PC_pack_num ++;
                    get_PC_num += RS232_input_packet.Get_num;
                }
            }
            else {
                GX_info_packet_fast_clean_Fun(&RS232_input_packet);
            }
        }
    }
    else {
        Mode_Use.UART.Send_Data_pFun(UART_SYS,&temp,1);
    }
}
/*
 * UART2
 */
SYS_BaseTIME_Type RS485_Get_last_time;
void UART_RS485_Getrx_Fun(void *data)
{
    u8 temp = *(u8 *)data;
    int temp_num;

    if(s_SYS_Config.RS485_Mode == 0)
    {
        get_PC_data ++;
        temp_num = center_BaseTIME.SYS_Sec - RS485_Get_last_time.SYS_Sec;
        if (temp_num > 1)       // 间隔超过1s
        {
            GX_info_packet_fast_clean_Fun(&RS485_input_packet);
            RS485_Get_last_time = center_BaseTIME;
        }
        GX_info_Make_packet_Fun(gx_standard, &RS485_input_packet, temp);
        if (RS485_input_packet.Result & 0x50)     // 加入队列
        {
            if (RS485_input_packet.Prot_W_485Type) {
                get_PC_pack_num ++;
                RS485_input_packet.Comm_way = UART_RS485;
                GX_Circular_queue_input (&RS485_input_packet,RS485_data_packet_buff,0,3);
            }
            else {
                GX_info_packet_fast_clean_Fun(&RS485_input_packet);
            }
        }
    }
    else {
        Mode_Use.UART.Send_Data_pFun(UART_SYS,&temp,1);
    }
}

u8 g_SYS_Buff_array[8][BUFF_MAX];       // buff缓冲区
u8 g_RFID_Buff_array[8][BUFF_MAX];      // buff缓冲区
u8 g_RS232_Buff_array[5][BUFF_MAX];     // buff缓冲区
u8 g_RS485_Buff_array[5][BUFF_MAX];     // buff缓冲区

int Center_Init(void)
{
    int retval = 0;
    int run_num = 0;
    // UART_SYS
    GX_info_packet_index_Fun(&SYS_input_packet, g_SYS_Buff_array[run_num++]);
    GX_info_packet_clean_Fun(&SYS_input_packet);
    GX_info_packet_index_Fun(&SYS_output_packet, g_SYS_Buff_array[run_num++]);
    GX_info_packet_clean_Fun(&SYS_output_packet);
    GX_info_packet_index_Fun(&SYS_temp_packet, g_SYS_Buff_array[run_num++]);
    GX_info_packet_clean_Fun(&SYS_temp_packet);

    GX_info_packet_index_Fun(&SYS_data_packet_buff[0], g_SYS_Buff_array[run_num++]);
    GX_info_packet_index_Fun(&SYS_data_packet_buff[1], g_SYS_Buff_array[run_num++]);
    GX_info_packet_index_Fun(&SYS_data_packet_buff[2], g_SYS_Buff_array[run_num++]);
    GX_info_packet_index_Fun(&SYS_data_packet_buff[3], g_SYS_Buff_array[run_num++]);
    GX_info_packet_index_Fun(&SYS_data_packet_buff[4], g_SYS_Buff_array[run_num++]);

    // UART_RFID
    run_num = 0;
    GX_info_packet_index_Fun(&RFID_input_packet, g_RFID_Buff_array[run_num++]);
    GX_info_packet_clean_Fun(&RFID_input_packet);
    GX_info_packet_index_Fun(&RFID_output_packet, g_RFID_Buff_array[run_num++]);
    GX_info_packet_clean_Fun(&RFID_output_packet);

    GX_info_packet_index_Fun(&RFID_data_packet_buff[0], g_RFID_Buff_array[run_num++]);
    GX_info_packet_index_Fun(&RFID_data_packet_buff[1], g_RFID_Buff_array[run_num++]);
    GX_info_packet_index_Fun(&RFID_data_packet_buff[2], g_RFID_Buff_array[run_num++]);
    GX_info_packet_index_Fun(&RFID_data_packet_buff[3], g_RFID_Buff_array[run_num++]);
    GX_info_packet_index_Fun(&RFID_data_packet_buff[4], g_RFID_Buff_array[run_num++]);

    // UART_RS232
    run_num = 0;
    GX_info_packet_index_Fun(&RS232_input_packet, g_RS232_Buff_array[run_num++]);
    GX_info_packet_clean_Fun(&RS232_input_packet);
    GX_info_packet_index_Fun(&RS232_output_packet, g_RS232_Buff_array[run_num++]);
    GX_info_packet_clean_Fun(&RS232_output_packet);

    GX_info_packet_index_Fun(&RS232_data_packet_buff[0], g_RS232_Buff_array[run_num++]);
    GX_info_packet_index_Fun(&RS232_data_packet_buff[1], g_RS232_Buff_array[run_num++]);
    GX_info_packet_index_Fun(&RS232_data_packet_buff[2], g_RS232_Buff_array[run_num++]);

    // UART_RS485
    run_num = 0;
    GX_info_packet_index_Fun(&RS485_input_packet, g_RS485_Buff_array[run_num++]);
    GX_info_packet_clean_Fun(&RS485_input_packet);
    GX_info_packet_index_Fun(&RS485_output_packet, g_RS485_Buff_array[run_num++]);
    GX_info_packet_clean_Fun(&RS485_output_packet);

    GX_info_packet_index_Fun(&RS485_data_packet_buff[0], g_RS485_Buff_array[run_num++]);
    GX_info_packet_index_Fun(&RS485_data_packet_buff[1], g_RS485_Buff_array[run_num++]);
    GX_info_packet_index_Fun(&RS485_data_packet_buff[2], g_RS485_Buff_array[run_num++]);

    Mode_Use.UART.Receive_Bind_pFun(UART_SYS, UART_SYS_Getrx_Fun);
    Mode_Use.UART.Receive_Bind_pFun(UART_RFID, UART_RFID_Getrx_Fun);
    Mode_Use.UART.Receive_Bind_pFun(UART_RS232, UART_RS232_Getrx_Fun);
    Mode_Use.UART.Receive_Bind_pFun(UART_RS485, UART_RS485_Getrx_Fun);

//    Caven_new_event_Fun(&SYS_events,LED_event_task_Fun,&LED_event_Handle);
//    Caven_trigger_event_Fun(&SYS_events,LED_event_Handle,0x01);
    Caven_new_event_Fun(&SYS_events,BZZ_event_task_Fun,&BZZ_event_Handle);
//    Caven_trigger_event_Fun(&SYS_events,BZZ_event_Handle,0x01);
    Caven_new_event_Fun(&SYS_events,GPO_event_task_Fun,&GPO_event_Handle);
//    Caven_trigger_event_Fun(&SYS_events,GPO_event_Handle,0x01);
    Caven_new_event_Fun(&SYS_events,RS232_Baud_event_task_Fun,&RS232_event_Handle);

    return retval;
}

