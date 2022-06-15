#ifndef _MODE_H__
#define _MODE_H__

#include "at32f4xx.h"
#include "uart_x.h"

/*
    本MODE文件依赖于 UART文件，AT32的SDK文件
    移植需要修改大量底层寄存器
    不支持跨平台
*/

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

#define U8 unsigned char
#define S8 signed char
#define U16 unsigned short
#define S16 signed short
#define U32 unsigned long
#define S32 signed long

struct _Mode
{
    void (*RF_GPIO_Init)(void);
    void (*LED_GPIO_Init)(void);

    char (*Switch_RF)(char RF_X);
    char (*LED_X)(char Channel);

    void (*UARTx_Init)(char Channel,int bound);
    void (*UARTx_Send_String)(char Channel,const char * String);
    void (*UARTx_Send_Data)(char Channel,const unsigned char * Data,int Length);
};

extern struct _Mode Mode_Test_6016;         //板载硬件资源

void Mode_Init(void);

void LED_GPIO_Init(void);
void RF_GPIO_Init(void);

char Switch_RF(char RF_X);
char LED_X(char Channel);

#endif
