/*
  **************************************************************************
  * File   : hw_config.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Hardware Configuration & Setup
  **************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include <at32f4xx.h>
#include "usb_type.h"


/* Exported types ------------------------------------------------------------*/
/* Codec Control defines */
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/* External variables --------------------------------------------------------*/
void Set_USBClock(void );
void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void Get_SerialNum(void);
void USB_Interrupts_Config(void);
#endif  /*__HW_CONFIG_H*/


