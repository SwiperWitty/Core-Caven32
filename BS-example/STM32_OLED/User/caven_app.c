#include "caven_app.h"
#include "zhiwen_app.h"

/*
0.1.0.2				2025.12.26
    1)支持gpio操作
    2)支持指纹操作

*/
#define Log_tag "Caven_app info"
#define CAVEN_PACK_M	6
static uint8_t info_packet_array[10][BUFF_MAX];
static uint8_t info_packet_buff_array[CAVEN_PACK_M][BUFF_MAX];
static Caven_info_packet_Type Caven_packet_buff[CAVEN_PACK_M];
static Caven_info_packet_Type Caven_packet_debug;
static Caven_info_packet_Type Caven_packet_rs232;
static Caven_info_packet_Type Caven_packet_rs485;
static Caven_info_packet_Type Caven_packet_net4g;
static Caven_info_packet_Type Caven_packet_server;
static Caven_info_packet_Type Caven_packet_client;
static Caven_info_packet_Type Caven_packet_http;
static Caven_info_packet_Type Caven_packet_mqtt;
static Caven_info_packet_Type Caven_packet_udp;
static Caven_info_packet_Type Caven_packet_Other;
static Caven_info_packet_Type Caven_standard = {
	.Head = 0xFA55,
    .Versions = 0x01,		// 版本
	.Type = 1,
    .dSize = BUFF_MAX,		// 最大长度
};

Caven_BaseTIME_Type Caven_app_time;

int Caven_app_cmd1_handle (Caven_info_packet_Type pack);
int Caven_app_cmd2_handle (Caven_info_packet_Type pack);
#if SYS_BTLD == 0
int Caven_app_cmd3_handle (Caven_info_packet_Type pack);
#endif
int Caven_app_send_packet (Caven_info_packet_Type pack);

int Caven_app_State_machine(Caven_BaseTIME_Type time)
{
	int retval = 0;
	uint8_t temp_array[BUFF_MAX];
    Caven_app_time = time;
    Caven_info_packet_Type handle_pack;
	Caven_info_packet_fast_clean_Fun(&handle_pack);
	Caven_info_packet_index_Fun(&handle_pack, temp_array);
    Caven_Circular_queue_output (&handle_pack,Caven_packet_buff,CAVEN_PACK_M);     // 从队列中提取
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
#if SYS_BTLD == 0
                case 3:
                    retval = Caven_app_cmd3_handle (handle_pack);
                    break;
#endif
                default:		// 不支持的CMD
					{
						handle_pack.Result = m_Result_Fail_CMD;
						handle_pack.dSize = 0;
						retval = Caven_app_send_packet(handle_pack);
					}
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
#if SYS_BTLD == 0
                case 3:
                    retval = Caven_app_cmd3_handle (handle_pack);
                    break;
#endif
                default:		// 不支持的CMD
					{
						handle_pack.Result = m_Result_Fail_CMD;
						handle_pack.dSize = 0;
						retval = Caven_app_send_packet(handle_pack);
					}
					break;
                }
            }
            break;
        }
        
    }
	return retval;
}

