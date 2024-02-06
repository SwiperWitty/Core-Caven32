#include "power_app.h"


#define Divider_RES 8.5     // ((75 + 10) / 10) adc * Divider_RES = vcc
#define MULTIP_RATIO 55.5	// INA119A1
#define Sampling_RES 0.005	// 0.005Ω
#define Sampling_RES_RATIO 200.0	// (5/1000)(Ω) -> 200(RATIO)

int show_cycle = 0;
float Val_IN_f = 0;
float Val_OUT_f = 0;
float Val_ELE_f = 0;
float Val_TEM_f = 0;
float Val_OUT_set = 0,Val_OUT_temp = 6.0;
float Val_ELE_set = 2.4,Val_ELE_temp = 2.4;

int PD_Set_Mode (char grade);
void SET_Val_Handle (float set_val,float get_val);

int Power_app_init (int Set)
{
    int retval = 0;
	
	TIM3_PWM_Start_Init(1000,12,ENABLE);
	TIM3_PWMx_SetValue(1,1000);              
	User_GPIO_Init(ENABLE);
	PD_Set_Mode (3);
	DC_OUT_OFF();
	
	Vofa_JustFloat_Init_Fun (7,Debug_Out);     // Vin,Vout,Temp,YG_x,YG_y,YG_key,in_temp
    
    return retval;
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

void SET_Val_Handle (float set_val,float get_val)
{
	int set_num,temp_num;
	float f_temp = (set_val / 18.0) * 1000;
	float f_diff_temp = 0;
	static int set_diff_max = 0;
	static float Last_set_val;
	if(Last_set_val != set_val)
	{
		DC_OUT_OFF();
		Last_set_val = set_val;
		set_diff_max = 0;
	}
    
	set_num = f_temp;				// pwm 
	f_temp = get_val - set_val;
	f_diff_temp = fabsf(f_temp);
	if (f_temp < 0.03 && f_temp > 0)
	{
		DC_OUT_ON();		// 启动
	}
    else					// 调
    {	
		if (f_diff_temp > 1.0)
		{
			temp_num = 30;
		}
		else if (f_diff_temp > 0.2)
		{
			temp_num = 10;
		}
		else
		{
			temp_num = 1;
		}
		if(f_temp > 0)
		{
			temp_num = -temp_num;
		}
			
		set_diff_max += temp_num;
        set_diff_max = MIN(set_diff_max,100);		// set_diff_max 微调值最多100
		
        set_num += set_diff_max;
		set_num = 1000 - set_num;
        set_num = MIN(set_num,1000);
        TIM3_PWMx_SetValue(1,set_num);
    }
}



int Power_app (Caven_App_Type * message)
{
    int retval = 0;
	int temp = 0,num = 0;
	char array_buff[300];

	//
//	Vofa_JustFloat_Show_Fun (Val_array);
	//
	show_cycle++;
	if(show_cycle > 100)	// 选中时颜色 LCD_BRRED，默认颜色 LCD_BLUE，LCD_MAGENTA
	{
		show_cycle = 0;
		temp = 2;
		sprintf(array_buff,"Vin :%6.2fv PD-20",Val_IN_f);
		Mode_Use.LCD.Show_String_pFun(1,temp++,array_buff,LCD_Word_Color,LCD_Back_Color,24);
		
		sprintf(array_buff,"Now Val:%6.2fv ",Val_OUT_f);
		Mode_Use.LCD.Show_String_pFun(1,temp++,array_buff,LCD_Word_Color,LCD_Back_Color,24);
		sprintf(array_buff,"Set Val:%6.2fv  ",Val_OUT_temp);
		Mode_Use.LCD.Show_String_pFun(1,temp++,array_buff,LCD_Word_Color,LCD_Back_Color,24);
		sprintf(array_buff,"Now ELE:%5.2fA ",Val_ELE_f);
		Mode_Use.LCD.Show_String_pFun(1,temp++,array_buff,LCD_MAGENTA,LCD_Back_Color,24);
		sprintf(array_buff,"Set ELE:%5.2fA(Max)",Val_ELE_temp);
		Mode_Use.LCD.Show_String_pFun(1,temp++,array_buff,LCD_MAGENTA,LCD_Back_Color,24);
		
		sprintf(array_buff,"TEMP:%6.2f C ",Val_TEM_f);
		Mode_Use.LCD.Show_String_pFun(1,temp++,array_buff,LCD_Word_Color,LCD_Back_Color,24);
		
		sprintf(array_buff,"<-Beak [user/power]");
		Mode_Use.LCD.Show_String_pFun(0,9,array_buff,LCD_Word_Color,LCD_Back_Color,24);
	}
    return retval;
}

