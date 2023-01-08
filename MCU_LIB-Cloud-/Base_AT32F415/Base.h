#ifndef _BASE_H__
#define _BASE_H__

#ifdef DEFAULT
#include "items.h"              //默认功能
#endif

#ifndef DEFAULT
#include "User_items.h"         //自行设置功能，一般出现在本地文件的User中
#endif

/****************/

#include "Base_Exist_GPIO.h"    //items的附属（LED、BZZ之类的）

#ifdef Exist_SYS_TIME          //这种保护不占内存，所以尽可能写
#include "Base_sys_time.h"
#endif

#ifdef Exist_FLASH
#include "Base_Flash.h"
#endif

#ifdef Exist_UART
#include "Base_UART.h" 
#endif

#ifdef Exist_SPI
#include "Base_SPI.h" 
#endif

#ifdef Exist_IIC
#include "Base_IIC.h" 
#endif

#ifdef Exist_ADC
#include "Base_ADC.h" 
#endif

#ifdef Exist_PWM
#include "Base_PWM.h"
#endif

//Base文件没有结构体索引
#endif
