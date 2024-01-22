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
int PD_Set_Mode (char grade);


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
	Mode_Init.Steering_Engine(ENABLE);
	
	Mode_Use.UART.Send_String_pFun(DEBUG_OUT,"Hello world ! \n");
	Mode_Use.Steering_Engine.Set_Angle(1,0);
	Mode_Use.Steering_Engine.Set_Angle(2,90);
	
	TIM3_PWM_Start_Init(1000,12,ENABLE);
	TIM3_PWMx_SetValue(1,500);
	User_GPIO_Init(ENABLE);
	PD_Set_Mode (3);
	
#ifdef PICTURE
	Mode_Use.LCD.Show_Picture_pFun(0,0,240,240,Photo2);     //Photo
#endif

}

int PD_Set_Mode (char grade)
{
	int val;
	switch (grade)
	{
		case 1:
		{
			PD_CGF_A_L();
			PD_CGF_B_L();
			val = 9;
		}
			break;
		case 2:
		{
			PD_CGF_A_L();
			PD_CGF_B_H();
			val = 12;
		}
			break;
		case 3:
		{
			PD_CGF_A_H();
			PD_CGF_B_L();
			val = 20;
		}
			break;
		default :
			PD_CGF_A_L();
			PD_CGF_B_L();
			val = 9;
			break;
	}
	return val;
}
