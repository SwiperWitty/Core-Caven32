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
int show_cycle = 0;
int yg_lock = 0; 

char array_buff[300];
u16 ADC_array[10];
float Val_set = 0,Val_set_temp = 6.0;
float Val_MAX_f = 0;
float Val_OUT_f = 0;
float Val_ELE_f = 0;
float Val_array[10];
float Val_OUT_array[10];
float Val_ELE_array[10];
char Val_OUT_num = 0;
char Val_ELE_num = 0;

#define LCD_DEF		LCD_GRAY
#define Val_YG_x    0
#define Val_YG_y    1
#define Val_Vin     2
#define Val_Vout    3
#define Val_Temp    4
#define Val_ELE_N	5

#define YG_DEFAULT	50
#define YG_DIF_MIN	10
#define YG_DIF_MAX	45
#define Divider_RES 8.5     // ((75 + 10) / 10) adc * Divider_RES = vcc
#define MULTIP_RATIO 55.5	// INA119A1
#define Sampling_RES 0.005	// 0.005Ω
#define Sampling_RES_RATIO 200.0	// (5/1000)(Ω) -> 200(RATIO)

int Median_filtering_Handle (float data,float *array,float *reverse,char *run,char num);
void Simple_SET_Val_Handle (float set_val);
void SET_Val_Handle (float set_val,float get_val);
void ADC_Data_Handle (void * data);
int PD_Set_Mode (char grade);
void Main_Init(void);

