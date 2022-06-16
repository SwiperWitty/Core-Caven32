#ifndef _Check_CRC16_H_
#define _Check_CRC16_H_

#define CRC_16_CCITT            0x1021

#include "Caven.h"

unsigned short CRC16_CCITT_CalculateBuf(unsigned char *ptr, unsigned short len);

#endif
