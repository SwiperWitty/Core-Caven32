#include "SYS_Time.h"

#define Exist_SYS_TIME
#define Sys_Time_VAL SysTick->VAL
#define LLTIMER_REG (Tick_Set_CMP - Sys_Time_VAL)
int Tick_Full;       //提取宏，很多宏都是以运输的形式存在的，每次调用都会算一遍
int Freq_ms;
int Freq_us;

void Base_Delay (int time,int Speed)
{
    volatile int temp;
    for (int i = 0; i < time; ++i)
    {
        temp = Speed;            //SET
        do{
            __NOP();
        }while((temp--) > 0);
    }
}

void Sys_Time_Init(int Set)
{
#ifdef Exist_SYS_TIME //这种保护不占内存，所以尽可能写
    Tick_Full = Tick_Set_CMP;
    Freq_ms = (Tick_Frequency / 1000);
    Freq_us = (Tick_Frequency / 1000000);
    
    if (Set)
    {
        while(SysTick_Config(Tick_Set_CMP));
        systick_clksource_set(SYSTICK_CLKSOURCE_HCLK_DIV8); // 8分
    }
    else
        NVIC_SystemReset();

#endif
}

struct _SYS_Ticktime SYS_Ticktime = {0};

#ifdef Exist_SYS_TIME
void SysTick_Handler(void)
{
    SYS_Ticktime.SYS_Tick_H++;
}
#endif

// Tick_Set_CMP 是起点值（设置的），因为是24位自减寄存器
//这个返回的是，总系统滴答数，这个数是U64的，巨大

int GET_Ticktime_H(void)
{
  int retval = 0;
#ifdef Exist_SYS_TIME
  retval = SYS_Ticktime.SYS_Tick_H;
  
#endif
  return retval;
}

uint64_t GET_SysTick(void)
{
    uint64_t temp = 0;
#ifdef Exist_SYS_TIME
    SYS_Ticktime.SYS_Tick_L = LLTIMER_REG; //滴答当前值
    temp = SYS_Ticktime.SYS_Tick_H;
    temp *= Tick_Full;                                             //乘法一定放后面，尤其是中断的东西
    temp += SYS_Ticktime.SYS_Tick_L;
#endif
    return (temp);
}

void SET_SysTick(uint64_t time)
{
#ifdef Exist_SYS_TIME
    SYS_Ticktime.SYS_Tick_H = time / Tick_Full;         //高位设置
    SYS_Ticktime.SYS_Tick_L = (time % Tick_Full);       //低位设置(不设也行)
    Sys_Time_VAL = Tick_Full - SYS_Ticktime.SYS_Tick_L; //载入低位
#endif
}

void IWDG_Configuration(void)
{
  wwdgt_config(127, 80, WWDGT_CFG_PSC_DIV8);    //40ms
  wwdgt_enable();
}

void feed_watchdog(void)
{
  wwdgt_counter_update(127);
} 

