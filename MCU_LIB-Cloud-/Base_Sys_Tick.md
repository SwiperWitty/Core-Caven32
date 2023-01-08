# Base的设计原理

### Sys_Time

​	基于滴答定时器（SysTick），主要服务于【Mode->Time】这个定时器在不同单片机上也有所不同，基本上很多单片机都是自减型的滴答定时器，我们设置的Base_Sys_Time结构如下：

![image-20220917170159290](https://raw.githubusercontent.com/SwiperWitty/img/main/img/image-20220917170159290.png)

#### 初始配置

​	**滴答定时器频率设置**

​	这个要分情况讨论，24位滴答定时器/64位滴答定时器，假设系统主时钟频率240Mhz。

~~~c
//	24位的滴答定时器
#include <math.h>

int MAX_SYS_TICK = pow(2,24);		//	也就是 16 777 216
/*如果此时用系统主时钟频率240Mhz作为滴答定时器频率，将初始值设满 MAX_SYS_TICK，溢出仅需 1/ (240mhz / MAX_SYS_TICK) = 0.0699S.明显太快了。所有要降低滴答定时器频率，也就是分频。

如果我们进行8分频，滴答定时器频率 = 系统主时钟频率 / 8 = 30Mhz,那么溢出需要 1/ (30mhz / MAX_SYS_TICK) = 0.5592S.这样看起来就合理多了。
*/
systick_clock_source_config(SYSTICK_CLOCK_SOURCE_AHBCLK_NODIV);		//不分频
systick_clock_source_config(SYSTICK_CLOCK_SOURCE_AHBCLK_DIV8);		//8分频


~~~



~~~c
//	64位的滴答定时器
#include <math.h>

u64 MAX_SYS_TICK = pow(2,64);		//	也就是 18,446,744,073,709,551,616
/*如果此时用系统主时钟频率240Mhz作为滴答定时器频率，将初始值设满 MAX_SYS_TICK，溢出需 1/ (240mhz / MAX_SYS_TICK ) = 76,861,433,640S.
*/
~~~

​	**对于64位的滴答定时器**就无所谓了，看需求配置，频频的主要意义就是设置一个合理的最大溢出时间，唯一的缺陷就是最小刻度会变大（不那么的精确），但是240Mhz的系统主时钟频率做8分频=30Mhz，T = 1/F，T = 0.03us.我想这个精度98%的场景下都是适用的。



​	**时钟初始值设置**

​	解决频率配置问题，时钟初始值设置就很容易了。设置为1S溢出一次，就是滴答定时器频率大小。But，以240Mhz系统主时钟频率为例，滴答定时器频率为其8分频，也就是30Mhz。

**对于24位的滴答定时器**，时钟初始值设置可以等于30M吗？

**2的24次方 = 16 777 216，这个值  <  30M**

所以，我们需要选一个合适的溢出时间，例如0.5S。

那么对于我们的时钟初始值来说就是 30M / 2 正好 < 2的24次方，初始值可以装得下它。

~~~c
int SystemCoreClock    = 240 000 000;
#define MCU_SYS_Freq SystemCoreClock		//系统频率

#define	Tick_Frequency	MCU_SYS_Freq/8		//滴答定时器频率
#define	Tick_Set_CMP	Tick_Frequency/2	//设置滴答初始值
#define	Tick_Over_IT	0.50  	            //定时器溢出时间（24位滴答才有）

systick_clock_source_config(SYSTICK_CLOCK_SOURCE_AHBCLK_DIV8);	//8分频
SysTick_Config(Tick_Set_CMP);									//设置滴答初始值（不能大于滴答定时器位数），也就是0.5S溢出一次

~~~

​	**对于64位的滴答定时器**，时钟初始值设置直接设满！

~~~c
#define Tick_Set_CMP (u64)(~0x00)		//设置滴答初始值

SysTick_Config(Tick_Set_CMP);
~~~



#### 中断

**对于24位的滴答定时器**，根据上面的描述，每0.5S进入一次中断。

我们设置一个记录的变量，记录总共过了多少个0.5s

~~~c
struct _SYS_Ticktime
{
    volatile uint32_t SYS_Tick_H;         //每中断一次进1 
    volatile uint32_t SYS_Tick_L;         //24bit 的
};
struct _SYS_Ticktime SYS_Ticktime = {0};

void SysTick_Handler(void)	//这是中断
{
    SYS_Ticktime.SYS_Tick_H++;
}
~~~



**对于64位的滴答定时器**，不需要中断。



#### 设置/获取时间

​	**设置时间**

~~~c
//24位的滴答定时器
void SET_SysTick (uint64_t time)
{
    SYS_Ticktime.SYS_Tick_H = time / Tick_Set_CMP;		//高位设置
    SYS_Ticktime.SYS_Tick_L = time % Tick_Set_CMP;		//低位设置
    
    SysTick->VAL = Tick_Set_CMP - SYS_Ticktime.SYS_Tick_L;	//载入低位
}
~~~



~~~c
//64位的滴答定时器
void SET_SysTick (uint64_t time)
{
    SysTick->VAL = time;	//载入
}
~~~

​	

​	**获取时间**

~~~c
//24位的滴答定时器
uint64_t GET_SysTick (void)
{
    uint64_t temp = 0;
    
    SYS_Ticktime.SYS_Tick_L = (Tick_Set_CMP - SysTick->VAL);   //滴答当前值（因为是自减）
    temp = SYS_Ticktime.SYS_Tick_H;
    temp *= Tick_Set_FUll;                                      //乘法一定放后面，尤其是中断的东西
    temp += SYS_Ticktime.SYS_Tick_L;
    return (temp);
}
~~~



~~~c
//64位的滴答定时器

uint64_t GET_SysTick (void)
{
    uint64_t temp = 0;
    
    temp = Tick_Set_CMP - SysTick->VAL;   //滴答当前值（因为是自减）
    return (temp);
}
~~~



#### Delay函数

~~~c
#define Freq_us     (Tick_Frequency/1000000)   //微秒
#define Freq_ms     (Tick_Frequency/1000)      //毫秒

void SYS_Delay_us (int n)
{
    uint64_t start_ticks,end_ticks;
    uint64_t temp;
    uint64_t set_time = n * Freq_us;
    start_ticks = GET_SysTick();
    while(1)
    {
        end_ticks = GET_SysTick();
        
        if (end_ticks > start_ticks)
        {
            temp = end_ticks - start_ticks;
        }
        else
        {
            uint64_t temp = 86400 * MCU_SYS_Freq;		//一天时间 * 滴答频率
            temp -= start_ticks;
            temp += end_ticks;
        }
        if (temp >= set_time)
        {
            break;
        }
    }
}

void SYS_Delay_ms (int n)
{
    uint64_t start_ticks,end_ticks;
    uint64_t temp;
    int set_time = n * Freq_ms;
    start_ticks = GET_SysTick();
    while(1)
    {
        end_ticks = GET_SysTick();
        
        if (end_ticks > start_ticks)
        {
            temp = end_ticks - start_ticks;
        }
        else
        {
            uint64_t temp = 86400 * MCU_SYS_Freq;		//一天时间 * 滴答频率
            temp -= start_ticks;
            temp += end_ticks;
        }
        if (temp >= set_time)
        {
            break;
        }
    }
}

void SYS_Delay_S (int n)
{
    for (int var = 0; var < n; ++var)
    {
        SYS_Delay_ms (1000);
    }
}
~~~









