#include "caven_app.h"

/*
    系统外设数据保存
    支持网络修改保存，区分boot和app层权限修改 TCP SERVER CLIENT HTTP MQTT 
    全新 BOOTLD
    ————26.2.6
    支持OTA+外部flash加载升级
    全新 BOOTLD
    ————26.7.16
*/
#define Log_tag "Caven_app info"

// 消息通道&循环队列数
#if SYS_BTLD != 1
#define CAVEN_PACK_M	6       // 列数
#else
#define CAVEN_PACK_M	3       // 列数
#endif

Caven_info_packet_Type *p_sys_pack = NULL;
Caven_info_packet_Type *p_usb_pack = NULL;
Caven_info_packet_Type *p_server_pack = NULL;
Caven_info_packet_Type *p_client_pack = NULL;
Caven_info_packet_Type *p_mqtt_pack = NULL;
Caven_info_packet_Type *p_other_pack = NULL;

static uint8_t info_packet_buff_array[CAVEN_PACK_M][BUFF_MAX];
static Caven_info_packet_Type Caven_packet_buff[CAVEN_PACK_M];

// info规则
static Caven_info_packet_Type Caven_standard = {
	.Head = 0xFA55,
    .Versions = 0x01,		// 版本
	.Type = 1,
    .dSize = 300,		    // 最大长度
};

Caven_BaseTIME_Type Caven_app_time;

int Caven_app_cmd1_handle (Caven_info_packet_Type pack);
int Caven_app_cmd2_handle (Caven_info_packet_Type pack);
#if SYS_BTLD != 1
int Caven_app_cmd3_handle (Caven_info_packet_Type pack);

#endif

int Caven_app_send_packet (Caven_info_packet_Type pack);
int Caven_app_Make_pack (uint8_t data,int way,Caven_BaseTIME_Type time);

