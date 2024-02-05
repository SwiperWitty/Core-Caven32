#include "power_app.h"

#define YG_DEFAULT	50
#define YG_DIF_MIN	10
#define YG_DIF_MAX	45
#define Divider_RES 8.5     // ((75 + 10) / 10) adc * Divider_RES = vcc
#define MULTIP_RATIO 55.5	// INA119A1
#define Sampling_RES 0.005	// 0.005Ω
#define Sampling_RES_RATIO 200.0	// (5/1000)(Ω) -> 200(RATIO)

int show_cycle = 0;
int yg_lock = 0; 

u16 ADC_array[10];
float Val_OUT_set = 0,Val_OUT_temp = 6.0;
float Val_ELE_set = 2.4,Val_ELE_temp = 2.4;

int Val_YG_x,Val_YG_y;
float Val_IN_f = 0;
float Val_OUT_f = 0;
float Val_ELE_f = 0;
float Val_TEM_f = 0;
float Val_array[10];

float Val_temp_f;
     
float Val_OUT_array[10];
float Val_ELE_array[10];
float Val_TEM_array[10];
char Val_OUT_num = 0;
char Val_ELE_num = 0;
char Val_TEM_num = 0;

void ADC_Data_Handle (void * data);
int PD_Set_Mode (char grade);
void SET_Val_Handle (float set_val,float get_val);

int Power_app_init (int Set)
{
    int retval = 0;
	
	Mode_Init.User_ADC(ENABLE);
	Mode_Use.USER_ADC.Receive_Bind_pFun(ADC_Data_Handle);
	
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

void ADC_Data_Handle (void * data)
{
    memcpy(ADC_array,data,sizeof(ADC_array));
    if(ADC_array[6] > 2000)
    {
        DC_5V_OFF();
    }
}

int Power_app (Caven_App_Type * message)
{
    int retval = 0;
	int temp = 0,num = 0;
	char array_buff[300];
	Val_array[temp++]  = (0x0fff - ADC_array[num++]) / 40;       // 0
	Val_array[temp++] = ADC_array[num++] / 40;
	Val_array[temp++] = Mode_Use.USER_ADC.Conversion_Vol_pFun(ADC_array[num++]) * Divider_RES;
	Val_array[temp++] = Mode_Use.USER_ADC.Conversion_Vol_pFun(ADC_array[num++]) * Divider_RES;
	Val_array[temp++] = ADC_array[num++] / 40;					//ntc
	Val_array[temp++] = Mode_Use.USER_ADC.Conversion_Vol_pFun(ADC_array[num++]) * (Sampling_RES_RATIO / MULTIP_RATIO);
	Val_array[temp++] = Mode_Use.USER_ADC.Get_MCU_Temperature_pFun();            // 内温 
	//
	Val_IN_f = Val_array[2];
	//
	Val_YG_x = Val_array[0];
	Val_YG_y = Val_array[1];
	temp = Val_YG_x - YG_DEFAULT;
	if ((temp > YG_DIF_MIN) && (yg_lock > 0))		// 左
	{
		yg_lock = 0;
		Val_OUT_temp += 1.0;
	}
	else if ((-temp > YG_DIF_MIN) && (yg_lock > 0))	// 右
	{
		yg_lock = 0;
		Val_OUT_temp -= 1.0;
	}
	else if(abs(temp) < YG_DIF_MIN)					// 空闲　
	{
		yg_lock++;
		yg_lock = MIN(yg_lock,100);
	}
	Val_OUT_temp = MIN(Val_OUT_temp,Val_IN_f);
	Val_OUT_temp = MAX(Val_OUT_temp,1);
	// 
	if (YG_KEY_STATE() == 0)
	{
		Val_OUT_set = Val_OUT_temp;
		do{
			
		}while(YG_KEY_STATE() == 0);
	}
	// 
	temp = Data_Median_filtering_Handle (Val_array[3],Val_OUT_array,&Val_OUT_f,&Val_OUT_num,10);
	if(temp)
	{
		SET_Val_Handle (Val_OUT_set,Val_OUT_f);
	}
	temp = Data_Median_filtering_Handle (Val_array[5],Val_ELE_array,&Val_ELE_f,&Val_ELE_num,10);
	temp = Data_Median_filtering_Handle (Val_array[6],Val_TEM_array,&Val_TEM_f,&Val_TEM_num,10);
	//
	Vofa_JustFloat_Show_Fun (Val_array);
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

