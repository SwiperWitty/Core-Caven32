#ifndef _HC138_H_
#define _HC138_H_

#include "Base.h"
#include "Caven.h"

/*
    38-译码器（8选1）
    管脚定义在 Base_Exist_GPIO 文件
*/

struct HC138_
{
    void (*Set_DATA) (int Data);
};

void HC138_Init (int SET);
void HC138_Set_Data (int Data);

#endif
