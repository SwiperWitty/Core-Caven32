#include "zhiwen_app.h"
#include "IC_AS608.h"

u8 zhiwen_state = 0;
u8 zhiwen_state_sub = 0;
u8 zhiwen_flag = 0;
u8 zhiwen_id = 0;
u8 zhiwen_num = 0;
AS608_Packet zhiwen_pack;

int zhiwen_img (u8 *data)
{
    int retval = 0;
    if (data != NULL)
    {
        zhiwen_pack.header = AS608_CMD_HEADER;
        zhiwen_pack.address = 0xffffffff;
        zhiwen_pack.type = 1;

        zhiwen_pack.length = 3;
        zhiwen_pack.data[0] = AS608_CMD_GENIMG;

        retval = AS608_info_Split_packet_Fun(zhiwen_pack,data);
        AS608_info_packet_fast_clean_Fun(&zhiwen_pack);
    }
    return retval;
}

int zhiwen_add_buff (u8 buff,u8 *data)
{
    int retval = 0;
    if (data != NULL)
    {
        zhiwen_pack.header = AS608_CMD_HEADER;
        zhiwen_pack.address = 0xffffffff;
        zhiwen_pack.type = 1;

        zhiwen_pack.length = 4;
        zhiwen_pack.data[0] = AS608_CMD_IMG2CHAR;
        zhiwen_pack.data[1] = buff;

        retval = AS608_info_Split_packet_Fun(zhiwen_pack,data);
        AS608_info_packet_fast_clean_Fun(&zhiwen_pack);
    }
    return retval;
}

int zhiwen_ask_search (int num,u8 *data)
{
    int retval = 0;
    if (data != NULL)
    {
        zhiwen_pack.header = AS608_CMD_HEADER;
        zhiwen_pack.address = 0xffffffff;
        zhiwen_pack.type = 1;

        zhiwen_pack.length = 8;
        zhiwen_pack.data[0] = AS608_CMD_SEARCH;
        zhiwen_pack.data[1] = 1;        // buf
        zhiwen_pack.data[2] = 0;
        zhiwen_pack.data[3] = 0;        // s
        zhiwen_pack.data[4] = (num >> 8) & 0xff;
        zhiwen_pack.data[5] = num & 0xff;   // e
        retval = AS608_info_Split_packet_Fun(zhiwen_pack,data);
        AS608_info_packet_fast_clean_Fun(&zhiwen_pack);
    }
    return retval;
}

int zhiwen_reg_model (u8 *data)
{
    int retval = 0;
    if (data != NULL)
    {
        zhiwen_pack.header = AS608_CMD_HEADER;
        zhiwen_pack.address = 0xffffffff;
        zhiwen_pack.type = 1;

        zhiwen_pack.length = 3;
        zhiwen_pack.data[0] = AS608_CMD_REGMODEL;

        retval = AS608_info_Split_packet_Fun(zhiwen_pack,data);
        AS608_info_packet_fast_clean_Fun(&zhiwen_pack);
    }
    return retval;
}

int zhiwen_save_id (int id,u8 *data)
{
    int retval = 0;
    if (data != NULL)
    {
        zhiwen_pack.header = AS608_CMD_HEADER;
        zhiwen_pack.address = 0xffffffff;
        zhiwen_pack.type = 1;

        zhiwen_pack.length = 6;
        zhiwen_pack.data[0] = AS608_CMD_STORE;
        zhiwen_pack.data[1] = 1;        // buf
        zhiwen_pack.data[2] = (id >> 8) & 0xff;
        zhiwen_pack.data[3] = id & 0xff;
        AS608_info_Split_packet_Fun(zhiwen_pack,data);
        AS608_info_packet_fast_clean_Fun(&zhiwen_pack);
    }
    return retval;
}

