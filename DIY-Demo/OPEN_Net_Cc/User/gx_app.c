#include "gx_app.h"

#define Log_tag "GX_app info"
#define GX_PACK_M 6
#define GX_TAG_MAX 100

static RFID_Tag_Type RFID_Tag_Buff[GX_TAG_MAX];
static int Tags_num = 0,Tags_run = 0;
static uint8_t info_packet_array[6][BUFF_MAX];
static uint8_t info_packet_buff_array[GX_PACK_M][BUFF_MAX];
static GX_info_packet_Type GX_packet_buff[GX_PACK_M];
static GX_info_packet_Type GX_packet_debug;
static GX_info_packet_Type GX_packet_rs232;
static GX_info_packet_Type GX_packet_USB;
static GX_info_packet_Type GX_packet_server;
static GX_info_packet_Type GX_packet_client;
static GX_info_packet_Type GX_packet_other;
//static GX_info_packet_Type GX_packet_mqtt;
//static GX_info_packet_Type GX_packet_udp;
//static GX_info_packet_Type GX_packet_BLE;
static GX_info_packet_Type GX_standard = {
	.Head = 0x5A,
    .Prot_W_Versions = 0x01,    // 版本
    .dSize = BUFF_MAX,		    // 最大长度
};
static uint8_t gx_temp_array[0x1000];

Caven_BaseTIME_Type GX_app_time;

int GX_app_SYS_info_handle (GX_info_packet_Type pack);
int GX_app_RFID_info_handle (GX_info_packet_Type pack);
void GX_tag_data_handle (GX_info_packet_Type pack);
void GX_tag_data_updata_http (void);

int GX_app_State_machine(Caven_BaseTIME_Type time)
{
	int retval = 0;
    GX_app_time = time;
    GX_info_packet_Type handle_pack;
	GX_info_packet_fast_clean_Fun(&handle_pack);
	GX_info_packet_index_Fun(&handle_pack, gx_temp_array);
    GX_Circular_queue_output (&handle_pack,GX_packet_buff,GX_PACK_M);     // 从队列中提取
    if (handle_pack.Result & m_Result_SUCC)
    {
        User_GPIO_set(1,1,0);       // info
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
	if(g_SYS_Config.tcp_http_enable)
	{
		GX_tag_data_updata_http ();
	}
	return retval;
}

int GX_app_cmd1_handle (GX_info_packet_Type pack);
int GX_app_cmd2_handle (GX_info_packet_Type pack);
int GX_app_cmd3_handle (GX_info_packet_Type pack);
int GX_app_send_packet(GX_info_packet_Type pack);

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

	pack.Comm_way = g_SYS_Config.temp_val->Connect_passage;
	switch (pack.Prot_W_Class)
	{
	case 1:
		retval = GX_app_send_packet(pack);
		break;
	case 2:
		{
			if(pack.Prot_W_DFlag && pack.Prot_W_MID == 0)	// tag data
			{
				User_GPIO_set(2,15,0);
				GX_tag_data_handle(pack);
			}
		}
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
	int temp_num = 0,temp_run = 0,temp_data = 0;
	char *str_num = NULL;
	uint8_t temp_array[500];
	pack.Comm_way = RS232_Link;	// SYS_Link RS232_Link
	switch (pack.Prot_W_MID)
	{
	case 0:
		GX_app_send_packet(pack);
		break;
	case query_reader_time_date_params_order:
	{
		GX_app_send_packet(pack);

		pack.dSize = 8;
		memset(pack.p_Data,0,pack.dSize);
		pack.Comm_way = g_SYS_Config.temp_val->Connect_passage;		// RS232_Link SYS_Link
		GX_app_send_packet(pack);
	}
		break;
	case config_reader_GPO_state_order:
	{
		GX_app_send_packet(pack);
		for(int i = 0;i < pack.dSize;)
		{
			temp_num = pack.p_Data[temp_run++];
			temp_data = pack.p_Data[temp_run++];
			line_gpo_set(temp_num,!temp_data);
			i += 2;
		}
		pack.dSize = 1;
		pack.p_Data[0] = 0;
		pack.Comm_way = g_SYS_Config.temp_val->Connect_passage;		// RS232_Link SYS_Link
		GX_app_send_packet(pack);
	}
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
				Caven_trigger_event_Fun(&g_SYS_events,sys_bzz_event,1);
			}
			else
			{
				temp_num = 1;
			}
		}
		sys_set_bzz_fun (temp_num);
		pack.dSize = 1;
		pack.p_Data[0] = 0;
		pack.Comm_way = g_SYS_Config.temp_val->Connect_passage;
		GX_app_send_packet(pack);
	}
		break;
