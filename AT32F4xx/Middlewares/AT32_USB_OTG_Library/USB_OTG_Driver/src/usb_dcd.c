/**
  ******************************************************************************
  * File   : usbd_dcd.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Peripheral Device Interface Layer
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usb_dcd.h"
#include "usb_bsp.h"


/** @addtogroup USB_OTG_DRIVER
* @{
*/

/** @defgroup USB_DCD 
* @brief This file is the interface between EFSL ans Host mass-storage class
* @{
*/


/** @defgroup USB_DCD_Private_Defines
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USB_DCD_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 



/** @defgroup USB_DCD_Private_Macros
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USB_DCD_Private_Variables
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USB_DCD_Private_FunctionPrototypes
* @{
*/ 

/**
* @}
*/ 


/** @defgroup USB_DCD_Private_Functions
* @{
*/ 



void USB_DCD_Init(USB_OTG_CORE_HANDLE *pusbdev , 
              USB_OTG_CORE_ID_TypeDef coreid)
{
  uint32_t i;
  USB_OTG_EPT *ept;
  
  USB_OTG_SelectSpeed (pusbdev , coreid);
  
  pusbdev->dev.device_status = USB_OTG_DEFAULT;
  pusbdev->dev.device_address = 0;
  
  /* Init ept structure */
  for (i = 0; i < pusbdev->cfg.dev_endpoints ; i++)
  {
    ept = &pusbdev->dev.in_ept[i];
    /* Init ept structure */
    ept->is_in = 1;
    ept->num = i;
    ept->tx_fifo_num = i;
    /* Control until ept is activated */
    ept->ept_type = EPT_TYPE_CTRL;
    ept->maxpacket =  USB_OTG_MAX_EPT0_SIZE;
    ept->xfer_buff = 0;
    ept->xfer_len = 0;
  }
  
  for (i = 0; i < pusbdev->cfg.dev_endpoints; i++)
  {
    ept = &pusbdev->dev.out_ept[i];
    /* Init ept structure */
    ept->is_in = 0;
    ept->num = i;
    ept->tx_fifo_num = i;
    /* Control until ept is activated */
    ept->ept_type = EPT_TYPE_CTRL;
    ept->maxpacket = USB_OTG_MAX_EPT0_SIZE;
    ept->xfer_buff = 0;
    ept->xfer_len = 0;
  }
  
  USB_OTG_DisableGlobalInt(pusbdev);

    /*Init the Core (common init.) */
  USB_OTG_CoreInit(pusbdev);

  /* Force Device Mode*/
  USB_OTG_SetCurrentMode(pusbdev, DEVICE_MODE);
  
  /* Init Device */
  USB_OTG_CoreInitDev(pusbdev);
  
  /* Enable USB Global interrupt */
  USB_OTG_EnableGlobalInt(pusbdev);
}


/**
* @brief  Configure an EP
* @param pusbdev : Device instance
* @param epdesc : Endpoint Descriptor
* @retval : status
*/
uint32_t USB_DCD_EPT_Open(USB_OTG_CORE_HANDLE *pusbdev , 
                     uint8_t ep_addr,
                     uint16_t ep_mps,
                     uint8_t ept_type)
{
  USB_OTG_EPT *ept;
  
  if ((ep_addr & 0x80) == 0x80)
  {
    ept = &pusbdev->dev.in_ept[ep_addr & 0x7F];
  }
  else
  {
    ept = &pusbdev->dev.out_ept[ep_addr & 0x7F];
  }
  ept->num   = ep_addr & 0x7F;
  
  ept->is_in = (0x80 & ep_addr) != 0;
  ept->maxpacket = ep_mps;
  ept->ept_type = ept_type;
  if (ept->is_in)
  {
    /* Assign a Tx FIFO */
    ept->tx_fifo_num = ept->num;
  }
  /* Set initial data PID. */
  if (ept_type == USB_OTG_EPT_BULK )
  {
    ept->data_pid_start = 0;
  }
  USB_OTG_EPTActivate(pusbdev , ept );
  return 0;
}
/**
* @brief  called when an EP is disabled
* @param pusbdev: device instance
* @param ep_addr: endpoint address
* @retval : status
*/
uint32_t USB_DCD_EPT_Close(USB_OTG_CORE_HANDLE *pusbdev , uint8_t  ep_addr)
{
  USB_OTG_EPT *ept;
  
  if ((ep_addr&0x80) == 0x80)
  {
    ept = &pusbdev->dev.in_ept[ep_addr & 0x7F];
  }
  else
  {
    ept = &pusbdev->dev.out_ept[ep_addr & 0x7F];
  }
  ept->num   = ep_addr & 0x7F;
  ept->is_in = (0x80 & ep_addr) != 0;
  USB_OTG_EPTDeactivate(pusbdev , ept );
  return 0;
}


