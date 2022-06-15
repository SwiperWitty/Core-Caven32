/**
  ******************************************************************************
  * File   : usbd_usr.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file includes the user application layer
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usbd_usr.h"
#include <stdio.h>
/** @addtogroup USBD_USER
  * @{
  */

/** @addtogroup USBD_MSC_DEMO_USER_CALLBACKS
  * @{
  */
  
/** @defgroup USBD_USR 
  * @brief    This file includes the user application layer
  * @{
  */ 

/** @defgroup USBD_USR_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_USR_Private_Defines
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_USR_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_USR_Private_Variables
  * @{
  */ 
/*  Points to the DEVICE_PROP structure of current device */
/*  The purpose of this register is to speed up the execution */

USB_Device_User_cb_Type USER_cb =
{
  USB_DEVICE_USER_Init,
  USB_DEVICE_USER_DeviceReset,
  USB_DEVICE_USER_DeviceConfigured,
  USB_DEVICE_USER_DeviceSuspended,
  USB_DEVICE_USER_DeviceResumed,
  USB_DEVICE_USER_DeviceConnected,
  USB_DEVICE_USER_DeviceDisconnected,    
};
/**
  * @}
  */

/** @defgroup USBD_USR_Private_Constants
  * @{
  */ 

#define USER_INFORMATION1  (uint8_t*)"INFO : Single Lun configuration"
#define USER_INFORMATION2  (uint8_t*)"INFO : microSD is used"
/**
  * @}
  */



/** @defgroup USBD_USR_Private_FunctionPrototypes
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_USR_Private_Functions
  * @{
  */ 

/**
* @brief  Displays the message on LCD on device lib initialization
* @param  None
* @retval None
*/
void USB_DEVICE_USER_Init(void)
{
  GPIO_InitType GPIO_InitStructure;
#ifdef USB_OTG_FS_LOW_PWR_MGMT_SUPPORT 
  EXTI_InitType EXTI_InitStructure;
#endif
  NVIC_InitType NVIC_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA, ENABLE);
  
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins  = GPIO_Pins_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU;
  GPIO_Init(USB_GPIO, &GPIO_InitStructure);
  
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins  = GPIO_Pins_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU;
  GPIO_Init(USB_GPIO, &GPIO_InitStructure);
  
  NVIC_InitStructure.NVIC_IRQChannel = USBOTG_IRQn;  
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority           = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd                   = ENABLE;
  NVIC_Init(&NVIC_InitStructure); 
#ifdef USB_OTG_FS_LOW_PWR_MGMT_SUPPORT  
  EXTI_ClearIntPendingBit(EXTI_Line18);

  EXTI_InitStructure.EXTI_Line    = EXTI_Line18; 
  EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineEnable = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  EXTI_ClearIntPendingBit(EXTI_Line18);   

  NVIC_InitStructure.NVIC_IRQChannel                      = USBWakeUp_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority           = 5;
  NVIC_InitStructure.NVIC_IRQChannelCmd                   = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  EXTI_ClearIntPendingBit(EXTI_Line18);

#endif
}

/**
* @brief  Displays the message on LCD on device reset event
* @param  speed : device speed
* @retval None
*/
void USB_DEVICE_USER_DeviceReset (uint8_t speed)
{
	switch (speed)
	{
		case USB_OTG_SPEED_HIGH: 
			break;
		
		case USB_OTG_SPEED_FULL: 
			break;
		
		default:
			break;
	}
}


/**
* @brief  Displays the message on LCD on device config event
* @param  None
* @retval Status
*/
void USB_DEVICE_USER_DeviceConfigured (void)
{

}
/**
* @brief  Displays the message on LCD on device suspend event 
* @param  None
* @retval None
*/
void USB_DEVICE_USER_DeviceSuspended(void)
{
}


/**
* @brief  Displays the message on LCD on device resume event
* @param  None
* @retval None
*/
void USB_DEVICE_USER_DeviceResumed(void)
{

}

/**
* @brief  USBD_USR_DeviceConnected
*         Displays the message on LCD on device connection Event
* @param  None
* @retval Status
*/
void USB_DEVICE_USER_DeviceConnected (void)
{
}


/**
* @brief  USBD_USR_DeviceDisonnected
*         Displays the message on LCD on device disconnection Event
* @param  None
* @retval Status
*/
void USB_DEVICE_USER_DeviceDisconnected (void)
{
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

/**
  * @}
  */

