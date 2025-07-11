#include "caven_app.h"

#define Log_tag "Caven_app info"

static uint8_t info_packet_array[10][BUFF_MAX];
static uint8_t info_packet_buff_array[6][BUFF_MAX];
static Caven_info_packet_Type Caven_packet_buff[6];
static Caven_info_packet_Type Caven_packet_debug;
static Caven_info_packet_Type Caven_packet_rs232;
static Caven_info_packet_Type Caven_packet_rs485;
static Caven_info_packet_Type Caven_packet_server;
static Caven_info_packet_Type Caven_packet_client;
static Caven_info_packet_Type Caven_packet_http;
static Caven_info_packet_Type Caven_packet_mqtt;
static Caven_info_packet_Type Caven_packet_udp;
static Caven_info_packet_Type Caven_packet_BLE;
static Caven_info_packet_Type Caven_standard = {
	.Head = 0xFA55,
    .Versions = 0x01,		// 版本
	.Type = 1,
    .dSize = BUFF_MAX,		// 最大长度
};

Caven_BaseTIME_Type Caven_app_time;

int Caven_app_cmd1_handle (Caven_info_packet_Type pack);
int Caven_app_cmd2_handle (Caven_info_packet_Type pack);
int Caven_app_cmd3_handle (Caven_info_packet_Type pack);
int Caven_app_send_packet(Caven_info_packet_Type pack);

int Caven_app_State_machine(Caven_BaseTIME_Type time)
{
	int retval = 0;
	uint8_t temp_array[BUFF_MAX];
    Caven_app_time = time;
    Caven_info_packet_Type handle_pack;
	Caven_info_packet_fast_clean_Fun(&handle_pack);
	Caven_info_packet_index_Fun(&handle_pack, temp_array);
    Caven_Circular_queue_output (&handle_pack,Caven_packet_buff,6);     // 从队列中提取
    if (handle_pack.Result & m_Result_SUCC)
    {
        switch (handle_pack.Comm_way)
        {
        case SYS_Link:
            {
                switch (handle_pack.Cmd)
                {
                case 1:
                    retval = Caven_app_cmd1_handle (handle_pack);
                    break;
                case 2:
                    retval = Caven_app_cmd2_handle (handle_pack);
                    break;
                case 3:
                    retval = Caven_app_cmd3_handle (handle_pack);
                    break;
                default:
                    retval = Caven_app_send_packet(handle_pack);
                    break;
                }
            }
            break;
        default:
            {
                switch (handle_pack.Cmd)
                {
                case 1:
                    retval = Caven_app_cmd1_handle (handle_pack);
                    break;
                case 2:
                    retval = Caven_app_cmd2_handle (handle_pack);
                    break;
                case 3:
                    retval = Caven_app_cmd3_handle (handle_pack);
                    break;
                default:
                    retval = Caven_app_send_packet(handle_pack);
                    break;
                }
            }
            break;
        }
        
    }
	return retval;
}