/*
retval = 0，不做返回
retval = 1，返回消息
retval = 2，完全返回
*/
int Caven_app_cmd1_handle (Caven_info_packet_Type pack)
{
    int retval = 0;		// 0 不回复，1 回复，2 完全转发
    int Result = 0,temp_num = 0,temp_run = 0,temp_val = 0;
    uint8_t rw_info = 0;
    uint8_t temp_array[100];
	if (pack.p_AllData == NULL || pack.p_Data == NULL)
	{
		return retval = -1;
	}
	if ((pack.Result & 0x0f) == 0x0f)
	{
		Result = 1;
	}
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
                pack.Result = m_Result_Fail_ERROR;
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
                temp_array[temp_run++] = (g_SYS_Config.Serial >> ( 8 * 7)) & 0xFF;
                temp_array[temp_run++] = (g_SYS_Config.Serial >> ( 8 * 6)) & 0xFF;
                temp_array[temp_run++] = (g_SYS_Config.Serial >> ( 8 * 5)) & 0xFF;
                temp_array[temp_run++] = (g_SYS_Config.Serial >> ( 8 * 4)) & 0xFF;
                temp_array[temp_run++] = (g_SYS_Config.Serial >> ( 8 * 3)) & 0xFF;
                temp_array[temp_run++] = (g_SYS_Config.Serial >> ( 8 * 2)) & 0xFF;
                temp_array[temp_run++] = (g_SYS_Config.Serial >> ( 8 * 1)) & 0xFF;
                temp_array[temp_run++] = (g_SYS_Config.Serial >> ( 8 * 0)) & 0xFF;
                pack.dSize = temp_run;
                pack.Result = 0;
                memcpy(pack.p_Data,temp_array,temp_run);
            }
            else
            {
                pack.Result = m_Result_Fail_ERROR;
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
                pack.Result = m_Result_Fail_ERROR;
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
				g_SYS_Config.Now_time = Mode_Use.TIME.Get_BaseTIME_pFun();
                temp_array[temp_run++] = (g_SYS_Config.Now_time.SYS_Sec >> ( 8 * 3)) & 0xFF;
                temp_array[temp_run++] = (g_SYS_Config.Now_time.SYS_Sec >> ( 8 * 2)) & 0xFF;
                temp_array[temp_run++] = (g_SYS_Config.Now_time.SYS_Sec >> ( 8 * 1)) & 0xFF;
                temp_array[temp_run++] = (g_SYS_Config.Now_time.SYS_Sec >> ( 8 * 0)) & 0xFF;
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
				Mode_Use.TIME.Set_BaseTIME_pFun(g_SYS_Config.Now_time);
                pack.Result = 0;
				pack.p_Data[temp_run++] = 0;
            }
			pack.dSize = temp_run;
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
                    pack.Result = m_Result_Fail_ERROR;
                }
                else
                {
                    g_SYS_Config.Addr = temp_val;
                    pack.Result = 0;
					pack.p_Data[temp_run++] = 0;
                }
				pack.dSize = temp_run;
            }
            retval = 1;
        }
        break;
    case m_CAVEN_CMD1_RS232Cfg_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            if(rw_info == 0)
            {
                temp_array[temp_run++] = (g_SYS_Config.RS232_UART_Cfg >> ( 8 * 3)) & 0xFF;
                temp_array[temp_run++] = (g_SYS_Config.RS232_UART_Cfg >> ( 8 * 2)) & 0xFF;
                temp_array[temp_run++] = (g_SYS_Config.RS232_UART_Cfg >> ( 8 * 1)) & 0xFF;
                temp_array[temp_run++] = (g_SYS_Config.RS232_UART_Cfg >> ( 8 * 0)) & 0xFF;
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
                g_SYS_Config.RS232_UART_Cfg = temp_val;
				
                pack.Result = 0;
				pack.p_Data[temp_run++] = 0;
            }
			pack.dSize = temp_run;
            retval = 1;
        }
        break;
    case m_CAVEN_CMD1_RS485Cfg_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            if(rw_info == 0)
            {
                temp_array[temp_run++] = (g_SYS_Config.RS485_UART_Cfg >> ( 8 * 3)) & 0xFF;
                temp_array[temp_run++] = (g_SYS_Config.RS485_UART_Cfg >> ( 8 * 2)) & 0xFF;
                temp_array[temp_run++] = (g_SYS_Config.RS485_UART_Cfg >> ( 8 * 1)) & 0xFF;
                temp_array[temp_run++] = (g_SYS_Config.RS485_UART_Cfg >> ( 8 * 0)) & 0xFF;
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
                g_SYS_Config.RS485_UART_Cfg = temp_val;
				
                pack.Result = 0;
				pack.p_Data[temp_run++] = 0;
            }
			pack.dSize = temp_run;
            retval = 1;
        }
        break;
    case m_CAVEN_CMD1_Reset_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            if(rw_info == 1)
            {
                g_SYS_Config.Reset_falg = 1;
				pack.Result = 0;
				pack.p_Data[temp_run++] = 0;
            }
            else
            {
                pack.Result = m_Result_Fail_ERROR;
            }
			pack.dSize = temp_run;
            retval = 1;
        }
        break;
    case m_CAVEN_CMD1_GetWork_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            if(rw_info == 0)
            {
                pack.p_Data[temp_run++] = g_SYS_Config.Work_falg & 0xff;
				pack.Result = 0;
            }
            else
            {
                pack.Result = m_Result_Fail_ERROR;
            }
			pack.dSize = temp_run;
            retval = 1;
        }
        break;
    case m_CAVEN_CMD1_GetNet_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            if(rw_info == 0)
            {
                pack.p_Data[temp_run++] = g_SYS_Config.Net_falg & 0xff;
				pack.Result = 0;
            }
            else
            {
                pack.Result = m_Result_Fail_ERROR;
            }
			pack.dSize = temp_run;
            retval = 1;
        }
        break;
    case m_CAVEN_CMD1_Worktime_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            if(rw_info == 0)
            {
				temp_array[temp_run++] = (g_SYS_Config.Work_sec >> ( 8 * 3)) & 0xFF;
                temp_array[temp_run++] = (g_SYS_Config.Work_sec >> ( 8 * 2)) & 0xFF;
                temp_array[temp_run++] = (g_SYS_Config.Work_sec >> ( 8 * 1)) & 0xFF;
                temp_array[temp_run++] = (g_SYS_Config.Work_sec >> ( 8 * 0)) & 0xFF;
				memcpy(pack.p_Data,temp_array,temp_run);
				pack.Result = 0;
            }
            else
            {
                pack.Result = m_Result_Fail_ERROR;
            }
			pack.dSize = temp_run;
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
				pack.p_Data[0] = 0;
                pack.Result = 0;
                pack.dSize = 1;
            }
            retval = 1;
        }
        break;
    case m_CAVEN_CMD1_IPv4Cfg_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            if(rw_info == 0)
            {
				if(g_SYS_Config.eth_mode)
				{
					memcpy(&temp_array[temp_run],"mode[static]",strlen("mode[static]"));
					temp_run += strlen("mode[static]");
				}
				else
				{
					memcpy(&temp_array[temp_run],"mode[dhcp]",strlen("mode[dhcp]"));
					temp_run += strlen("mode[dhcp]");
				}
                memcpy(&temp_array[temp_run],"ip[",3);
				temp_run += 3;
                temp_val = strlen(g_SYS_Config.eth_ip_str);
                memcpy(&temp_array[temp_run],g_SYS_Config.eth_ip_str,temp_val);
                temp_run += temp_val;
                memcpy(&temp_array[temp_run],"]",1);
				temp_run += 1;
                //
                memcpy(&temp_array[temp_run],"gw[",3);
				temp_run += 3;
                temp_val = strlen(g_SYS_Config.eth_gw_str);
                memcpy(&temp_array[temp_run],g_SYS_Config.eth_gw_str,temp_val);
                temp_run += temp_val;
                memcpy(&temp_array[temp_run],"]",1);
				temp_run += 1;
                //
                memcpy(&temp_array[temp_run],"netmask[",strlen("netmask["));
				temp_run += strlen("netmask[");
                temp_val = strlen(g_SYS_Config.eth_netmask_str);
                memcpy(&temp_array[temp_run],g_SYS_Config.eth_netmask_str,temp_val);
                temp_run += temp_val;
                memcpy(&temp_array[temp_run],"]",1);
				temp_run += 1;
                //
                memcpy(&temp_array[temp_run],"DNS1[",strlen("DNS1["));
				temp_run += strlen("DNS1[");
                temp_val = strlen(g_SYS_Config.eth_DNS1_str);
                memcpy(&temp_array[temp_run],g_SYS_Config.eth_DNS1_str,temp_val);
                temp_run += temp_val;
                memcpy(&temp_array[temp_run],"]",1);
				temp_run += 1;
                //
                memcpy(&temp_array[temp_run],"DNS2[",strlen("DNS2["));
				temp_run += strlen("DNS2[");
                temp_val = strlen(g_SYS_Config.eth_DNS2_str);
                memcpy(&temp_array[temp_run],g_SYS_Config.eth_DNS2_str,temp_val);
                temp_run += temp_val;
                memcpy(&temp_array[temp_run],"]",1);
				temp_run += 1;
				//
                pack.dSize = temp_run;
                pack.Result = 0;
                memcpy(pack.p_Data,temp_array,temp_run);
            }
            else
            {
//                g_SYS_Config.HTTPHBT_En = pack.p_Data[temp_num++];
				pack.p_Data[0] = 0;
                pack.Result = 0;
                pack.dSize = 1;
            }
            retval = 1;
        }
        break;
    case m_CAVEN_CMD1_TCPServer_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            if(rw_info == 0)
            {
				if(g_SYS_Config.tcp_server_enable)
				{
					memcpy(&temp_array[temp_run],"server[on]",strlen("server[on]"));
					temp_run += strlen("server[on]");
				}
				else
				{
					memcpy(&temp_array[temp_run],"server[off]",strlen("server[off]"));
					temp_run += strlen("server[off]");
				}
                memcpy(&temp_array[temp_run],"port[",strlen("port["));
				temp_run += strlen("port[");
                temp_val = strlen(g_SYS_Config.TCPServer);
                memcpy(&temp_array[temp_run],g_SYS_Config.TCPServer,temp_val);
                temp_run += temp_val;
                memcpy(&temp_array[temp_run],"]",1);
				temp_run += 1;
                //
				pack.dSize = temp_run;
                pack.Result = 0;
                memcpy(pack.p_Data,temp_array,temp_run);
            }
            else
            {
                
				pack.p_Data[0] = 0;
                pack.Result = 0;
                pack.dSize = 1;
            }
            retval = 1;
        }
        break;
    case m_CAVEN_CMD1_TCPClient_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            if(rw_info == 0)
            {
				if(g_SYS_Config.tcp_client_enable)
				{
					memcpy(&temp_array[temp_run],"client[on]",strlen("client[on]"));
					temp_run += strlen("client[on]");
				}
				else
				{
					memcpy(&temp_array[temp_run],"client[off]",strlen("client[off]"));
					temp_run += strlen("client[off]");
				}
                memcpy(&temp_array[temp_run],"url[",strlen("url["));
				temp_run += strlen("url[");
                temp_val = strlen(g_SYS_Config.TCPClient);
                memcpy(&temp_array[temp_run],g_SYS_Config.TCPClient,temp_val);
                temp_run += temp_val;
                memcpy(&temp_array[temp_run],"]",1);
				temp_run += 1;
                //
				pack.dSize = temp_run;
                pack.Result = 0;
                memcpy(pack.p_Data,temp_array,temp_run);
            }
            else
            {
				pack.p_Data[0] = 0;
                pack.Result = 0;
                pack.dSize = 1;
            }
            retval = 1;
        }
        break;
    case m_CAVEN_CMD1_GetMAC_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            if(rw_info == 0)
            {
				memcpy(temp_array,g_SYS_Config.MAC,sizeof(g_SYS_Config.MAC));
				temp_run += sizeof(g_SYS_Config.MAC);
                pack.Result = 0;
                memcpy(pack.p_Data,temp_array,temp_run);
            }
            else
            {
                pack.Result = m_Result_Fail_ERROR;
            }
			pack.dSize = temp_run;
            retval = 1;
        }
        break;
