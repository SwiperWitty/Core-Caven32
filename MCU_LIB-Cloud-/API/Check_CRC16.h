#ifndef _Check_CRC16_H_
#define _Check_CRC16_H_

#define CRC_16_CCITT            0x1021

#include "Caven.h"
/*
        【API】      ————C语言环境即可运行
    Over_Time       数据超时函数


*/

U16 CRC16_CCITT_CalculateBuf(unsigned char *ptr, U16 len);

#endif
