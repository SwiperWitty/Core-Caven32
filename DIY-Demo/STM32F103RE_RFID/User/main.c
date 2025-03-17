#include "Mode.h"
#include "API.h"
#include "GX_info_frame.h"

char temp_array[100];
void Main_Init(void);

const GX_info_packet_Type gx_standard = {
    .Head = 0x5A,
    .Prot_W_Versions = 0x01,    // 版本
    .dSize = 300,          // 最大长度
};

uint8_t RS232_pack_buff[512];
GX_info_packet_Type rs232_get_pack;
uint8_t UHF_pack_buff[512];
GX_info_packet_Type UHF_get_pack;
Caven_BaseTIME_Type Now_time; 
int main(void)
{
	Main_Init();
    
	while (1)
	{
        Now_time = Mode_Use.TIME.Get_BaseTIME_pFun();
        if(rs232_get_pack.Result & 0x50)        // RS232 TO RFID
        {
            Mode_Use.UART.Send_Data_pFun(3,rs232_get_pack.p_AllData,rs232_get_pack.Get_num);
            GX_info_packet_clean_Fun (&rs232_get_pack);
        }
        if(UHF_get_pack.Result & 0x50)          // RFID TO RS232
        {
            Mode_Use.UART.Send_Data_pFun(1,UHF_get_pack.p_AllData,UHF_get_pack.Get_num);
            GX_info_packet_clean_Fun (&UHF_get_pack);
        }
	}
}

Caven_BaseTIME_Type RS232_Get_last_time;
void RS232_handle_Fun (void *data)
{
    int temp_retval = 0;
    uint8_t temp_data = *(uint8_t *)data;
    int temp_num = Now_time.SYS_Sec - RS232_Get_last_time.SYS_Sec;
    if (temp_num > 1 && RS232_Get_last_time.SYS_Sec)       // 间隔超过1s
    {
        GX_info_packet_fast_clean_Fun(&rs232_get_pack);
        RS232_Get_last_time = Now_time;
    }
    temp_retval = GX_info_Make_packet_Fun(gx_standard, &rs232_get_pack, temp_data);
    if (temp_retval == 0XFF)
    {
        RS232_Get_last_time = Now_time;
    }
    else
    {

    }
}

Caven_BaseTIME_Type RFID_Get_last_time;
void RFID_handle_Fun (void *data)
{
    int temp_retval = 0;
    uint8_t temp_data = *(uint8_t *)data;
    int temp_num = Now_time.SYS_Sec - RFID_Get_last_time.SYS_Sec;
    if (temp_num > 1 && RFID_Get_last_time.SYS_Sec)       // 间隔超过1s
    {
        GX_info_packet_fast_clean_Fun(&UHF_get_pack);
        RFID_Get_last_time = Now_time;
    }
    temp_retval = GX_info_Make_packet_Fun(gx_standard, &UHF_get_pack, temp_data);
    if (temp_retval == 0XFF)
    {
        RFID_Get_last_time = Now_time;    
    }
    else
    {

    }
}

void Main_Init(void)
{
    int reverse = 0;
	// 直接使用外部时钟
	Mode_Index();
    reverse |= Mode_Init.TIME(ENABLE);
    
    Mode_Init.UART(1,115200,ENABLE);
    Mode_Init.UART(3,115200,ENABLE);
	//
    GX_info_packet_index_Fun(&rs232_get_pack, RS232_pack_buff);
    GX_info_packet_fast_clean_Fun(&rs232_get_pack);
    GX_info_packet_index_Fun(&UHF_get_pack, UHF_pack_buff);
    GX_info_packet_fast_clean_Fun(&UHF_get_pack);
    Mode_Use.UART.Receive_Bind_pFun(1,RS232_handle_Fun);
    Mode_Use.UART.Receive_Bind_pFun(3,RFID_handle_Fun);
    Mode_Use.UART.Send_String_pFun(1,"hello \n");
}
