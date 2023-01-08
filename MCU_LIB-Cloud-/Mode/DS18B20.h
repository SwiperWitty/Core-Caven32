#ifndef _DS18B20__H_
#define _DS18B20__H_

#include "Base.h"

/*
	// 30min  set 60max
	// 1-L  15min read (60min next)
  本文件只提供方法-WAY  
*/

struct DS18B20_
{
    float (*Get_Temp) (void);
};


char DS18B20_Init (int Set);

float DS18B20_Get_Temp(void);


#endif
