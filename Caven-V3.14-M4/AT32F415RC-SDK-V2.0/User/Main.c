#include "Mode.h"
#include "API.h"
#include "pic.h"

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

void Main_Init(void);
int main (void)
{
    Main_Init();
    while(1)
    {
        Mode_User.Sys_Clock.Get_TIME();
		if(Mode_User.KEY.KEY_State(1) == 0)
		{
			if(Mode_User.KEY.KEY_State(1) == 0)
			{
				i++;
				if(i > 100)
				{i = 1;}
				printf("Key num : %d",i);
			}
			do{
				Mode_User.Delay.Delay_ms(5);
				
			}while(Mode_User.KEY.KEY_State(1) == 0);
		}
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

#ifdef PICTURE
	Mode_User.LCD.Show_Picture(0,0,240,240,Photo1);     //Photo
#endif
    printf("system_core_clock: %d \r\n",SystemCoreClock);

//    temp_list = Mode_User.USER_ADC.DATA_List;

}
