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
		
    while(1)
    {
		now_time = Mode_Use.TIME.Get_BaseTIME_pFun();

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