#endif
    default:
		{
			pack.Result = m_Result_Fail_CMDS;
			pack.dSize = temp_run;
			retval = 1;
		}
        break;
    }
	if(Result)			// 静默执行
	{
		retval = 0;
	}
	if (retval && (pack.Result == 0 || pack.Result == 4 || pack.Result == 5 || pack.Result == 6 || 		\
		pack.Result == m_Result_Fail_Spoil || pack.Result == m_Result_Fail_ERROR || pack.Result == m_Result_Back_Other))
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

/*
retval = 0，不做返回
retval = 1，返回消息
retval = 2，完全返回
*/
u32 BT_val = 0xff,BT_addr = 0;
int Caven_app_cmd2_handle (Caven_info_packet_Type pack)
{
    int retval = 0;
    int Result = 0,temp_num = 0,temp_run = 0,temp_sum = 0,temp_rt = 0;
	u32 temp_val = 0;
    uint8_t rw_info = 0;
    uint8_t temp_array[500];
	if (pack.p_AllData == NULL || pack.p_Data == NULL)
	{
		return retval = -1;
	}
	if ((pack.Result & 0x0f) == 0x0f)
	{
		Result = 1;
	}
    switch (pack.Cmd_sub)
    {
    case 0:
        Caven_app_send_packet(pack);
        break;
    case m_CAVEN_CMD2_BTLD_Order:
		{
			rw_info = pack.p_Data[temp_num++];
            if(rw_info == 0)
            {
		#if SYS_BTLD
				pack.p_Data[temp_run++] = 0;
		#else
				pack.p_Data[temp_run++] = 1;
		#endif
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
				
		#if SYS_BTLD == 0
			{
				if(temp_val == 0)
				{
					temp_rt = 0x00;		// 跳转到boot
					g_SYS_Config.Bt_mode = 0;
					System_app_SYS_Config_Save ();
					g_SYS_Config.Reset_falg = 1;
				}
				else
				{
					temp_rt = 0x02;		// 不符合条件
				}
				pack.p_Data[temp_run++] = (temp_val >> (8 * 3)) & 0xff;
				pack.p_Data[temp_run++] = (temp_val >> (8 * 2)) & 0xff;
				pack.p_Data[temp_run++] = (temp_val >> (8 * 1)) & 0xff;
				pack.p_Data[temp_run++] = (temp_val >> (8 * 0)) & 0xff;
				pack.p_Data[temp_run++] = temp_rt;
			}
		#else
			{
				if (temp_val == 0)
				{
					temp_rt = 0x00;
					BT_val = 0;
					g_SYS_Config.app_crc = 0;
					g_SYS_Config.app_crc = pack.p_Data[temp_num++];
					g_SYS_Config.app_crc <<= 8;
					g_SYS_Config.app_crc |= pack.p_Data[temp_num++];
					
					Base_Flash_Erase (SYS_APP_ADDR,SYS_APP_SIZE);
				}
				else if (temp_val == 0xFFFFFFFF)						// 完成
				{
					temp_num = BT_addr - SYS_APP_ADDR;
					if(temp_num > 0)
					{
						uint8_t * addr_p = (uint8_t *)SYS_APP_ADDR;
						temp_sum = Encrypt_XMODEM_CRC16_Fun(addr_p, temp_num);
						if(g_SYS_Config.app_crc == temp_sum)
						{
							g_SYS_Config.AppEnd_addr = BT_addr;
							temp_rt = 0x00;
							BT_val = 0;
							g_SYS_Config.Bt_mode = 1;
							System_app_SYS_Config_Save ();
							g_SYS_Config.Reset_falg = 1;
						}
						else
						{
							BT_val = 0;
							temp_rt = 0x01;
						}
					}
					else
					{
						BT_val = 0;
						temp_rt = 0x01;
					}
				}
				else if (BT_val == temp_val || (BT_val + 1) == temp_val)		// 正常情况
				{
					temp_sum = pack.dSize - temp_num;
					if (temp_sum > sizeof(temp_array))
					{
						temp_rt = 0x01;
					}
					else
					{
						memcpy(temp_array,&pack.p_Data[temp_num],temp_sum);
						BT_val = temp_val;
						// flash
						if (BT_addr < SYS_APP_ADDR)
						{
							BT_addr = SYS_APP_ADDR;
						}
						if (temp_sum)
						{
							temp_num = Base_Flash_Write (temp_array,BT_addr,temp_sum);
						}
						else
						{
							temp_num = 1;
						}
						if(temp_num == 0)
						{
							temp_rt = 0;
							BT_addr += temp_sum;
						}
						else
						{
							temp_rt = 1;
						}
					}
				}
				else
				{
					temp_rt = 0x02;
				}
				pack.p_Data[temp_run++] = (temp_val >> (8 * 3)) & 0xff;
				pack.p_Data[temp_run++] = (temp_val >> (8 * 2)) & 0xff;
				pack.p_Data[temp_run++] = (temp_val >> (8 * 1)) & 0xff;
				pack.p_Data[temp_run++] = (temp_val >> (8 * 0)) & 0xff;
				pack.p_Data[temp_run++] = temp_rt;
			}
		#endif
			}
			pack.dSize = temp_run;
			pack.Result = 0;
			retval = 1;
		}
        break;
    case m_CAVEN_CMD2_Reset_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            if(rw_info == 1)
            {
                g_SYS_Config.Reset_falg = 1;
				pack.Result = 0;
				pack.p_Data[temp_run++] = 0;
            }
            else
            {
                pack.Result = m_Result_Fail_ERROR;
            }
			pack.dSize = temp_run;
            retval = 1;
        }
        break;
    case m_CAVEN_CMD2_DEBUG_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            if(rw_info == 1)
            {
				pack.Result = 0;
				temp_val = pack.p_Data[temp_num++];
				if (temp_val > 0)
				{
					g_SYS_Config.debug = temp_val & 0xff;
					pack.p_Data[temp_run++] = 0;
					System_app_SYS_Config_Save ();
				}
				else
				{
					pack.p_Data[temp_run++] = 1;
				}
            }
            else
            {
                pack.Result = 0;
				pack.p_Data[temp_run++] = g_SYS_Config.debug;
            }
			pack.dSize = temp_run;
            retval = 1;
        }
        break;
    case m_CAVEN_CMD2_Restore_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            if(rw_info == 1)
            {
				// 重置，重启
				System_app_Restore ();
                g_SYS_Config.Reset_falg = 1;
				pack.Result = m_Result_Back_Succ;
				pack.p_Data[temp_run++] = 0;
            }
            else
            {
                pack.Result = m_Result_Fail_ERROR;
            }
			pack.dSize = temp_run;
            retval = 1;
        }
        break;
    case m_CAVEN_CMD2_Serial_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            if(rw_info == 0)
            {
                temp_array[temp_run++] = (g_SYS_Config.Serial >> ( 8 * 7)) & 0xFF;
                temp_array[temp_run++] = (g_SYS_Config.Serial >> ( 8 * 6)) & 0xFF;
                temp_array[temp_run++] = (g_SYS_Config.Serial >> ( 8 * 5)) & 0xFF;
                temp_array[temp_run++] = (g_SYS_Config.Serial >> ( 8 * 4)) & 0xFF;
                temp_array[temp_run++] = (g_SYS_Config.Serial >> ( 8 * 3)) & 0xFF;
                temp_array[temp_run++] = (g_SYS_Config.Serial >> ( 8 * 2)) & 0xFF;
                temp_array[temp_run++] = (g_SYS_Config.Serial >> ( 8 * 1)) & 0xFF;
                temp_array[temp_run++] = (g_SYS_Config.Serial >> ( 8 * 0)) & 0xFF;
                pack.dSize = temp_run;
                pack.Result = m_Result_Back_Succ;
                memcpy(pack.p_Data,temp_array,temp_run);
            }
            else
            {
				g_SYS_Config.Serial = pack.p_Data[temp_num++];
				g_SYS_Config.Serial <<= 8;
				g_SYS_Config.Serial |= pack.p_Data[temp_num++];
				g_SYS_Config.Serial <<= 8;
				g_SYS_Config.Serial |= pack.p_Data[temp_num++];
				g_SYS_Config.Serial <<= 8;
				g_SYS_Config.Serial |= pack.p_Data[temp_num++];
				g_SYS_Config.Serial <<= 8;
				g_SYS_Config.Serial |= pack.p_Data[temp_num++];
				g_SYS_Config.Serial <<= 8;
				g_SYS_Config.Serial |= pack.p_Data[temp_num++];
				g_SYS_Config.Serial <<= 8;
				g_SYS_Config.Serial |= pack.p_Data[temp_num++];
				g_SYS_Config.Serial <<= 8;
				g_SYS_Config.Serial |= pack.p_Data[temp_num++];
				
				pack.p_Data[temp_run++] = 0;
				pack.Result = m_Result_Back_Succ;
				System_app_SYS_Config_Save ();
            }
			pack.dSize = temp_run;
            retval = 1;
        }
        break;
