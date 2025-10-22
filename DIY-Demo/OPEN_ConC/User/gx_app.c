#include "gx_app.h"

#define Log_tag "GX_app info"
#define GX_PACK_M 5

static uint8_t info_packet_array[10][BUFF_MAX];
static uint8_t info_packet_buff_array[GX_PACK_M][BUFF_MAX];
static GX_info_packet_Type GX_packet_buff[GX_PACK_M];
static GX_info_packet_Type GX_packet_debug;
static GX_info_packet_Type GX_packet_rs232;
static GX_info_packet_Type GX_packet_rs485;
static GX_info_packet_Type GX_packet_server;
static GX_info_packet_Type GX_packet_client;
static GX_info_packet_Type GX_packet_http;
static GX_info_packet_Type GX_packet_other;
//static GX_info_packet_Type GX_packet_mqtt;
//static GX_info_packet_Type GX_packet_udp;
//static GX_info_packet_Type GX_packet_BLE;
static GX_info_packet_Type GX_standard = {
	.Head = 0x5A,
    .Prot_W_Versions = 0x01,    // 版本
    .dSize = BUFF_MAX,		    // 最大长度
};

Caven_BaseTIME_Type GX_app_time;

int GX_app_SYS_info_handle (GX_info_packet_Type pack);
int GX_app_RFID_info_handle (GX_info_packet_Type pack);

int GX_app_State_machine(Caven_BaseTIME_Type time)
{
	int retval = 0;
	uint8_t temp_array[BUFF_MAX];

    GX_app_time = time;
    GX_info_packet_Type handle_pack;
	GX_info_packet_fast_clean_Fun(&handle_pack);
	GX_info_packet_index_Fun(&handle_pack, temp_array);
    GX_Circular_queue_output (&handle_pack,GX_packet_buff,GX_PACK_M);     // 从队列中提取
    if (handle_pack.Result & m_Result_SUCC)
    {
        switch (handle_pack.Comm_way)
        {
        case SYS_Link:
			GX_app_SYS_info_handle (handle_pack);
            break;
        case RS232_Link:		// RFID
			GX_app_RFID_info_handle (handle_pack);
            break;
        default:
			GX_app_SYS_info_handle (handle_pack);
            break;
        }
    }
	return retval;
}

int GX_app_cmd1_handle (GX_info_packet_Type pack);
int GX_app_cmd2_handle (GX_info_packet_Type pack);
int GX_app_cmd3_handle (GX_info_packet_Type pack);
int GX_app_send_packet(GX_info_packet_Type pack);
int GX_app_send_array(GX_info_packet_Type pack);

int GX_app_SYS_info_handle (GX_info_packet_Type pack)
{
	int retval = 0;
	switch (pack.Prot_W_Class)
	{
	case 1:
	#if 0
		pack.Comm_way = SYS_Link;		// 转给
		retval = GX_app_send_packet(pack);
	#else
		retval = GX_app_cmd1_handle (pack);
	#endif
		break;
	default:
		pack.Comm_way = RS232_Link;		// 转给RFID RS232_Link SYS_Link
		retval = GX_app_send_packet(pack);
		break;
	}
	return retval;
}

int GX_app_RFID_info_handle (GX_info_packet_Type pack)
{
	int retval = 0;
	if (pack.Prot_W_DFlag)
	{
		User_GPIO_set(2,15,0);
	}
	pack.Comm_way = g_SYS_Config.Connect_passage;
	switch (pack.Prot_W_Class)
	{
	case 1:
		retval = GX_app_send_packet(pack);
		break;
	default:
		retval = GX_app_send_packet(pack);
		break;
	}
	return retval;
}

int GX_app_cmd1_handle (GX_info_packet_Type pack)
{
    int retval = 0;
	int temp_num = 0,temp_run = 0;
	pack.Comm_way = RS232_Link;	// SYS_Link RS232_Link
	switch (pack.Prot_W_MID)
	{
	case 0:
		GX_app_send_packet(pack);
		break;
	case config_reader_GPO_state_order:
		temp_num = pack.p_Data[temp_run++];
		sys_set_gpo_fun (0,0);
		pack.dSize = 1;
		pack.p_Data[0] = 0;
		pack.Comm_way = g_SYS_Config.Connect_passage;		// RS232_Link SYS_Link
		GX_app_send_packet(pack);
		break;
	case beep_device_control_order:
	{
		temp_num = pack.p_Data[temp_run++];
		if (temp_num == 0)
		{
			temp_num = 0;
		}
		else
		{
			temp_num = pack.p_Data[temp_run++];
			if (temp_num == 0)
			{
				temp_num = 2;		// once
				Caven_trigger_event_Fun(&g_SYS_events,bzz_event,1);
			}
			else
			{
				temp_num = 1;
			}
		}
		sys_set_bzz_fun (temp_num);
		pack.dSize = 1;
		pack.p_Data[0] = 0;
		pack.Comm_way = g_SYS_Config.Connect_passage;
		GX_app_send_packet(pack);
	}
		break;
	default:
		GX_app_send_packet(pack);
		break;
	}
    return retval;
}

int GX_app_cmd2_handle (GX_info_packet_Type pack)
{
    int retval = 0;
	pack.Comm_way = RS232_Link;
    switch (pack.Prot_W_MID)
    {
    case 0:
        GX_app_send_packet(pack);
        break;
    
    default:
		GX_app_send_packet(pack);
        break;
    }
    return retval;
}

