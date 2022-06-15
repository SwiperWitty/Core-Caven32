/**
  ******************************************************************************
  * File   : USB_Device/VirtualMassStorage_IAP_Demo/inc/hw_config.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : USB Hardware Configuration & setup header
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
#include "usb_type.h"


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define USBCLK_FROM_HSE  1
#define USBCLK_FROM_HSI  2
#define BULK_MAX_PACKET_SIZE  0x00000040
/* Exported functions ------------------------------------------------------- */
void Set_System(void);
void Set_USBClock(u8 Clk_Source);
void GPIO_AINConfig(void);
void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void USB_Interrupts_Config(void);
void USB_Cable_Config (FunctionalState NewState);
void Joystick_Send(u8 buf0,u8 buf1,u8 buf2,u8 buf3);
u8 JoyState(void);
void Get_SerialNum(void);

#endif  /*__HW_CONFIG_H*/


