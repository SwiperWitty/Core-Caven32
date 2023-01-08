#include "MODE_Time.h"
#include "API.h"

//防止被编译器优化，并且只能在本文件使用
/*
 * while(a);
 * 和
 * while(get(a));
 * 不一样
 */


struct _SYS_Time SYS_Time = {
    .Watch.hour = 23,
    .Watch.minutes = 59,
    .Watch.second = 56,
};
int Tick_Freq;
int Tick_ms;
int Tick_us;

void Sys_Clock_Init(int SET)
{
#ifdef Exist_SYS_TIME
    Sys_Time_Init (SET);
    Tick_Freq = Tick_Frequency;
    Tick_ms = Tick_Freq / 1000;
    Tick_us = Tick_Freq / 1000000;
#endif
}

/*
 * Set_Time只精确到秒
 */
void Set_TIME (struct Caven_Watch time)
{
#ifdef Exist_SYS_TIME
    int Seconds;
    Seconds = Hourly_to_Seconds(time);
    SET_SysTick((U64)Seconds*(Tick_Freq));

#endif
}


struct Caven_Watch Get_TIME (void)
{
    struct Caven_Watch temp_Watch = {0};
    
#ifdef Exist_SYS_TIME
    U64 Temp = GET_SysTick();
    int Seconds = (int)(Temp / Tick_Freq);
    temp_Watch = Seconds_to_Hourly(Seconds);

    if((Seconds / 86400) > 0)        //下一天
    {
        Destroy(&temp_Watch,sizeof(temp_Watch));
        Set_TIME (temp_Watch);      //重设时间戳0,0,0

    }
    temp_Watch.time_us = (Temp % Tick_Freq) / Tick_us;
    SYS_Time.Watch = temp_Watch;
#endif
    return temp_Watch;
}

int Get_Lose_Tiem (struct Caven_Watch time)
{
	int temp = 0;
#ifdef Exist_SYS_TIME
	struct Caven_Watch Now = Get_TIME();
	temp = Now.time_us - time.time_us;
	if(temp < 0)
	{
		Now.second--;
	}
	temp += (Now.second - time.second) * 1000000;
#endif
	return temp;
}

void Delay_10us(int num)
{

}

void Delay_ms(int num)
{

}

void Delay_S(char num)
{

}
