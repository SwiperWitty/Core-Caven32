/**
  ******************************************************************************
  * File   : RCC/NewPLLconfig/PLLconfig.h
  * Version: V1.x.x
  * Date   : 2020-xx-xx
  * Brief  : header file of pll config.
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AT32_PLLCONFIG_H
#define __AT32_PLLCONFIG_H

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"

/* Exported types ------------------------------------------------------------*/
/* typedef return status */
typedef enum {
  Error = 0,
  Ok,
}Status;

/* typedef pll config mode */
typedef enum {
  NORMAL_MODE = 0,
  FLEXIBLE_MODE,
}HSE;

/* typedef hsi48 to sysclk mode */
typedef enum {
  HSI8M_FOR_SYSCLK_ENABLE = 0,
  HSI48M_FOR_SYSCLK_ENABLE,
}HSIFreqMode;

/* typedef usb clock mode */
typedef enum {
  HSI_FOR_USB_DISABLE = 0,
  HSI_FOR_USB_ENABLE,
}USBClkMode;

/* typedef osc init struct */
typedef struct OscInitType {
  uint8_t  LSEState;
  uint32_t HSIState;
  uint32_t HSEState;
  uint32_t PLLState;
  uint32_t PLLConfigMode;
  uint32_t PLLRC;
  uint32_t PLLSourceHSEPsc;
  uint32_t PLLMUL;
  uint32_t PLLNS;
  uint32_t PLLMS;
  uint32_t PLLFR;
}OscInitType;

/* typedef clock init struct */
typedef struct SysClkInitType {
  uint32_t SYSCLKSel;
  uint32_t Flatency;
  uint32_t AHBClkDivider;
  uint32_t APB2ClkDivider;
  uint32_t APB1ClkDivider;
  uint32_t USBDivider;
  uint32_t CLKOUTSel;
  uint32_t CLKOUTPre;
  uint32_t HSI48MToSYSCLK;
  uint32_t HSI48MToUSB;
}SysClkInitType;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define RCC_HSI_DISABLE                 ((uint32_t)0x00000000)
#define RCC_HSI_ENABLE                  ((uint32_t)0x00000001)

#define RCC_PLL_DISABLE                 ((uint32_t)0x00000000)
#define RCC_PLL_ENABLE                  ((uint32_t)0x01000000)

/* Exported functions ------------------------------------------------------- */
void NewPLLconfig(void);

#endif /* __AT32_PLLCONFIG_H */

/******************* (C) COPYRIGHT 2020 ArteryTek *****END OF FILE****/

