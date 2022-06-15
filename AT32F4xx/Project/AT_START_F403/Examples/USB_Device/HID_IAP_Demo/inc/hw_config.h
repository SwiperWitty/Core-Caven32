/**
  ******************************************************************************
  * File   : USB_Device/HID_IAP_Demo/inc/hw_config.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : USB Hardware Configuration & setup header
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include <at32f4xx.h>
#include "usb_type.h"


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define DOWN            1
#define LEFT            2
#define RIGHT           3
#define UP              4
#define CURSOR_STEP     40

/* Exported functions ------------------------------------------------------- */
void Set_System(void);
void Set_USBClock(void);
void GPIO_AINConfig(void);
void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void USB_Interrupts_Config(void);
void USB_Cable_Config (FunctionalState NewState);

void Get_SerialNum(void);
#endif  /*__HW_CONFIG_H*/