/**
* @brief  USB_DCD_EPT_PrepareRx
* @param pusbdev: device instance
* @param ep_addr: endpoint address
* @param pbuf: pointer to Rx buffer
* @param buf_len: data length
* @retval : status
*/
uint32_t   USB_DCD_EPT_PrepareRx( USB_OTG_CORE_HANDLE *pusbdev,
                            uint8_t   ep_addr,
                            uint8_t *pbuf,                        
                            uint16_t  buf_len)
{
  USB_OTG_EPT *ept;
  
  ept = &pusbdev->dev.out_ept[ep_addr & 0x7F];
  
  /*setup and start the Xfer */
  ept->xfer_buff = pbuf;  
  ept->xfer_len = buf_len;
  ept->xfer_count = 0;
  ept->is_in = 0;
  ept->num = ep_addr & 0x7F;
  
  if ( ept->num == 0 )
  {
    USB_OTG_EPT0StartXfer(pusbdev , ept);
  }
  else
  {
    USB_OTG_EPTStartXfer(pusbdev, ept );
  }
  return 0;
}

/**
* @brief  Transmit data over USB
* @param pusbdev: device instance
* @param ep_addr: endpoint address
* @param pbuf: pointer to Tx buffer
* @param buf_len: data length
* @retval : status
*/
uint32_t  USB_DCD_EPT_Tx ( USB_OTG_CORE_HANDLE *pusbdev,
                     uint8_t   ep_addr,
                     uint8_t   *pbuf,
                     uint32_t   buf_len)
{
  USB_OTG_EPT *ept;
  
  ept = &pusbdev->dev.in_ept[ep_addr & 0x7F];
  
  /* Setup and start the Transfer */
  ept->is_in = 1;
  ept->num = ep_addr & 0x7F;  
  ept->xfer_buff = pbuf;
  ept->dma_addr = (uint32_t)pbuf;  
  ept->xfer_count = 0;
  ept->xfer_len  = buf_len;
  
  if ( ept->num == 0 )
  {
    USB_OTG_EPT0StartXfer(pusbdev , ept);
  }
  else
  {
    USB_OTG_EPTStartXfer(pusbdev, ept );
  }
  return 0;
}


/**
* @brief  Stall an endpoint.
* @param pusbdev: device instance
* @param bepnum: endpoint address
* @retval : status
*/
uint32_t  USB_DCD_EPT_Stall (USB_OTG_CORE_HANDLE *pusbdev, uint8_t   bepnum)
{
  USB_OTG_EPT *ept;
  if ((0x80 & bepnum) == 0x80)
  {
    ept = &pusbdev->dev.in_ept[bepnum & 0x7F];
  }
  else
  {
    ept = &pusbdev->dev.out_ept[bepnum];
  }

  ept->is_stall = 1;
  ept->num   = bepnum & 0x7F;
  ept->is_in = ((bepnum & 0x80) == 0x80);
  
  USB_OTG_EPTSetStall(pusbdev , ept);
  return (0);
}


/**
* @brief  Clear stall condition on endpoints.
* @param pusbdev: device instance
* @param bepnum: endpoint address
* @retval : status
*/
uint32_t  USB_DCD_EPT_ClrStall (USB_OTG_CORE_HANDLE *pusbdev, uint8_t bepnum)
{
  USB_OTG_EPT *ept;
  if ((0x80 & bepnum) == 0x80)
  {
    ept = &pusbdev->dev.in_ept[bepnum & 0x7F];    
  }
  else
  {
    ept = &pusbdev->dev.out_ept[bepnum];
  }
  
  ept->is_stall = 0;  
  ept->num   = bepnum & 0x7F;
  ept->is_in = ((bepnum & 0x80) == 0x80);
  
  USB_OTG_EPTClearStall(pusbdev , ept);
  return (0);
}