int Caven_app_State_machine(Caven_BaseTIME_Type time)
{
	int retval = 0;
    
    Caven_app_time = time;
    Caven_info_packet_Type *handle_pack = NULL;
	handle_pack = Caven_Buff_Request_Full_Data (Caven_packet_buff,CAVEN_PACK_M);
	if (handle_pack != NULL)
    {
        Sys_user_infoled_set (1);
        if(handle_pack->Type == Caven_standard.Type || handle_pack->Type == 0)    // 白名单
        {
        }
        else        // other type
        {
            if(handle_pack->Comm_way == m_Other_Link)
            {
                handle_pack->Comm_way = g_SYS_Config.temp_val->Connect_passage;
                Caven_app_send_packet(*handle_pack);
            }
            else
            {
                Mode_Use.UART.Send_Data_pFun(m_UART_CH3,handle_pack->p_AllData,handle_pack->Get_num);
            }
            Caven_info_packet_clean_Fun(handle_pack);
            return retval;
        }
        if((handle_pack->Addr == g_SYS_Config.Addr) || handle_pack->Addr == 0 || handle_pack->Comm_way > m_RS485_Link)    // 白名单
        {
            switch (handle_pack->Cmd)
            {
            case 1:
                retval = Caven_app_cmd1_handle (*handle_pack);
                break;
            case 2:
                retval = Caven_app_cmd2_handle (*handle_pack);
                break;
    #if SYS_BTLD != 1
            case 3:
                retval = Caven_app_cmd3_handle (*handle_pack);
                break;
    #endif
            default:		// 不支持的CMD
                {
                    handle_pack->Result = m_Result_Fail_CMD;
                    handle_pack->dSize = 0;
                    retval = Caven_app_send_packet(*handle_pack);
                }
                break;
            }
        }
        else
        {
            
        }
        Caven_info_packet_clean_Fun(handle_pack);
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
        retval = 2;
        break;
    case m_CAVEN_CMD1_Version_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            retval = 1;
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
        }
        break;
    case m_CAVEN_CMD1_Serial_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            retval = 1;
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
        }
        break;
	case m_CAVEN_CMD1_Model_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            retval = 1;
            if(rw_info == 0)
            {
				temp_run = strlen(g_SYS_Config.Hostname);
                if(temp_run)
                {
                    memcpy(temp_array,g_SYS_Config.Hostname,temp_run);
                }
                pack.dSize = temp_run;
                pack.Result = 0;
                memcpy(pack.p_Data,temp_array,temp_run);
            }
            else
            {
                pack.Result = m_Result_Fail_ERROR;
                pack.dSize = 0;
            }
        }
        break;
    case m_CAVEN_CMD1_Bdtime_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            retval = 1;
            if(rw_info == 0)
            {
                temp_run = strlen(g_SYS_Config.Bddate);
                if(temp_run)
                {
                    memcpy(temp_array,g_SYS_Config.Bddate,temp_run);
                }
                pack.dSize = temp_run;
                pack.Result = 0;
                memcpy(pack.p_Data,temp_array,temp_run);
            }
            else
            {
                pack.Result = m_Result_Fail_ERROR;
                pack.dSize = 0;
            }
        }
        break;
    case m_CAVEN_CMD1_UTCtime_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            retval = 1;
            if(rw_info == 0)
            {
				g_SYS_Config.temp_val->Now_time = Mode_Use.TIME.Get_BaseTIME_pFun();
                temp_array[temp_run++] = (g_SYS_Config.temp_val->Now_time.SYS_Sec >> ( 8 * 3)) & 0xFF;
                temp_array[temp_run++] = (g_SYS_Config.temp_val->Now_time.SYS_Sec >> ( 8 * 2)) & 0xFF;
                temp_array[temp_run++] = (g_SYS_Config.temp_val->Now_time.SYS_Sec >> ( 8 * 1)) & 0xFF;
                temp_array[temp_run++] = (g_SYS_Config.temp_val->Now_time.SYS_Sec >> ( 8 * 0)) & 0xFF;
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
                g_SYS_Config.temp_val->Now_time.SYS_Sec = temp_val;
                g_SYS_Config.temp_val->Now_time.SYS_Us = 0;
				Mode_Use.TIME.Set_BaseTIME_pFun(g_SYS_Config.temp_val->Now_time);
                System_app_save_UTCtime();
                pack.Result = 0;
				pack.p_Data[temp_run++] = 0;
            }
			pack.dSize = temp_run;
        }
        break;
    case m_CAVEN_CMD1_Addr_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            retval = 1;
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
                    System_app_save_Addr();
                }
				pack.dSize = temp_run;
            }
        }
        break;
    case m_CAVEN_CMD1_RS232Cfg_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            retval = 1;
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
                if((temp_val & 0x00FFFFFF) % 9600 == 0 && temp_val > 0)
                {
                    g_SYS_Config.RS232_UART_Cfg = temp_val;
                    System_app_save_RS232Cfg();
                    pack.Result = 0;
                    pack.p_Data[temp_run++] = 0;
                }
                else
                {
                    pack.Result = m_Result_Fail_ERROR;
                    pack.p_Data[temp_run++] = 1;
                }
            }
			pack.dSize = temp_run;
        }
        break;
    case m_CAVEN_CMD1_RS485Cfg_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            retval = 1;
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
                if((temp_val & 0x00FFFFFF) % 9600 == 0 && temp_val > 0)
                {
                    g_SYS_Config.RS485_UART_Cfg = temp_val;
                    System_app_save_RS485Cfg();
                    pack.Result = 0;
                    pack.p_Data[temp_run++] = 0;
                }
                else
                {
                    pack.Result = m_Result_Fail_ERROR;
                    pack.p_Data[temp_run++] = 1;
                }
            }
			pack.dSize = temp_run;
        }
        break;
    case m_CAVEN_CMD1_Reset_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            retval = 1;
            if(rw_info == 1)
            {
                g_SYS_Config.temp_val->Reset_falg = 1;
				pack.Result = 0;
				pack.p_Data[temp_run++] = 0;
            }
            else
            {
                pack.Result = m_Result_Fail_ERROR;
            }
			pack.dSize = temp_run;
        }
        break;
    case m_CAVEN_CMD1_GetWork_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            retval = 1;
            if(rw_info == 0)
            {
                pack.p_Data[temp_run++] = g_SYS_Config.temp_val->Work_falg & 0xff;
				pack.Result = 0;
            }
            else
            {
                pack.Result = m_Result_Fail_ERROR;
            }
			pack.dSize = temp_run;
        }
        break;
    case m_CAVEN_CMD1_GetNet_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            retval = 1;
            if(rw_info == 0)
            {
                pack.p_Data[temp_run++] = g_SYS_Config.temp_val->Net_falg & 0xff;
				pack.Result = 0;
            }
            else
            {
                pack.Result = m_Result_Fail_ERROR;
            }
			pack.dSize = temp_run;
        }
        break;
    case m_CAVEN_CMD1_Worktime_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            retval = 1;
            if(rw_info == 0)
            {
				temp_array[temp_run++] = (g_SYS_Config.temp_val->Work_sec >> ( 8 * 3)) & 0xFF;
                temp_array[temp_run++] = (g_SYS_Config.temp_val->Work_sec >> ( 8 * 2)) & 0xFF;
                temp_array[temp_run++] = (g_SYS_Config.temp_val->Work_sec >> ( 8 * 1)) & 0xFF;
                temp_array[temp_run++] = (g_SYS_Config.temp_val->Work_sec >> ( 8 * 0)) & 0xFF;
				memcpy(pack.p_Data,temp_array,temp_run);
				pack.Result = 0;
            }
            else
            {
                pack.Result = m_Result_Fail_ERROR;
            }
			pack.dSize = temp_run;
        }
        break;
