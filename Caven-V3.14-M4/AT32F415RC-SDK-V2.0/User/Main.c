#include "Mode.h"
#include "pic.h"

/*
            软件文件夹->AT32文件夹->Keil工程
项目文件->  
            云端库文件夹...
*/
#ifdef PICTURE
	#define Photo1 0
	#define Photo2 gImage_pai
#endif

int temp = 0;
int i;

u8 array_buff[300];
u8 array_r_temp[300];
u8 array_t_temp[300];


void Main_Init(void);
int main (void)
{
    Main_Init();

    while(1)
    {

    }
}

void Main_Init(void)
{
    system_clock_config();
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
    Mode_Index();
    
    Mode_Init.TIME(ENABLE);
	Mode_Init.UART(DEBUG_OUT,115200,ENABLE);
	Mode_Init.LCD(ENABLE);
	
	Mode_Use.UART.Send_String_pFun(DEBUG_OUT,"Hello world ! \n");
#ifdef PICTURE
	Mode_Use.LCD.Show_Picture_pFun(0,0,240,240,Photo2);     //Photo
#endif

}
