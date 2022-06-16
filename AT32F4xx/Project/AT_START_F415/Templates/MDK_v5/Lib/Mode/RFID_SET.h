#ifndef _RFID_SET__H_
#define _RFID_SET__H_

#include "base.h"
#include "Caven.h"

#define RF_01() SKY_13418(1, 7)
#define RF_02() SKY_13418(1, 5)
#define RF_03() SKY_13418(1, 3)
#define RF_04() SKY_13418(1, 1)
#define RF_05() SKY_13418(1, 0)
#define RF_06() SKY_13418(1, 2)
#define RF_07() SKY_13418(1, 4)
#define RF_08() SKY_13418(1, 6)

#define RF_09() SKY_13418(2, 7)
#define RF_10() SKY_13418(2, 5)
#define RF_11() SKY_13418(2, 3)
#define RF_12() SKY_13418(2, 1)
#define RF_13() SKY_13418(2, 0)
#define RF_14() SKY_13418(2, 2)
#define RF_15() SKY_13418(2, 4)
#define RF_16() SKY_13418(2, 6)

struct _RFID
{
    char (*Switch)(char Channel);
};


void RF_GPIO_Init(void);
char Switch_RF(char RF_X);

#endif