#if NETWORK
	case config_reader_ip_address_params_order:
	{
		temp_num = pack.p_Data[temp_run++];
		if(temp_num)
		{
			g_SYS_Config.eth_mode = 0;		// static
		}
		else
		{
			g_SYS_Config.eth_mode = 1;
		}
		temp_num = pack.p_Data[temp_run++];
		if(temp_num == 0x01)	// ip
		{
			uint8_t temp_ip[4];
			memcpy(temp_ip,&pack.p_Data[temp_run],4);
			temp_run += 4;
			memset(g_SYS_Config.eth_ip_str,0,sizeof(g_SYS_Config.eth_ip_str));
			sprintf(g_SYS_Config.eth_ip_str,"%d.%d.%d.%d",temp_ip[0],temp_ip[1],temp_ip[2],temp_ip[3]);
			temp_num = pack.p_Data[temp_run++];
		}
		if(temp_num == 0x02)	// netmask
		{
			uint8_t temp_ip[4];
			memcpy(temp_ip,&pack.p_Data[temp_run],4);
			temp_run += 4;
			memset(g_SYS_Config.eth_netmask_str,0,sizeof(g_SYS_Config.eth_netmask_str));
			sprintf(g_SYS_Config.eth_netmask_str,"%d.%d.%d.%d",temp_ip[0],temp_ip[1],temp_ip[2],temp_ip[3]);
			temp_num = pack.p_Data[temp_run++];
		}
		if(temp_num == 0x03)	// gw
		{
			uint8_t temp_ip[4];
			memcpy(temp_ip,&pack.p_Data[temp_run],4);
			temp_run += 4;
			memset(g_SYS_Config.eth_gw_str,0,sizeof(g_SYS_Config.eth_gw_str));
			sprintf(g_SYS_Config.eth_gw_str,"%d.%d.%d.%d",temp_ip[0],temp_ip[1],temp_ip[2],temp_ip[3]);
			temp_num = pack.p_Data[temp_run++];
		}
		if(temp_num == 0x04)	// DNS1
		{
			uint8_t temp_ip[4];
			memcpy(temp_ip,&pack.p_Data[temp_run],4);
			temp_run += 4;
			memset(g_SYS_Config.eth_DNS1_str,0,sizeof(g_SYS_Config.eth_DNS1_str));
			sprintf(g_SYS_Config.eth_DNS1_str,"%d.%d.%d.%d",temp_ip[0],temp_ip[1],temp_ip[2],temp_ip[3]);
			temp_num = pack.p_Data[temp_run++];
		}
		if(temp_num == 0x05)	// DNS2
		{
			uint8_t temp_ip[4];
			memcpy(temp_ip,&pack.p_Data[temp_run],4);
			temp_run += 4;
			memset(g_SYS_Config.eth_DNS2_str,0,sizeof(g_SYS_Config.eth_DNS2_str));
			sprintf(g_SYS_Config.eth_DNS2_str,"%d.%d.%d.%d",temp_ip[0],temp_ip[1],temp_ip[2],temp_ip[3]);
			temp_num = pack.p_Data[temp_run++];
		}
		System_app_save_IPv4Cfg ();
		pack.dSize = 1;
		pack.p_Data[0] = 0;
		pack.Comm_way = g_SYS_Config.temp_val->Connect_passage;
		GX_app_send_packet(pack);
		g_SYS_Config.temp_val->Reset_falg = 1;
	}
	break;
	case query_reader_ip_address_params_order:
	{
		if (g_SYS_Config.eth_mode)
		{
			temp_array[temp_run++] = 0;
		}
		else
		{
			temp_array[temp_run++] = 1;
		}
		Caven_Str_To_ip (g_SYS_Config.eth_ip_str,&temp_array[temp_run],4);
		temp_run += 4;
		Caven_Str_To_ip (g_SYS_Config.eth_netmask_str,&temp_array[temp_run],4);
		temp_run += 4;
		Caven_Str_To_ip (g_SYS_Config.eth_gw_str,&temp_array[temp_run],4);
		temp_run += 4;
		Caven_Str_To_ip (g_SYS_Config.eth_DNS1_str,&temp_array[temp_run],4);
		temp_run += 4;
		Caven_Str_To_ip (g_SYS_Config.eth_DNS2_str,&temp_array[temp_run],4);
		temp_run += 4;
		memcpy(pack.p_Data,temp_array,temp_run);
		pack.dSize = temp_run;
		pack.Comm_way = g_SYS_Config.temp_val->Connect_passage;
		GX_app_send_packet(pack);
	}
	break;
	case query_reader_mac_address_params_order:
	{
		sys_get_mac_fun(temp_array);
		temp_run += sizeof(g_SYS_Config.MAC);
		pack.Result = 0;
		memcpy(pack.p_Data,temp_array,temp_run);
		pack.dSize = temp_run;
		pack.Comm_way = g_SYS_Config.temp_val->Connect_passage;
		GX_app_send_packet(pack);
	}
	break;
	case config_server_client_modle_params_order:
	{
		temp_num = pack.p_Data[temp_run++];
		if (temp_num)
		{
			g_SYS_Config.tcp_server_enable = 0;
			g_SYS_Config.tcp_client_enable = 1;
		}
		else
		{
			g_SYS_Config.tcp_server_enable = 1;
			g_SYS_Config.tcp_client_enable = 0;
		}
		temp_num = pack.p_Data[temp_run++];
		if (temp_num == 0x01)
		{
			temp_data = 0;
			temp_data = pack.p_Data[temp_run++];
			temp_data <<= 8;
			temp_data |= pack.p_Data[temp_run++];
			memset(g_SYS_Config.TCPServer_port,0,sizeof(g_SYS_Config.TCPServer_port));
			sprintf(g_SYS_Config.TCPServer_port,"%d",temp_data);
			temp_num = pack.p_Data[temp_run++];
		}
		if (temp_num == 0x02)
		{
			uint8_t temp_ip[4];
			memcpy(temp_ip,&pack.p_Data[temp_run],4);
			temp_run += 4;
			memset(g_SYS_Config.TCPClient_url,0,sizeof(g_SYS_Config.TCPClient_url));
			sprintf(g_SYS_Config.TCPClient_url,"%d.%d.%d.%d",temp_ip[0],temp_ip[1],temp_ip[2],temp_ip[3]);
			temp_num = pack.p_Data[temp_run++];
		}
		if (temp_num == 0x03)
		{
			temp_data = 0;
			temp_data = pack.p_Data[temp_run++];
			temp_data <<= 8;
			temp_data |= pack.p_Data[temp_run++];
			memset(temp_array,0,sizeof(temp_array));
			sprintf((char *)temp_array,":%d",temp_data);
			strcat(g_SYS_Config.TCPClient_url,(char *)temp_array);
		}
		System_app_save_TCPServer ();
		pack.dSize = 1;
		pack.p_Data[0] = 0;
		pack.Comm_way = g_SYS_Config.temp_val->Connect_passage;
		GX_app_send_packet(pack);
		g_SYS_Config.temp_val->Reset_falg = 1;
	}
	break;
	case query_server_client_modle_params_order:
	{
		if (g_SYS_Config.tcp_server_enable)
		{
			temp_array[temp_run++] = 0;
		}
		else
		{
			temp_array[temp_run++] = 1;
		}
		temp_data = atoi(g_SYS_Config.TCPServer_port);
		temp_array[temp_run++] = (temp_data >> 8) & 0xff;
		temp_array[temp_run++] = (temp_data >> 0) & 0xff;

		Caven_Str_To_ip (g_SYS_Config.TCPClient_url,&temp_array[temp_run],4);
		temp_run += 4;
		
		str_num = memstr(g_SYS_Config.TCPClient_url, ":",strlen(g_SYS_Config.TCPClient_url));
		temp_data = 0;
		if(str_num != NULL)
		{
			str_num += 1;
			temp_data = atoi(str_num);
		}
		temp_array[temp_run++] = (temp_data >> 8) & 0xff;
		temp_array[temp_run++] = (temp_data >> 0) & 0xff;

		pack.dSize = temp_run;
		memcpy(pack.p_Data,temp_array,temp_run);
		pack.Comm_way = g_SYS_Config.temp_val->Connect_passage;
		GX_app_send_packet(pack);
	}
	break;
	case config_reader_http_send_data_params_order:
	{
		temp_num = pack.p_Data[temp_run++];
		if (temp_num)
		{
			g_SYS_Config.tcp_http_enable = 1;
		}
		else
		{
			g_SYS_Config.tcp_http_enable = 0;
		}
		temp_data = pack.p_Data[temp_run++];	// cycle
		temp_data <<= 8;
		temp_data |= pack.p_Data[temp_run++];
		g_SYS_Config.HTTP_cycle = temp_data;
		temp_run ++;	// json
		temp_run ++;	// over time
		temp_run ++;
		temp_run ++;	// cache
		temp_num = pack.p_Data[temp_run++];	// url
		if(temp_num == 0x01)
		{
			temp_data = pack.p_Data[temp_run++];
			temp_data <<= 8;
			temp_data |= pack.p_Data[temp_run++];
			memset(g_SYS_Config.HTTP_url,0,sizeof(g_SYS_Config.HTTP_url));
			if(temp_data < sizeof(g_SYS_Config.HTTP_url))
			{
				memcpy(g_SYS_Config.HTTP_url,&pack.p_Data[temp_run],temp_data);
			}
		}
		System_app_save_HTTPCfg();
		pack.dSize = 1;
		pack.p_Data[0] = 0;
		pack.Comm_way = g_SYS_Config.temp_val->Connect_passage;
		GX_app_send_packet(pack);
		g_SYS_Config.temp_val->Reset_falg = 1;
	}
	break;
	case query_reader_http_send_data_params_order:
	{
		temp_array[temp_run++] = g_SYS_Config.tcp_http_enable;
		temp_array[temp_run++] = (g_SYS_Config.HTTP_cycle >> 8) & 0xff;
		temp_array[temp_run++] = (g_SYS_Config.HTTP_cycle >> 0) & 0xff;
		temp_array[temp_run++] = 1;		// json
		temp_array[temp_run++] = 0;
		temp_array[temp_run++] = 2;		// over time
		temp_array[temp_run++] = 2;		// cache
		temp_data = strlen(g_SYS_Config.HTTP_url);
		if(temp_data)
		{
			temp_array[temp_run++] = 0x01;
			temp_array[temp_run++] = (temp_data >> 8) & 0xff;
			temp_array[temp_run++] = (temp_data >> 0) & 0xff;
			memcpy(&temp_array[temp_run],g_SYS_Config.HTTP_url,temp_data);
			temp_run += temp_data;
		}
		pack.dSize = temp_run;
		memcpy(pack.p_Data,temp_array,pack.dSize);
		pack.Comm_way = g_SYS_Config.temp_val->Connect_passage;
		GX_app_send_packet(pack);
	}
	break;