#if NETWORK & SYS_BTLD != 1
    case m_CAVEN_CMD1_IPv4Cfg_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            retval = 1;
            if(rw_info == 0)
            {
				if(g_SYS_Config.eth_mode == 0)
				{
					memcpy(&temp_array[temp_run],"mode<static>",strlen("mode<static>"));
					temp_run += strlen("mode<static>");
				}
				else
				{
					memcpy(&temp_array[temp_run],"mode<dhcp>",strlen("mode<dhcp>"));
					temp_run += strlen("mode<dhcp>");
				}
                memcpy(&temp_array[temp_run],"ip<",3);
				temp_run += 3;
                temp_val = strlen(g_SYS_Config.eth_ip_str);
                memcpy(&temp_array[temp_run],g_SYS_Config.eth_ip_str,temp_val);
                temp_run += temp_val;
                memcpy(&temp_array[temp_run],">",1);
				temp_run += 1;
                //
                memcpy(&temp_array[temp_run],"gw<",3);
				temp_run += 3;
                temp_val = strlen(g_SYS_Config.eth_gw_str);
                memcpy(&temp_array[temp_run],g_SYS_Config.eth_gw_str,temp_val);
                temp_run += temp_val;
                memcpy(&temp_array[temp_run],">",1);
				temp_run += 1;
                //
                memcpy(&temp_array[temp_run],"netmask<",strlen("netmask<"));
				temp_run += strlen("netmask<");
                temp_val = strlen(g_SYS_Config.eth_netmask_str);
                memcpy(&temp_array[temp_run],g_SYS_Config.eth_netmask_str,temp_val);
                temp_run += temp_val;
                memcpy(&temp_array[temp_run],">",1);
				temp_run += 1;
                //
                memcpy(&temp_array[temp_run],"DNS1<",strlen("DNS1<"));
				temp_run += strlen("DNS1<");
                temp_val = strlen(g_SYS_Config.eth_DNS1_str);
                memcpy(&temp_array[temp_run],g_SYS_Config.eth_DNS1_str,temp_val);
                temp_run += temp_val;
                memcpy(&temp_array[temp_run],">",1);
				temp_run += 1;
                //
                memcpy(&temp_array[temp_run],"DNS2<",strlen("DNS2<"));
				temp_run += strlen("DNS2<");
                temp_val = strlen(g_SYS_Config.eth_DNS2_str);
                memcpy(&temp_array[temp_run],g_SYS_Config.eth_DNS2_str,temp_val);
                temp_run += temp_val;
                memcpy(&temp_array[temp_run],">",1);
				temp_run += 1;
				//
                pack.dSize = temp_run;
                pack.Result = 0;
                memcpy(pack.p_Data,temp_array,temp_run);
            }
            else
            {
                pack.Result = m_Result_Fail_ERROR;
				memset(temp_array,0,sizeof(temp_array));
				temp_val = Caven_gain_str_by_sign((char*)pack.p_Data,pack.dSize,(char*)temp_array,"mode",'<');
				if(temp_val > 0)
				{
                    pack.Result = SYS_CMD_RESULT;
                    if(pack.Result) 
                    {
                        pack.dSize = 0;
                        break;
                    }
					if(memcmp(temp_array,"static",sizeof("static")) == 0)
					{
						g_SYS_Config.eth_mode = 0;
					}
					else
					{
						g_SYS_Config.eth_mode = 1;
					}
				}
				memset(temp_array,0,sizeof(temp_array));
				temp_val = Caven_gain_str_by_sign((char*)pack.p_Data,pack.dSize,(char*)temp_array,"ip",'<');
				if(temp_val > 0)
				{
					memset(g_SYS_Config.eth_ip_str,0,sizeof(g_SYS_Config.eth_ip_str));
					strcpy(g_SYS_Config.eth_ip_str,(char*)temp_array);
				}
				memset(temp_array,0,sizeof(temp_array));
				temp_val = Caven_gain_str_by_sign((char*)pack.p_Data,pack.dSize,(char*)temp_array,"gw",'<');
				if(temp_val > 0)
				{
					memset(g_SYS_Config.eth_gw_str,0,sizeof(g_SYS_Config.eth_gw_str));
					strcpy(g_SYS_Config.eth_gw_str,(char*)temp_array);
				}
				memset(temp_array,0,sizeof(temp_array));
				temp_val = Caven_gain_str_by_sign((char*)pack.p_Data,pack.dSize,(char*)temp_array,"netmask",'<');
				if(temp_val > 0)
				{
					memset(g_SYS_Config.eth_netmask_str,0,sizeof(g_SYS_Config.eth_netmask_str));
					strcpy(g_SYS_Config.eth_netmask_str,(char*)temp_array);
				}
				memset(temp_array,0,sizeof(temp_array));
				temp_val = Caven_gain_str_by_sign((char*)pack.p_Data,pack.dSize,(char*)temp_array,"DNS1",'<');
				if(temp_val > 0)
				{
					memset(g_SYS_Config.eth_DNS1_str,0,sizeof(g_SYS_Config.eth_DNS1_str));
					strcpy(g_SYS_Config.eth_DNS1_str,(char*)temp_array);
				}
				memset(temp_array,0,sizeof(temp_array));
				temp_val = Caven_gain_str_by_sign((char*)pack.p_Data,pack.dSize,(char*)temp_array,"DNS2",'<');
				if(temp_val > 0)
				{
					memset(g_SYS_Config.eth_DNS2_str,0,sizeof(g_SYS_Config.eth_DNS2_str));
					strcpy(g_SYS_Config.eth_DNS2_str,(char*)temp_array);
				}
				pack.p_Data[0] = 0;
                pack.dSize = 1;
                System_app_save_IPv4Cfg();
            }
        }
        break;
	case m_CAVEN_CMD1_TCPHBT_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            retval = 1;
            if(rw_info == 0)
            {
                temp_array[temp_run++] = g_SYS_Config.TCPHBT_En;
				temp_array[temp_run++] = 0;
				temp_array[temp_run++] = (g_SYS_Config.TCPHBT_cycle >> ( 8 * 1)) & 0xFF;
                temp_array[temp_run++] = (g_SYS_Config.TCPHBT_cycle >> ( 8 * 0)) & 0xFF;
                pack.dSize = temp_run;
                pack.Result = 0;
                memcpy(pack.p_Data,temp_array,temp_run);
            }
            else
            {
                pack.Result = SYS_CMD_RESULT;
                if(pack.Result) 
                {
                    pack.dSize = 0;
                    break;
                }
                g_SYS_Config.TCPHBT_En = pack.p_Data[temp_num++];
				temp_num ++;
				g_SYS_Config.TCPHBT_cycle = pack.p_Data[temp_num++];
				g_SYS_Config.TCPHBT_cycle <<= 8;
				g_SYS_Config.TCPHBT_cycle |= pack.p_Data[temp_num++];
				pack.p_Data[0] = 0;
                pack.dSize = 1;
                System_app_save_TCPHBT();
            }
        }
        break;
    case m_CAVEN_CMD1_TCPServer_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            retval = 1;
            if(rw_info == 0)
            {
                memset(temp_array,0,sizeof(temp_array));
				if(g_SYS_Config.tcp_server_enable)
				{
					strcat((char *)temp_array,"server<on>");
				}
				else
				{
					strcat((char *)temp_array,"server<off>");
				}
				if(g_SYS_Config.Server_break_off)
				{
                    strcat((char *)temp_array,"break_off<on>");
				}
				else
				{
					strcat((char *)temp_array,"break_off<off>");
				}
                strcat((char *)temp_array,"port<");
                strcat((char *)temp_array,g_SYS_Config.TCPServer_port);
                strcat((char *)temp_array,">");
				temp_run = strlen((char *)temp_array);
                //
				pack.dSize = temp_run;
                pack.Result = 0;
                memcpy(pack.p_Data,temp_array,temp_run);
            }
            else
            {
                pack.Result = m_Result_Fail_ERROR;
				memset(temp_array,0,sizeof(temp_array));
				temp_val = Caven_gain_str_by_sign((char*)pack.p_Data,pack.dSize,(char*)temp_array,"server",'<');
				if(temp_val > 0)
				{
                    pack.Result = SYS_CMD_RESULT;
                    if(pack.Result) 
                    {
                        pack.dSize = 0;
                        break;
                    }
					if(memcmp(temp_array,"on",sizeof("on")) == 0)
					{
						g_SYS_Config.tcp_server_enable = 1;
					}
					else
					{
						g_SYS_Config.tcp_server_enable = 0;
					}
				}
				memset(temp_array,0,sizeof(temp_array));
				temp_val = Caven_gain_str_by_sign((char*)pack.p_Data,pack.dSize,(char*)temp_array,"break_off",'<');
				if(temp_val > 0)
				{
					if(memcmp(temp_array,"on",sizeof("on")) == 0)
					{
						g_SYS_Config.Server_break_off = 1;
					}
					else
					{
						g_SYS_Config.Server_break_off = 0;
					}
				}
				memset(temp_array,0,sizeof(temp_array));
				temp_val = Caven_gain_str_by_sign((char*)pack.p_Data,pack.dSize,(char*)temp_array,"port",'<');
				if(temp_val > 0)
				{
					memset(g_SYS_Config.TCPServer_port,0,sizeof(g_SYS_Config.TCPServer_port));
					strcpy(g_SYS_Config.TCPServer_port,(char*)temp_array);
				}
				pack.p_Data[0] = 0;
                pack.dSize = 1;
                System_app_save_TCPServer();
            }
        }
        break;
    case m_CAVEN_CMD1_TCPClient_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            retval = 1;
            if(rw_info == 0)
            {
                memset(temp_array,0,sizeof(temp_array));
				if(g_SYS_Config.tcp_client_enable)
				{
                    strcat((char *)temp_array,"client<on>");
				}
				else
				{
					strcat((char *)temp_array,"client<off>");
				}
                strcat((char *)temp_array,"url<");
                strcat((char *)temp_array,g_SYS_Config.TCPClient_url);
                strcat((char *)temp_array,">");
				temp_run = strlen((char *)temp_array);
                //
				pack.dSize = temp_run;
                pack.Result = 0;
                memcpy(pack.p_Data,temp_array,temp_run);
            }
            else
            {
                pack.Result = m_Result_Fail_ERROR;
                memset(temp_array,0,sizeof(temp_array));
				temp_val = Caven_gain_str_by_sign((char*)pack.p_Data,pack.dSize,(char*)temp_array,"client",'<');
				if(temp_val > 0)
				{
                    pack.Result = SYS_CMD_RESULT;
                    if(pack.Result) 
                    {
                        pack.dSize = 0;
                        break;
                    }
					if(memcmp(temp_array,"on",sizeof("on")) == 0)
					{
						g_SYS_Config.tcp_client_enable = 1;
					}
					else
					{
						g_SYS_Config.tcp_client_enable = 0;
					}
				}
				memset(temp_array,0,sizeof(temp_array));
                temp_val = Caven_gain_str_by_sign((char*)pack.p_Data,pack.dSize,(char*)temp_array,"url",'<');
				if(temp_val > 0)
				{
                    memset(g_SYS_Config.TCPClient_url,0,sizeof(g_SYS_Config.TCPClient_url));
					strcpy(g_SYS_Config.TCPClient_url,(char*)temp_array);
				}
				pack.p_Data[0] = 0;
                pack.dSize = 1;
                System_app_save_TCPClient();
            }
        }
        break;
	case m_CAVEN_CMD1_HTTPHBT_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            retval = 1;
            if(rw_info == 0)
            {
                temp_array[temp_run++] = g_SYS_Config.HTTPHBT_En;
                pack.dSize = temp_run;
                pack.Result = 0;
                memcpy(pack.p_Data,temp_array,temp_run);
            }
            else
            {
                pack.Result = SYS_CMD_RESULT;
                if(pack.Result) 
                {
                    pack.dSize = 0;
                    break;
                }
                g_SYS_Config.HTTPHBT_En = pack.p_Data[temp_num++];
				pack.p_Data[0] = 0;
                pack.dSize = 1;
                System_app_save_HTTPHBT();
            }
        }
        break;
    case m_CAVEN_CMD1_HTTPCfg_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            retval = 1;
            if(rw_info == 0)
            {
				memset(temp_array,0,sizeof(temp_array));
				if(g_SYS_Config.tcp_http_enable)
				{
					memcpy(&temp_array[temp_run],"HTTP<on>",strlen("HTTP<on>"));
					temp_run += strlen("HTTP<on>");
				}
				else
				{
					memcpy(&temp_array[temp_run],"HTTP<off>",strlen("HTTP<off>"));
					temp_run += strlen("HTTP<off>");
				}
				if(g_SYS_Config.HTTP_cycle > 0)
				{
					memcpy(&temp_array[temp_run],"cycle<",strlen("cycle<"));
					temp_run += strlen("cycle<");
					sprintf((char*)&temp_array[temp_run],"%04ds>",g_SYS_Config.HTTP_cycle);
					temp_run = strlen((char*)temp_array);
				}
				sprintf((char*)&temp_array[temp_run],"url<%s>",g_SYS_Config.HTTP_url);
				temp_run = strlen((char*)temp_array);
                //
				pack.dSize = temp_run;
                pack.Result = 0;
                memcpy(pack.p_Data,temp_array,temp_run);
            }
            else
            {
                pack.Result = m_Result_Fail_ERROR;
				memset(temp_array,0,sizeof(temp_array));
				temp_val = Caven_gain_str_by_sign((char*)pack.p_Data,pack.dSize,(char*)temp_array,"HTTP",'<');
				if(temp_val > 0)
				{
                    pack.Result = SYS_CMD_RESULT;
                    if(pack.Result) 
                    {
                        pack.dSize = 0;
                        break;
                    }
					if(memcmp(temp_array,"on",sizeof("on")) == 0)
					{
						g_SYS_Config.tcp_http_enable = 1;
					}
					else
					{
						g_SYS_Config.tcp_http_enable = 0;
					}
				}
				memset(temp_array,0,sizeof(temp_array));
				temp_val = Caven_gain_str_by_sign((char*)pack.p_Data,pack.dSize,(char*)temp_array,"cycle",'<');
				if(temp_val > 0)
				{
					temp_val = atoi((char*)temp_array);
					g_SYS_Config.HTTP_cycle = MAX(1,temp_val);
				}
				memset(temp_array,0,sizeof(temp_array));
				temp_val = Caven_gain_str_by_sign((char*)pack.p_Data,pack.dSize,(char*)temp_array,"url",'<');
				if(temp_val > 0)
				{
					memset(g_SYS_Config.HTTP_url,0,sizeof(g_SYS_Config.HTTP_url));
					strcpy(g_SYS_Config.HTTP_url,(char*)temp_array);
				}
				pack.p_Data[0] = 0;
                pack.dSize = 1;
                System_app_save_HTTPCfg();
            }
        }
        break;
    case m_CAVEN_CMD1_MQTTCfg_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            retval = 1;
            if(rw_info == 0)
            {
                pack.Result = 0;
                memset(temp_array,0,sizeof(temp_array));
                if(g_SYS_Config.tcp_mqtt_enable)
                {
                    sprintf((char *)temp_array,"MQTT<%s>","on");
                }
                else
                {
                    sprintf((char *)temp_array,"MQTT<%s>","off");
                }
                strcat((char *)temp_array, g_SYS_Config.MQTTCfg);
                temp_run = strlen((char *)temp_array);
                memcpy(pack.p_Data,temp_array,temp_run);
            }
            else
            {
                pack.Result = m_Result_Fail_ERROR;
                memset(temp_array,0,sizeof(temp_array));
				temp_val = Caven_gain_str_by_sign((char*)pack.p_Data,pack.dSize,(char*)temp_array,"MQTT",'<');
				if(temp_val > 0)
				{
                    pack.Result = SYS_CMD_RESULT;
                    if(pack.Result) 
                    {
                        pack.dSize = 0;
                        break;
                    }
					if(memcmp(temp_array,"on",sizeof("on")) == 0)
					{
						g_SYS_Config.tcp_mqtt_enable = 1;
					}
					else
					{
						g_SYS_Config.tcp_mqtt_enable = 0;
					}
                    memset(temp_array,0,sizeof(temp_array));
                    char *temp_pointer = NULL;
                    temp_pointer = memstr(pack.p_Data,"url<",pack.dSize);
                    if(temp_pointer)
                    {
                        temp_val = MIN(strlen(temp_pointer),pack.dSize);
                        if(temp_val <= sizeof(g_SYS_Config.MQTTCfg))
                        {
                            memcpy(g_SYS_Config.MQTTCfg,temp_pointer,temp_val);
                        }
                    }
				}
                temp_run = 0;
                System_app_save_MQTTCfg();
            }
			pack.dSize = temp_run;
        }
        break;
        case m_CAVEN_CMD1_TCPUpHtdata_Order:
        {
            Result = 1;
            temp_num = 4;
            temp_val = pack.p_Data[temp_num++];
            temp_val <<= 8;
            temp_val |= pack.p_Data[temp_num++];
            temp_val <<= 8;
            temp_val |= pack.p_Data[temp_num++];
            temp_val <<= 8;
            temp_val |= pack.p_Data[temp_num++];
            if(temp_val > 0x69B3E5F5)
            {
                g_SYS_Config.temp_val->Now_time.SYS_Sec = temp_val;
                g_SYS_Config.temp_val->TCPHBT_num ++;
                g_SYS_Config.temp_val->TCPHBT_Run = 0;
                // Mode_Use.TIME.Set_BaseTIME_pFun(g_SYS_Config.temp_val->Now_time);
            }
        }
        break;
