#ifndef __MLX90614_H
#define __MLX90614_H

//PB10   SCL
//PB11   SDA

#include "stm32f10x.h"
#include "IIC.h"

//使用软件IIC			--2022.04.06

#define	IIC_Speed 3

#define IIC_Read(ack_nack, Speed)	IICs_ReadByte(ack_nack, Speed)
#define IIC_Send(data, Speed)		IICs_SendByte(data, Speed)
#define IIC_Init(SET)	IIC_Software_Init(SET)

void Mlx90614_Init(void);
float Mlx90614_ReadTemp(void);



#endif

