#ifndef _TIME_H__
#define _TIME_H__

/*
    这个文件不依赖MCU的MDK（指编译能过）
    但是需要使用Base文件才能正常运行
    一般依赖于 SysTick & SysTick_Handler，缺一不可
    如果没有，即可使用普通定时器（16位及以上）
    
    我做了一件及其愚蠢的事情（耗时2天），以后所有的栈都必须加括号！！！！！
                                                                        ————2022.10.21
    
    
*/

#include "Base.h"
#include "Caven.h"

struct _SYS_Time
{
    struct Caven_Date Date;
    struct Caven_Watch Watch;
    volatile int sys_sec;
};

struct Delay_
{
    void (*Delay_us)(int num);
    void (*Delay_ms)(int num);
    void (*Delay_S)(int num);
};

struct TIME_
{
	void (*Set_TIME) (struct Caven_Watch);
	struct Caven_Watch (*Get_TIME) (void);
	int (*Get_Lose_Tiem) (struct Caven_Watch time);
};

#ifdef Exist_SYS_TIME
extern struct _SYS_Time SYS_Time;
#endif

void Sys_Clock_Init(int SET);

void Delay_10us(int num);
void Delay_ms(int num);
void Delay_S(char num);

void Set_TIME (struct Caven_Watch);
struct Caven_Watch Get_TIME (void);
int Get_Lose_Tiem (struct Caven_Watch time);


#endif
