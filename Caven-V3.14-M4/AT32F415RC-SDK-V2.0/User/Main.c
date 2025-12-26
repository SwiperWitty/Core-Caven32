#include "center_app.h"

void Main_Init(void);

u8 send_array[64];
int run_num;
TIM_Capture_Type time3_Capture_val1;
TIM_Capture_Type time3_Capture_val2;

void Capture_pwm_handle (void *data)
{
	TIM_Capture_Type temp_Capture_val;
	if(data != NULL)
	{
		memcpy(&temp_Capture_val,data,sizeof(TIM_Capture_Type));
		if (temp_Capture_val.Channel == 1)
		{
			memcpy(&time3_Capture_val1,data,sizeof(TIM_Capture_Type));
		}
        else if (temp_Capture_val.Channel == 2)
		{
			memcpy(&time3_Capture_val2,data,sizeof(TIM_Capture_Type));
		}
	}
}

int main(void)
{
    int temp_num = 0;
    Caven_BaseTIME_Type now_time;
    Main_Init();
    now_time.SYS_Sec = 1746514130;
    Mode_Use.TIME.Set_BaseTIME_pFun(now_time);
    now_time = Mode_Use.TIME.Get_BaseTIME_pFun();

    Task_Overtime_Type LED_Task = {
            .Switch = 1,
            .Begin_time = now_time,
            .Set_time.SYS_Sec = 1,
            .Set_time.SYS_Us = 500000,
            .Flip_falg = 0,
    };
    User_GPIO_config(2,4,1);
    User_GPIO_config(3,2,1);
    TIMx_Capture_Callback_pFunBind(3,Capture_pwm_handle);
    TIM3_Capture_Start_Init(0xffff,144,0x03,0,1);
    
    TIM4_PWM_Start_Init(2000,144,1);
    temp_num = 0;
    TIM4_PWMx_SetValue(1,&temp_num);
    temp_num = 500;
    TIM4_PWMx_SetValue(2,&temp_num);
    temp_num = 1000;
    TIM4_PWMx_SetValue(3,&temp_num);
    temp_num = 1500;
    TIM4_PWMx_SetValue(4,&temp_num);
    while(1)
    {
        now_time = Mode_Use.TIME.Get_BaseTIME_pFun();
        API_Task_Timer (&LED_Task,now_time);        // LED任务
        User_GPIO_set(2,4,LED_Task.Flip_falg);
        User_GPIO_set(3,2,LED_Task.Flip_falg);
//        if(LED_Task.Trigger_Flag)
//        {
//            printf("utc %d",now_time.SYS_Sec);
//        }
        if(Center_State_machine(now_time))          // 状态机入口
        {
            break;                                  // 状态机退出,程序重启
        }
        if(time3_Capture_val1.Channel == 1)
        {
            if(time3_Capture_val1.finish_flag)
            {
                time3_Capture_val1.finish_flag = 0;
            }
        }
        if(time3_Capture_val2.Channel == 2)
        {
            if(time3_Capture_val2.finish_flag)
            {
                time3_Capture_val2.finish_flag = 0;
            }
        }
        temp_num = TIMx_Encoder_Capture(3);
        if(temp_num)
        {
            Mode_Use.TIME.Delay_Ms(1000);
            temp_num = TIMx_Encoder_Capture(3);
            Mode_Use.TIME.Delay_Ms(10);
        }
    }
    SYS_RESET();
}

void Main_Init(void)
{
    Mode_Index();
    Mode_Init.TIME(ENABLE);
    Mode_Use.TIME.Delay_Ms(10);
    Mode_Init.UART(DEBUG_CH,115200,ENABLE);
    Mode_Init.OLED(ENABLE);
    Mode_Init.Steering_Engine(ENABLE);
    Mode_Init.USB(ENABLE);
//    Mode_Use.Steering_Engine.Bind_PWM_Time (TIM4_PWMx_SetValue);
//    Mode_Use.Steering_Engine.Set_Angle(1,0);
//    Mode_Use.Steering_Engine.Set_Angle(2,90);
//    Mode_Use.Steering_Engine.Set_Angle(3,180);
    
    printf("SystemClk:%d Hz\r\n", MCU_SYS_FREQ);
}
