/**
  ******************************************************************************
  * File   : usb_bsp.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file implements the board support package for the USB host library
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/

#include "usb_bsp.h"
#include "usb_conf.h"
#include "usb_core.h"
#include "wm8988.h"
/** @addtogroup USBH_USER
* @{
*/

/** @defgroup USB_BSP
  * @brief This file is responsible to offer board support package
  * @{
  */ 

/** @defgroup USB_BSP_Private_Defines
  * @{
  */ 
#define TIM_MSEC_DELAY                     0x01
#define TIM_USEC_DELAY                     0x02

#define HOST_POWERSW_PORT_RCC             RCC_APB2PERIPH_GPIOC
#define HOST_POWERSW_PORT                 GPIOC
#define HOST_POWERSW_VBUS                 GPIO_Pins_9

#define HOST_SOF_OUTPUT_RCC                RCC_APB2PERIPH_GPIOA
#define HOST_SOF_PORT                      GPIOA
#define HOST_SOF_SIGNAL                    GPIO_Pins_8

/**
  * @}
  */ 


/** @defgroup USB_BSP_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 



/** @defgroup USB_BSP_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBH_BSP_Private_Variables
  * @{
  */ 
//ErrorStatus HSEStartUpStatus;
#ifdef USE_ACCURATE_TIME 
__IO uint32_t BSP_delay = 0;
#endif
/**
  * @}
  */ 

/** @defgroup USBH_BSP_Private_FunctionPrototypes
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup USB_BSP_Private_Functions
  * @{
  */ 
static void USB_OTG_BSP_TimeInit ( void );
/**
  * @brief  USB_OTG_BSP_Init
  *         Initializes BSP configurations
  * @param  None
  * @retval None
  */

void USB_OTG_BSP_Init(USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_OTG_BSP_TimeInit();
}

/**
  * @brief  BSP_Drive_VBUS
  *         Drives the Vbus signal through IO
  * @param  state : VBUS states
  * @retval None
  */

void USB_OTG_BSP_DriveVBUS(USB_OTG_CORE_HANDLE *pusbdev, uint8_t state)
{
  /*
  On-chip 5 V VBUS generation is not supported. For this reason, a charge pump 
  or, if 5 V are available on the application board, a basic power switch, must 
  be added externally to drive the 5 V VBUS line. The external charge pump can 
  be driven by any GPIO output. When the application decides to power on VBUS 
  using the chosen GPIO, it must also set the port power bit in the host port 
  control and status register (PPWR bit in OTG_FS_HPRT).
  
  Bit 12 PPWR: Port power
  The application uses this field to control power to this port, and the core 
  clears this bit on an overcurrent condition.
  */
  
  if (0 == state)
  { 
    /* DISABLE is needed on output of the Power Switch */
    GPIO_SetBits(HOST_POWERSW_PORT, HOST_POWERSW_VBUS);
  }
  else
  {
    /*ENABLE the Power Switch by driving the Enable LOW */
    GPIO_ResetBits(HOST_POWERSW_PORT, HOST_POWERSW_VBUS);
  }

}

/**
  * @brief  USB_OTG_BSP_ConfigVBUS
  *         Configures the IO for the Vbus and OverCurrent
  * @param  None
  * @retval None
  */

void  USB_OTG_BSP_ConfigVBUS(USB_OTG_CORE_HANDLE *pusbdev)
{ 
}



/**
  * @brief  USB_OTG_BSP_TimeInit
  *         Initializes delay unit using Timer2
  * @param  None
  * @retval None
  */
static void USB_OTG_BSP_TimeInit ( void )
{
}
/**
  * @brief  USB_OTG_BSP_mDelay
  *          This function provides delay time in milli sec
  * @param  msec : Value of delay required in milli sec
  * @retval None
  */
void USB_OTG_BSP_mDelay (const uint32_t msec)
{ 
  Delay_ms(msec);   
}

/**
  * @brief  USB_OTG_BSP_uDelay
  *         This function provides delay time in micro sec
  * @param  usec : Value of delay required in micro sec
  * @retval None
  */
void USB_OTG_BSP_uDelay (const uint32_t usec)
{
  Delay_us(usec);  
}

/**
* @}
*/ 

/**
* @}
*/ 

/**
* @}
*/

