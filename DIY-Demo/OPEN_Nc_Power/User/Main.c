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
u8 array_buff[300];
u16 ADC_array[10];
float Val_array[10];

#define Val_YG_x    0
#define Val_YG_y    1
#define Val_Vin     2
#define Val_Vout    3
#define Val_Temp    4
#define Val_ELE     5

void ADC_Data_Handle (void * data);
int PD_Set_Mode (char grade);
void Main_Init(void);

int main (void)
{
    Main_Init();

    while(1)
    {
        Val_array[Val_YG_x]  = ADC_array[Val_YG_x];
        Val_array[Val_YG_y] = ADC_array[Val_YG_y];
        Val_array[Val_Vin] = ADC_array[Val_Vin];
        Val_array[Val_Vout] = ADC_array[Val_Vout];
        Val_array[Val_Temp] = ADC_array[Val_Temp];
        Val_array[Val_ELE] = ADC_array[Val_ELE];
        
        Vofa_JustFloat_Show_Fun (Val_array);
//        Mode_Use.TIME.Delay_Ms(100);
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
	Mode_Init.User_ADC(ENABLE);
    
	Mode_Use.UART.Send_String_pFun(DEBUG_OUT,"Hello world ! \n");
	Mode_Use.Steering_Engine.Set_Angle(1,0);
	Mode_Use.Steering_Engine.Set_Angle(2,90);
	Mode_Use.USER_ADC.Receive_Bind_pFun(ADC_Data_Handle);
    
	TIM3_PWM_Start_Init(1000,12,ENABLE);
	TIM3_PWMx_SetValue(1,500);
	User_GPIO_Init(ENABLE);
	PD_Set_Mode (2);
    
	Vofa_JustFloat_Init_Fun (6,Debug_Out);     // Vin,Vout,Temp,YG_x,YG_y,YG_key
    
#ifdef PICTURE
	Mode_Use.LCD.Show_Picture_pFun(0,0,240,240,Photo2);     //Photo
#endif

}

void ADC_Data_Handle (void * data)
{
    memcpy(ADC_array,data,sizeof(ADC_array));
    if(ADC_array[Val_ELE] > 5000)
    {
        DC_5V_OFF();
    }
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
