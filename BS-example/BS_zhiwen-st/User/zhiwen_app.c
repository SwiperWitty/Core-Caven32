#include "zhiwen_app.h"
#include "Caven_info_frame.h"
#include "IC_AS608.h"

u8 zhiwen_state = 0;
u8 zhiwen_state_sub = 0;

u8 zhiwen_flag = 0;
u8 zhiwen_id = 1;
int zhiwen_id_get = 0;
int zhiwen_num = 0;
int del_id = 0;
AS608_Packet zhiwen_pack;

int zhiwen_img (u8 *data)
{
    int retval = 0;
    AS608_Packet pack;
    memset(&pack,0,sizeof(pack));
    if (data != NULL)
    {
        pack.header = AS608_CMD_HEADER;
        pack.address = 0xffffffff;
        pack.type = 1;

        pack.length = 3;
        pack.data[0] = AS608_CMD_GENIMG;

        retval = AS608_info_Split_packet_Fun(pack,data);
        AS608_info_packet_fast_clean_Fun(&pack);
    }
    return retval;
}

int zhiwen_add_buff (u8 buff,u8 *data)
{
    int retval = 0;
    AS608_Packet pack;
    memset(&pack,0,sizeof(pack));
    if (data != NULL)
    {
        pack.header = AS608_CMD_HEADER;
        pack.address = 0xffffffff;
        pack.type = 1;

        pack.length = 4;
        pack.data[0] = AS608_CMD_IMG2CHAR;
        pack.data[1] = buff;

        retval = AS608_info_Split_packet_Fun(pack,data);
        AS608_info_packet_fast_clean_Fun(&pack);
    }
    return retval;
}

int zhiwen_ask_search (int num,u8 *data)
{
    int retval = 0;
    AS608_Packet pack;
    memset(&pack,0,sizeof(pack));
    if (data != NULL)
    {
        pack.header = AS608_CMD_HEADER;
        pack.address = 0xffffffff;
        pack.type = 1;

        pack.length = 8;
        pack.data[0] = AS608_CMD_SEARCH;
        pack.data[1] = 1;        // buf
        pack.data[2] = 0;
        pack.data[3] = 0;        // s
        pack.data[4] = (num >> 8) & 0xff;
        pack.data[5] = num & 0xff;   // e
        retval = AS608_info_Split_packet_Fun(pack,data);
        AS608_info_packet_fast_clean_Fun(&pack);
    }
    return retval;
}

int zhiwen_reg_model (u8 *data)
{
    int retval = 0;
    AS608_Packet pack;
    memset(&pack,0,sizeof(pack));
    if (data != NULL)
    {
        pack.header = AS608_CMD_HEADER;
        pack.address = 0xffffffff;
        pack.type = 1;

        pack.length = 3;
        pack.data[0] = AS608_CMD_REGMODEL;

        retval = AS608_info_Split_packet_Fun(pack,data);
        AS608_info_packet_fast_clean_Fun(&pack);
    }
    return retval;
}

int zhiwen_save_id (int id,u8 *data)
{
    int retval = 0;
    AS608_Packet pack;
    memset(&pack,0,sizeof(pack));
    if (data != NULL)
    {
        pack.header = AS608_CMD_HEADER;
        pack.address = 0xffffffff;
        pack.type = 1;

        pack.length = 6;
        pack.data[0] = AS608_CMD_STORE;
        pack.data[1] = 1;        // buf
        pack.data[2] = (id >> 8) & 0xff;
        pack.data[3] = id & 0xff;
        retval = AS608_info_Split_packet_Fun(pack,data);
        AS608_info_packet_fast_clean_Fun(&pack);
    }
    return retval;
}

int zhiwen_ask_num (u8 *data)
{
    int retval = 0;
    AS608_Packet pack;
    memset(&pack,0,sizeof(pack));
    if (data != NULL)
    {
        pack.header = AS608_CMD_HEADER;
        pack.address = 0xffffffff;
        pack.type = 1;

        pack.length = 3;
        pack.data[0] = AS608_CMD_TEMPLATE_NUM;
        retval = AS608_info_Split_packet_Fun(pack,data);
        AS608_info_packet_fast_clean_Fun(&pack);
    }
    return retval;
}

