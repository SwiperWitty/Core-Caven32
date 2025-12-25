#include "zhiwen_app.h"
#include "IC_AS608.h"

u8 zhiwen_state = 0;
u8 zhiwen_state_sub = 0;

u8 zhiwen_flag = 1;
u8 zhiwen_id = 1;
int zhiwen_id_get = 0;
int zhiwen_num = 0;
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

int zhiwen_del_num (int start,int num,u8 *data)
{
    int retval = 0;
    AS608_Packet pack;
    memset(&pack,0,sizeof(pack));
    if (start > 0)
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

void zhiwen_app_flag_set (u8 flag)
{
    zhiwen_flag = flag;
}

/*
	mode = 1 录指纹
	mode = 2 检索指纹
	mode = 3 读指纹数
*/
void zhiwen_app_mode (u8 mode)
{
	zhiwen_state_sub = 0;
	zhiwen_state = mode;
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

int zhiwen_app_State_machine (Caven_BaseTIME_Type time)
{
    u8 temp_array[100];
    int temp_num = 0;
	AS608_Packet temp_pack;
	
	if(zhiwen_pack.Run_status & 0x40)
	{
		memcpy(&temp_pack,&zhiwen_pack,sizeof(zhiwen_pack));
		AS608_info_packet_fast_clean_Fun(&zhiwen_pack);
        switch (zhiwen_state)
        {
        case 1:     // 录入
            {
                if (zhiwen_check_able (temp_pack) == 0)
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
                if (zhiwen_check_able (temp_pack) == 0)
                {
                    zhiwen_flag = 1;
					if (zhiwen_state_sub == 5)
					{
						zhiwen_id_get = temp_pack.data[1];
						zhiwen_id_get <<= 8;
						zhiwen_id_get |= temp_pack.data[2];
						// EF 01 FF FF FF FF 07 00 07 [00] [00 01] [00 AA] 00 B9
					}
                }
                else
                {
                    zhiwen_flag = 1;
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
					}
                }
                else
                {
                    zhiwen_flag = 1;
					zhiwen_id_get = 0;
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
    switch (zhiwen_state)
    {
    case 1:     // 录入
        {
            if(zhiwen_flag == 1)
            {
				stb_printf("AS608 app [%d][%d] secc\n",zhiwen_state,zhiwen_state_sub);
				zhiwen_state_sub ++;
                zhiwen_flag = 0;
            }
            else
            {
                Mode_Use.TIME.Delay_Ms (10);
            }
            switch (zhiwen_state_sub)
            {
            case 1:
				if(zhiwen_pack.Run_status & 0x40)
				{
					break;
				}
                temp_num = zhiwen_img(temp_array);
                Mode_Use.UART.Send_Data_pFun(2,temp_array,temp_num);
                Mode_Use.TIME.Delay_Ms (1000);
                break;
            case 2:
                temp_num = zhiwen_add_buff (1,temp_array);
                Mode_Use.UART.Send_Data_pFun(2,temp_array,temp_num);
                Mode_Use.TIME.Delay_Ms (100);
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
                Mode_Use.UART.Send_Data_pFun(2,temp_array,temp_num);
                Mode_Use.TIME.Delay_Ms (1000);
                break;
            case 5:
                temp_num = zhiwen_add_buff (2,temp_array);
                Mode_Use.UART.Send_Data_pFun(2,temp_array,temp_num);
                Mode_Use.TIME.Delay_Ms (100);
				zhiwen_state_sub++;
                break;
			case 6:

                break;
            case 7:
                temp_num = zhiwen_reg_model (temp_array);
                Mode_Use.UART.Send_Data_pFun(2,temp_array,temp_num);
                Mode_Use.TIME.Delay_Ms (100);
				zhiwen_state_sub++;
                break;
			case 8:

                break;
            case 9:
                temp_num = zhiwen_save_id (zhiwen_id,temp_array);
                Mode_Use.UART.Send_Data_pFun(2,temp_array,temp_num);
                Mode_Use.TIME.Delay_Ms (100);
				zhiwen_state_sub++;
                break;
			case 10:
                // wait finish
                break;
            case 11:
                // updata zhiwen_id
                stb_printf("zhiwen over id [%d]",zhiwen_id);
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
            if(zhiwen_flag == 1)
            {
				stb_printf("AS608 app [%d][%d] secc\n",zhiwen_state,zhiwen_state_sub);
				zhiwen_state_sub ++;
                zhiwen_flag = 0;
            }
            else
            {
                Mode_Use.TIME.Delay_Ms (10);
            }
            switch (zhiwen_state_sub)
            {
            case 1:
				if(zhiwen_pack.Run_status & 0x40)
				{
					break;
				}
                temp_num = zhiwen_img(temp_array);
                Mode_Use.UART.Send_Data_pFun(2,temp_array,temp_num);
                Mode_Use.TIME.Delay_Ms (1000);
                break;
            case 2:
                temp_num = zhiwen_add_buff (1,temp_array);
                Mode_Use.UART.Send_Data_pFun(2,temp_array,temp_num);
                Mode_Use.TIME.Delay_Ms (100);
				zhiwen_state_sub++;
                break;
            case 3:
                
                break;
            case 4:
                temp_num = zhiwen_ask_search (20,temp_array);
                Mode_Use.UART.Send_Data_pFun(2,temp_array,temp_num);
                Mode_Use.TIME.Delay_Ms (100);
                zhiwen_state_sub ++;
                break;
            case 5:
                // wait id
                break;
            case 6:
                // updata id
                stb_printf("zhiwen get id [%d]",zhiwen_id_get);
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
	case 3:
        {
			if(zhiwen_flag == 1)
            {
				stb_printf("AS608 app [%d][%d] secc\n",zhiwen_state,zhiwen_state_sub);
				zhiwen_state_sub ++;
                zhiwen_flag = 0;
            }
            else
            {
                Mode_Use.TIME.Delay_Ms (10);
            }
			switch (zhiwen_state_sub)
            {
            case 1:
                temp_num = zhiwen_ask_num(temp_array);
                Mode_Use.UART.Send_Data_pFun(2,temp_array,temp_num);
                Mode_Use.TIME.Delay_Ms (100);
				zhiwen_state_sub++;
                break;
			case 2:
                break;
			case 3:
				// updata id
                stb_printf("zhiwen get num [%d]",zhiwen_num);
				zhiwen_num = 0;
                zhiwen_state = 0;
                zhiwen_state_sub = 0;
                break;
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
}

void zhiwen_info_handle (void *data)
{
	u8 temp_dat = *(u8*)data;
	AS608_info_Make_packet_Fun(&zhiwen_pack, temp_dat);
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

