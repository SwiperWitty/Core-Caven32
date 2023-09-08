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
typedef struct flash_data
{
    U8 start;
    int L32;
    char array[10];
    U8 end;
} test_flash;

test_flash temp_a;
test_flash temp_default = {
    .start = 0x5a,
    .L32 = NULL,
    .array = "12345\n",
    .end = 0xa5,
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
        Mode_User.LED.LED_SET(2,ENABLE);
        Mode_User.Delay.Delay_ms(20);
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

void Main_Init(void)
{
    system_clock_config();
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
    Mode_Index();
    API_Index();
    
    Mode_Init.Sys_Clock(ENABLE);
	
    Mode_Init.LCD(ENABLE);
	Mode_Init.UART(DEBUG_OUT,115200,ENABLE);
    Mode_Init.KEY(1,ENABLE);
    Mode_Init.LED(ENABLE);

    
    RTC8564_Init (ENABLE);
    Mode_User.Delay.Delay_ms(50);
    
    Flash_Read_Data (FLASH_DATA_START,&temp_a,sizeof(temp_a));
    printf("get flash: %x \r\n",temp_a.start);
    
    if(temp_a.start == 0x5a && temp_a.end == 0xa5)
    {
        temp_a.start += 1;
        temp_a.L32 = (int)&temp_a.start;
    }
    else
    {
        temp_a = temp_default;
    }
    Flash_Save_Data (FLASH_DATA_START,&temp_a,sizeof(temp_a));
    
    
//    Motor_BYJ_Init(1);
//    
//    Motor_BYJ_Drive(1,0,360);
//    Motor_BYJ_Drive(0,0,360);

#ifdef PICTURE
	Mode_User.LCD.Show_Picture(0,0,240,240,Photo1);     //Photo
#endif
    printf("system_core_clock: %d \r\n",SystemCoreClock);
    
}