int GX_app_cmd3_handle (GX_info_packet_Type pack)
{
    int retval = 0;
	pack.Comm_way = RS232_Link;
    switch (pack.Prot_W_MID)
    {
    case 0:
        GX_app_send_packet(pack);
        break;
    
    default:
		GX_app_send_packet(pack);
        break;
    }
    return retval;
}

/*
	无需重组的邪修做法
*/
int GX_app_send_array(GX_info_packet_Type pack)
{
    int retval = 0;
    switch (pack.Comm_way)
    {
    case SYS_Link:
        {
			MODE_UART_DMA_Send_Data_Fun(DEBUG_OUT,pack.p_AllData,pack.Get_num);
        }
        break;
    case RS232_Link:
        {
			MODE_UART_DMA_Send_Data_Fun(m_UART_CH2,pack.p_AllData,pack.Get_num);
        }
        break;
    case USB_Link:
        {
            Mode_Use.USB_HID.Send_Data(pack.p_AllData,pack.Get_num);
        }
        break;
    default:
        break;
    }
    return retval;
}

int GX_app_send_packet(GX_info_packet_Type pack)
{
    uint8_t temp_array[BUFF_MAX];
    int retval = 0;
    int temp_num = 0;
    temp_num = GX_info_Split_packet_Fun(pack,temp_array);
	if(temp_num <= 0)
	{
		temp_num = 0;
	}
    switch (pack.Comm_way)
    {
    case SYS_Link:
        {
			MODE_UART_DMA_Send_Data_Fun(DEBUG_OUT,temp_array,temp_num);
//            Mode_Use.UART.Send_Data_pFun(m_UART_CH1,temp_array,temp_num);
//            debug_log (LOG_Info,Log_tag,"sys link send");
//            debug_log_hex (temp_array,temp_num);
        }
        break;
    case RS232_Link:
        {
			MODE_UART_DMA_Send_Data_Fun(m_UART_CH2,temp_array,temp_num);	//MODE_UART_DMA_Send_Data_Fun
//            Mode_Use.UART.Send_Data_pFun(m_UART_CH2,temp_array,temp_num);
        }
        break;
    case USB_Link:
        {
            Mode_Use.USB_HID.Send_Data(temp_array,temp_num);
        }
        break;
    default:
        break;
    }
    return retval;
}

static Caven_BaseTIME_Type debug_time = {0},rs232_time = {0},rs485_time = {0},server_time = {0},other_time = {0};
/*
	retval > 0;		进行中
	retval = 0xff;	完整
	retval = -1;	队列满
	retval = -2;	地址不符
	retval = x;		其他
*/
int GX_app_Make_pack (uint8_t data,int way,Caven_BaseTIME_Type time)
{
    int retval = 0;
    int temp_num = 0;
    GX_info_packet_Type * temp_pack = NULL;
    switch (way)
    {
    case SYS_Link:
        {
            temp_pack = &GX_packet_debug;
            temp_num = time.SYS_Sec - debug_time.SYS_Sec;
			debug_time = time;
        }
        break;
    case RS232_Link:
        {
            temp_pack = &GX_packet_rs232;
            temp_num = time.SYS_Sec - rs232_time.SYS_Sec;
			rs232_time = time;
        }
        break;
    case RS485_Link:
        {
            temp_pack = &GX_packet_rs485;
            temp_num = time.SYS_Sec - rs485_time.SYS_Sec;
			rs485_time = time;
            if(temp_pack->Addr != g_SYS_Config.Addr)
            {
                temp_num = 0xee;
                retval = (-2);
            }
        }
        break;
    case TCP_Server_Link:
        {
            temp_pack = &GX_packet_server;
            temp_num = time.SYS_Sec - server_time.SYS_Sec;
			server_time = time;
        }
        break;
    default:
		{
            temp_pack = &GX_packet_other;
            temp_num = time.SYS_Sec - other_time.SYS_Sec;
			other_time = time;
		}
		break;
    }
    if (temp_num > 1)   // 去掉数据包
    {
        GX_info_packet_fast_clean_Fun(temp_pack);
    }
    if (temp_pack != NULL && retval == 0)
    {
        retval = GX_info_Make_packet_Fun(GX_standard, temp_pack, data);
        if (retval == 0xFF)
        {
            temp_pack->Comm_way = way;
            retval = GX_Circular_queue_input (*temp_pack,GX_packet_buff,GX_PACK_M);   // 入队 
            GX_info_packet_fast_clean_Fun(temp_pack);
			if(retval >= 0)
			{
				retval = 0xff;
			}
        }
        if (retval < 0)
        {
            GX_info_packet_fast_clean_Fun(temp_pack);
        }
    }
    return retval;
}

void GX_app_Init (void)
{
    int temp_run = 0;
    GX_info_packet_index_Fun(&GX_packet_debug, info_packet_array[temp_run++]);
    GX_info_packet_index_Fun(&GX_packet_rs232, info_packet_array[temp_run++]);
    GX_info_packet_index_Fun(&GX_packet_rs485, info_packet_array[temp_run++]);
    GX_info_packet_index_Fun(&GX_packet_server, info_packet_array[temp_run++]);
    GX_info_packet_index_Fun(&GX_packet_client, info_packet_array[temp_run++]);
    GX_info_packet_index_Fun(&GX_packet_http, info_packet_array[temp_run++]);
	GX_info_packet_index_Fun(&GX_packet_other, info_packet_array[temp_run++]);
    for (int i = 0; i < GX_PACK_M; i++)
    {
        GX_info_packet_index_Fun(&GX_packet_buff[i], info_packet_buff_array[i]);
		GX_info_packet_fast_clean_Fun(&GX_packet_buff[i]);
    }
}

void GX_app_Exit (void)
{

}
