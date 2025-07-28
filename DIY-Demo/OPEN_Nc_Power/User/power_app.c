#include "power_app.h"


#define Divider_RES 8.5     // ((75 + 10) / 10) adc * Divider_RES = vcc
#define MULTIP_RATIO 51		// INA119A1
#define Sampling_RES 0.005	// 0.005Ω
#define Sampling_RES_RATIO 200.0	// (5/1000)(Ω) -> 200(RATIO)

#define EQUA_a		0.00871
#define EQUA_b		1.46
#define ERROR_val	100

Power_Control_Type power_config;

int PD_Set_Mode (char grade);
void Power_OUT_Set (char num,char set);
int SET_Val_Handle (float set_val,float get_val);

int Power_app_init (int Set)
{
    int retval = 0;
	int temp_num = 400;		// 406 488   456 533
	TIM3_PWM_Start_Init(2000,8,ENABLE);		// 144/(2+1) = 48
	TIM3_PWMx_SetValue(1,&temp_num);
	
	User_GPIO_config(3,0,1);	// 5v
	User_GPIO_config(3,1,1);	// out
	User_GPIO_config(3,12,1);	// PD_B
	User_GPIO_config(4,2,1);	// PD_A

	Power_OUT_Set (1,1);
	Power_OUT_Set (2,0);
	power_config.set_pd = 3;
	PD_Set_Mode (power_config.set_pd);
	
	memset(&power_config,0,sizeof(power_config));

	Vofa_JustFloat_Init_Fun (Debug_Out);     // Vin,Vout,Temp,YG_x,YG_y,YG_key,in_temp
    return retval;
}

int Power_app_exit (int *falg)
{
    int retval = 0;
	
	TIM3_PWM_Start_Init(1000,12,DISABLE);
	Power_OUT_Set (1,1);
	Power_OUT_Set (2,0);
//	DC_OUT_OFF();
	*falg = 1;

    return retval;
}

// 1 5v;2 12v;3 20v;
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
			User_GPIO_set(3,0,1);	// 5v
		}
		else
		{
			User_GPIO_set(3,0,0);	// 5v
		}
	}
	else if (num == 2)
	{
		if(set)
		{
			User_GPIO_set(3,1,1);	// out
		}
		else
		{
			User_GPIO_set(3,1,0);	// out
		}
	}
}

int show_cycle = 0;

float Val_OUT_array[40];
float Val_ELE_array[10];
float Val_TEM_array[10];
char Val_OUT_num = 0;
char Val_ELE_num = 0;
char Val_TEM_num = 0;