int main (void)
{
    Main_Init();
    sprintf(array_buff,"Caven Desk ");
    Mode_Use.LCD.Show_String_pFun(5,0,array_buff,LCD_BLUE,BACK_COLOR,24);

    while(1)
    {
        Val_array[Val_YG_x]  = (0x0fff - ADC_array[Val_YG_x]) / 40;
        Val_array[Val_YG_y] = ADC_array[Val_YG_y] / 40;
        Val_array[Val_Vin] = Mode_Use.USER_ADC.Conversion_Vol_pFun(ADC_array[Val_Vin]) * Divider_RES;
        Val_array[Val_Vout] = Mode_Use.USER_ADC.Conversion_Vol_pFun(ADC_array[Val_Vout]) * Divider_RES;
        Val_array[Val_Temp] = ADC_array[Val_Temp] / 40;
        Val_array[Val_ELE_N] = Mode_Use.USER_ADC.Conversion_Vol_pFun(ADC_array[Val_ELE_N]) * (float)(Sampling_RES_RATIO / MULTIP_RATIO);
        Val_array[6] = Mode_Use.USER_ADC.Get_MCU_Temperature_pFun();            // 内温 
        
        Val_MAX_f = Val_array[Val_Vin];
		temp = Val_array[Val_YG_x] - YG_DEFAULT;
		
        if ((temp > YG_DIF_MIN) && (yg_lock > 0))		// 左
		{
			yg_lock = 0;
			Val_set_temp += 1.0;
		}
		else if ((-temp > YG_DIF_MIN) && (yg_lock > 0))	// 右
		{
			yg_lock = 0;
			Val_set_temp -= 1.0;
		}
		else if(abs(temp) < YG_DIF_MIN)					// 空闲　
		{
			yg_lock++;
			yg_lock = MIN(yg_lock,100);
		}
		Val_set_temp = MIN(Val_set_temp,Val_MAX_f);
		Val_set_temp = MAX(Val_set_temp,1);
		// 
		if (YG_KEY_STATE() == 0)
		{
			Val_set = Val_set_temp;
			do{
				
			}while(YG_KEY_STATE() == 0);
		}
		// 
		temp = Median_filtering_Handle (Val_array[Val_Vout],Val_OUT_array,&Val_OUT_f,&Val_OUT_num,10);
		if(temp)
		{
			SET_Val_Handle (Val_set,Val_OUT_f);
		}

		//
		temp = Median_filtering_Handle (Val_array[Val_ELE_N],Val_ELE_array,&Val_ELE_f,&Val_ELE_num,10);
		
        Vofa_JustFloat_Show_Fun (Val_array);
		show_cycle++;
		if(show_cycle > 100)	// 选中时颜色 LCD_BRRED，默认颜色 LCD_BLUE，LCD_MAGENTA
		{
			show_cycle = 0;
			temp = 2;
			sprintf(array_buff,"Vin :%6.2fv PD-20",Val_array[Val_Vin]);
			Mode_Use.LCD.Show_String_pFun(1,temp++,array_buff,LCD_DEF,BACK_COLOR,24);
			
			sprintf(array_buff,"Now Val:%6.2fv ",Val_array[Val_Vout]);
			Mode_Use.LCD.Show_String_pFun(1,temp++,array_buff,LCD_DEF,BACK_COLOR,24);
			sprintf(array_buff,"Set Val:%6.2fv  ",Val_set_temp);
			Mode_Use.LCD.Show_String_pFun(1,temp++,array_buff,LCD_DEF,BACK_COLOR,24);
			sprintf(array_buff,"Now ELE:%5.2fA ",Val_ELE_f);
			Mode_Use.LCD.Show_String_pFun(1,temp++,array_buff,LCD_MAGENTA,BACK_COLOR,24);
			sprintf(array_buff,"Set ELE:%5.2fA(Max)",2.4);
			Mode_Use.LCD.Show_String_pFun(1,temp++,array_buff,LCD_MAGENTA,BACK_COLOR,24);
			
			sprintf(array_buff,"TEMP:%6.2f C ",Val_array[6]);
			Mode_Use.LCD.Show_String_pFun(1,temp++,array_buff,LCD_DEF,BACK_COLOR,24);
			
			sprintf(array_buff,"<-Beak [user/power]");
			Mode_Use.LCD.Show_String_pFun(0,9,array_buff,LCD_DEF,BACK_COLOR,24);
        }
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
	PD_Set_Mode (3);
    
	Vofa_JustFloat_Init_Fun (7,Debug_Out);     // Vin,Vout,Temp,YG_x,YG_y,YG_key,in_temp
    DC_OUT_OFF();
#ifdef PICTURE
	Mode_Use.LCD.Show_Picture_pFun(0,0,240,240,Photo2);     //Photo
#endif

}

int Median_filtering_Handle (float data,float *array,float *reverse,char *run,char num)
{
	int retval = 0;
	int temp_run = *run;
	char max_sort = 0;
	char min_sort = 0;
	float temp_data_f;
	if(temp_run < num)
	{
		array[temp_run++] = data;
	}
	else
	{
		for(int i = 0;i < num;i++)
		{
			temp_data_f = MAX(array[i],array[max_sort]);
			if(temp_data_f == array[i])
			{
				max_sort = i;
			}
		}
		for(int i = 0;i < num;i++)
		{
			temp_data_f = MIN(array[i],array[min_sort]);
			if(temp_data_f == array[i])
			{
				min_sort = i;
			}
		}
		array[max_sort] = 0;
		array[min_sort] = 0;
		temp_data_f = 0;
		for(int i = 0;i < num;i++)
		{
			temp_data_f += array[i];
		}
		temp_data_f /= (num - 2);
		*reverse = temp_data_f;
		temp_run = 0;
		retval = 1;
	}
	*run = temp_run;
	return retval;
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

void Simple_SET_Val_Handle (float set_val)
{
	int set_num;
	float f_temp = (set_val / 18.0) * 1000;
	set_num = f_temp;
    
	set_num = 1000 - set_num;
	set_num = MIN(set_num,1000);
	TIM3_PWMx_SetValue(1,set_num);
	DC_OUT_ON();
}

void ADC_Data_Handle (void * data)
{
    memcpy(ADC_array,data,sizeof(ADC_array));
    if(ADC_array[Val_ELE_N] > 2000)
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
