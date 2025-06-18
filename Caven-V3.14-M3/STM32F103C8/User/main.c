#include "center_app.h"
#include "pic.h" //图片


void Main_Init(void);

void tim4_pwm_period_switch (char num);

int main(void)
{
    int temp_num = 0;
    Caven_BaseTIME_Type now_time;
	
    Main_Init();
    
    now_time.SYS_Sec = 1742299486;
    Mode_Use.TIME.Set_BaseTIME_pFun(now_time);
    now_time = Mode_Use.TIME.Get_BaseTIME_pFun();

    Task_Overtime_Type LED_Task = {
            .Switch = 1,
            .Begin_time = {0},
            .Set_time.SYS_Sec = 1,
            .Set_time.SYS_Us = 0,
            .Flip_falg = 0,
    };
	int key_sw = 0,PWM_period_switch = 0;
		
    while(1)
    {
        now_time = Mode_Use.TIME.Get_BaseTIME_pFun();
		// LED任务
        API_Task_Timer (&LED_Task,now_time);        
        User_GPIO_set(1,11,LED_Task.Flip_falg);
		// KEY
		if (User_GPIO_get(3,13) == 0)
		{
			key_sw = 1;
			User_GPIO_set(1,12,1);
			Mode_Use.TIME.Delay_Ms(10);
		}
		else
		{
			if(key_sw)
			{
				key_sw = 0;
				User_GPIO_set(1,12,0);
				PWM_period_switch ++;
				if(PWM_period_switch > 2)
				{
					PWM_period_switch = 0;
				}
				tim4_pwm_period_switch (PWM_period_switch);
                temp_num = 100;
                TIM4_PWMx_SetValue(1,&temp_num);
                temp_num = 500;
                TIM4_PWMx_SetValue(2,&temp_num);
                temp_num = 1000;
                TIM4_PWMx_SetValue(3,&temp_num);
                temp_num = 1500;
                TIM4_PWMx_SetValue(4,&temp_num);
			}
		}
       if(Center_State_machine(now_time))          // 状态机入口
       {
           break;                                  // 状态机退出,程序重启
       }
    }
    SYS_RESET();
}

void Main_Init(void)
{
    Mode_Index();

	Center_app_Init ();
	System_app_Init ();
	Caven_app_Init ();
	GX_app_Init ();
	
    tim4_pwm_period_switch (2);
    int temp_num = 100;
    TIM4_PWMx_SetValue(1,&temp_num);
    temp_num = 500;
    TIM4_PWMx_SetValue(2,&temp_num);
    temp_num = 1000;
    TIM4_PWMx_SetValue(3,&temp_num);
    temp_num = 1500;
    TIM4_PWMx_SetValue(4,&temp_num);
}

void tim4_pwm_period_switch (char num)
{
	switch (num)
	{
		case (0):
		{
			TIM4_PWM_Start_Init(2000-1,(MCU_SYS_FREQ / 18000000)-1,ENABLE);	// b6-b9
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
