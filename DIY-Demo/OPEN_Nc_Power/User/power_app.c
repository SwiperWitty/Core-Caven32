#include "power_app.h"


#define Divider_RES 8.5     // ((75 + 10) / 10) adc * Divider_RES = vcc
#define MULTIP_RATIO 51		// INA119A1
#define Sampling_RES 0.005	// 0.005Ω
#define Sampling_RES_RATIO 200.0	// (5/1000)(Ω) -> 200(RATIO)

Power_Control_Type power_config;

int PD_Set_Mode (char grade);
void Power_OUT_Set (char num,char set);
int SET_Val_Handle (float set_val,float get_val);

int Power_app_init (int Set)
{
    int retval = 0;
	
	TIM3_PWM_Start_Init(1000,12,ENABLE);
	TIM3_PWMx_SetValue(1,100);
//	User_GPIO_Init(ENABLE);
	User_GPIO_config(3,0,1);	// 5v
	User_GPIO_config(3,1,1);	// out
	User_GPIO_config(3,12,1);	// a
	User_GPIO_config(4,2,1);
	Power_OUT_Set (1,1);
	Power_OUT_Set (2,0);
	
	PD_Set_Mode (3);

	memset(&power_config,0,sizeof(power_config));

	Vofa_JustFloat_Init_Fun (Debug_Out);     // Vin,Vout,Temp,YG_x,YG_y,YG_key,in_temp
    return retval;
}

int Power_app_exit (int *falg)
{
    int retval = 0;
	
	TIM3_PWM_Start_Init(1000,12,DISABLE);
	PD_Set_Mode (1);
	Power_OUT_Set (1,1);
	Power_OUT_Set (2,0);
//	DC_OUT_OFF();
	memset(&power_config,0,sizeof(power_config));
	*falg = 1;

    return retval;
}

int PD_Set_Mode (char grade)
{
	int val;
	switch (grade)
	{
		case 1:
		{
			User_GPIO_set(3,12,0);	// a
			User_GPIO_set(4,2,0);
			val = 9;
		}
			break;
		case 2:
		{
			User_GPIO_set(3,12,0);	// a
			User_GPIO_set(4,2,1);
			val = 12;
		}
			break;
		case 3:
		{
			User_GPIO_set(3,12,1);	// a
			User_GPIO_set(4,2,0);
			val = 20;
		}
			break;
		default :
			User_GPIO_set(3,12,0);	// a
			User_GPIO_set(4,2,0);
			val = 9;
			break;
	}
	return val;
}

/*
	1:5v的开关
	2:out的开关
*/
void Power_OUT_Set (char num,char set)
{
	if (num == 1)
	{
		if(set)
		{
			User_GPIO_config(3,0,1);	// 5v
		}
		else
		{
			User_GPIO_config(3,0,0);	// 5v
		}
	}
	else if (num == 2)
	{
		if(set)
		{
			User_GPIO_config(3,1,1);	// out
		}
		else
		{
			User_GPIO_config(3,1,0);	// out
		}
	}
}

int show_cycle = 0;

float Val_temp = 0;

float Val_OUT_array[10];
float Val_ELE_array[10];
float Val_TEM_array[10];
char Val_OUT_num = 0;
char Val_ELE_num = 0;
char Val_TEM_num = 0;

