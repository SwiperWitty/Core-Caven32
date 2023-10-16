#include "Mode.h"
#include "API.h"
#include "pic.h"

#include "MODE_Motor_BYJ.h"

/*
            软件文件夹->AT32文件夹->Keil工程
项目文件->  
            云端库文件夹...
*/
#ifdef PICTURE
#define Photo1 gImage_maoL
#define Photo2 gImage_fan
#endif

int temp = 0;
int i;

u8 array_buff[300];
u8 array_r_temp[300];
u8 array_t_temp[300];

Caven_info_packet_Type standard = {
    .Head =0xFA8A,
    .Versions = 1,
    .Type = 1,
    .Addr = 0xF2,
    .Size = 300,
};
Caven_info_packet_Type get_pack_temp;
Caven_info_packet_Type send_pack_temp = {
    .Head = 0xFA8A,
    .Versions = 0x01,
    .Type = 1,
    .Addr = 0xF0,
    .Cmd = 0,
    .Cmd_sub = 7,
    .Size = 0x02,
    
    .Result = 0,
    .End_crc = 0x1122,
};


void Main_Init(void);
int main (void)
{
    Main_Init();

    while(1)
    {
        Mode_User.Sys_Clock.Get_TIME();
		if(Mode_User.KEY.K_State(1) == 0)
		{
			if(Mode_User.KEY.K_State(1) == 0)
			{
				i++;
				if(i > 100)
				{i = 1;}
				printf("Key num : %d",i);
//                Mode_User.USB_HID.Keyboard_Send_String("USB-HID Hello world !\r\n");
			}
			do{
				Mode_User.Delay.Delay_ms(5);
				
			}while(Mode_User.KEY.K_State(1) == 0);
		}
        if(get_pack_temp.Result &= 0x80)
        {
            printf("Head        : %x \n",get_pack_temp.Head);
            printf("Versions    : %x \n",get_pack_temp.Versions);
            printf("Type        : %x \n",get_pack_temp.Type);
            printf("Addr        : %x \n",get_pack_temp.Addr);
            printf("Cmd         : %x \n",get_pack_temp.Cmd);
            printf("Cmd_sub     : %x \n",get_pack_temp.Cmd_sub);
            printf("Size        : %x \n",get_pack_temp.Size);
            printf("p_Data_addr : %p \n",get_pack_temp.p_Data);
            printf("Result      : %x \n",get_pack_temp.Result);
            printf("End_crc     : %x \n",get_pack_temp.End_crc);

            temp = Caven_info_Split_packet_Fun(get_pack_temp, array_buff);
            Mode_User.UART.WAY_Send_Data(3,array_buff,temp);
            Caven_info_packet_clean_Fun(&get_pack_temp);
        }
        Mode_User.LED.LED_SET(2,ENABLE);
        Mode_User.Delay.Delay_ms(200);
        Mode_User.LED.LED_SET(2,DISABLE);
        Mode_User.Delay.Delay_ms(20);
        
        #ifdef PICTURE
		if(i%2 == 1 && temp != i)
		{
			temp = i;
			Mode_User.LCD.Show_Picture(0,0,240,240,Photo2);     //Photo
		}
		else if(i%2 == 0 && temp != i)
		{
			temp = i;
			Mode_User.LCD.Show_Picture(0,0,240,240,Photo1);     //Photo
		}
        #endif
    }
}

void Uart_Caven_info_packet_Handle (u8 data)
{
    Caven_info_Make_packet_Fun(standard, &get_pack_temp,data);
}

void Main_Init(void)
{
    system_clock_config();
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
    Mode_Index();
    API_Index();
    
    Mode_Init.Sys_Clock(ENABLE);
	
    Mode_Init.LCD(ENABLE);
	Mode_Init.UART(DEBUG_OUT,115200,&Uart_Caven_info_packet_Handle,ENABLE);   
    Mode_Init.KEY(1,ENABLE);
    Mode_Init.LED(ENABLE);

    
//    RTC8564_Init (ENABLE);
    Mode_User.Delay.Delay_ms(50);
    Caven_info_packet_index_Fun(&get_pack_temp, array_r_temp);
    Caven_info_packet_index_Fun(&send_pack_temp, array_t_temp);
    Caven_info_packet_clean_Fun(&get_pack_temp);

//    Motor_BYJ_Init(1);
//    
//    Motor_BYJ_Drive(1,0,360);
//    Motor_BYJ_Drive(0,0,360);

#ifdef PICTURE
	Mode_User.LCD.Show_Picture(0,0,240,240,Photo1);     //Photo
#endif
    printf("system_core_clock: %d \r\n",SystemCoreClock);
    Mode_User.Delay.Delay_ms(200);
}
