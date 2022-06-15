#ifndef _Control_Mode_h__
#define _Control_Mode_h__

#include "PWM.h"

#define OUT1 1
#define OUT2 2
#define OUT_ON 1000
#define OUT_OF 0

void Control_Mode_Init (void);
void Control_Mode_OUT(char OUTX,int PWM);

#endif