#if NETWORK
    case m_CAVEN_CMD2_MACCfg_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            if(rw_info == 0)
            {
				memcpy(temp_array,g_SYS_Config.MAC,sizeof(g_SYS_Config.MAC));
				temp_run += sizeof(g_SYS_Config.MAC);
                pack.Result = m_Result_Back_Succ;
                memcpy(pack.p_Data,temp_array,temp_run);
            }
            else
            {
				memcpy(g_SYS_Config.MAC,&pack.p_Data[temp_num],sizeof(g_SYS_Config.MAC));
				pack.p_Data[temp_run++] = 0;
                pack.Result = m_Result_Back_Succ;
				System_app_SYS_Config_Save ();
            }
			pack.dSize = temp_run;
            retval = 1;
        }
        break;
#endif
    default:
		{
			pack.Result = m_Result_Fail_CMDS;
			pack.dSize = temp_run;
			retval = 1;
		}
        break;
    }
	if(Result)			// 静默执行
	{
		retval = 0;
	}
	if (retval && (pack.Result == 0 || pack.Result == 4 || pack.Result == 5 || pack.Result == 6 || 		\
		pack.Result == m_Result_Fail_Spoil || pack.Result == m_Result_Fail_ERROR || pack.Result == m_Result_Back_Other))
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

