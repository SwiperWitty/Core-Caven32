#include "Base_Sys_time.h" 

#define Sys_Time_VAL    SysTick->VAL
#define LLTIMER_REG     (Tick_Set_CMP - Sys_Time_VAL)

// 提取宏，很多宏都是以运算(a * b)的形式存在的，每次调用都会算一遍
int Tick_Full;       
int Freq_ms;
int Freq_us;

//
SYS_Tick_type SYS_Ticktime = {0};       //无需担心，他是自动的

void Sys_Time_Init(int Set)
{
#ifdef Exist_SYS_TIME //这种保护不占内存，所以尽可能写
    Tick_Full = Tick_Set_CMP;
    Freq_ms = (Tick_Frequency / 1000);
    Freq_us = (Tick_Frequency / 1000000);
    
    if (Set)
    {
//        while(SysTick_Config(Tick_Set_CMP));
//        systick_clock_source_config(SYSTICK_CLOCK_SOURCE_AHBCLK_DIV8); // 8分
//        if(Set == 2)
//        {}
    }
    else
        NVIC_SystemReset();

#endif
}



#ifdef Exist_SYS_TIME
void SysTick_Handler(void)
{
    SYS_Ticktime.SYS_Tick_H++;
}
#endif

uint64_t SysTick_type_Change_NUM (SYS_Tick_type stamp)
{
    uint64_t temp = 0;
#ifdef Exist_SYS_TIME
//    stamp.SYS_Tick_L = LLTIMER_REG; //滴答当前值
//    temp = stamp.SYS_Tick_H;
//    temp *= Tick_Full;              //乘法一定放后面，尤其是中断的东西
//    temp += stamp.SYS_Tick_L;
#endif
    return (temp);
}

//这个返回的是，总系统滴答数，这个数是U64的，巨大
uint64_t GET_SysTick (SYS_Tick_type *stamp)
{
    uint64_t temp = 0;
#ifdef Exist_SYS_TIME
//    SYS_Ticktime.SYS_Tick_L = LLTIMER_REG; // 滴答当前值
//    stamp->SYS_Tick_H = SYS_Ticktime.SYS_Tick_H;
//    stamp->SYS_Tick_L = SYS_Ticktime.SYS_Tick_L;
//    temp = stamp->SYS_Tick_H;
//    
//    temp *= Tick_Full;              //乘法一定放后面，尤其是中断的东西
//    temp += SYS_Ticktime.SYS_Tick_L;
#endif
    return (temp);
}

void SET_SysTick(uint64_t time)
{
#ifdef Exist_SYS_TIME
//    SYS_Ticktime.SYS_Tick_H = time / Tick_Full;         //高位设置
//    SYS_Ticktime.SYS_Tick_L = (time % Tick_Full);       //低位设置(不设也行)
//    Sys_Time_VAL = Tick_Full - SYS_Ticktime.SYS_Tick_L; //载入低位
#endif
}

void IWDG_Configuration(void)
{

}

void Feed_watchdog(void)
{

} 

// Delay
/*
    a * b 个 NOP
*/
void Base_NOP_Delay (int time,int Speed)
{
    #ifdef NOP
    volatile int temp;
    for (int i = 0; i < time; ++i)
    {
        temp = Speed;            //SET
        do{
            NOP();
        }while((temp--) > 0);
    }
    #endif
}

void SYS_Delay_us(int n)
{
    uint64_t start_ticks, end_ticks; //都是滴答数，而非具体标准时间
    uint64_t temp;
    int set_time = n * Freq_us;
    SYS_Tick_type stt_temp;
    start_ticks = GET_SysTick(&stt_temp);
#ifdef Exist_SYS_TIME
    while (1)
    {
        end_ticks = GET_SysTick(&stt_temp);
        
        if (end_ticks > start_ticks)
        {
            temp = end_ticks - start_ticks;
        }
        else
        {
            temp = 86400 * Tick_Frequency; //一天时间 * 滴答频率
            temp -= start_ticks;
            temp += end_ticks;
        }
        if (temp >= set_time)
        {
            break;
        }
    }
#endif
}

void SYS_Delay_ms(int n)
{
    uint64_t start_ticks, end_ticks; //都是滴答数，而非具体标准时间
    uint64_t temp;
    int set_time = n * Freq_ms;
    SYS_Tick_type stt_temp;
    start_ticks = GET_SysTick(&stt_temp);
#ifdef Exist_SYS_TIME
    while (1)
    {
        end_ticks = GET_SysTick(&stt_temp);

        if (end_ticks > start_ticks)
        {
            temp = end_ticks - start_ticks;
        }
        else
        {
            temp = 86400 * Tick_Frequency; //一天时间 * 滴答频率
            temp -= start_ticks;
            temp += end_ticks;
        }
        if (temp >= set_time)
        {
            break;
        }
    }
#endif
}

void SYS_Delay_S(int n)
{
    for (int var = 0; var < n; ++var)
    {
        SYS_Delay_ms(1000);
    }
}
