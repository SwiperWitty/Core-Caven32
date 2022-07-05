#ifndef _DATA__H_
#define _DATA__H_

/*
    本文件用于各种数据处理，它一定会很庞大的             ————2022.6.20
    Over_Time       数据超时函数

    API不兼容                       ————2022.6.20
    
*/

#include "Caven.h"


struct _Over_time
{
    char *Now_data_1;
    U16 *Now_data_2;
    int *Now_data;
//	int Data_num;
    int last_data;                                  //不用填
    char load;                                      //装载标志位 0 允许装载， 1 装载完成(尽可能使用实体，不用使用公共的静态，不然多个使用会出错)
    char Flag;                                      //瞬态标志位，不用管
	char Flip;                                      //翻转态标志位，不用管
    struct Caven_Watch Set_Time;
    struct Caven_Watch last_Time;                   //不用填
	const struct Caven_Watch * Now_Time;			//系统时间，只读（需要手动绑定系统时钟）
};

struct DATA_Time_
{
    int (*Hourly_to_Seconds)(struct  Caven_Watch Item);
	struct  Caven_Watch (*Seconds_to_Hourly) (int Seconds);
    char (*Over_Time)(struct _Over_time *Item);
};

//函数目录
char Over_Time(struct _Over_time *Item);            //此函数会被多次调用，请不要循环对多个功能使用函数内部变量
int Hourly_to_Seconds(struct Caven_Watch Item);
struct Caven_Watch Seconds_to_Hourly (int Seconds);


#endif