int Power_app (Caven_App_Type * message)
{
    int retval = 0;
    int run_num = 0;
    int temp_num;
    static int first = 1;
    Caven_Control_Type control;
	u16 adc_buff[10];
    char string_temp[50];
    if (message != NULL)
    {
        memcpy(&control,message->p_Data,sizeof(control));
		memcpy(adc_buff,control.Control_value,sizeof(adc_buff));
        if(first)
        {
            message->str_switch = 1;
            message->cursor = 4;
            message->layer = 1;
			power_config.set_ele_temp = 6;
			power_config.set_out_temp = 6.6;

            first = 0;
        }
		//
		if(control.Control_value != NULL)
        {
			run_num = 2;
			power_config.set_out_temp = MIN(power_config.set_out_temp,18);
			power_config.set_out_temp = MAX(power_config.set_out_temp,0.8);

			// 获取VIN电压
            power_config.IN_vol = Mode_Use.USER_ADC.Conversion_Vol_pFun(adc_buff[run_num++]) * Divider_RES;
			if(power_config.IN_vol < 5.2)
			{
				power_config.PD_val = 0;
			}
			else if(power_config.IN_vol > 8 && power_config.IN_vol < 9.2)
			{
				power_config.PD_val = 9;
			}
			else if(power_config.IN_vol > 11.4 && power_config.IN_vol < 12.2)
			{
				power_config.PD_val = 12;
			}
			else if(power_config.IN_vol > 19 && power_config.IN_vol < 20.2)
			{
				power_config.PD_val = 20;
			}
			else{
				power_config.PD_val = 0;
			}
			// 获取VOUT电压
			Val_temp = Mode_Use.USER_ADC.Conversion_Vol_pFun(adc_buff[run_num++]) * Divider_RES;
			temp_num = Caven_Data_Median_filtering_Handle (Val_temp,Val_OUT_array,&power_config.OUT_vol,&Val_OUT_num,10);
			if(temp_num > 0)
			{
				power_config.set_out_sync ++;
				if (power_config.set_out_sync > 10)
				{
					power_config.set_out_sync = 0;
					SET_Val_Handle (power_config.set_out_temp,power_config.OUT_vol);
				}
			}
			// ntc
			Val_temp = Mode_Use.USER_ADC.Conversion_Vol_pFun(adc_buff[run_num++]);
			// ele
			Val_temp = Mode_Use.USER_ADC.Conversion_Vol_pFun(adc_buff[run_num++]) * (Sampling_RES_RATIO / MULTIP_RATIO);
			temp_num = Caven_Data_Median_filtering_Handle (Val_temp,Val_ELE_array,&power_config.ELE_val,&Val_ELE_num,10);

		}
        if(message->layer == 1)
        {
            
        }
		//
		power_config.out_witch = 1;
        if(power_config.out_witch)
        {
			Power_OUT_Set (2,1);
        }
        else
        {
            Power_OUT_Set (2,0);
        }
        //

		show_cycle ++;

        if(show_cycle > 1000)
        {
            show_cycle = 0;
            message->str_switch = 1;
            run_num = 0;
			printf("vout = %5.2f \r\n",power_config.OUT_vol);
            sprintf(message->string,"Caven Open power");            // 0
            run_num += sizeof("Caven Open power");
            if(message->layer == 1)
            {
                sprintf(message->string + run_num," ");                 // 1
                run_num += sizeof(" ");
                
                sprintf(string_temp,"IN:%5.2fv PD-%2d",power_config.IN_vol,power_config.PD_val);
                temp_num = strlen(string_temp);
                memcpy(message->string + run_num,string_temp,temp_num);
                run_num += (temp_num + 1);
                if(power_config.out_mode > 0)
                {
                    sprintf(string_temp,"%s","ELE Mode");
                    temp_num = strlen(string_temp);
                    memcpy(message->string + run_num,string_temp,temp_num);
                    run_num += (temp_num + 1);
                }
                else
                {
                    sprintf(string_temp,"%s","VOL Mode");
                    temp_num = strlen(string_temp);
                    memcpy(message->string + run_num,string_temp,temp_num);
                    run_num += (temp_num + 1);
                }
                sprintf(string_temp,"OUT:%5.2fv Set:%4.1fv",power_config.OUT_vol,power_config.set_out_temp);
                temp_num = strlen(string_temp);
                memcpy(message->string + run_num,string_temp,temp_num);
                run_num += (temp_num + 1);
                sprintf(string_temp,"ELE:%5.2fA Set:%4.1fA",power_config.ELE_val,power_config.set_ele_temp);
                temp_num = strlen(string_temp);
                memcpy(message->string + run_num,string_temp,temp_num);
                run_num += (temp_num + 1);
                sprintf(string_temp,"Temp:%5.2f C",power_config.TEM_val);
                temp_num = strlen(string_temp);
                memcpy(message->string + run_num,string_temp,temp_num);
                run_num += (temp_num + 1);
                sprintf(message->string + run_num,"\nBack [user/power]");   // end(x)
            }
        }
        else
        {
            message->str_switch = 0;
        }
    }
//    Vofa_JustFloat_Show_Fun (7,Val_array);

    return retval;
}

int SET_Val_Handle (float set_val,float get_val)
{
	int retval = 0;
	int set_num,temp_num;
	int MAX_adj = 100;
	float f_temp = (set_val / 18.0) * 1000;
	float f_diff_temp = 0;
	static int set_diff_max = 0;
	static int standard_num = 0;
	static float Last_set_val;
	if(Last_set_val != set_val)
	{
		// DC_OUT_OFF();
		Last_set_val = set_val;
		set_diff_max = 0;
	}
    
	set_num = f_temp;				// pwm 
	f_temp = get_val - set_val;		// 差距
	f_diff_temp = fabsf(f_temp);
	if (f_temp < 0.03 && f_temp > 0)
	{
		standard_num ++;
		if(standard_num > 10)
		{
			standard_num = 0;
			retval = 1;		// 启动
		}
	}
    else					// 调
    {	
		standard_num = 0;
		if (f_diff_temp > 1.0)
		{
			temp_num = 30;
		}
		else if (f_diff_temp > 0.2)
		{
			temp_num = 12;
		}
		else
		{
			temp_num = 1;
		}
		if(f_temp > 0)				// 如果当前测量值>设置值
		{
			temp_num = -temp_num;	// 减少占空比 (PWM = 0,DAC = 3.3v,VOUT = 0.7v)
		}
			
		set_diff_max += temp_num;
        set_diff_max = MIN(set_diff_max,MAX_adj);		// set_diff_max 微调值
		set_diff_max = MAX(set_diff_max,-MAX_adj);
		
        set_num += set_diff_max;
        set_num = MIN(set_num,1000);
		set_num = MAX(set_num,0);
//		set_num = 900;
        TIM3_PWMx_SetValue(1,set_num);
    }
	return retval;
}
