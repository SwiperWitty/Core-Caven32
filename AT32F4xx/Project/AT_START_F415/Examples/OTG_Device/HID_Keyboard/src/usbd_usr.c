/**
  ******************************************************************************
  * File   : OTG_Device/VCP_Loopback/src/usbd_usr.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file includes the user application layer
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "usbd_usr.h"

/** @addtogroup AT32_USB_OTG_DEVICE_LIBRARY
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
* @brief  USB_DEVICE_USER_Init 
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
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority           = 3;
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
* @brief  USB_DEVICE_USER_DeviceReset 
* @param  speed : device speed
* @retval None
*/
void USB_DEVICE_USER_DeviceReset(uint8_t speed )
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
* @brief  USB_DEVICE_USER_DeviceConfigured
*         Displays the message  on device configuration Event
* @param  None
* @retval Status
*/
void USB_DEVICE_USER_DeviceConfigured (void)
{
}

/**
* @brief  USB_DEVICE_USER_DeviceSuspended 
*         Displays the message on device suspend Event
* @param  None
* @retval None
*/
void USB_DEVICE_USER_DeviceSuspended(void)
{
  /* Users can do their application actions here for the USB-Reset */
}


/**
* @brief  USB_DEVICE_USER_DeviceResumed 
*         Displays the message ondevice resume Event
* @param  None
* @retval None
*/
void USB_DEVICE_USER_DeviceResumed(void)
{

}


/**
* @brief  USB_DEVICE_USER_DeviceConnected
*         Displays the message on device connection Event
* @param  None
* @retval Status
*/
void USB_DEVICE_USER_DeviceConnected (void)
{
}


/**
* @brief  USBD_USR_DeviceDisonnected
*         Displays the message on device disconnection Event
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

