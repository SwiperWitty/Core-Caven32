#ifndef _Check_CRC16_H_
#define _Check_CRC16_H_

#define CRC_16_CCITT            0x1021

#include "Caven.h"

U16 CRC16_CCITT_CalculateBuf(unsigned char *ptr, U16 len);

#endif