int zhiwen_kill_all (void)
{
    int retval = 0;
    AS608_Packet pack;
	u8 temp_array[100];
    memset(&pack,0,sizeof(pack));

	pack.header = AS608_CMD_HEADER;
	pack.address = 0xffffffff;
	pack.type = 1;

	pack.length = 3;
	pack.data[0] = AS608_CMD_EMPTY;
	retval = AS608_info_Split_packet_Fun(pack,temp_array);
	AS608_info_packet_fast_clean_Fun(&pack);
	Mode_Use.UART.Send_Data_pFun(m_UART_CH1,temp_array,retval);
	
	zhiwen_id = 1;
	zhiwen_num = 0;
    return retval;
}

int zhiwen_del_num (int start,int num,u8 *data)
{
    int retval = 0;
    AS608_Packet pack;
    memset(&pack,0,sizeof(pack));
    if (start >= 0)
    {
        pack.header = AS608_CMD_HEADER;
        pack.address = 0xffffffff;
        pack.type = 1;

        pack.length = 7;
        pack.data[0] = AS608_CMD_DELETE;
        pack.data[1] = (start >> 8) & 0xff;
        pack.data[2] = (start) & 0xff;
        pack.data[3] = (num >> 8) & 0xff;
        pack.data[4] = (num) & 0xff;

        retval = AS608_info_Split_packet_Fun(pack,data);
        AS608_info_packet_fast_clean_Fun(&pack);
    }
    return retval;
}

/*
    mode = 0 读指纹数
	mode = 1 录指纹
	mode = 2 检索指纹
	mode = 3 删指纹
*/
void zhiwen_app_mode (u8 mode,u8 sub,int id)
{
	zhiwen_state_sub = sub;
    switch (mode)
    {
    case 0:
        zhiwen_state = 3;
        break;
    case 1:
        zhiwen_state = 1;
        break;
    case 2:
        zhiwen_state = 2;
        break;
    case 3:
        zhiwen_state = 4;
        break;
    case 4:
        zhiwen_kill_all ();
        break;
    default:
        break;
    }
	
    if (zhiwen_state == 4)
    {
        del_id = id;
    }
}

int zhiwen_check_able (AS608_Packet pack)
{
    int retval = 0;

    if (pack.type == 7)
    {
        retval = pack.data[0];
    }
    return retval;
}