int Caven_app_cmd1_handle (Caven_info_packet_Type pack)
{
    int retval = 0;
    int temp_num = 0,temp_run = 0,temp_val = 0;
    uint8_t rw_info = 0;
    uint8_t temp_array[100];
    switch (pack.Cmd_sub)
    {
    case 0:
        retval = 2;
        break;
    case m_CAVEN_CMD1_Version_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            if(rw_info == 0)
            {
                temp_array[temp_run++] = DEMO_VER;
                temp_array[temp_run++] = DEMO_VER_sub;
                temp_array[temp_run++] = DEMO_VER_sub_bit;
                temp_array[temp_run++] = 0;
                pack.dSize = temp_run;
                pack.Result = 0;
                memcpy(pack.p_Data,temp_array,temp_run);
            }
            else
            {
                pack.Result = m_Result_Back_ERROR;
                pack.dSize = 0;
            }
            retval = 1;
        }
        break;
    case m_CAVEN_CMD1_Serial_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            if(rw_info == 0)
            {
                temp_array[temp_run++] = 0;
                temp_array[temp_run++] = 8;
                temp_array[temp_run++] = (DEMO_Serial >> ( 8 * 7)) & 0xFF;
                temp_array[temp_run++] = (DEMO_Serial >> ( 8 * 6)) & 0xFF;
                temp_array[temp_run++] = (DEMO_Serial >> ( 8 * 5)) & 0xFF;
                temp_array[temp_run++] = (DEMO_Serial >> ( 8 * 4)) & 0xFF;
                temp_array[temp_run++] = (DEMO_Serial >> ( 8 * 3)) & 0xFF;
                temp_array[temp_run++] = (DEMO_Serial >> ( 8 * 2)) & 0xFF;
                temp_array[temp_run++] = (DEMO_Serial >> ( 8 * 1)) & 0xFF;
                temp_array[temp_run++] = (DEMO_Serial >> ( 8 * 0)) & 0xFF;
                pack.dSize = temp_run;
                pack.Result = 0;
                memcpy(pack.p_Data,temp_array,temp_run);
            }
            else
            {
                pack.Result = m_Result_Back_ERROR;
                pack.dSize = 0;
            }
            retval = 1;
        }
        break;
    case m_CAVEN_CMD1_Bdtime_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            if(rw_info == 0)
            {
                temp_array[temp_run++] = (DEMO_Build_UTC >> ( 8 * 3)) & 0xFF;
                temp_array[temp_run++] = (DEMO_Build_UTC >> ( 8 * 2)) & 0xFF;
                temp_array[temp_run++] = (DEMO_Build_UTC >> ( 8 * 1)) & 0xFF;
                temp_array[temp_run++] = (DEMO_Build_UTC >> ( 8 * 0)) & 0xFF;
                pack.dSize = temp_run;
                pack.Result = 0;
                memcpy(pack.p_Data,temp_array,temp_run);
            }
            else
            {
                pack.Result = m_Result_Back_ERROR;
                pack.dSize = 0;
            }
            retval = 1;
        }
        break;
    case m_CAVEN_CMD1_UTCtime_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            if(rw_info == 0)
            {
                temp_array[temp_run++] = (g_SYS_Config.Now_time.SYS_Sec >> ( 8 * 3)) & 0xFF;
                temp_array[temp_run++] = (g_SYS_Config.Now_time.SYS_Sec >> ( 8 * 2)) & 0xFF;
                temp_array[temp_run++] = (g_SYS_Config.Now_time.SYS_Sec >> ( 8 * 1)) & 0xFF;
                temp_array[temp_run++] = (g_SYS_Config.Now_time.SYS_Sec >> ( 8 * 0)) & 0xFF;
                pack.dSize = temp_run;
                pack.Result = 0;
                memcpy(pack.p_Data,temp_array,temp_run);
            }
            else
            {
                temp_val = pack.p_Data[temp_num++];
                temp_val <<= 8;
                temp_val |= pack.p_Data[temp_num++];
                temp_val <<= 8;
                temp_val |= pack.p_Data[temp_num++];
                temp_val <<= 8;
                temp_val |= pack.p_Data[temp_num++];
                g_SYS_Config.Now_time.SYS_Sec = temp_val;
                g_SYS_Config.Now_time.SYS_Us = 0;
                pack.Result = 0;
                pack.dSize = 0;
            }
            retval = 1;
        }
        break;
    case m_CAVEN_CMD1_Addr_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            if(rw_info == 0)
            {
                temp_array[temp_run++] = g_SYS_Config.Addr;
                pack.dSize = temp_run;
                pack.Result = 0;
                memcpy(pack.p_Data,temp_array,temp_run);
            }
            else
            {
                temp_val = pack.p_Data[temp_num++];
                if (temp_val == 0 || temp_val == 0xFF)
                {
                    pack.Result = m_Result_Back_ERROR;
                }
                else
                {
                    g_SYS_Config.Addr = temp_val;
                    pack.Result = 0;
                }
                pack.dSize = 0;
            }
            retval = 1;
        }
        break;
#if NETWORK
    case m_CAVEN_CMD1_TCPHBT_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            if(rw_info == 0)
            {
                temp_array[temp_run++] = g_SYS_Config.TCPHBT_En;
                pack.dSize = temp_run;
                pack.Result = 0;
                memcpy(pack.p_Data,temp_array,temp_run);
            }
            else
            {
                g_SYS_Config.TCPHBT_En = pack.p_Data[temp_num++];
                pack.Result = 0;
                pack.dSize = 0;
            }
            retval = 1;
        }
        break;
    case m_CAVEN_CMD1_IPv4Cfg_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            if(rw_info == 0)
            {
                temp_array[temp_run++] = 0;
                temp_array[temp_run++] = 0;
                temp_val = strlen(g_SYS_Config.eth_ip_str);
                memcpy(&temp_array[temp_run],g_SYS_Config.eth_ip_str,temp_val);
                temp_run += temp_val;
                memcpy(&temp_array[temp_run],"|",1);
                temp_run += 1;
                //
                temp_val = strlen(g_SYS_Config.wifi_gw_str);
                memcpy(&temp_array[temp_run],g_SYS_Config.wifi_gw_str,temp_val);
                temp_run += temp_val;
                memcpy(&temp_array[temp_run],"|",1);
                temp_run += 1;
                //
                temp_val = strlen(g_SYS_Config.wifi_netmask_str);
                memcpy(&temp_array[temp_run],g_SYS_Config.wifi_netmask_str,temp_val);
                temp_run += temp_val;
                memcpy(&temp_array[temp_run],"|",1);
                temp_run += 1;
                //
                temp_val = strlen(g_SYS_Config.wifi_DNS1_str);
                memcpy(&temp_array[temp_run],g_SYS_Config.wifi_DNS1_str,temp_val);
                temp_run += temp_val;
                memcpy(&temp_array[temp_run],"|",1);
                temp_run += 1;
                //
                temp_val = strlen(g_SYS_Config.wifi_DNS2_str);
                memcpy(&temp_array[temp_run],g_SYS_Config.wifi_DNS2_str,temp_val);
                temp_run += temp_val;
                memcpy(&temp_array[temp_run],"|",1);
                temp_run += 1;
                temp_array[0] = (temp_run >> 8) & 0xff;
                temp_array[1] = (temp_run >> 0) & 0xff;
                pack.dSize = temp_run;
                pack.Result = 0;
                memcpy(pack.p_Data,temp_array,temp_run);
            }
            else
            {
                g_SYS_Config.HTTPHBT_En = pack.p_Data[temp_num++];
                pack.Result = 0;
                pack.dSize = 0;
            }
            retval = 1;
        }
        break;