int zhiwen_ask_num (u8 *data)
{
    int retval = 0;
    if (data != NULL)
    {
        zhiwen_pack.header = AS608_CMD_HEADER;
        zhiwen_pack.address = 0xffffffff;
        zhiwen_pack.type = 1;

        zhiwen_pack.length = 3;
        zhiwen_pack.data[0] = AS608_CMD_TEMPLATE_NUM;
        retval = AS608_info_Split_packet_Fun(zhiwen_pack,data);
        AS608_info_packet_fast_clean_Fun(&zhiwen_pack);
    }
    return retval;
}

int zhiwen_del_num (int start,int num,u8 *data)
{
    int retval = 0;
    if (start > 0)
    {
        zhiwen_pack.header = AS608_CMD_HEADER;
        zhiwen_pack.address = 0xffffffff;
        zhiwen_pack.type = 1;

        zhiwen_pack.length = 7;
        zhiwen_pack.data[0] = AS608_CMD_DELETE;
        zhiwen_pack.data[1] = (start >> 8) & 0xff;
        zhiwen_pack.data[2] = (start) & 0xff;
        zhiwen_pack.data[3] = (num >> 8) & 0xff;
        zhiwen_pack.data[4] = (num) & 0xff;

        retval = AS608_info_Split_packet_Fun(zhiwen_pack,data);
        AS608_info_packet_fast_clean_Fun(&zhiwen_pack);
    }
    return retval;
}

void zhiwen_app_flag_set (u8 flag)
{
    zhiwen_flag = flag;
}

int zhiwen_app_State_machine (Caven_BaseTIME_Type time)
{
    u8 temp_array[100];
    int temp_num = 0;
    switch (zhiwen_state)
    {
    case 1:     // 录入
        {
            if(zhiwen_flag == 1)
            {
                zhiwen_state_sub ++;
                zhiwen_flag = 0;
            }
            else
            {
                zhiwen_state_sub = 0xff;
            }
            switch (zhiwen_state_sub)
            {
            case 1:
                temp_num = zhiwen_img(temp_array);
                Mode_Use.UART.Send_Data_pFun(2,temp_array,temp_num);
                break;
            case 2:
                temp_num = zhiwen_add_buff (1,temp_array);
                Mode_Use.UART.Send_Data_pFun(2,temp_array,temp_num);
                break;
            case 3:
                temp_num = zhiwen_img(temp_array);
                Mode_Use.UART.Send_Data_pFun(2,temp_array,temp_num);
                break;
            case 4:
                temp_num = zhiwen_add_buff (2,temp_array);
                Mode_Use.UART.Send_Data_pFun(2,temp_array,temp_num);
                break;
            case 5:
                temp_num = zhiwen_reg_model (temp_array);
                Mode_Use.UART.Send_Data_pFun(2,temp_array,temp_num);
                break;
            case 6:
                temp_num = zhiwen_save_id (zhiwen_id++,temp_array);
                Mode_Use.UART.Send_Data_pFun(2,temp_array,temp_num);
                break;
            default:
                Mode_Use.TIME.Delay_Ms (500);
                break;
            }
        }
        break;
    case 2:     // 识别
        {
            if(zhiwen_flag == 1)
            {
                zhiwen_state_sub ++;
                zhiwen_flag = 0;
            }
            else
            {
                zhiwen_state_sub = 0xff;
            }
            switch (zhiwen_state_sub)
            {
            case 1:
                temp_num = zhiwen_img(temp_array);
                Mode_Use.UART.Send_Data_pFun(2,temp_array,temp_num);
                break;
            case 2:
                temp_num = zhiwen_add_buff (1,temp_array);
                Mode_Use.UART.Send_Data_pFun(2,temp_array,temp_num);
                break;
            case 3:
                temp_num = zhiwen_ask_search (20,temp_array);
                Mode_Use.UART.Send_Data_pFun(2,temp_array,temp_num);
                break;
            default:
                Mode_Use.TIME.Delay_Ms (500);
                break;
            }
        }
        break;
    default:
        break;
    }
}


