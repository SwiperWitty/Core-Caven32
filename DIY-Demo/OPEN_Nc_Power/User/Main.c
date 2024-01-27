#include "Mode.h"
#include "pic.h"
#include "math.h"

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

char array_buff[300];
u16 ADC_array[10];
float Val_set = 0,Val_set_temp;
float Val_MAX = 0;
float Val_array[10];

#define Val_YG_x    0
#define Val_YG_y    1
#define Val_Vin     2
#define Val_Vout    3
#define Val_Temp    4
#define Val_ELE     5

#define YG_DEFAULT	50
#define YG_DIF_MIN	20
#define YG_DIF_MAX	45
#define Divider_RES 8.5     // ((75 + 10) / 10) adc * Divider_RES = vcc
#define MULTIP_RATIO 50     // INA119A1
#define Sampling_RES 0.005  // 0.005Ω
#define Sampling_RES_RATIO 200  // (5/1000)(Ω) -> 200(RATIO)

void SET_Val_Handle (float set_val,float get_val);
void ADC_Data_Handle (void * data);
int PD_Set_Mode (char grade);
void Main_Init(void);

int main (void)
{
    Main_Init();
    sprintf(array_buff,"OPEN Desk Power ");
    Mode_Use.LCD.Show_String_pFun(3,1,array_buff,LCD_BLUE,LCD_WHITE,24);
    while(1)
    {
        Val_array[Val_YG_x]  = (0x0fff - ADC_array[Val_YG_x]) / 40;
        Val_array[Val_YG_y] = ADC_array[Val_YG_y] / 40;
        Val_array[Val_Vin] = Mode_Use.USER_ADC.Conversion_Vol_pFun(ADC_array[Val_Vin]) * Divider_RES;
        Val_array[Val_Vout] = Mode_Use.USER_ADC.Conversion_Vol_pFun(ADC_array[Val_Vout]) * Divider_RES;
        Val_array[Val_Temp] = ADC_array[Val_Temp] / 40;
        Val_array[Val_ELE] = Mode_Use.USER_ADC.Conversion_Vol_pFun(ADC_array[Val_ELE]) * Sampling_RES_RATIO / MULTIP_RATIO;
        Val_array[6] = Mode_Use.USER_ADC.Get_MCU_Temperature_pFun();            // 内温 
        
        Val_MAX = Val_array[Val_Vin];
		temp = Val_array[Val_YG_x] - YG_DEFAULT;
        if (temp > YG_DIF_MIN)
		{
			Val_set_temp += 1;
		}
		else if (-temp > YG_DIF_MIN)
		{
			Val_set_temp -= 1;
		}

		if (YG_KEY_STATE() == 0)
		{
			Val_set = Val_set_temp;
		}
		Val_set = MIN(Val_set,Val_MAX);
		Val_set = MAX(Val_set,1);
		SET_Val_Handle (Val_set,Val_array[Val_Vout]);

        Vofa_JustFloat_Show_Fun (Val_array);
        temp = 3;
        sprintf(array_buff,"Vin  -> %6.3f V ",Val_array[Val_Vin]);
        Mode_Use.LCD.Show_String_pFun(2,temp++,array_buff,LCD_RED,LCD_WHITE,24);
        sprintf(array_buff,"Vout -> %6.3f V ",Val_array[Val_Vout]);
        Mode_Use.LCD.Show_String_pFun(2,temp++,array_buff,LCD_RED,LCD_WHITE,24);
        sprintf(array_buff,"TEMP -> %5.3f C ",Val_array[6]);
        Mode_Use.LCD.Show_String_pFun(2,temp++,array_buff,LCD_BROWN,LCD_WHITE,24);
        sprintf(array_buff,"ELE  -> %5.3f A ",Val_array[Val_ELE]);
        Mode_Use.LCD.Show_String_pFun(2,temp++,array_buff,LCD_GREEN,LCD_WHITE,24);
		sprintf(array_buff,"VAL set :%2.2f(%2.2f)",Val_set,Val_set_temp);
        Mode_Use.LCD.Show_String_pFun(1,temp++,array_buff,LCD_BLACK,LCD_WHITE,24);
        sprintf(array_buff,"x:%3.0f,y:%3.0f ",Val_array[Val_YG_x],Val_array[Val_YG_y]);
        Mode_Use.LCD.Show_String_pFun(4,temp+1,array_buff,LCD_BLACK,LCD_WHITE,24);
        
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
	TIM3_PWMx_SetValue(1,1000);              
	User_GPIO_Init(ENABLE);
	PD_Set_Mode (2);
    
	Vofa_JustFloat_Init_Fun (7,Debug_Out);     // Vin,Vout,Temp,YG_x,YG_y,YG_key,in_temp
    DC_OUT_OFF();
#ifdef PICTURE
	Mode_Use.LCD.Show_Picture_pFun(0,0,240,240,Photo2);     //Photo
#endif

}

void SET_Val_Handle (float set_val,float get_val)
{
	int set_num;
    static int set_diff_max = 0;
	float f_temp = (set_val / 18.0) * 1000;
	set_num = f_temp;
    
	f_temp = get_val - set_val;
	if (fabsf(f_temp) < 0.2)
	{
		DC_OUT_ON();		// 启动
	}
    else					// 调
    {
		if (f_temp > 0)
		{
			set_diff_max -= 5;
		}
		else
		{
			set_diff_max += 5;
		}
        DC_OUT_OFF();
        set_num = MIN(set_diff_max,100);
		
        set_num += set_diff_max;
		set_num = 1000 - set_num;
        set_num = MIN(set_num,1000);
        TIM3_PWMx_SetValue(1,set_num);
    }
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
