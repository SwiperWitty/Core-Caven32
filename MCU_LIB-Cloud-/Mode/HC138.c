#include "HC138.h"

void HC138_Init (int SET)
{
#ifdef Exist_HC138
    HC138_GPIO_Init(SET);
#endif
}

//结果（构想）--动作--结果（实现）
void HC138_Set_Data (int Data)
{
#ifdef Exist_HC138
    if(Data < 1 || Data > 8)
        return ;
    Data--;
    if (Data & 0x01)
    {
        HC138_D1_H();
    }
    else
    {
        HC138_D1_L();
    }
    if (Data & 0x02)
    {
        HC138_D2_H();
    }
    else
    {
        HC138_D2_L();
    }
    if (Data & 0x04)
    {
        HC138_D3_H();
    }
    else
    {
        HC138_D3_L();
    }
    printf("asd %d \r\n",Data);
#endif
}
