#include "Mode.h"
#include "clock_pic.h"

/*
            软件文件夹->AT32文件夹->Keil工程
项目文件->  
            云端库文件夹...
*/
#ifdef PICTURE
	#define Photo1 gImage_example
	#define Photo2 0
#endif

int temp = 0;

char array_buff[300];
float Val_array[10];

#define Val_YG_x    0
#define Val_YG_y    1
#define Val_Vin     2
#define Val_Vout    3
#define Val_Temp    4
#define Val_ELE     5

void Main_Init(void);

int main (void)
{
    Main_Init();
    while(1)
    {
        Val_array[Val_YG_x]  = 1;
        Val_array[Val_YG_y] = 2;
        Val_array[Val_Vin] = 3;
        Val_array[Val_Vout] = 4;
        Val_array[Val_Temp] = 5;
        Val_array[Val_ELE] = 6;
        
//        Vofa_JustFloat_Show_Fun (Val_array);
        Mode_Use.TIME.Delay_Ms(100);
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
    MODE_UART_DMA_Send_Data_Fun(3, "dubug dma send\n", strlen("dubug dma send\n"));
	Vofa_JustFloat_Init_Fun (6,Debug_Out);                  // YG_x,YG_y,Vin,Vout,Temp,in_temp

#ifdef PICTURE
	Mode_Use.LCD.Show_Picture_pFun(0,0,240,240,Photo1);     // Photo
#endif
}