#endif
    case m_CAVEN_CMD1_GetMAC_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            retval = 1;
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
        }
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
	if (retval && (pack.Result == 0 || pack.Result == 4 || pack.Result == 5 || pack.Result == 6 || 		\
		pack.Result == m_Result_Fail_ERROR || pack.Result == m_Result_Back_Other))
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
    uint8_t temp_array[128];
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
            retval = 1;
            pack.Result = m_Result_Back_Succ;
            if(rw_info == 0)
            {
		#if SYS_BTLD == 1
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
				
		#if SYS_BTLD != 1
			{
				if(temp_val == 0)
				{
					temp_rt = 0x00;		// 跳转到boot
					g_SYS_Config.Boot.Bt_mode = 0;      // bootld
					System_app_save_boot ();
					g_SYS_Config.temp_val->Reset_falg = 1;
				}
				else
				{
					temp_rt = 0x02;		// 不符合条件
                    pack.Result = m_Result_Fail_ERROR;
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
					g_SYS_Config.Boot.app_crc = 0;
					g_SYS_Config.Boot.app_crc = pack.p_Data[temp_num++];
					g_SYS_Config.Boot.app_crc <<= 8;
					g_SYS_Config.Boot.app_crc |= pack.p_Data[temp_num++];
					temp_rt = Bootld_Save_bin_Fun(1,NULL,0,temp_val,g_SYS_Config.Boot.app_crc);
				}
				else
                {
                    temp_sum = pack.dSize - temp_num;   // 这一帧，bin包大小
                    temp_rt = Bootld_Save_bin_Fun(1,&pack.p_Data[temp_num],temp_sum,temp_val,g_SYS_Config.Boot.app_crc);
                    if (temp_rt == 0) 
                    {
                        if(temp_val == 0xFFFFFFFF)
                        {
                            g_SYS_Config.Boot.Bt_mode = 1;
                            System_app_save_boot();
                            g_SYS_Config.temp_val->Reset_falg = 1;
                        }
                    }
                }
                if(temp_rt != 0)
                {
                    pack.Result = m_Result_Fail_ERROR;
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
		}
        break;
    case m_CAVEN_CMD2_Reset_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            retval = 1;
            if(rw_info == 1)
            {
                g_SYS_Config.temp_val->Reset_falg = 1;
				pack.Result = 0;
				pack.p_Data[temp_run++] = 0;
            }
            else
            {
                pack.Result = m_Result_Fail_ERROR;
            }
			pack.dSize = temp_run;
        }
        break;
    case m_CAVEN_CMD2_DEBUG_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            retval = 1;
            if(rw_info == 1)
            {
				pack.Result = 0;
				temp_val = pack.p_Data[temp_num++];
				if (temp_val > 0)
				{
					g_SYS_Config.debug = temp_val & 0xff;
					pack.p_Data[temp_run++] = 0;
					System_app_save_debug ();
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
        }
        break;
    case m_CAVEN_CMD2_Restore_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            retval = 1;
            if(rw_info == 1)
            {
				// 重置，重启
				System_app_Restore ();
                g_SYS_Config.temp_val->Reset_falg = 1;
				pack.Result = m_Result_Back_Succ;
				pack.p_Data[temp_run++] = 0;
            }
            else
            {
                pack.Result = m_Result_Fail_ERROR;
            }
			pack.dSize = temp_run;
        }
        break;
    case m_CAVEN_CMD2_Serial_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            retval = 1;
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
                if(g_SYS_Config.Serial > 0)
                {
                    pack.Result = m_Result_Fail_ERROR;
                    pack.dSize = 0;
                    break;
                }
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
				System_app_save_Serial ();
            }
			pack.dSize = temp_run;
        }
        break;
    case m_CAVEN_CMD2_AuthZ_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            retval = 1;
        #if SYS_BTLD != 1
            pack.Result = m_Result_Fail_ERROR;
            pack.p_Data[temp_run++] = 1;
        #else
            if(rw_info == 0)
            {
                System_app_Gain_ICID(temp_array);
				temp_run += 8;
                pack.Result = m_Result_Back_Succ;
                memcpy(pack.p_Data,temp_array,temp_run);
            }
            else
            {
                memcpy(g_SYS_Config.Boot.Encrypt,&pack.p_Data[temp_num],sizeof(g_SYS_Config.Boot.Encrypt));
                pack.Result = m_Result_Back_Succ;
                pack.p_Data[temp_run++] = 0;
                System_app_SYS_Config_Save ();
            }
        #endif
			pack.dSize = temp_run;
        }
        break;
    case m_CAVEN_CMD2_FLASH_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            retval = 1;
            pack.Result = m_Result_Fail_ERROR;
            if(rw_info == 0)
            {
                temp_val = MODE_W25Q_Get_Max_Addr_Fun();
                temp_array[temp_run++] = (temp_val >> (8 * 3)) & 0XFF;
                temp_array[temp_run++] = (temp_val >> (8 * 2)) & 0XFF;
                temp_array[temp_run++] = (temp_val >> (8 * 1)) & 0XFF;
                temp_array[temp_run++] = (temp_val >> (8 * 0)) & 0XFF;
                pack.Result = m_Result_Back_Succ;
                memcpy(pack.p_Data,temp_array,temp_run);
            }
            else
            {
                if(rw_info > 1)
                {
                    temp_val = pack.p_Data[temp_num++];
                    temp_val <<= 8;
                    temp_val |= pack.p_Data[temp_num++];
                    temp_val <<= 8;
                    temp_val |= pack.p_Data[temp_num++];
                    temp_val <<= 8;
                    temp_val |= pack.p_Data[temp_num++];
                    temp_sum = pack.p_Data[temp_num++];
                    temp_sum <<= 8;
                    temp_sum |= pack.p_Data[temp_num++];
                    temp_sum <<= 8;
                    temp_sum |= pack.p_Data[temp_num++];
                    temp_sum <<= 8;
                    temp_sum |= pack.p_Data[temp_num++];
                }
				switch (rw_info) {
                    case 1:
                    {
                        pack.p_Data[temp_run++] = 0;
                        pack.Result = m_Result_Back_Succ;
                    }break;
                    case 2:
                    {
                        temp_rt = MODE_W25Q_Read_Data_Fun(temp_val, pack.p_Data, temp_sum);
                        if(temp_rt == 0)
                        {
                            pack.Result = m_Result_Back_Succ;
                            temp_run = temp_sum;
                        }
                    }break;
                    case 3:
                    {
                        temp_rt = MODE_W25Q_Write_Data_Fun(temp_val, &pack.p_Data[temp_num], temp_sum);
                        if(temp_rt == 0)
                        {
                            pack.p_Data[temp_run++] = 0;
                            pack.Result = m_Result_Back_Succ;
                        }
                    }break;
                    case 4:
                    {
                        for(uint32_t i = temp_val; i < (temp_val + temp_sum);)
                        {
                            temp_rt = MODE_W25Q_Erase_Sector_Fun(i);
                            if(temp_rt)
                            {
                                retval = 1;
                                break;
                            }
                            i += W25Q_SECTOR_SIZE;
                        }
                        if(temp_rt == 0)
                        {
                            pack.p_Data[temp_run++] = 0;
                            pack.Result = m_Result_Back_Succ;
                        }
                    }break;
                    default:
                    {
                    }break;
                }
            }
			pack.dSize = temp_run;
        }
        break;
    case m_CAVEN_CMD2_MACCfg_Order:
        {
            rw_info = pack.p_Data[temp_num++];
            retval = 1;
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
				System_app_save_MACCfg ();
            }
			pack.dSize = temp_run;
        }
        break;
        case m_CAVEN_CMD2_ETHOTA_Order:
		{
			rw_info = pack.p_Data[temp_num++];
            retval = 1;
            pack.Result = m_Result_Back_Succ;
            temp_rt = 1;
            if(rw_info == 0)
            {
                temp_sum = MODE_W25Q_Get_Max_Addr_Fun();
                if(temp_sum == 0)
                {
                    pack.p_Data[temp_run++] = 1;
                }
				else
                {
                    pack.p_Data[temp_run++] = 0;     // succ
                    temp_rt = 0;
                }
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
                if(rw_info == 1 || rw_info == 2)
                {
                    rw_info ++;     // 跳过1(APP)
                    if (temp_val == 0)
                    {
                        g_SYS_Config.temp_val->u32_val = pack.p_Data[temp_num++];
                        g_SYS_Config.temp_val->u32_val <<= 8;
                        g_SYS_Config.temp_val->u32_val |= pack.p_Data[temp_num++];
                        temp_rt = Bootld_Save_bin_Fun(rw_info,NULL,0,temp_val,g_SYS_Config.temp_val->u32_val);
                    }
                    else
                    {
                        temp_sum = (pack.dSize - temp_num);
                        temp_rt = Bootld_Save_bin_Fun(rw_info,&pack.p_Data[temp_num],temp_sum,temp_val,g_SYS_Config.temp_val->u32_val);
                        if (temp_rt == 0) 
                        { 
                            if(temp_val == 0xFFFFFFFF)
                            {
                                // g_SYS_Config.Boot.Bt_mode = (rw_info + 1) & 0xff;
                                // System_app_save_boot();
                                // g_SYS_Config.temp_val->Reset_falg = 1;
                            }
                        }
                    }
                }
                else if(rw_info == 3)       // 强制加载外部flash
                {
                    if(temp_val < 0x03 && ((MODE_W25Q_Get_Max_Addr_Fun() > 0) || temp_val == 0))     // 启动跳转需要外部flash
                    {
                        temp_rt = 0;
                        g_SYS_Config.Boot.Bt_mode = (temp_val + 1) & 0xff;
                        System_app_save_boot();
                        g_SYS_Config.temp_val->Reset_falg = 1;
                    }
                }
                if(temp_rt != 0)
                {
                    pack.Result = m_Result_Fail_ERROR;
                }
				pack.p_Data[temp_run++] = (temp_val >> (8 * 3)) & 0xff;
				pack.p_Data[temp_run++] = (temp_val >> (8 * 2)) & 0xff;
				pack.p_Data[temp_run++] = (temp_val >> (8 * 1)) & 0xff;
				pack.p_Data[temp_run++] = (temp_val >> (8 * 0)) & 0xff;
				pack.p_Data[temp_run++] = temp_rt;
			}
			pack.dSize = temp_run;
		}
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
	if (retval && (pack.Result == 0 || pack.Result == 4 || pack.Result == 5 || pack.Result == 6 || 		\
		pack.Result == m_Result_Fail_ERROR || pack.Result == m_Result_Back_Other))
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

