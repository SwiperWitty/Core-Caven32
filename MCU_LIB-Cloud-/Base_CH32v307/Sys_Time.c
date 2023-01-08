#include "sys_time.h"

/*
        SYS_Time
    功能在模块中实现，这里全是底层SDK
                                ———— 2022.7.7
*/


//* 底层 *//

#ifdef Base_SysTick
static uint32_t SysTick_Config(u64 ticks)
{
    SysTick->CTLR = (u32)0x00;                  //关闭系统计数器STK，计数器停止计数

    SysTick->SR = (u32)0;
    SysTick->CNT = (u64)0;
    SysTick->CMP = ticks;
//    NVIC_SetPriority(SysTicK_IRQn, 15);      //设置SysTick中断优先级
//    NVIC_EnableIRQ(SysTicK_IRQn);            //使能开启Systick中断
    SysTick->CTLR = (u32)(0X2D);            //0X2D 0x2F
//    SysTick->CTLR |= (u32)(0x00 << 31);
    return (0);
}
#endif


void Sys_Time_Init (int Set)
{
#ifdef Exist_SYS_TIME
    #ifdef Base_SysTick
    if(Set)
    {
        if(SysTick_Config(Frequency)         //系统使用滴答定时器，因为RTC定时器的最小细分不足以用于一些场景
            while(1);
    }
    else
//        NVIC_SystemReset();
        SysTick->CTLR = (u32)0x00;
    
    #else
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

    #endif
#endif
}

void SysTick_Reload (uint64_t time)
{
    SysTick->CNT = time;
}

void SYS_Delay_us (int n)
{
    u64 start_ticks,end_ticks;
    int set_time = n * (SystemCoreClock / 1000000);
    start_ticks = GET_SysTick();

    while(1)
    {
        end_ticks = GET_SysTick();
        if (end_ticks > start_ticks)
        {
            if ((end_ticks - start_ticks) >= set_time)
                break;
        }
        else
        {
            SysTick->SR = 0;    //溢出了
            if ((end_ticks + ((~((u64)0x00)) - start_ticks)) >= set_time)
                break;
        }
    }

}

void SYS_Delay_ms (int n)
{
    u64 start_ticks,end_ticks;
    int set_time = n * (SystemCoreClock / 1000);
    start_ticks = GET_SysTick();

    while(1)
    {
        end_ticks = GET_SysTick();
        if (end_ticks > start_ticks)
        {
            if ((end_ticks - start_ticks) >= set_time)
                break;
        }
        else
        {
            SysTick->SR = 0;    //溢出了
            if ((end_ticks + ((~((u64)0x00)) - start_ticks)) >= set_time)
                break;
        }
    }
}

void SYS_Delay_S (int n)
{
    for (int var = 0; var < n; ++var)
    {
        SYS_Delay_ms (1000);
        printf("1S \r\n");
    }
}

