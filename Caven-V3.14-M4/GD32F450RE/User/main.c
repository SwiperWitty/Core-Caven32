
#if 1

#include "gd32f4xx.h"
#include <stdio.h>
#include "main.h"
#include "string.h"
#include "Mode.h"
#include "base_flash.h"

void Main_Init(void);

int main(void)
{
    Caven_BaseTIME_Type now_time;
    struct tm now_date;
    Main_Init();
    now_time.SYS_Sec = 1746514130;
    Mode_Use.TIME.Set_BaseTIME_pFun(now_time);
    now_time = Mode_Use.TIME.Get_BaseTIME_pFun();

    Task_Overtime_Type LED_Task = {
            .Switch = 1,
            .Begin_time = now_time,
            .Set_time.SYS_Sec = 1,
            .Set_time.SYS_Us = 0,
            .Flip_falg = 0,
    };
    while(1)
    {
        now_time = Mode_Use.TIME.Get_BaseTIME_pFun();
        API_Task_Timer (&LED_Task,now_time);        // LED任务
        User_GPIO_set(3,0,LED_Task.Flip_falg);
        if(LED_Task.Trigger_Flag)
        {
            now_date = Mode_Use.TIME.Get_Date_pFun(8*60*60);
            printf("date %d/%d/%d %02d:%02d:%02d  utc [%d] [%d]\n",
                now_date.tm_year,now_date.tm_mon,now_date.tm_mday,
                now_date.tm_hour,now_date.tm_min,now_date.tm_sec,
                now_time.SYS_Sec,now_time.SYS_Us);
        }
    }
}

void Main_Init(void)
{
    Mode_Index();
    Mode_Init.TIME(ENABLE);
    Mode_Init.UART(m_UART_CH0,115200,1);
    Mode_Init.UART(m_UART_CH1,115200,1);
    Mode_Init.UART(m_UART_CH3,115200,1);
    Mode_Init.UART(m_UART_CH4,115200,1);
    Mode_Init.OLED(1);
    
    TIM3_PWM_Start_Init(2000-1,1200-1,1);
    
    TIM3_PWMx_SetValue(1,100);
    TIM3_PWMx_SetValue(2,200);
    TIM3_PWMx_SetValue(3,1000);
    TIM3_PWMx_SetValue(4,1500);
    
    Mode_Use.TIME.Delay_Ms(10);
    User_GPIO_config(3,0,1);
    User_GPIO_config(3,1,1);
    User_GPIO_set(3,0,0);
    User_GPIO_set(3,1,0);
    Mode_Use.UART.Send_String_pFun(m_UART_CH0,"hello 0 \n");
    Mode_Use.UART.Send_String_pFun(m_UART_CH1,"hello 1 \n");
    Mode_Use.UART.Send_String_pFun(m_UART_CH3,"hello 3 \n");
    Mode_Use.UART.Send_String_pFun(m_UART_CH4,"hello 4 \n");
    printf("SystemClk:%d Hz\r\n", MCU_SYS_FREQ);
}

#endif
