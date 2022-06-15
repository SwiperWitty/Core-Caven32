#ifndef __MLX90614_H
#define __MLX90614_H

//	无法跨芯片移植

//PB10   SCL
//PB11   SDA

#include "stm32f10x.h"

void Mlx90614_Init(void);
float Mlx90614_ReadTemp(void);

#endif