#endif
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
    case RS232_Link:
        {
			MODE_UART_DMA_Send_Data_Fun(m_UART_CH2,temp_array,temp_num);
//            Mode_Use.UART.Send_Data_pFun(m_UART_CH2,temp_array,temp_num);
        }
        break;
    case RS485_Link:
        {
            
        }
        break;
    case TCP_Server_Link:
        {
    #if NETWORK == 1
            Base_TCP_Server_Send (temp_array,temp_num);
    #endif
        }
        break;
    case TCP_Client_Link:
        {
    #if NETWORK == 1
            Base_TCP_Client_Send (temp_array,temp_num);
    #endif
        }
        break;
    case USB_Link:
        {
    #if Exist_USB
        Mode_Use.USB_HID.Send_Data_pFun(temp_array,temp_num);
    #endif
        }
        break;
    case Other_Link:
        {
            Mode_Use.UART.Send_Data_pFun(m_UART_CH3,temp_array,temp_num);
        }
        break;
    default:
		{
			MODE_UART_DMA_Send_Data_Fun(m_UART_CH1,temp_array,temp_num);
		}
        break;
    }
    return retval;
}

static Caven_BaseTIME_Type debug_time = {0},rs232_time = {0},USB_time = {0},server_time = {0},client_time = {0},other_time = {0};
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
    case TCP_Server_Link:
        {
            temp_pack = &GX_packet_server;
            temp_num = time.SYS_Sec - server_time.SYS_Sec;
			server_time = time;
        }
        break;
    case TCP_Client_Link:
        {
            temp_pack = &GX_packet_client;
            temp_num = time.SYS_Sec - client_time.SYS_Sec;
			client_time = time;
        }
        break;
    case USB_Link:
        {
            temp_pack = &GX_packet_USB;
            temp_num = time.SYS_Sec - USB_time.SYS_Sec;
			USB_time = time;
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

void GX_tag_data_handle (GX_info_packet_Type pack)
{
	RFID_Tag_Type temp_tag;
	int temp_num = 0,temp_data = 0,temp_run = 0,temp_check = 0;
	if(pack.p_Data != NULL)
	{
		memset(&temp_tag,0,sizeof(temp_tag));
		temp_num = pack.p_Data[temp_run++];
		temp_num <<= 8;
		temp_num |= pack.p_Data[temp_run++];
		temp_tag.epc_len = MIN(temp_num,sizeof(temp_tag.epc));
		memcpy(temp_tag.epc,&pack.p_Data[temp_run],temp_tag.epc_len);
		temp_run += temp_num;
		temp_run += 2; // PC
		temp_tag.ant = pack.p_Data[temp_run++]; // ANT
		temp_num = pack.p_Data[temp_run++];
		if (temp_num == 1) 		// rssi
		{
			temp_tag.RSSI = pack.p_Data[temp_run++];
			temp_num = pack.p_Data[temp_run++];
		}
		if (temp_num == 2) 		// Result
		{
			temp_run ++;
			temp_num = pack.p_Data[temp_run++];
		}
		if (temp_num == 3) 		// tid
		{
			temp_num = pack.p_Data[temp_run++];
			temp_num <<= 8;
			temp_num |= pack.p_Data[temp_run++];
			temp_tag.tid_len = MIN(temp_num,sizeof(temp_tag.tid));
			memcpy(temp_tag.tid,&pack.p_Data[temp_run],temp_tag.tid_len);
			temp_run += temp_num;
		}
		if(temp_tag.tid_len)
		{
			temp_data = Encrypt_XMODEM_CRC16_Fun(temp_tag.tid,temp_tag.tid_len);
		}
		else
		{
			temp_data = Encrypt_XMODEM_CRC16_Fun(temp_tag.epc,temp_tag.epc_len);
		}
		temp_tag.tag_crc = temp_data & 0xffff;
		if(Tags_num < GX_TAG_MAX)
		{
			for(int i = 0; i < Tags_num; i++)
			{
				if(temp_tag.tag_crc == RFID_Tag_Buff[i].tag_crc)
				{
					if(temp_tag.epc_len == RFID_Tag_Buff[i].epc_len)
					{
						temp_check = i+1;
						break;
					}
				}
			}
			if(temp_check == 0)
			{
				memcpy(&RFID_Tag_Buff[Tags_num],&temp_tag,sizeof(temp_tag));
				Tags_num ++;
			}
			else if(temp_tag.tid_len > 0)
			{
				temp_data = memcmp(RFID_Tag_Buff[temp_check - 1].tid,temp_tag.tid,temp_tag.tid_len);
				if(temp_data != 0)
				{
					memcpy(&RFID_Tag_Buff[Tags_num],&temp_tag,sizeof(temp_tag));
					Tags_num ++;
				}
			}
			if(Tags_num >= GX_TAG_MAX)
			{
				Tags_num = GX_TAG_MAX >> 1;
			}
		}
	}
}

void GX_tag_data_updata_http (void)
{
	static Task_Overtime_Type http_tag = 
	{
		.Trigger_Flag = 0,
		.Begin_time = {0},
	};
	RFID_Tag_Type *temp_tag;;
	char *p_temp = (char *)gx_temp_array;;
	char temp_array[300],temp_buf[50];
	int temp_num = 0,temp_data = 0;
	if((Tags_num != Tags_run) && g_SYS_Config.temp_val->Net_falg)
	{
		http_tag.Switch = 1;
		http_tag.Set_time.SYS_Us = 0;
		http_tag.Set_time.SYS_Sec = g_SYS_Config.HTTP_cycle;
		API_Task_Timer(&http_tag,GX_app_time);	// 接管http_tag，由此函数填充

		if(http_tag.Trigger_Flag)
		{
			http_tag.Trigger_Flag = 0;
			temp_num = MIN(10, Tags_num - Tags_run);
			memset(p_temp,0,10);

			strcat(p_temp,"{\r\n");
			sprintf(temp_array,"\"deviceSerial\":\"%s\",\r\n","123");
			strcat(p_temp,temp_array);
			sprintf(temp_array,"\"size\":\"%d\",\r\n",temp_num);
			strcat(p_temp,temp_array);
			sprintf(temp_array,"\"reportTime\":\"%d\",\r\n",GX_app_time.SYS_Sec);
			strcat(p_temp,temp_array);
			strcat(p_temp,"\"tags\":[\r\n");
			for(int i = 0; i < temp_num;)
			{
				temp_tag = &RFID_Tag_Buff[Tags_run+i];
				i++;
				strcat(p_temp,"\t{\r\n");
				sprintf(temp_array,"\t\t\"ant\":\"%d\",\r\n",temp_tag->ant);
				strcat(p_temp,temp_array);
				sprintf(temp_array,"\t\t\"rssi\":\"%d\",\r\n",temp_tag->RSSI);
				strcat(p_temp,temp_array);
				temp_data = Caven_Hex_To_String (temp_tag->epc,temp_tag->epc_len,temp_buf);
				if(temp_data)
				{
					sprintf(temp_array,"\t\t\"epc\":\"%s\",\r\n",temp_buf);
					strcat(p_temp,temp_array);
				}
				temp_data = Caven_Hex_To_String (temp_tag->tid,temp_tag->tid_len,temp_buf);
				if(temp_data)
				{
					sprintf(temp_array,"\t\t\"tid\":\"%s\",\r\n",temp_buf);
					strcat(p_temp,temp_array);
				}
				sprintf(temp_array,"\t\t\"cacheID\":\"%d\"\r\n",Tags_run+i);
				strcat(p_temp,temp_array);
				strcat(p_temp,"\t},\r\n");
			}
			strcat(p_temp,"\t{\"tag\":\"over\"}\r\n");
			strcat(p_temp,"\t]\r\n");
			strcat(p_temp,"}\r\n");
			temp_data = strlen(p_temp);
			temp_data = Base_TCP_HTTP_cache_Send_Fun(p_temp,temp_data);
			if(temp_data)
			{
				Tags_run += temp_num;
			}
			// Debug_OutStr(p_temp);
		}
	}
	else if(Tags_num == Tags_run)
	{
		Tags_run = 0;
		Tags_num = 0;
	}
}

void GX_app_Init (void)
{
    int temp_run = 0;
    GX_info_packet_index_Fun(&GX_packet_debug, info_packet_array[temp_run++]);
    GX_info_packet_index_Fun(&GX_packet_rs232, info_packet_array[temp_run++]);
    GX_info_packet_index_Fun(&GX_packet_USB, info_packet_array[temp_run++]);
    GX_info_packet_index_Fun(&GX_packet_server, info_packet_array[temp_run++]);
    GX_info_packet_index_Fun(&GX_packet_client, info_packet_array[temp_run++]);
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
