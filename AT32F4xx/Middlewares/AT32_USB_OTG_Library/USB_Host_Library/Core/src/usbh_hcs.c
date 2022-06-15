/**
  ******************************************************************************
  * File   : usbh_hcs.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file implements functions for opening and closing host channels
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/
#include "usbh_hcs.h"

/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_LIB_CORE
* @{
*/
  
/** @defgroup USBH_HCS
  * @brief This file includes opening and closing host channels
  * @{
  */ 

/** @defgroup USBH_HCS_Private_Defines
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBH_HCS_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBH_HCS_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBH_HCS_Private_Variables
  * @{
  */ 

/**
  * @}
  */ 


/** @defgroup USBH_HCS_Private_FunctionPrototypes
  * @{
  */ 
static uint16_t USB_HOST_GetFreeChannel (USB_OTG_CORE_HANDLE *pusbdev);
/**
  * @}
  */ 


/** @defgroup USBH_HCS_Private_Functions
  * @{
  */ 



/**
  * @brief  USB_Host_Open_Channel
  *         Open a  pipe
  * @param  pusbdev : Selected device
  * @param  hc_num: Host channel Number
  * @param  dev_address: USB Device address allocated to attached device
  * @param  speed : USB device speed (Full/Low)
  * @param  ept_type: end point ept_type (Bulk/int/ctl)
  * @param  maxpsize: max pkt size
  * @retval Status
  */
uint8_t USB_Host_Open_Channel  (USB_OTG_CORE_HANDLE *pusbdev,
                            uint8_t hc_num,
                            uint8_t dev_address,
                            uint8_t speed,
                            uint8_t ept_type,
                            uint16_t maxpsize)
{

  pusbdev->host.hc[hc_num].ept_num = pusbdev->host.channel[hc_num]& 0x7F;
  pusbdev->host.hc[hc_num].ept_is_in = (pusbdev->host.channel[hc_num] & 0x80 ) == 0x80;  
  pusbdev->host.hc[hc_num].dev_addr = dev_address;  
  pusbdev->host.hc[hc_num].ept_type = ept_type;  
  pusbdev->host.hc[hc_num].max_packet = maxpsize; 
  pusbdev->host.hc[hc_num].speed = speed; 
  pusbdev->host.hc[hc_num].toggle_in = 0; 
  pusbdev->host.hc[hc_num].toggle_out = 0;   
  if(speed == HPRT0_PRTSPD_HIGH_SPEED)
  {
    pusbdev->host.hc[hc_num].do_ping = 1;
  }
  
  USB_OTG_HCH_Init(pusbdev, hc_num) ;
  
  return HCH_OK; 

}

/**
  * @brief  USB_Host_Modify_Channel
  *         Modify a  pipe
  * @param  pusbdev : Selected device
  * @param  hc_num: Host channel Number
  * @param  dev_address: USB Device address allocated to attached device
  * @param  speed : USB device speed (Full/Low)
  * @param  ept_type: end point ept_type (Bulk/int/ctl)
  * @param  maxpsize: max pkt size
  * @retval Status
  */
uint8_t USB_Host_Modify_Channel (USB_OTG_CORE_HANDLE *pusbdev,
                            uint8_t hc_num,
                            uint8_t dev_address,
                            uint8_t speed,
                            uint8_t ept_type,
                            uint16_t maxpsize)
{
  
  if(dev_address != 0)
  {
    pusbdev->host.hc[hc_num].dev_addr = dev_address;  
  }
  
  if((pusbdev->host.hc[hc_num].max_packet != maxpsize) && (maxpsize != 0))
  {
    pusbdev->host.hc[hc_num].max_packet = maxpsize; 
  }
  
  if((pusbdev->host.hc[hc_num].speed != speed ) && (speed != 0 )) 
  {
    pusbdev->host.hc[hc_num].speed = speed; 
  }
  
  USB_OTG_HCH_Init(pusbdev, hc_num);
  return HCH_OK; 

}

/**
  * @brief  USB_Host_Alloc_Channel
  *         Allocate a new channel for the pipe
  * @param  ept_addr: End point for which the channel to be allocated
  * @retval hc_num: Host channel number
  */
uint8_t USB_Host_Alloc_Channel  (USB_OTG_CORE_HANDLE *pusbdev, uint8_t ept_addr)
{
  uint16_t hc_num;
  
  hc_num =  USB_HOST_GetFreeChannel(pusbdev);

  if (hc_num != HCH_ERROR)
  {
	pusbdev->host.channel[hc_num] = HCH_USED | ept_addr;
  }
  return hc_num;
}

/**
  * @brief  USBH_Free_Pipe
  *         Free the USB host channel
  * @param  idx: Channel number to be freed 
  * @retval Status
  */
uint8_t USB_Host_Free_Channel  (USB_OTG_CORE_HANDLE *pusbdev, uint8_t idx)
{
   if(idx < HCH_MAX)
   {
	 pusbdev->host.channel[idx] &= HCH_USED_MASK;
   }
   return USB_HOST_OK;
}


/**
  * @brief  USB_Host_DeAllocate_AllChannel
  *         Free all USB host channel
* @param  pusbdev : core instance
  * @retval Status
  */
uint8_t USB_Host_DeAllocate_AllChannel  (USB_OTG_CORE_HANDLE *pusbdev)
{
   uint8_t idx;
   
   for (idx = 2; idx < HCH_MAX ; idx ++)
   {
	 pusbdev->host.channel[idx] = 0;
   }
   return USB_HOST_OK;
}

/**
  * @brief  USB_HOST_GetFreeChannel
  *         Get a free channel number for allocation to a device endpoint
  * @param  None
  * @retval idx: Free Channel number
  */
static uint16_t USB_HOST_GetFreeChannel (USB_OTG_CORE_HANDLE *pusbdev)
{
  uint8_t idx = 0;
  
  for (idx = 0 ; idx < HCH_MAX ; idx++)
  {
	if ((pusbdev->host.channel[idx] & HCH_USED) == 0)
	{
	   return idx;
	} 
  }
  return HCH_ERROR;
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



