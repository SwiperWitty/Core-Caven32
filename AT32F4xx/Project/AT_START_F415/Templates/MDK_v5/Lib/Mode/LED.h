#ifndef _LED__H_
#define _LED__H_

#define LED_6016    1

#include "base.h"           //相当于载入 #include "at32f4xx.h"
#include "Caven.h"

struct LED_
{
    char (*Switch)(char Channel);
    void (*Power_LED)(char state);
};


void LED_GPIO_Init(void);
char LED_X(char Channel);
void Power_LED(char state);
#endif
