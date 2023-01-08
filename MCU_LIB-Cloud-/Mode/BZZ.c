#include "BZZ.h"

void BZZ_Init(int SET)
{
    #ifdef Exist_BZZ
    BZZ_GPIO_Init(SET);
    BZZ_SET(DISABLE);
    #endif
}

/*
 * 蜂鸣器必须用NPN管不然给爷死
 */
void BZZ_SET(int SET)
{
    #ifdef Exist_BZZ
    if(SET)
        BZZ_H();
    else
        BZZ_L();
    #endif
}