#if SYS_BTLD != 1
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
			pack.Result = m_Result_Back_Succ;
			temp_data = pack.p_Data[temp_num++];
			sys_set_bzz_fun (temp_data);
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
		pack.Result == m_Result_Fail_ERROR || pack.Result == m_Result_Back_Other))
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
	if (pack.Addr == 0xff && pack.Comm_way == m_RS485_Link)		// 广播从机不回复
	{
		return retval;
    }
    // memset(temp_array,0,sizeof(temp_array));
    temp_num = Caven_info_Split_packet_Fun(pack,temp_array);
    if (temp_num > 0)
    {
        System_Send_data (temp_array,temp_num,pack.Comm_way);
        retval = temp_num;
    }
    else
    {
        retval = temp_num;
    }
    return retval;
}

int Caven_app_Make_pack (uint8_t data,int way,Caven_BaseTIME_Type time)
{
    int retval = 0;
    uint32_t temp_num = 0;
    Caven_info_packet_Type *temp_pack = NULL;
    Caven_info_packet_Type **pp_temp_pack = NULL;
    switch (way)
    {
    case m_Connect_SYS:
        {
            if (p_sys_pack == NULL) {
                p_sys_pack = Caven_Buff_Request_Occupy_Data (Caven_packet_buff,CAVEN_PACK_M);
            }
            temp_pack = p_sys_pack;
            if (temp_pack != NULL) {
                pp_temp_pack = &p_sys_pack;
            }
            else {
                retval = -1;
            }
        }
        break;
    case m_RS232_Link:
        {
        }
        break;
    case m_USB_Link:
        {
            if (p_usb_pack == NULL) {
                p_usb_pack = Caven_Buff_Request_Occupy_Data (Caven_packet_buff,CAVEN_PACK_M);
            }
            temp_pack = p_usb_pack;
            if (temp_pack != NULL) {
                pp_temp_pack = &p_usb_pack;
            }
            else {
                retval = -1;
            }
        }
        break;
    case m_Server_Link:
        {
            if (p_server_pack == NULL) {
                p_server_pack = Caven_Buff_Request_Occupy_Data (Caven_packet_buff,CAVEN_PACK_M);
            }
            temp_pack = p_server_pack;
            if (temp_pack != NULL) {
                pp_temp_pack = &p_server_pack;
            }
            else {
                retval = -1;
            }
        }
        break;
    case m_Client_Link:
        {
            if (p_client_pack == NULL) {
                p_client_pack = Caven_Buff_Request_Occupy_Data (Caven_packet_buff,CAVEN_PACK_M);
            }
            temp_pack = p_client_pack;
            if (temp_pack != NULL) {
                pp_temp_pack = &p_client_pack;
            }
            else {
                retval = -1;
            }
        }
        break;
    case m_MQTT_Link:
        {
        }
        break;
    case m_UDP_Link:
        {
        }
        break;
    default:
        {
            if (p_other_pack == NULL) {
                p_other_pack = Caven_Buff_Request_Occupy_Data (Caven_packet_buff,CAVEN_PACK_M);
            }
            temp_pack = p_other_pack;
            if (temp_pack != NULL) {
                pp_temp_pack = &p_other_pack;
            }
            else {
                retval = -1;
            }
        }
        break;
    }

    if (retval == 0 && temp_pack != NULL)
    {
		if (temp_pack->Time.SYS_Sec > 0) {
            temp_num = time.SYS_Sec - temp_pack->Time.SYS_Sec;
			if (temp_num > 1)   // 去掉数据包 
			{
				Caven_info_packet_clean_Fun(temp_pack);
				temp_pack->Occupy = 1;
			}
        }
        temp_pack->Time = time;
        retval = Caven_info_Make_packet_Fun(Caven_standard, temp_pack, data);
        if (retval == 0xFF)
        {
            temp_pack->Comm_way = way;
            *pp_temp_pack = NULL;
        }
        else if (retval < 0)
        {
            Caven_info_packet_clean_Fun(temp_pack);
            *pp_temp_pack = NULL;
        }
    }
    return retval;
}

