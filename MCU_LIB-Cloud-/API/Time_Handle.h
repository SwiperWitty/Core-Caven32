#ifndef _TIME_HANDLE__H_
#define _TIME_HANDLE__H_

/*
        【API】      ————C语言环境即可运行
    Over_Time       数据超时函数
    Over_Time增加稳定性
                            ————2022年9月5日
    
*/

#include "Caven.h"

struct _Over_time
{
    const void *Now_data;                           //【填】数据的地址，极限是int型
//	int Data_num;
    int last_data;                                  //不用填 （用 int 做向下兼容）【系统行为】
    char load;                                      //装载标志位 0 允许装载， 1 装载完成(尽可能使用实体，不用使用公共的静态，不然多个使用会出错)，不用管【系统行为】
    volatile char Flag;                             //瞬间态标志位，用来【读取】
    volatile char Flip;                             //翻转态标志位，用来【读取】
    struct Caven_Watch Set_Time;                    //【填】
    struct Caven_Watch last_Time;                   //不用填【系统行为】
	const struct Caven_Watch * Now_Time;			//【填】系统时间，只读（需要手动绑定系统时钟）
};

struct Time_Handle_
{
    int (*Hourly_to_Seconds)(struct Caven_Watch Item);
	struct  Caven_Watch (*Seconds_to_Hourly) (int Seconds);
    char (*Over_Time)(struct _Over_time *Item);
};

//函数目录
char Over_Time(struct _Over_time *Item);            //此函数会被多次调用，请不要循环对多个功能使用函数内部变量
int Hourly_to_Seconds(struct Caven_Watch Item);
struct Caven_Watch Seconds_to_Hourly (int Seconds);


#endif
