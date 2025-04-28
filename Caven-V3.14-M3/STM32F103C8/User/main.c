#include "Mode.h"
#include "API.h"
#include "pic.h" //图片


char temp_array[100];
void Main_Init(void);
void Capture_pwm_handle (void *data);
TIM_Capture_Type time1_Capture_val;
void tim4_pwm_period_switch (char num);

int main(void)
{
    Caven_BaseTIME_Type now_time,show_time;
    Main_Init();
    now_time.SYS_Sec = 1742299486;
    Mode_Use.TIME.Set_BaseTIME_pFun(now_time);
    now_time = Mode_Use.TIME.Get_BaseTIME_pFun();

    Task_Overtime_Type LED_Task = {
            .Switch = 1,
            .Begin_time = now_time,
            .Set_time.SYS_Sec = 1,
            .Set_time.SYS_Us = 0,
            .Flip_falg = 0,
    };
	Task_Overtime_Type Vofa_JustFloat_Show_Task = {
            .Switch = 1,
            .Begin_time = now_time,
            .Set_time.SYS_Sec = 0,
            .Set_time.SYS_Us = 50000,
            .Flip_falg = 0,
    };

	char Capture_flag = 0;
	int period = 0,temp_val = 0,Freq = 0,Freq_last = 0xff;
	int key_sw = 0,PWM_period_switch = 0;
	float period_t,temp_f = 0.00f,cycle = 0;
	float show_buff[10];
	char str_array[100];
    while(1)
    {
        now_time = Mode_Use.TIME.Get_BaseTIME_pFun();
        API_Task_Timer (&LED_Task,now_time);        // LED任务
		API_Task_Timer (&Vofa_JustFloat_Show_Task,now_time);        // LED任务
        User_GPIO_set(1,11,LED_Task.Flip_falg);
		
		if(LED_Task.Trigger_Flag)
		{
//			printf("time %d s :%d us \n",now_time.SYS_Sec,now_time.SYS_Us);
		}
		// Capture
		if (time1_Capture_val.finish_flag)
		{
			time1_Capture_val.finish_flag = 0;
			period = time1_Capture_val.period_val;
			cycle = time1_Capture_val.high_val;
			temp_f = period;
			cycle /= temp_f;
			period_t = period;
			period_t *= 0.001;
			Capture_flag = 1;
			temp_f = 1 / (period_t / 1000);
			Freq = (int)temp_f;
		}
		// Vofa
		if(Vofa_JustFloat_Show_Task.Trigger_Flag && Capture_flag)
		{
			Capture_flag = 0;
			show_time = now_time;
			show_buff[0] = period_t;
			show_buff[1] = cycle;
			Vofa_JustFloat_Show_Fun (2,show_buff);
			period_t = 0.00f;
			cycle = 0.00f;
		}
		temp_val = now_time.SYS_Sec - show_time.SYS_Sec;
		temp_val *= 1000000;
		temp_val += now_time.SYS_Us;
		if (temp_val > 1200000)
		{
			show_time = now_time;
			show_buff[0] = 0;
			show_buff[1] = 0;
			Vofa_JustFloat_Show_Fun (2,show_buff);
		}
		// LCD
		if (Freq_last != Freq)
		{
			Freq_last = Freq;
			memset(str_array,0,sizeof(str_array));
			sprintf(str_array,"Freq :%d Hz",Freq);
			//Mode_Use.LCD.Show_String_pFun (0,0,str_array,LCD_WHITE,LCD_BLACK,16);
		}
		// KEY
		if (User_GPIO_get(3,13) == 0)
		{
			key_sw = 1;
			Mode_Use.TIME.Delay_Ms(10);
		}
		else
		{
			if(key_sw)
			{
				key_sw = 0;
				PWM_period_switch ++;
				if(PWM_period_switch > 2)
				{
					PWM_period_switch = 0;
				}
				tim4_pwm_period_switch (PWM_period_switch);
			}
		}
//        if(Center_State_machine(now_time))          // 状态机入口
//        {
//            break;                                  // 状态机退出,程序重启
//        }
    }
    SYS_RESET();
}

void Main_Init(void)
{
    Mode_Index();
    Mode_Init.TIME(ENABLE);
    Mode_Use.TIME.Delay_Ms(10);
    
	Mode_Init.UART(m_UART_CH2,115200,ENABLE);
    Mode_Init.UART(m_UART_CH3,115200,ENABLE);
	Mode_Use.LCD.Set_TargetModel_pFun(m_LCD_TYPE_1_30);
    Mode_Init.LCD(ENABLE);
	
	TIM1_Capture_Start_Init(0xffff,72-1,0x03,0,ENABLE);				// a8,a9
	
	User_GPIO_config(1,11,1);
    User_GPIO_config(3,13,0);
	
	tim4_pwm_period_switch (1);
	
    Mode_Use.UART.Send_String_pFun(m_UART_CH2,"hello !\n");
    Mode_Use.UART.Send_String_pFun(m_UART_CH3,"hello !\n");
	TIMx_Capture_Callback_pFunBind(1,Capture_pwm_handle);
	TIM4_PWMx_SetValue(1,100);
	TIM4_PWMx_SetValue(2,500);
	TIM4_PWMx_SetValue(3,1000);
	TIM4_PWMx_SetValue(4,1500);
	Vofa_JustFloat_Init_Fun (Debug_Out);
}

void tim4_pwm_period_switch (char num)
{
	switch (num)
	{
		case (0):
		{
			TIM4_PWM_Start_Init(2000-1,(MCU_SYS_FREQ / 3600000)-1,ENABLE);	// b6-b9
		}
		break;
		case (1):
		{
			TIM4_PWM_Start_Init(2000-1,(MCU_SYS_FREQ / 1000000)-1,ENABLE);	// b6-b9
		}
		break;
		case (2):
		{
			TIM4_PWM_Start_Init(2000-1,(MCU_SYS_FREQ / 100000)-1,ENABLE);	// b6-b9
		}
		break;
	}
}

void Capture_pwm_handle (void *data)
{
	TIM_Capture_Type temp_Capture_val;
	if(data != NULL)
	{
		memcpy(&temp_Capture_val,data,sizeof(TIM_Capture_Type));
		if (temp_Capture_val.Channel == 1)
		{
			memcpy(&time1_Capture_val,data,sizeof(TIM_Capture_Type));
		}
	}
}
