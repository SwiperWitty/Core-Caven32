/**
  ******************************************************************************
  * File   : usb_hcd.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Host Interface Layer
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "usb_core.h"
#include "usb_hcd.h"
#include "usb_conf.h"
//#include "usb_bsp.h"


/** @addtogroup USB_OTG_DRIVER
  * @{
  */
  
/** @defgroup USB_HCD 
  * @brief This file is the interface between EFSL ans Host mass-storage class
  * @{
  */


/** @defgroup USB_HCD_Private_Defines
  * @{
  */ 
/**
  * @}
  */ 
 

/** @defgroup USB_HCD_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 



/** @defgroup USB_HCD_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USB_HCD_Private_Variables
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USB_HCD_Private_FunctionPrototypes
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USB_HCD_Private_Functions
  * @{
  */ 
/**
  * @brief  USB_HCD_Init 
  *         Initialize the HOST portion of the driver.
  * @param  pusbdev: Selected device
  * @param  base_address: OTG base address
  * @retval Status
  */
uint32_t USB_HCD_Init(USB_OTG_CORE_HANDLE *pusbdev , 
                  USB_OTG_CORE_ID_TypeDef coreid)
{
  uint8_t i = 0;
  pusbdev->host.ConnectSts = 0;
  
  for (i= 0; i< USB_OTG_MAX_TX_FIFO; i++)
  {
  pusbdev->host.ErrCnt[i]  = 0;
  pusbdev->host.XferCnt[i]   = 0;
  pusbdev->host.HCH_Status[i]   = HC_IDLE;
  }
  pusbdev->host.hc[0].max_packet  = 8; 

  USB_OTG_SelectSpeed(pusbdev, coreid);
#ifndef DUAL_ROLE_MODE_ENABLED
  USB_OTG_DisableGlobalInt(pusbdev);
  USB_OTG_CoreInit(pusbdev);

  /* Force Host Mode*/
  USB_OTG_SetCurrentMode(pusbdev , HOST_MODE);
  USB_OTG_CoreInitHost(pusbdev);
  USB_OTG_EnableGlobalInt(pusbdev);
#endif
   
  return 0;
}


/**
  * @brief  USB_HCD_GetCurrentSpeed
  *         Get Current device Speed.
  * @param  pusbdev : Selected device
  * @retval Status
  */

uint32_t USB_HCD_GetCurrentSpeed (USB_OTG_CORE_HANDLE *pusbdev)
{    
    USB_OTG_HPRT0_Type  HPRT0;
    HPRT0.u32val = USB_OTG_READ_R32(pusbdev->regs.HPRT0);
    
    return HPRT0.b.prtspd;
}

/**
  * @brief  USB_HCD_ResetPort
  *         Issues the reset command to device
  * @param  pusbdev : Selected device
  * @retval Status
  */
uint32_t USB_HCD_ResetPort(USB_OTG_CORE_HANDLE *pusbdev)
{
  /*
  Before starting to drive a USB reset, the application waits for the OTG 
  interrupt triggered by the denounce done bit (DBCDNE bit in OTG_FS_GOTGINT), 
  which indicates that the bus is stable again after the electrical denounce 
  caused by the attachment of a pull-up resistor on DP (FS) or DM (LS).
  */
  
  USB_OTG_ResetPort(pusbdev); 
  return 0;
}

/**
  * @brief  USB_HCD_IsDeviceConnected
  *         Check if the device is connected.
  * @param  pusbdev : Selected device
  * @retval Device connection status. 1 -> connected and 0 -> disconnected
  * 
  */
uint32_t USB_HCD_IsDeviceConnected(USB_OTG_CORE_HANDLE *pusbdev)
{
  return (pusbdev->host.ConnectSts);
}


/**
  * @brief  USB_HCD_IsPortEnabled 
  *         This function checks if port is enabled
  * @param  pusbdev : Selected device
  * @retval Frame number
  * 
  */
uint32_t USB_HCD_IsPortEnabled(USB_OTG_CORE_HANDLE *pusbdev)
{
  return (pusbdev->host.PortEnabled);
}

/**
  * @brief  USB_HCD_GetCurrentFrame 
  *         This function returns the frame number for sof packet
  * @param  pusbdev : Selected device
  * @retval Frame number
  * 
  */
uint32_t USB_HCD_GetCurrentFrame (USB_OTG_CORE_HANDLE *pusbdev) 
{
 return (USB_OTG_READ_R32(&pusbdev->regs.HREGS->HFNUM) & 0xFFFF) ;
}

/**
  * @brief  USB_HCD_GetURB_State 
  *         This function returns the last URBstate
  * @param  pusbdev: Selected device
  * @retval URB_STATE
  * 
  */
URB_STATE USB_HCD_GetURB_State (USB_OTG_CORE_HANDLE *pusbdev , uint8_t ch_num) 
{
  return pusbdev->host.URB_State[ch_num] ;
}

/**
  * @brief  USB_HCD_GetXferCnt 
  *         This function returns the last URBstate
  * @param  pusbdev: Selected device
  * @retval No. of data bytes transferred
  * 
  */
uint32_t USB_HCD_GetXferCnt (USB_OTG_CORE_HANDLE *pusbdev, uint8_t ch_num) 
{
  return pusbdev->host.XferCnt[ch_num] ;
}



/**
  * @brief  USB_HCD_GetHCState 
  *         This function returns the HC Status 
  * @param  pusbdev: Selected device
  * @retval HCH_STS
  * 
  */
HCH_STS USB_HCD_GetHCState (USB_OTG_CORE_HANDLE *pusbdev ,  uint8_t ch_num) 
{
  return pusbdev->host.HCH_Status[ch_num] ;
}

/**
  * @brief  USB_HCD_HCH_Init 
  *         This function prepare a HC and start a transfer
  * @param  pusbdev: Selected device
  * @param  hc_num: Channel number 
  * @retval status 
  */
uint32_t USB_HCD_HCH_Init (USB_OTG_CORE_HANDLE *pusbdev , uint8_t hc_num) 
{
  return USB_OTG_HCH_Init(pusbdev, hc_num);  
}

/**
  * @brief  USB_HCD_SubmitRequest 
  *         This function prepare a HC and start a transfer
  * @param  pusbdev: Selected device
  * @param  hc_num: Channel number 
  * @retval status
  */
uint32_t USB_HCD_SubmitRequest (USB_OTG_CORE_HANDLE *pusbdev , uint8_t hc_num) 
{
  
  pusbdev->host.URB_State[hc_num] =   URB_IDLE;  
  pusbdev->host.hc[hc_num].xfer_count = 0 ;
  return USB_OTG_HCH_StartXfer(pusbdev, hc_num);
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

