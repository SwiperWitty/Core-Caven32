#include "power_app.h"


#define Divider_RES 8.5     // ((75 + 10) / 10) adc * Divider_RES = vcc
#define MULTIP_RATIO 55.5	// INA119A1
#define Sampling_RES 0.005	// 0.005Ω
#define Sampling_RES_RATIO 200.0	// (5/1000)(Ω) -> 200(RATIO)

int PD_Set_Mode (char grade);
int SET_Val_Handle (float set_val,float get_val);

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

int SET_Val_Handle (float set_val,float get_val)
{
	int retval = 0;
	int set_num,temp_num;
	float f_temp = (set_val / 18.0) * 1000;
	float f_diff_temp = 0;
	static int set_diff_max = 0;
	static int standard_num = 0;
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
	return retval;
}

int show_cycle = 0;

float Val_temp = 0;

u16 adc_buff[10];
float Val_OUT_array[10];
float Val_ELE_array[10];
float Val_TEM_array[10];
char Val_OUT_num = 0;
char Val_ELE_num = 0;
char Val_TEM_num = 0;

Power_Control_Type power_config;

int Power_app (Caven_App_Type * message)
{
    int retval = 0;
    int run_num = 0;
    int temp_num;
    static int first = 1;
    Caven_Control_Type control;
    char string_temp[50];
    if (message != NULL)
    {
        memcpy(&control,message->p_Data,sizeof(control));
        if(first)
        {
            message->str_switch = 1;
            message->cursor = 4;
            message->layer = 1;
			power_config.set_ele_temp = 3;
			power_config.set_out_temp = 6;
			power_config.line_max = 6;
			power_config.line_end = 9;
			power_config.out_witch = 0;
            first = 0;
        }
		//
		switch (message->cursor)
		{
			case (2):
				break;
			case (6):
				break;
			case (3):
				if(control.Control_x > 0)
				{
					power_config.out_mode ++;
				}
				else if(control.Control_x < 0)
				{
					power_config.out_mode --;
				}
				power_config.out_mode = MIN(power_config.out_mode,1);
				power_config.out_mode = MAX(power_config.out_mode,0);
				break;
			case (4):
				if(control.Control_x > 0)
				{
					power_config.set_out_temp ++;
				}
				else if(control.Control_x < 0)
				{
					power_config.set_out_temp --;
				}
				power_config.set_out_temp = MIN(power_config.set_out_temp,18);
				power_config.set_out_temp = MAX(power_config.set_out_temp,0.8);
				if(control.Control_botton == 1)
				{
					power_config.set_out_vol = power_config.set_out_temp;
					power_config.out_witch = !power_config.out_witch;
				}
				break;
			case (5):
				if(control.Control_x > 0)
				{
					power_config.set_ele_temp ++;
				}
				else if(control.Control_x < 0)
				{
					power_config.set_ele_temp --;
				}
				power_config.set_ele_temp = MIN(power_config.set_ele_temp,10);
				power_config.set_ele_temp = MAX(power_config.set_ele_temp,0.1);
				if(control.Control_botton == 1)
				{
					power_config.set_ele_val = power_config.set_ele_temp;
				}
				break;
			default:
				message->app_ID = 1;        // 返回home
				first = 1;
				break;
		}
        //
        if(control.Control_y > 0)
        {
            if(message->cursor >= power_config.line_end)
            {
                message->cursor = 2;
            }
            else
            {
                message->cursor ++;
            }
            if(message->cursor > power_config.line_max)
            {
                message->cursor = power_config.line_end;
            }
        }
        else if(control.Control_y < 0)
        {
            if(message->cursor <= 2)
            {
                message->cursor = power_config.line_end;
            }
            else if(message->cursor == power_config.line_end)
            {
                message->cursor = power_config.line_max;
            }
            else
            {
                message->cursor --;
            }
        }
        message->cursor = MAX(message->cursor,2);       // 第一个可选是2
        //doing
        if(control.Control_value != NULL)
        {
            run_num = 2;
            //
            memcpy(adc_buff,control.Control_value,sizeof(adc_buff));
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
            //
            Val_temp = Mode_Use.USER_ADC.Conversion_Vol_pFun(adc_buff[run_num++]) * Divider_RES;
            temp_num = Data_Median_filtering_Handle (Val_temp,Val_OUT_array,&power_config.OUT_vol,&Val_OUT_num,10);
            if(temp_num > 0)
            {
                temp_num = SET_Val_Handle (power_config.set_out_vol,power_config.OUT_vol);
				if(temp_num && power_config.out_witch > 0)
				{
					DC_OUT_ON();
				}
				else
				{
					DC_OUT_OFF();
				}
            }
            //
            Val_temp = Mode_Use.USER_ADC.Conversion_Vol_pFun(adc_buff[run_num++]);
            //
            Val_temp = Mode_Use.USER_ADC.Conversion_Vol_pFun(adc_buff[run_num++]) * (Sampling_RES_RATIO / MULTIP_RATIO);
            temp_num = Data_Median_filtering_Handle (Val_temp,Val_ELE_array,&power_config.ELE_val,&Val_ELE_num,10);
            if(temp_num > 0)
            {
				if(power_config.ELE_val < power_config.set_ele_val)
				{
//					power_config.out_witch = 0;
				}
            }
            //
            Val_temp = Mode_Use.USER_ADC.Get_MCU_Temperature_pFun();
            temp_num = Data_Median_filtering_Handle (Val_temp,Val_TEM_array,&power_config.TEM_val,&Val_TEM_num,10);
            //
            Mode_Use.TIME.Delay_Us(100);            // 给电路反应的时间，这很重要
        }
        //
        show_cycle ++;
        if(show_cycle > 100)
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
//    Vofa_JustFloat_Show_Fun (Val_array);

    return retval;
}