float last_set_val = 0;
/*

retval:
	0:正常运行
	1:退出程序
*/
int Power_app (Caven_App_Type * message)
{
    int retval = 0;
    int run_num = 0;
    int temp_num;
    static int first = 1;
    
	u16 adc_buff[10];
    char string_temp[50];
	float Val_buff[10];
	float Val_temp = 0;
	Caven_Control_Type control;
    if (message != NULL)
    {
		message->offset = 0;
        memcpy(&control,message->p_Data,sizeof(control));
		memcpy(adc_buff,control.Control_value,sizeof(adc_buff));
        if(first)
        {
            message->str_switch = 1;
			message->type = 0;
			message->layer = 1;
            message->cursor = 2;
			power_config.set_ele_temp = 6;
			power_config.set_out_temp = 11.0;	// 1.6  1.17
			SET_Val_Handle (power_config.set_out_temp,0);
            first = 0;
        }
		if(message->type == 0)	// 没选 
		{
			if(control.Control_y != 0)
			{
				if(control.Control_y > 0)
				{
					message->cursor--;
				}
				else
				{
					message->cursor++;
				}
				if(message->cursor < 2)
				{
					message->cursor = 9;
				}
				else if(message->cursor > 9)
				{
					message->cursor = 2;
				}
				else if(message->cursor < 9 && message->cursor > 5 && control.Control_y > 0)
				{
					message->cursor = 5;
				}
				else if(message->cursor > 5 && control.Control_y < 0)
				{
					message->cursor = 9;
				}
			}
			else if(control.Control_x != 0)
			{
				if(control.Control_x > 0)
				{
					message->offset = 1;
				}
				else
				{
					message->offset = -1;
				}
				temp_num =  ((0x0fff - adc_buff[0]) / 40) - 50;
			}
			switch(message->cursor)
			{
				case (2):		// VIN dc
				{
					if(message->offset)
					{
						power_config.set_pd += message->offset;
						if(power_config.set_pd > 3)
						{
							power_config.set_pd = 1;
						}
						else if(power_config.set_pd < 1)
						{
							power_config.set_pd = 3;
						}
						PD_Set_Mode (power_config.set_pd);
					}
				}break;
				case (3):		// VAL恒压/ELE恒流
				{
					
				}break;
				case (4):		// VAL set
				{
					//
					if(message->offset != 0)
					{
						if(message->offset > 0)
						{
							power_config.set_out_temp += 0.5;
						}
						else
						{
							power_config.set_out_temp -= 0.5;
						}
					}
				}break;
				case (5):		// ELE set
				{
					
				}break;
				case (9):		// back
				{
					if(control.Control_botton)
					{
						retval = 1;
					}
				}break;
				default:
					break;
			}
		}
		else if(message->type == 21)	// 选VIN dc
		{
		}
		else if(message->type == 31)	// VAL恒压/ELE恒流
		{
		}
		else if(message->type == 41)	// ELE set
		{
		}
		else if(message->type == 51)	// ELE set
		{
		}
		//
		if(control.Control_value != NULL)
        {
			run_num = 2;
			power_config.set_out_temp = MIN(power_config.set_out_temp,18);
			power_config.set_out_temp = MAX(power_config.set_out_temp,0.8);
			if(last_set_val != power_config.set_out_temp)
			{
				last_set_val = power_config.set_out_temp;
				SET_Val_Handle (power_config.set_out_temp,0);
			}
			// 获取VIN电压
            power_config.IN_vol = Mode_Use.Base_ADC.Conversion_Vol_pFun(adc_buff[run_num++]) * Divider_RES;
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
			Val_temp = Mode_Use.Base_ADC.Conversion_Vol_pFun(adc_buff[run_num++]) * Divider_RES;
			temp_num = Caven_Data_Median_filtering_Handle (Val_temp,Val_OUT_array,&power_config.OUT_vol,&Val_OUT_num,30);
			if(temp_num > 0)
			{
				
			}
			// ntc
			Val_temp = Mode_Use.Base_ADC.Conversion_Vol_pFun(adc_buff[run_num++]);
			// ele
			Val_temp = Mode_Use.Base_ADC.Conversion_Vol_pFun(adc_buff[run_num++]) * (Sampling_RES_RATIO / MULTIP_RATIO);
			temp_num = Caven_Data_Median_filtering_Handle (Val_temp,Val_ELE_array,&power_config.ELE_val,&Val_ELE_num,10);

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
		if(show_cycle % 500 == 0)
		{
			Val_buff[0] = power_config.IN_vol;
			Val_buff[1] = power_config.OUT_vol;
			Val_buff[2] = power_config.ELE_val;
			Val_buff[3] = power_config.TEM_val;
			printf("utc %d s [%d] \n",message->Watch.SYS_Sec,message->Watch.SYS_Us);
//			Vofa_JustFloat_Show_Fun (4,Val_buff);
		}
        else if(show_cycle > 1000)
        {
            show_cycle = 0;
            message->str_switch = 1;
            run_num = 0;

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
    

    return retval;
}

int SET_Val_Handle (float set_val,float get_val)
{
	int retval = 0;
	int set_pwm = 0,temp_num;
	float temp_f = 0;
	// y = 0.00871x+1.46
	if(set_val >= 1.0)
	{
		temp_f = set_val - EQUA_b;
		temp_f /= EQUA_a;
		set_pwm = (int)(temp_f);
		retval = 1;
	}
	else
	{
		
	}
	if(retval == 1)
	{
		set_pwm = MIN(set_pwm,2000);
		set_pwm = MAX(set_pwm,0);
		TIM3_PWMx_SetValue(1,&set_pwm);
    }
	
	return retval;
}
