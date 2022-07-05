#include "sys_time.h"

/*
        SYS_Time
1.Sys_Watch
2.Delay_10us
3.Delay_ms
4.Delay_S

                -2021.10.07
减少中断负担           -2022. 07. 04
*/
struct _SYS_Time SYS_Time = {
        .Date.day = 0,
        .Watch.hour = 23,
        .Watch.minutes = 59,
        .Watch.second = 56,
};
int Delay_Time;
static char Daley_Falg = 0;

void Sys_Watch_Handle (void)
{
    SYS_Time.Watch.time_num++;
    if(SYS_Time.Watch.time_num > Frequency)
    {
        SYS_Time.Watch.time_num = 1;
        SYS_Time.sys_time++;
        SYS_Time.Watch.second++;
        if(SYS_Time.Watch.second > 59)
        {
            SYS_Time.Watch.second = 0;
            SYS_Time.Watch.minutes++;
            if(SYS_Time.Watch.minutes > 59)
            {
                SYS_Time.Watch.minutes = 0;
                SYS_Time.Watch.hour ++;
                if(SYS_Time.Watch.hour > 23)
                {
                    SYS_Time.Watch.hour = 0;
                }
            }
        }
    }
}

void Delay_10us(int num)
{
    Delay_Time = 0;
    Daley_Falg = 1;
    while(Delay_Time < num);
    Daley_Falg = 0;
}

void Delay_ms(int num)
{
    int n = num;
    while((n--) > 0)
    {
        Delay_Time = 0;
        Daley_Falg = 1;
        while(Delay_Time < 100);
        Daley_Falg = 0;
    };
}

void Delay_S(char num)
{
    int n = num;
    while((n--) > 0)
    {
        Delay_ms(1000);
    };
}

//**//
#ifdef  Base_SysTick
    void SysTick_Handler(void)          //SYS_TIME中断
    {
        if (Daley_Falg) {
            Delay_Time++;
        }
        Sys_Watch_Handle();
    }

    void Sys_Time_Init (FunctionalState Set)
    {
        if(Set)
        {
            if(SysTick_Config(SystemCoreClock/Frequency))         //系统使用滴答定时器，因为RTC定时器的最小细分不足以用于一些场景（例如超声波）
                while(1);
        }
        else
            NVIC_SystemReset();
    }

#elif   Base_Time
    void TIM4_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

    void TIM4_IRQHandler(void)          //SYS_TIME中断
    {
        if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
        {
            TIM_ClearFlag(TIM4, TIM_IT_Update);

            if (Daley_Falg) {
                Delay_Time++;
            }
            Sys_Watch_Handle();
        }

    }

    void Sys_Time_Init (FunctionalState Set)
    {
        if(Set)
        {
            TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};
            NVIC_InitTypeDef NVIC_InitStructure = {0};
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

            TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
            TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
            TIM_TimeBaseInitStructure.TIM_Period = 1;
            TIM_TimeBaseInitStructure.TIM_Prescaler = 718;
            TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0x00;
            TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure); //时钟配置

            NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
            NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
            NVIC_Init(&NVIC_InitStructure);

            TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);          //使能中断
            TIM_Cmd(TIM4, ENABLE);

        }
        else
            NVIC_SystemReset();
    }
#endif
//**//
