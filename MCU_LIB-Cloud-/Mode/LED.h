#ifndef _LED__H_
#define _LED__H_

#include "Base.h"
#include "Caven.h"

/*




*/
struct LED_
{
    void (*LED_SET)(char Channel,int SET);
    void (*LED_REG)(struct Caven_Color Color,int SET);
};

void LED_Init(int SET);

void LED_SET(char Channel,int SET);
void LED_REG(struct Caven_Color Color,int SET);

#endif