#endif
    default:
        break;
    }
    if (retval && (pack.Result == 0 || pack.Result == 1 || pack.Result == 4 || pack.Result == 5 || pack.Result == 6 || pack.Result == 9 || pack.Result == 0x0A))
    {
        Caven_app_send_packet(pack);
    }
    else if (retval == 2)
    {
		Caven_app_send_packet(pack);
    }
	else
	{
		
	}
    return retval;
}

int Caven_app_cmd2_handle (Caven_info_packet_Type pack)
{
    int retval = 0;
    switch (pack.Cmd_sub)
    {
    case 0:
        Caven_app_send_packet(pack);
        break;
    
    default:
        break;
    }
    return retval;
}

int Caven_app_cmd3_handle (Caven_info_packet_Type pack)
{
    int retval = 0;
    switch (pack.Cmd_sub)
    {
    case 0:
        Caven_app_send_packet(pack);
        break;
    
    default:
        break;
    }
    return retval;
}

int Caven_app_send_packet(Caven_info_packet_Type pack)
{
    uint8_t temp_array[BUFF_MAX];
    int retval = 0;
    int temp_num = 0;
    temp_num = Caven_info_Split_packet_Fun(pack,temp_array);
    switch (pack.Comm_way)
    {
    case SYS_Link:
        {
            Mode_Use.UART.Send_Data_pFun(DEBUG_OUT,temp_array,temp_num);

//            debug_log (LOG_Info,Log_tag,"sys link send");
//            debug_log_hex (temp_array,temp_num);
        }
        break;
    case RS232_Link:
        {
            
        }
        break;
    case RS485_Link:
        {
            
        }
        break;
    case TCP_Server_Link:
        {
            
        }
        break;
    default:
        break;
    }
    return retval;
}

static Caven_BaseTIME_Type debug_time = {0},rs232_time = {0},rs485_time = {0},server_time = {0};
int Caven_app_Make_pack (uint8_t data,int way,Caven_BaseTIME_Type time)
{
    int retval = 0;
    int temp_num = 0;
    Caven_info_packet_Type * temp_pack = NULL;
    switch (way)
    {
    case SYS_Link:
        {
            temp_pack = &Caven_packet_debug;
            temp_num = time.SYS_Sec - debug_time.SYS_Sec;
			debug_time = time;
        }
        break;
    case RS232_Link:
        {
            temp_pack = &Caven_packet_rs232;
            temp_num = time.SYS_Sec - rs232_time.SYS_Sec;
			rs232_time = time;
        }
        break;
    case RS485_Link:
        {
            temp_pack = &Caven_packet_rs485;
            temp_num = time.SYS_Sec - rs485_time.SYS_Sec;
			rs485_time = time;
            if(temp_pack->Addr != g_SYS_Config.Addr)
            {
                temp_num = 0xee;
                retval = 0xee;
            }
        }
        break;
    case TCP_Server_Link:
        {
            temp_pack = &Caven_packet_server;
            temp_num = time.SYS_Sec - server_time.SYS_Sec;
			server_time = time;
        }
        break;
    default:
        break;
    }
    if (temp_num > 1)   // 去掉数据包 
    {
        Caven_info_packet_fast_clean_Fun(temp_pack);
    }
    if (temp_pack != NULL && retval == 0)
    {
        retval = Caven_info_Make_packet_Fun(Caven_standard, temp_pack, data);
        if (retval == 0xFF)
        {
            temp_pack->Comm_way = way;
            Caven_Circular_queue_input (*temp_pack,Caven_packet_buff,6);   // 入队 
            Caven_info_packet_fast_clean_Fun(temp_pack);
        }
        else if (retval < 0)
        {
            Caven_info_packet_fast_clean_Fun(temp_pack);
        }
    }
    return retval;
}

void Caven_app_Init (void)
{
    int temp_run = 0;
    Caven_info_packet_index_Fun(&Caven_packet_debug, info_packet_array[temp_run++]);
    Caven_info_packet_index_Fun(&Caven_packet_rs232, info_packet_array[temp_run++]);
    Caven_info_packet_index_Fun(&Caven_packet_rs485, info_packet_array[temp_run++]);
    Caven_info_packet_index_Fun(&Caven_packet_server, info_packet_array[temp_run++]);
    Caven_info_packet_index_Fun(&Caven_packet_client, info_packet_array[temp_run++]);
    Caven_info_packet_index_Fun(&Caven_packet_http, info_packet_array[temp_run++]);

    for (int i = 0; i < 6; i++)
    {
        Caven_info_packet_index_Fun(&Caven_packet_buff[i], info_packet_buff_array[i]);
		Caven_info_packet_fast_clean_Fun(&Caven_packet_buff[i]);
    }
}

void Caven_app_Exit (void)
{

}