#if SYS_BTLD == 0
/*
retval = 0，不做返回
retval = 1，返回消息
retval = 2，完全返回
*/
int Caven_app_cmd3_handle (Caven_info_packet_Type pack)
{
    int retval = 0;
    int Result = 0,temp_num = 0,temp_run = 0,temp_data = 0,temp_data_sun;
	uint8_t rw_info = 0;
	if (pack.p_AllData == NULL || pack.p_Data == NULL)
	{
		return retval = -1;
	}
	if ((pack.Result & 0x0f) == 0x0f)
	{
		Result = 1;
	}
    switch (pack.Cmd_sub)
    {
    case 0:
        Caven_app_send_packet(pack);
        break;
	case m_CAVEN_CMD3_BEEP_Order:
	{
		rw_info = pack.p_Data[temp_num++];
		if(rw_info)
		{
			temp_data = pack.p_Data[temp_num++];
			// User_GPIO_set(1,12,temp_data);
			sys_set_bzz_fun (temp_data);
			pack.Result = m_Result_Back_Succ;
		}
		else
		{
			pack.Result = m_Result_Fail_ERROR;
		}
		pack.p_Data[temp_run++] = 0;
		pack.dSize = temp_run;
		retval = 1;
	}
	break;
	case m_CAVEN_CMD3_GPOCfg_Order:
	{
		rw_info = pack.p_Data[temp_num++];
		if(rw_info)
		{
			int temp_io = 0;
			temp_data_sun = pack.dSize - temp_num;
			for(int i = 0;i < temp_data_sun;)
			{
				temp_io = pack.p_Data[temp_num++];
				temp_data = pack.p_Data[temp_num++];
				line_gpo_set(temp_io,!temp_data);
				i += 2;
			}
			pack.Result = m_Result_Back_Succ;
		}
		else
		{
			pack.Result = m_Result_Fail_ERROR;
		}
		pack.p_Data[temp_run++] = 0;
		pack.dSize = temp_run;
		retval = 1;
	}
	break;
	case m_CAVEN_CMD3_GPIGet_Order:
        pack.Result = m_Result_Back_Succ;
		pack.p_Data[temp_run++] = 0;
		pack.p_Data[temp_run++] = 0;
		pack.dSize = temp_run;
		retval = 1;
        break;
    case m_CAVEN_CMD3_zhiwen_Order:
        temp_data = pack.p_Data[temp_num++];
		temp_run = pack.p_Data[temp_num++];
		temp_run <<= 8;
		temp_run |= pack.p_Data[temp_num++];
		temp_run <<= 8;
		temp_run |= pack.p_Data[temp_num++];
		temp_run <<= 8;
		temp_run |= pack.p_Data[temp_num++];
	
        zhiwen_app_mode (temp_data,1,temp_run);
        Result = 1;
		retval = 1;
        break;
    default:
		{
			pack.Result = m_Result_Fail_CMDS;
			pack.dSize = temp_run;
			retval = 1;
		}
        break;
    }
	if(Result)			// 静默执行
	{
		retval = 0;
	}
	if (retval && (pack.Result == 0 || pack.Result == 4 || pack.Result == 5 || pack.Result == 6 ||
		pack.Result == m_Result_Fail_Spoil || pack.Result == m_Result_Fail_ERROR || pack.Result == m_Result_Back_Other))
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
#endif

int Caven_app_send_packet(Caven_info_packet_Type pack)
{
    uint8_t temp_array[BUFF_MAX];
    int retval = 0;
    int temp_num = 0;
	if (pack.Addr == 0xff && pack.Comm_way == RS485_Link)		// 广播从机不回复
	{
		return retval;
	}
	memset(temp_array,0,sizeof(temp_array));
    temp_num = Caven_info_Split_packet_Fun(pack,temp_array);
    switch (pack.Comm_way)
    {
    case SYS_Link:
        {
            Mode_Use.UART.Send_Data_pFun(DEBUG_CH,temp_array,temp_num);
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
    case USB_Link:
        {
#if Exist_USB
            Mode_Use.USB_HID.Send_Data(temp_array,temp_num);
#endif
        }
        break;
    default:
        break;
    }
    return retval;
}

static Caven_BaseTIME_Type debug_time,rs232_time,rs485_time,server_time,client_time,net4g_time,udp_time,Other_time;
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
    case NET4G_Link:
        {
            temp_pack = &Caven_packet_net4g;
            temp_num = time.SYS_Sec - net4g_time.SYS_Sec;
			net4g_time = time;
        }
        break;
    case TCP_Server_Link:
        {
            temp_pack = &Caven_packet_server;
            temp_num = time.SYS_Sec - server_time.SYS_Sec;
			server_time = time;
        }
        break;
    case TCP_Client_Link:
        {
            temp_pack = &Caven_packet_client;
            temp_num = time.SYS_Sec - client_time.SYS_Sec;
			client_time = time;
        }
        break;
    case TCP_UDP_Link:
        {
            temp_pack = &Caven_packet_udp;
            temp_num = time.SYS_Sec - udp_time.SYS_Sec;
			udp_time = time;
        }
        break;
    default:
        {
            temp_pack = &Caven_packet_Other;
            temp_num = time.SYS_Sec - Other_time.SYS_Sec;
			Other_time = time;
        }
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
            Caven_Circular_queue_input (*temp_pack,Caven_packet_buff,CAVEN_PACK_M);   // 常规入队 
            Caven_info_packet_fast_clean_Fun(temp_pack);
        }
        else if (retval < 0)
        {
            Caven_info_packet_fast_clean_Fun(temp_pack);
        }
    }
    return retval;
}