char oled_array_num[100];
char oled_array[100];
int zhiwen_app_State_machine (Caven_BaseTIME_Type time)
{
	int retval = 0;
    u8 temp_array[100];
    u8 temp_buff[100];
    int temp_num = 0,temp_run = 0;
	AS608_Packet temp_pack;
	
	if(zhiwen_pack.Run_status & 0x40)
	{
		memcpy(&temp_pack,&zhiwen_pack,sizeof(zhiwen_pack));
		AS608_info_packet_fast_clean_Fun(&zhiwen_pack);
        switch (zhiwen_state)
        {
        case 1:     // 录入
            {
				if (zhiwen_state_sub == 1 || zhiwen_state_sub == 4)
				{
					if (zhiwen_check_able (temp_pack) == 2)
					{
						// wait
					}
					else
					{
						zhiwen_flag = 1;
					}
				}
				else if (zhiwen_check_able (temp_pack) == 0)
                {
                    zhiwen_flag = 1;
                }
                else
                {
                    zhiwen_flag = 0xff;
                }
            }
            break;
        case 2:     // 识别
            {
                if (zhiwen_state_sub == 5)
                {
                    if (zhiwen_check_able (temp_pack) == 0)				
					{
						zhiwen_id_get = temp_pack.data[1];
						zhiwen_id_get <<= 8;
						zhiwen_id_get |= temp_pack.data[2];
						zhiwen_flag = 1;
						if(zhiwen_id_get > 0)
						{
							User_GPIO_set(1,11,0);
							User_GPIO_set(1,12,0);
						}
						else
						{
							User_GPIO_set(1,11,1);
							User_GPIO_set(1,12,1);
						}
						// EF 01 FF FF FF FF 07 00 07 [00] [00 01] [00 AA] 00 B9
					}
                    else
                    {
                        zhiwen_id_get = 0;
						zhiwen_flag = 1;
                    }
                }
				else if (zhiwen_state_sub == 1)
				{
					if (zhiwen_check_able (temp_pack) == 2)
					{
						// wait
					}
					else
					{
						zhiwen_flag = 1;
					}
				}
                else if (zhiwen_check_able (temp_pack) == 0)
                {
                    zhiwen_flag = 1;
                }
                else
                {
                    zhiwen_flag = 0xff;
					zhiwen_id_get = 0;
                }
            }
            break;
		case 3:
            {
                if (zhiwen_check_able (temp_pack) == 0)
                {
                    zhiwen_flag = 1;
					if (zhiwen_state_sub == 2)
					{
						zhiwen_num = temp_pack.data[1];
						zhiwen_num <<= 8;
						zhiwen_num |= temp_pack.data[2];
						zhiwen_id = zhiwen_num + 1;
						Mode_Use.OLED.Show_String_pFun (0,3,"ID num ",0,0,16);
					}
                }
                else
                {
                    zhiwen_flag = 0xff;
					zhiwen_id_get = 0;
                }
            }
            break;
		case 4:
            {
                if (zhiwen_check_able (temp_pack) == 0)
                {
                    zhiwen_flag = 1;
					if (zhiwen_state_sub == 2)
					{
						
					}
                }
                else
                {
                    zhiwen_flag = 0xff;
					del_id = 0;
                }
            }
            break;
        default:
            break;
        }
		stb_printf("AS608 PACK\n");
		stb_printf("type: %d\n",temp_pack.type);
		stb_printf("code: %d\n \n",temp_pack.data[0]);
	}
	if (zhiwen_flag == 1)
	{
		stb_printf("AS608 app [%d][%d] secc\n",zhiwen_state,zhiwen_state_sub);
		zhiwen_state_sub ++;
		zhiwen_flag = 0;
	}
	else if(zhiwen_flag == 0xff)
	{
		temp_buff[temp_run++] = 0x01;
		temp_num = Caven_info_return_Fun (0x01,0,0x01,3,m_CAVEN_CMD3_zhiwen_Order,temp_run,temp_buff,0,temp_array);
		Mode_Use.UART.Send_Data_pFun(DEBUG_CH,temp_array,temp_num);
		zhiwen_flag = 0;
		zhiwen_state = 0;
		zhiwen_state_sub = 0;
	}
    switch (zhiwen_state)
    {
    case 1:     // 录入
        {
            switch (zhiwen_state_sub)
            {
            case 1:
				if(zhiwen_pack.Run_status & 0x40)
				{
					break;
				}
				memset (oled_array,0,sizeof(oled_array));
				sprintf (oled_array,"Press your hand ");
                temp_num = zhiwen_img(temp_array);
                Mode_Use.UART.Send_Data_pFun(m_UART_CH1,temp_array,temp_num);
                Mode_Use.TIME.Delay_Ms (1000);
                break;
            case 2:
                temp_num = zhiwen_add_buff (1,temp_array);
				Mode_Use.TIME.Delay_Ms (100);
                Mode_Use.UART.Send_Data_pFun(m_UART_CH1,temp_array,temp_num);
				zhiwen_state_sub++;
                break;
			case 3:

                break;
            case 4:
				if(zhiwen_pack.Run_status & 0x40)
				{
					break;
				}
                temp_num = zhiwen_img(temp_array);
                Mode_Use.UART.Send_Data_pFun(m_UART_CH1,temp_array,temp_num);
				Mode_Use.TIME.Delay_Ms (1000);
                break;
            case 5:
                temp_num = zhiwen_add_buff (2,temp_array);
				Mode_Use.TIME.Delay_Ms (100);
                Mode_Use.UART.Send_Data_pFun(m_UART_CH1,temp_array,temp_num);
				zhiwen_state_sub++;
                break;
			case 6:

                break;
            case 7:
                temp_num = zhiwen_reg_model (temp_array);
				Mode_Use.TIME.Delay_Ms (100);
                Mode_Use.UART.Send_Data_pFun(m_UART_CH1,temp_array,temp_num);
				zhiwen_state_sub++;
                break;
			case 8:

                break;
            case 9:
                temp_num = zhiwen_save_id (zhiwen_id,temp_array);
				Mode_Use.TIME.Delay_Ms (100);
                Mode_Use.UART.Send_Data_pFun(m_UART_CH1,temp_array,temp_num);
				zhiwen_state_sub++;
                break;
			case 10:
                // wait finish
                break;
            case 11:
                // updata zhiwen_id
				temp_buff[temp_run++] = 0;
                temp_buff[temp_run++] = (zhiwen_id >> 24) & 0xff;
                temp_buff[temp_run++] = (zhiwen_id >> 16) & 0xff;
                temp_buff[temp_run++] = (zhiwen_id >> 8) & 0xff;
                temp_buff[temp_run++] = (zhiwen_id >> 0) & 0xff;
                temp_num = Caven_info_return_Fun (0x01,0,0x01,3,m_CAVEN_CMD3_zhiwen_Order,temp_run,temp_buff,0,temp_array);
                Mode_Use.UART.Send_Data_pFun(DEBUG_CH,temp_array,temp_num);
				Mode_Use.TIME.Delay_Ms (100);
                stb_printf("zhiwen over id [%d]",zhiwen_id);
				memset (oled_array,0,sizeof(oled_array));
				sprintf (oled_array,"wait Task ...    ");
				zhiwen_num ++;
				sprintf (oled_array_num,"ID num [%d] ",zhiwen_num);
                zhiwen_id ++;
                zhiwen_state = 0;
                zhiwen_state_sub = 0;
                break;
            default:
                Mode_Use.TIME.Delay_Ms (500);
                zhiwen_state_sub = 0;
                // again
                break;
            }
        }
        break;
    case 2:     // 识别
        {
            switch (zhiwen_state_sub)
            {
            case 1:
				if(zhiwen_pack.Run_status & 0x40)
				{
					break;
				}
				memset (oled_array,0,sizeof(oled_array));
				sprintf (oled_array,"Press your hand ");
                temp_num = zhiwen_img(temp_array);
                Mode_Use.UART.Send_Data_pFun(m_UART_CH1,temp_array,temp_num);
                Mode_Use.TIME.Delay_Ms (1000);
                break;
            case 2:
                temp_num = zhiwen_add_buff (1,temp_array);
				Mode_Use.TIME.Delay_Ms (100);
                Mode_Use.UART.Send_Data_pFun(m_UART_CH1,temp_array,temp_num);
				zhiwen_state_sub++;
                break;
            case 3:
                
                break;
            case 4:
                temp_num = zhiwen_ask_search (20,temp_array);
				Mode_Use.TIME.Delay_Ms (100);
                Mode_Use.UART.Send_Data_pFun(m_UART_CH1,temp_array,temp_num);
                zhiwen_state_sub ++;
                break;
            case 5:
                // wait id
                break;
            case 6:
                // updata id
                if(zhiwen_id_get > 0)
                {
                    temp_buff[temp_run++] = 0;
                }
                else
                {
                    temp_buff[temp_run++] = 1;
                }
                temp_buff[temp_run++] = (zhiwen_id_get >> 24) & 0xff;
                temp_buff[temp_run++] = (zhiwen_id_get >> 16) & 0xff;
                temp_buff[temp_run++] = (zhiwen_id_get >> 8) & 0xff;
                temp_buff[temp_run++] = (zhiwen_id_get >> 0) & 0xff;
                temp_num = Caven_info_return_Fun (0x01,0,0x01,3,m_CAVEN_CMD3_zhiwen_Order,temp_run,temp_buff,0,temp_array);
                Mode_Use.UART.Send_Data_pFun(DEBUG_CH,temp_array,temp_num);
				Mode_Use.TIME.Delay_Ms (100);
                stb_printf("zhiwen get id [%d]",zhiwen_id_get);
				memset (oled_array,0,sizeof(oled_array));
				sprintf (oled_array,"wait Task ...    ");
                zhiwen_id_get = 0;
                zhiwen_state = 0;
                zhiwen_state_sub = 0;
                break;
            default:
                Mode_Use.TIME.Delay_Ms (500);
                zhiwen_state_sub = 0;
                break;
            }
        }
        break;
	case 3:     // 查指纹数
        {
			switch (zhiwen_state_sub)
            {
            case 1:
                temp_num = zhiwen_ask_num(temp_array);
                Mode_Use.UART.Send_Data_pFun(m_UART_CH1,temp_array,temp_num);
                Mode_Use.TIME.Delay_Ms (100);
				zhiwen_state_sub++;
                break;
			case 2:
                break;
			case 3:
				// updata id
                temp_buff[temp_run++] = (zhiwen_num >> 24) & 0xff;
                temp_buff[temp_run++] = (zhiwen_num >> 16) & 0xff;
                temp_buff[temp_run++] = (zhiwen_num >> 8) & 0xff;
                temp_buff[temp_run++] = (zhiwen_num >> 0) & 0xff;
                temp_num = Caven_info_return_Fun (0x01,0,0x01,3,m_CAVEN_CMD3_zhiwen_Order,temp_run,temp_buff,0,temp_array);
                Mode_Use.UART.Send_Data_pFun(DEBUG_CH,temp_array,temp_num);
				Mode_Use.TIME.Delay_Ms (100);
                stb_printf("zhiwen get num [%d]",zhiwen_num);
				sprintf (oled_array_num,"ID num [%d] ",zhiwen_num);
				zhiwen_num = 0;
                zhiwen_state = 0;
                zhiwen_state_sub = 0;
                break;
			default:
				Mode_Use.TIME.Delay_Ms (500);
                zhiwen_state_sub = 0;
				break;
			}
		}
	case 4:     // del指纹
        {
			switch (zhiwen_state_sub)
            {
            case 1:
                temp_num = zhiwen_del_num (del_id,1,temp_array);
                Mode_Use.UART.Send_Data_pFun(m_UART_CH1,temp_array,temp_num);
                Mode_Use.TIME.Delay_Ms (100);
				zhiwen_state_sub++;
                break;
            case 2:
                break;
            case 3:
				// del id
                temp_buff[temp_run++] = 0;
                temp_num = Caven_info_return_Fun (0x01,0,0x01,3,m_CAVEN_CMD3_zhiwen_Order,temp_run,temp_buff,0,temp_array);
                Mode_Use.UART.Send_Data_pFun(DEBUG_CH,temp_array,temp_num);
				Mode_Use.TIME.Delay_Ms (100);
                stb_printf("zhiwen del num [%d]",del_id);
				del_id = 0;
                zhiwen_state = 0;
                zhiwen_state_sub = 0;
                break;
			default:
				Mode_Use.TIME.Delay_Ms (500);
                zhiwen_state_sub = 0;
				break;
			}
		}
    default:
        break;
    }
	Mode_Use.OLED.Show_String_pFun (2,2,oled_array_num,0,0,16);
	Mode_Use.OLED.Show_String_pFun (0,3,oled_array,0,0,16);
	return retval;
}

void zhiwen_info_handle (void *data)
{
	u8 temp_dat = *(u8*)data;
	AS608_info_Make_packet_Fun(&zhiwen_pack, temp_dat);
}

void zhiwen_app_Init (void)
{
	sprintf (oled_array_num,"ID num [%d] ",zhiwen_num);
	sprintf (oled_array,"wait Task ...  ");
}

void zhiwen_app_Exit (void)
{
//	u8 data[100];
//	int temp_num = 0;
//	zhiwen_pack.header = AS608_CMD_HEADER;
//	zhiwen_pack.address = 0xffffffff;
//	zhiwen_pack.type = 1;

//	zhiwen_pack.length = 4;
//	zhiwen_pack.data[0] = AS608_CMD_IMG2CHAR;
//	zhiwen_pack.data[1] = 0;

//	temp_num = AS608_info_Split_packet_Fun(zhiwen_pack,data);
//	Mode_Use.UART.Send_Data_pFun(2,data,temp_num);
}