/**
* @brief  This Function flushes the FIFOs.
* @param pusbdev: device instance
* @param bepnum: endpoint address
* @retval : status
*/
uint32_t  USB_DCD_EPT_Flush (USB_OTG_CORE_HANDLE *pusbdev , uint8_t bepnum)
{

  if ((bepnum & 0x80) == 0x80)
  {
    USB_OTG_FlushTxFifo(pusbdev, bepnum & 0x7F);
  }
  else
  {
    USB_OTG_FlushRxFifo(pusbdev);
  }

  return (0);
}


/**
* @brief  This Function set USB device address
* @param pusbdev: device instance
* @param address: new device address
* @retval : status
*/
void  USB_DCD_EPT_SetAddress (USB_OTG_CORE_HANDLE *pusbdev, uint8_t address)
{
  USB_OTG_DCFG_Type  dcfg;
  dcfg.u32val = 0;
  dcfg.b.devaddr = address;
  USB_OTG_MODIFY_R32( &pusbdev->regs.DREGS->DCFG, 0, dcfg.u32val);
}

/**
* @brief  Connect device (enable internal pull-up)
* @param pusbdev: device instance
* @retval : None
*/
void  USB_DCD_DevConnect (USB_OTG_CORE_HANDLE *pusbdev)
{
#ifndef USE_OTG_MODE
  USB_OTG_DCTL_Type  dctl;
  dctl.u32val = USB_OTG_READ_R32(&pusbdev->regs.DREGS->DCTRL);
  /* Connect device */
  dctl.b.sftdis  = 0;
  USB_OTG_WRITE_R32(&pusbdev->regs.DREGS->DCTRL, dctl.u32val);
  USB_OTG_BSP_mDelay(3);
#endif
}


/**
* @brief  Disconnect device (disable internal pull-up)
* @param pusbdev: device instance
* @retval : None
*/
void  USB_DCD_DevDisconnect (USB_OTG_CORE_HANDLE *pusbdev)
{
#ifndef USE_OTG_MODE
  USB_OTG_DCTL_Type  dctl;
  dctl.u32val = USB_OTG_READ_R32(&pusbdev->regs.DREGS->DCTRL);
  /* Disconnect device for 3ms */
  dctl.b.sftdis  = 1;
  USB_OTG_WRITE_R32(&pusbdev->regs.DREGS->DCTRL, dctl.u32val);
  USB_OTG_BSP_mDelay(3);
#endif
}


/**
* @brief  returns the EP Status
* @param  pusbdev : Selected device
*         bepnum : endpoint address
* @retval : EP status
*/

uint32_t USB_DCD_GetEPTStatus(USB_OTG_CORE_HANDLE *pusbdev ,uint8_t bepnum)
{
  USB_OTG_EPT *ept;
  uint32_t Status = 0;  
  
  if ((0x80 & bepnum) == 0x80)
  {
    ept = &pusbdev->dev.in_ept[bepnum & 0x7F];    
  }
  else
  {
    ept = &pusbdev->dev.out_ept[bepnum];
  }
  
  Status = USB_OTG_GetEPStatus(pusbdev ,ept);

  /* Return the current status */
  return Status;
}

/**
* @brief  Set the EP Status
* @param  pusbdev : Selected device
*         Status : new Status
*         bepnum : EP address
* @retval : None
*/
void USB_DCD_SetEPTStatus (USB_OTG_CORE_HANDLE *pusbdev , uint8_t bepnum , uint32_t Status)
{
  USB_OTG_EPT *ept;
  
  if ((0x80 & bepnum) == 0x80)
  {
    ept = &pusbdev->dev.in_ept[bepnum & 0x7F];    
  }
  else
  {
    ept = &pusbdev->dev.out_ept[bepnum];
  }
  
   USB_OTG_SetEPStatus(pusbdev ,ept , Status);
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