/*
retval < 0 失败

*/
int Caven_app_JSON_Make_pack (char *data,int way)
{
	int retval = 0;
    int temp_len = 0;
	char *temp_str = NULL;
	char array[500];
    Caven_info_packet_Type * temp_pack = NULL;
	
	temp_len = strlen(data);
	temp_str = memstr(data, "\"Caven_pack\"",temp_len);
	if(temp_str != NULL)
	{
		memset(array,0,sizeof(array));
		temp_len = strlen(temp_str);
		switch (way)
		{
		case TCP_HTTP_Link:
			temp_pack = &Caven_packet_http;
			break;
		case TCP_MQTT_Link:
			temp_pack = &Caven_packet_mqtt;
			break;
		default:
			break;
		}
		if (temp_pack == NULL)
		{
			return retval;
		}
		if (temp_pack->p_AllData != NULL)
		{
			temp_pack->p_Data = temp_pack->p_AllData + 2 + 5 + 2;
		}
		if (memstr(temp_str, "\"cmd1\"",temp_len) != NULL)
		{
			temp_pack->Cmd = 1;
		}
		else if (memstr(temp_str, "\"cmd2\"",temp_len) != NULL)
		{
			temp_pack->Cmd = 2;
		}
		else if (memstr(temp_str, "\"cmd3\"",temp_len) != NULL)
		{
			temp_pack->Cmd = 3;
		}
		else if (memstr(temp_str, "\"cmd4\"",temp_len) != NULL)
		{
			temp_pack->Cmd = 4;
		}
		else
		{
			retval = -1;
		}
		if (retval >= 0)
		{
			retval = Caven_gain_str_by_sign(temp_str,temp_len,array,"\"cmd hub\": ",'\"');
		}
		temp_pack->Cmd_sub = atoi(array) & 0xFF;
		memset(array,0,sizeof(array));
		if (retval >= 0)
		{
			retval = Caven_gain_str_by_sign(temp_str,temp_len,array,"\"W/R\": ",'\"');
		}
		if(array[0] == 'W')
		{
			array[0] = 1;
		}
		else
		{
			array[0] = 0;
		}
		if (retval >= 0)
		{
			Caven_gain_str_by_sign(temp_str,temp_len,&array[1],"\"data\": ",'\"');
		}
		if (temp_pack->p_Data != NULL)
		{
			temp_len = strlen(&array[1]) + 1;
			memcpy(temp_pack->p_Data,array,temp_len);
		}
		if (retval >= 0)
		{
			temp_pack->Comm_way = way;
			temp_pack->dSize = temp_len;
			temp_pack->Get_num = temp_len + 2 + 5 + 2;
			temp_pack->Result = 0x50;
			temp_pack->Type = 0;
			temp_pack->Versions = 1;
			Caven_Circular_queue_input (*temp_pack,Caven_packet_buff,CAVEN_PACK_M);   // JSON入队 
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
	Caven_info_packet_index_Fun(&Caven_packet_Other, info_packet_array[temp_run++]);
    for (int i = 0; i < CAVEN_PACK_M; i++)
    {
        Caven_info_packet_index_Fun(&Caven_packet_buff[i], info_packet_buff_array[i]);
		Caven_info_packet_fast_clean_Fun(&Caven_packet_buff[i]);
    }
}

void Caven_app_Exit (void)
{

}