// Delay
//
void cpu_delay_us(int n)
{
    uint64_t start_ticks, end_ticks; //都是滴答数，而非具体标准时间
    uint64_t temp;
    int set_time = n * Freq_us;
    start_ticks = GET_SysTick();
#ifdef Exist_SYS_TIME
    while (1)
    {
        end_ticks = GET_SysTick();

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

void cpu_delay_ms(int n)
{
    uint64_t start_ticks, end_ticks; //都是滴答数，而非具体标准时间
    uint64_t temp;
    int set_time = n * Freq_ms;
    start_ticks = GET_SysTick();
#ifdef Exist_SYS_TIME
    while (1)
    {
        end_ticks = GET_SysTick();

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
        cpu_delay_ms(1000);
    }
}

int cpu_reset_device(unsigned int magickey)
{
	int retval = 0;

	if(magickey != RESET_DEVICE_MAGIC_KEY)
	{
		retval = -1;
		goto done_exit;
	}


	/* wait 10 milliseconds for good measure - hack hack */
	cpu_delay_ms(10);
        NVIC_SystemReset();


	/* if we get here something bad happened and we didn't reset */
	retval = -1;

	done_exit:
	return retval;
}

int cpu_get_lltimer(curtime_t* timestamp)
{
	timestamp->s_tenth = SYS_Ticktime.SYS_Tick_H;
	timestamp->ticks= LLTIMER_REG;

	/* to avoid the 's' and 'ticks' of timestamp asynchronizated by interrupt */
	if(SYS_Ticktime.SYS_Tick_H != timestamp->s_tenth)
	{
        timestamp->s_tenth = SYS_Ticktime.SYS_Tick_H;
        timestamp->ticks= LLTIMER_REG;
	}
	return 0;

}

/*
  cpu_diff_lltimer

  Description:
  Returns the difference in mckticks since lltiemr_stamp
 

  Returns (always) the difference since lltimer_stamp
*/
unsigned int cpu_diff_lltimer(curtime_t lltimer_stamp)
{
	uint64_t curr_s,curr_ticks,diff;

	/* get current time */
	curr_s =  SYS_Ticktime.SYS_Tick_H;
	curr_ticks = LLTIMER_REG;
	/* to avoid the 's' and 'ticks' of timestamp asynchronizated by interrupt */
	if(SYS_Ticktime.SYS_Tick_H != curr_s)
	{
		curr_s = SYS_Ticktime.SYS_Tick_H;
		curr_ticks = LLTIMER_REG;
	}

	if(curr_s >= lltimer_stamp.s_tenth)
	{
        diff = GET_SysTick();
		diff =  diff - lltimer_stamp.s_tenth * Tick_Set_CMP  - lltimer_stamp.ticks;		      
	}
	else
	{          
		diff = ((curr_s + (0xffffffff - lltimer_stamp.s_tenth)+1)*Tick_Set_CMP + 
		curr_ticks - lltimer_stamp.ticks);			
	}
	return (unsigned int)diff;

}

/*
  cpu_diff_us

  Description:
  Returns the difference in MICROseconds since lltiemr_stamp

  CAVEAT
  - must call within the lltier wrap time = which is (0xFFFFFFFF*LLTIMER_TICK_NS)

  Returns (always) the difference since lltimer_stamp
*/
 
unsigned int cpu_diff_us(curtime_t lltimer_stamp)
{
	uint64_t curr_s,curr_ticks,diff;

	/* get current time */
	curr_s =  SYS_Ticktime.SYS_Tick_H;
	curr_ticks = LLTIMER_REG;
	/* to avoid the 's' and 'ticks' of timestamp asynchronizated by interrupt */
	if(SYS_Ticktime.SYS_Tick_H != curr_s)
	{
		curr_s = SYS_Ticktime.SYS_Tick_H;
		curr_ticks = LLTIMER_REG;
	}

	if(curr_s >= lltimer_stamp.s_tenth)
	{     
        diff = GET_SysTick();
		diff = (diff - lltimer_stamp.s_tenth * Tick_Set_CMP - lltimer_stamp.ticks) / Freq_us;  
	}
	else
	{          
		diff = ((curr_s + (0xffffffff - lltimer_stamp.s_tenth)+1)*Tick_Set_CMP +curr_ticks - lltimer_stamp.ticks) 
		         / Freq_us;		                  
	}

	return (unsigned int)diff;

}

/*
  cpu_diff_ms

  Description:
  Returns the difference in MILLIseconds since lltiemr_stamp

  CAVEAT
  - must call within the lltier wrap time = which is (0xFFFFFFFF*LLTIMER_TICK_NS)

  Returns (always) the difference since lltimer_stamp
*/
unsigned int cpu_diff_ms(curtime_t lltimer_stamp)
{
	uint64_t curr_s,curr_ticks,diff;

	/* get current time */
	curr_s =  SYS_Ticktime.SYS_Tick_H;
	curr_ticks = LLTIMER_REG;
	/* to avoid the 's' and 'ticks' of timestamp asynchronizated by interrupt */
	if(SYS_Ticktime.SYS_Tick_H != curr_s)
	{
        curr_s =  SYS_Ticktime.SYS_Tick_H;
        curr_ticks = LLTIMER_REG;
	}
	if(curr_s >= lltimer_stamp.s_tenth)
	{	
		diff = ((curr_s - lltimer_stamp.s_tenth)*Tick_Set_CMP 
		         + curr_ticks - lltimer_stamp.ticks) / Freq_ms;

	}
	else
	{
		diff = ((curr_s + (0xffffffff - lltimer_stamp.s_tenth)+1)*Tick_Set_CMP 
		        + curr_ticks - lltimer_stamp.ticks) / Freq_ms;		
	}
	      
	return (unsigned int)diff;

}



