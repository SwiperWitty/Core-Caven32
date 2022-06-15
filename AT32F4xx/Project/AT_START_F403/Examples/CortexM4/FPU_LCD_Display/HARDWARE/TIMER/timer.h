/**
  **************************************************************************
  * File   : time.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : none
  **************************************************************************
  */

#ifndef _TIMER_H
#define _TIMER_H
#include "at32f4xx.h"

void TMR3_Int_Init(u16 arr,u16 psc);
#endif
