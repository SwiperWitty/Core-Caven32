#ifndef _BASE_EXIST_GPIO__H_
#define _BASE_EXIST_GPIO__H_

#ifdef DEFAULT
#include "items.h"              //默认功能
#endif

#ifndef DEFAULT
#include "User_items.h"         //自行设置功能，一般出现在本地文件的User中
#endif

/*
**		GPIO Map 
** KEY	P03
** GPO	P23 
** GPI	P24 
** MOSA	P26 
** MOSB	P25 
** ADC	P35
** 
*/

typedef enum
{
    GPO = 1,
    MOSA,
    MOSB,
}User_GPIO_Type;


#define GPO_L()     Gpio_SetIO(2,3,0)   // GPO FALSE 
#define MOSA_L()    Gpio_SetIO(2,6,0)   // MOSA
#define MOSB_L()    Gpio_SetIO(2,5,0)   // MOSB

#define GPO_H()     Gpio_SetIO(2,3,1)   // GPO
#define MOSA_H()    Gpio_SetIO(2,6,1)   // MOSA
#define MOSB_H()    Gpio_SetIO(2,5,1)   // MOSB

#define GPI_R()     Gpio_GetIO(2,4)     // GPI
#define KEY_R()     Gpio_GetIO(0,3)     // KEY

void User_GPIO_Init(int Set);
void User_GPIO_set (User_GPIO_Type Type,int Set);
#endif 
