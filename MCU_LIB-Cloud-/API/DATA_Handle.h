#ifndef _DATA__H_
#define _DATA__H_

/*
        【API】      ————C语言环境即可运行
        本文件用于各种数据处理，它一定会很庞大的             ————2022.6.20
    Over_Time       数据超时函数

    API不兼容                       ————2022.6.20
    
*/

#include "Caven.h"

struct _Chang_Num
{
    const char *Source;
    char Sign[8];           //取数符
    float NUM[8];           //取数结果
    char flag;
};

struct Data_Handle_
{
    int (*Find_Data)(const U8 *source, char Target, int Length);
    
};

int Find_Data(const U8 *source, char Target, int Length);
char Data_Chang_NUM (struct _Chang_Num *Item);

#endif