/*
retval < 0 失败

*/
int Caven_app_JSON_Make_pack (char *data,int way)
{
	int retval = -1;
#if Exist_ETH
    int temp_len = 0;
	char *temp_str = NULL;
	char array[500];
    Caven_info_packet_Type * temp_pack = NULL;
	
	temp_len = strlen(data);
	temp_str = memstr(data, "\"Caven_pack\"",temp_len);

	if(temp_str != NULL)
	{
        temp_pack = Caven_Buff_Request_Occupy_Data (Caven_packet_buff,CAVEN_PACK_M);
        if (temp_pack == NULL)
        {
            return retval;
        }
		memset(array,0,sizeof(array));
		temp_len = strlen(temp_str);

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
			temp_pack->Result = 0;
			temp_pack->Type = 0;
			temp_pack->Versions = 1;
            temp_pack->Run_status = 0xff;
		}
        else {
            Caven_info_packet_clean_Fun(temp_pack);
        }
	}
#endif
	return retval;
}

int Caven_send_Heartbeat_Fun (void *data)
{
    int retval = 0;
#if Exist_ETH
    uint8_t temp_array[100];
    int temp_num = 0;
    memcpy(temp_array,&g_SYS_Config.Serial,sizeof(g_SYS_Config.Serial));
    temp_num += sizeof(g_SYS_Config.Serial);
    temp_array[temp_num ++] = (g_SYS_Config.temp_val->TCPHBT_num >> (8 * 3)) & 0xff;
    temp_array[temp_num ++] = (g_SYS_Config.temp_val->TCPHBT_num >> (8 * 2)) & 0xff;
    temp_array[temp_num ++] = (g_SYS_Config.temp_val->TCPHBT_num >> (8 * 1)) & 0xff;
    temp_array[temp_num ++] = (g_SYS_Config.temp_val->TCPHBT_num >> (8 * 0)) & 0xff;
    temp_array[temp_num ++] = (g_SYS_Config.temp_val->Now_time.SYS_Sec >> (8 * 3)) & 0xff;
    temp_array[temp_num ++] = (g_SYS_Config.temp_val->Now_time.SYS_Sec >> (8 * 2)) & 0xff;
    temp_array[temp_num ++] = (g_SYS_Config.temp_val->Now_time.SYS_Sec >> (8 * 1)) & 0xff;
    temp_array[temp_num ++] = (g_SYS_Config.temp_val->Now_time.SYS_Sec >> (8 * 0)) & 0xff;
    retval = Caven_info_return_Fun (Caven_standard.Versions,Caven_standard.Type,g_SYS_Config.Addr,  \
    1,m_CAVEN_CMD1_TCPUpHtdata_Order,temp_num,temp_array,0,(uint8_t *)data);
#endif
    return retval;
}

void Caven_app_Init (void)
{
    for (int i = 0; i < CAVEN_PACK_M; i++)
    {
        Caven_info_packet_index_Fun(&Caven_packet_buff[i], info_packet_buff_array[i]);
		Caven_info_packet_clean_Fun(&Caven_packet_buff[i]);
    }
    Sys_TCP_send_Heartbeat_Bind_Fun (Caven_send_Heartbeat_Fun);
}

void Caven_app_Exit (void)
{

}
