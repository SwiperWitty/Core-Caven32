/**
  ******************************************************************************
  * File   : usb_core.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : USB-OTG Core Layer
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usb_core.h"
#include "usb_bsp.h"


/** @addtogroup USB_OTG_DRIVER
* @{
*/

/** @defgroup USB_CORE 
* @brief This file includes the USB-OTG Core Layer
* @{
*/


/** @defgroup USB_CORE_Private_Defines
* @{
*/ 

/**
* @}
*/ 


/** @defgroup USB_CORE_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 



/** @defgroup USB_CORE_Private_Macros
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USB_CORE_Private_Variables
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USB_CORE_Private_FunctionPrototypes
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USB_CORE_Private_Functions
* @{
*/ 

/**
* @brief  USB_OTG_EnableCommonInt
*         Initializes the commmon interrupts, used in both device and modes
* @param  pusbdev : Selected device
* @retval None
*/
static void USB_OTG_EnableCommonInt(USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_OTG_GINTMSK_Type  usb_mask;
  
  usb_mask.u32val = 0;
  /* Clear any pending USB_OTG Interrupts */
#ifndef USE_OTG_MODE
  USB_OTG_WRITE_R32( &pusbdev->regs.GREGS->GOTGINT, 0xFFFFFFFF);
#endif
  /* Clear any pending interrupts */
  USB_OTG_WRITE_R32( &pusbdev->regs.GREGS->GINTSTS, 0xBFFFFFFF);
  /* Enable the interrupts in the INTMSK */
  usb_mask.b.wkupintm = 1;
  usb_mask.b.usbsuspm = 1; 
  
#ifdef USE_OTG_MODE
  usb_mask.b.otgintm = 1;
  usb_mask.b.sreqintm = 1;
  usb_mask.b.conidschgm = 1;
#endif
  USB_OTG_WRITE_R32( &pusbdev->regs.GREGS->GINTMASK, usb_mask.u32val);
}

/**
* @brief  USB_OTG_CoreReset : Soft reset of the core
* @param  pusbdev : Selected device
* @retval USB_OTG_STS : status
*/
static USB_OTG_STS USB_OTG_CoreReset(USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_OTG_STS status = USB_OTG_OK;
  __IO USB_OTG_GRSTCTL_Type  greset;
  uint32_t count = 0;
  
  greset.u32val = 0;
  /* Wait for AHB master IDLE state. */
  do
  {
    USB_OTG_BSP_uDelay(3);
    greset.u32val = USB_OTG_READ_R32(&pusbdev->regs.GREGS->GRSTCTRL);
    if (++count > 200000)
    {
      return USB_OTG_OK;
    }
  }
  while (greset.b.ahbidle == 0);
  /* Core Soft Reset */
  count = 0;
  greset.b.csftrst = 1;
  USB_OTG_WRITE_R32(&pusbdev->regs.GREGS->GRSTCTRL, greset.u32val );
  do
  {
    greset.u32val = USB_OTG_READ_R32(&pusbdev->regs.GREGS->GRSTCTRL);
    if (++count > 200000)
    {
      break;
    }
  }
  while (greset.b.csftrst == 1);
  /* Wait for 3 PHY Clocks*/
  USB_OTG_BSP_uDelay(3);
  return status;
}

/**
* @brief  USB_OTG_WritePacket : Writes a packet into the Tx FIFO associated 
*         with the EP
* @param  pusbdev : Selected device
* @param  src : source pointer
* @param  ch_ep_num : end point number
* @param  bytes : No. of bytes
* @retval USB_OTG_STS : status
*/
USB_OTG_STS USB_OTG_WritePacket(USB_OTG_CORE_HANDLE *pusbdev, 
                                uint8_t             *src, 
                                uint8_t             ch_ep_num, 
                                uint16_t            len)
{
  USB_OTG_STS status = USB_OTG_OK;
  {
    uint32_t count32b= 0 , i= 0;
    __IO uint32_t *fifo;
    
    count32b =  (len + 3) / 4;
    fifo = pusbdev->regs.DFIFO[ch_ep_num];
    for (i = 0; i < count32b; i++)
    {
      USB_OTG_WRITE_R32( fifo, *((__packed uint32_t *)src) );
      src+=4;
    }
  }
  return status;
}


/**
* @brief  USB_OTG_ReadPacket : Reads a packet from the Rx FIFO
* @param  pusbdev : Selected device
* @param  dest : Destination Pointer
* @param  bytes : No. of bytes
* @retval None
*/
void *USB_OTG_ReadPacket(USB_OTG_CORE_HANDLE *pusbdev, 
                         uint8_t *dest, 
                         uint16_t len)
{
  uint32_t i=0;
  uint32_t count32b = (len + 3) / 4;
  
  __IO uint32_t *fifo = pusbdev->regs.DFIFO[0];
  
  for( i = 0; i < count32b; i++)
  {
    *(__packed uint32_t *)dest = USB_OTG_READ_R32(fifo);
    dest += 4 ;
  }
  return ((void *)dest);
}

/**
* @brief  USB_OTG_SelectSpeed 
*         Initialize core registers address.
* @param  pusbdev : Selected device
* @param  coreid : USB OTG Core ID
* @retval USB_OTG_STS : status
*/
USB_OTG_STS USB_OTG_SelectSpeed(USB_OTG_CORE_HANDLE *pusbdev, 
                               USB_OTG_CORE_ID_TypeDef coreid)
{
  uint32_t i , baseAddress = 0;
  USB_OTG_STS status = USB_OTG_OK;
  
//  pusbdev->cfg.dma_enable       = 0;
  
  /* at startup the core is in FS mode */
  pusbdev->cfg.speed            = USB_OTG_SPEED_FULL;
  pusbdev->cfg.maxpsize              = USBFS_OTG_MAX_PACKET_SIZE ;    
  
  /* initialize device cfg following its address */
  if (coreid == USB_OTG_FS_CORE_ID)
  {
    baseAddress                = USBFS_OTG_BASE_ADDR;
    pusbdev->cfg.coreid           = USB_OTG_FS_CORE_ID;
    pusbdev->cfg.host_channels    = 8 ;
    pusbdev->cfg.dev_endpoints    = 4 ;
    pusbdev->cfg.totalfifosize    = 320; /* in 32-bits */
    pusbdev->cfg.phy_itface       = USB_OTG_EMBEDDED_PHY;     
    
#ifdef USB_OTG_FS_SOF_OUTPUT_ENABLED    
    pusbdev->cfg.sof_output       = 1;    
#endif 
    
#ifdef USB_OTG_FS_LOW_PWR_MGMT_SUPPORT    
    pusbdev->cfg.low_power        = 1;    
#endif     
  } 
  else
  {
    /* Do Nothing */
  }
  
  pusbdev->regs.GREGS = (USB_OTG_GLOBAL_REGS *)(baseAddress + \
    USB_OTG_GLOBAL_R_OFFSET);
  pusbdev->regs.DREGS =  (USB_OTG_DEV_REGS  *)  (baseAddress + \
    USB_OTG_DEV_R_OFFSET);
  
  for (i = 0; i < pusbdev->cfg.dev_endpoints; i++)
  {
    pusbdev->regs.INEP_REGS[i]  = (USB_OTG_INEPT_REGS *)  \
      (baseAddress + USB_OTG_DEV_INEP_R_OFFSET + \
        (i * USB_OTG_EPT_R_OFFSET));
    pusbdev->regs.OUTEP_REGS[i] = (USB_OTG_OUTEPT_REGS *) \
      (baseAddress + USB_OTG_DEV_OUTEP_R_OFFSET + \
        (i * USB_OTG_EPT_R_OFFSET));
  }
  pusbdev->regs.HREGS = (USB_OTG_HOST_REGS *)(baseAddress + \
    USB_OTG_HOST_GLOBAL_R_OFFSET);
  pusbdev->regs.HPRT0 = (uint32_t *)(baseAddress + USB_OTG_HOST_PORT_R_OFFSET);
  
  for (i = 0; i < pusbdev->cfg.host_channels; i++)
  {
    pusbdev->regs.HC_REGS[i] = (USB_OTG_HOSTCH_REGS *)(baseAddress + \
      USB_OTG_HOST_CH_R_OFFSET + \
        (i * USB_OTG_CH_R_OFFSET));
  }
  for (i = 0; i < pusbdev->cfg.host_channels; i++)
  {
    pusbdev->regs.DFIFO[i] = (uint32_t *)(baseAddress + USB_OTG_DATA_FIFO_OFFSET +\
      (i * USB_OTG_DATA_FIFO_SIZE));
  }
  pusbdev->regs.PCGCCTL = (uint32_t *)(baseAddress + USB_OTG_PCGCCTL_OFFSET);
  
  return status;
}


/**
* @brief  USB_OTG_CoreInit
*         Initializes the USB_OTG controller registers and prepares the core
*         device mode or host mode operation.
* @param  pusbdev : Selected device
* @retval USB_OTG_STS : status
*/
USB_OTG_STS USB_OTG_CoreInit(USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_OTG_STS status = USB_OTG_OK;
  USB_OTG_GUSBCFG_Type  usbcfg;
  USB_OTG_GCCFG_Type    gccfg;
//  USB_OTG_GAHBCFG_Type  ahbcfg;
//  USB_OTG_DCTL_Type     dctl;
  usbcfg.u32val = 0;
  gccfg.u32val = 0;
//  ahbcfg.u32val = 0;
  {
    
    usbcfg.u32val = USB_OTG_READ_R32(&pusbdev->regs.GREGS->GUSBCFG);;
    usbcfg.b.physel  = 1; /* FS Interface */
    USB_OTG_WRITE_R32 (&pusbdev->regs.GREGS->GUSBCFG, usbcfg.u32val);
    /* Reset after a PHY select and set Host mode */
    USB_OTG_CoreReset(pusbdev);
    /* Deactivate the power down*/
    gccfg.u32val = 0;
    gccfg.b.pwrdown = 1;
    gccfg.b.vbusasesen = 1 ;
    gccfg.b.vbusbsesen = 1 ; 
   
//#ifndef VBUS_SENSING_ENABLED
//    gccfg.b.bdisablevbussensing = 1; 
//#endif    
    
    if(pusbdev->cfg.sof_output)
    {
      gccfg.b.sofouten = 1;  
    }
    
    USB_OTG_WRITE_R32 (&pusbdev->regs.GREGS->GCCFG, gccfg.u32val);
    USB_OTG_BSP_mDelay(20);
  }
  /* case the HS core is working in FS mode */
  /* initialize OTG features */
#ifdef  USE_OTG_MODE
//  usbcfg.u32val = USB_OTG_READ_R32(&pusbdev->regs.GREGS->GUSBCFG);
//  usbcfg.b.hnpcap = 1;
//  usbcfg.b.srpcap = 1;
//  USB_OTG_WRITE_R32(&pusbdev->regs.GREGS->GUSBCFG, usbcfg.u32val);
  USB_OTG_EnableCommonInt(pusbdev);
#endif
  
  return status;
}
/**
* @brief  USB_OTG_EnableGlobalInt
*         Enables the controller's Global Int in the AHB Config reg
* @param  pusbdev : Selected device
* @retval USB_OTG_STS : status
*/
USB_OTG_STS USB_OTG_EnableGlobalInt(USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_OTG_STS status = USB_OTG_OK;
  USB_OTG_GAHBCFG_Type  ahbcfg;
  
  ahbcfg.u32val = 0;
  ahbcfg.b.gintmsk = 1; /* Enable interrupts */
  USB_OTG_MODIFY_R32(&pusbdev->regs.GREGS->GAHBCFG, 0, ahbcfg.u32val);
  return status;
}


/**
* @brief  USB_OTG_DisableGlobalInt
*         Enables the controller's Global Int in the AHB Config reg
* @param  pusbdev : Selected device
* @retval USB_OTG_STS : status
*/
USB_OTG_STS USB_OTG_DisableGlobalInt(USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_OTG_STS status = USB_OTG_OK;
  USB_OTG_GAHBCFG_Type  ahbcfg;
  ahbcfg.u32val = 0;
  ahbcfg.b.gintmsk = 1; /* Enable interrupts */
  USB_OTG_MODIFY_R32(&pusbdev->regs.GREGS->GAHBCFG, ahbcfg.u32val, 0);
  return status;
}


/**
* @brief  USB_OTG_FlushTxFifo : Flush a Tx FIFO
* @param  pusbdev : Selected device
* @param  num : FO num
* @retval USB_OTG_STS : status
*/
USB_OTG_STS USB_OTG_FlushTxFifo (USB_OTG_CORE_HANDLE *pusbdev , uint32_t num )
{
  USB_OTG_STS status = USB_OTG_OK;
  __IO USB_OTG_GRSTCTL_Type  greset;
  
  uint32_t count = 0;
  greset.u32val = 0;
  greset.b.txfflsh = 1;
  greset.b.txfnum  = num;
  USB_OTG_WRITE_R32( &pusbdev->regs.GREGS->GRSTCTRL, greset.u32val );
  do
  {
    greset.u32val = USB_OTG_READ_R32( &pusbdev->regs.GREGS->GRSTCTRL);
    if (++count > 200000)
    {
      break;
    }
  }
  while (greset.b.txfflsh == 1);
  /* Wait for 3 PHY Clocks*/
  USB_OTG_BSP_uDelay(3);
  return status;
}


/**
* @brief  USB_OTG_FlushRxFifo : Flush a Rx FIFO
* @param  pusbdev : Selected device
* @retval USB_OTG_STS : status
*/
USB_OTG_STS USB_OTG_FlushRxFifo( USB_OTG_CORE_HANDLE *pusbdev )
{
  USB_OTG_STS status = USB_OTG_OK;
  __IO USB_OTG_GRSTCTL_Type  greset;
  uint32_t count = 0;
  
  greset.u32val = 0;
  greset.b.rxfflsh = 1;
  USB_OTG_WRITE_R32( &pusbdev->regs.GREGS->GRSTCTRL, greset.u32val );
  do
  {
    greset.u32val = USB_OTG_READ_R32( &pusbdev->regs.GREGS->GRSTCTRL);
    if (++count > 200000)
    {
      break;
    }
  }
  while (greset.b.rxfflsh == 1);
  /* Wait for 3 PHY Clocks*/
  USB_OTG_BSP_uDelay(3);
  return status;
}


/**
* @brief  USB_OTG_SetCurrentMode : Set ID line
* @param  pusbdev : Selected device
* @param  mode :  (Host/device)
* @retval USB_OTG_STS : status
*/
USB_OTG_STS USB_OTG_SetCurrentMode(USB_OTG_CORE_HANDLE *pusbdev , uint8_t mode)
{
  USB_OTG_STS status = USB_OTG_OK;
  USB_OTG_GUSBCFG_Type  usbcfg;
  
  usbcfg.u32val = USB_OTG_READ_R32(&pusbdev->regs.GREGS->GUSBCFG);
  
  usbcfg.b.fhstmod = 0;
  usbcfg.b.fdevmod = 0;
  
  if ( mode == HOST_MODE)
  {
    usbcfg.b.fhstmod = 1;
  }
  else if ( mode == DEVICE_MODE)
  {
    usbcfg.b.fdevmod = 1;
  }
  
  else
  {
    /* Do Nothing */
  }
  
  USB_OTG_WRITE_R32(&pusbdev->regs.GREGS->GUSBCFG, usbcfg.u32val);
  USB_OTG_BSP_mDelay(50);
  return status;
}


/**
* @brief  USB_OTG_GetMode : Get current mode
* @param  pusbdev : Selected device
* @retval current mode
*/
uint32_t USB_OTG_GetMode(USB_OTG_CORE_HANDLE *pusbdev)
{
  return (USB_OTG_READ_R32(&pusbdev->regs.GREGS->GINTSTS ) & 0x1);
}


/**
* @brief  USB_OTG_IsDeviceMode : Check if it is device mode
* @param  pusbdev : Selected device
* @retval num_in_ep
*/
uint8_t USB_OTG_IsDeviceMode(USB_OTG_CORE_HANDLE *pusbdev)
{
  return (USB_OTG_GetMode(pusbdev) != HOST_MODE);
}


/**
* @brief  USB_OTG_IsHostMode : Check if it is host mode
* @param  pusbdev : Selected device
* @retval num_in_ep
*/
uint8_t USB_OTG_IsHostMode(USB_OTG_CORE_HANDLE *pusbdev)
{
  return (USB_OTG_GetMode(pusbdev) == HOST_MODE);
}


/**
* @brief  USB_OTG_ReadCoreInterruptr : returns the Core Interrupt register
* @param  pusbdev : Selected device
* @retval Status
*/
uint32_t USB_OTG_ReadCoreInterruptr(USB_OTG_CORE_HANDLE *pusbdev)
{
  uint32_t v = 0;
  v = USB_OTG_READ_R32(&pusbdev->regs.GREGS->GINTSTS);
  v &= USB_OTG_READ_R32(&pusbdev->regs.GREGS->GINTMASK);
  return v;
}


/**
* @brief  USB_OTG_ReadOtgIterruptr : returns the USB_OTG Interrupt register
* @param  pusbdev : Selected device
* @retval Status
*/
uint32_t USB_OTG_ReadOtgIterruptr (USB_OTG_CORE_HANDLE *pusbdev)
{
  return (USB_OTG_READ_R32 (&pusbdev->regs.GREGS->GOTGINT));
}

#ifdef USE_HOST_MODE
/**
* @brief  USB_OTG_CoreInitHost : Initializes USB_OTG controller for host mode
* @param  pusbdev : Selected device
* @retval status
*/
USB_OTG_STS USB_OTG_CoreInitHost(USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_OTG_STS                     status = USB_OTG_OK;
  USB_OTG_FSIZ_Type            nptxfifosize;
  USB_OTG_FSIZ_Type            ptxfifosize;  
  USB_OTG_HCFG_Type            hcfg;
  
#ifdef USE_OTG_MODE
  USB_OTG_OTGCTL_TypeDef          gotgctl;
#endif
  
  uint32_t                        i = 0;
  
  nptxfifosize.u32val = 0;  
  ptxfifosize.u32val = 0;
#ifdef USE_OTG_MODE
  gotgctl.u32val = 0;
#endif
  hcfg.u32val = 0;
  
  
  /* configure charge pump IO */
  USB_OTG_BSP_ConfigVBUS(pusbdev);
  
  /* Restart the Phy Clock */
  USB_OTG_WRITE_R32(pusbdev->regs.PCGCCTL, 0);
  USB_OTG_InitFSLSPClkSel(pusbdev , HCFG_48_MHZ); 
  USB_OTG_ResetPort(pusbdev);
  
  hcfg.u32val = USB_OTG_READ_R32(&pusbdev->regs.HREGS->HCFG);
  hcfg.b.fslssupp = 0;
  USB_OTG_WRITE_R32(&pusbdev->regs.HREGS->HCFG, hcfg.u32val);
  
  /* Configure data FIFO sizes */
  /* Rx FIFO */
  if(pusbdev->cfg.coreid == USB_OTG_FS_CORE_ID)
  {
    /* set Rx FIFO size */
    USB_OTG_WRITE_R32(&pusbdev->regs.GREGS->GRXFSIZE, RX_FIFO_FS_SIZE);
    nptxfifosize.b.startaddr = RX_FIFO_FS_SIZE;   
    nptxfifosize.b.depth = TXH_NP_FS_FIFOSIZ;  
    USB_OTG_WRITE_R32(&pusbdev->regs.GREGS->DINEPTTXF0_HNPTXFSIZE, nptxfifosize.u32val);
    
    ptxfifosize.b.startaddr = RX_FIFO_FS_SIZE + TXH_NP_FS_FIFOSIZ;
    ptxfifosize.b.depth     = TXH_P_FS_FIFOSIZ;
    USB_OTG_WRITE_R32(&pusbdev->regs.GREGS->HPTXFSIZE, ptxfifosize.u32val);      
  }
  
#ifdef USE_OTG_MODE
  /* Clear Host Set HNP Enable in the USB_OTG Control Register */
  gotgctl.b.hshnpen = 1;
  USB_OTG_MODIFY_R32( &pusbdev->regs.GREGS->GOTGCTRL, gotgctl.u32val, 0);
#endif
  
  /* Make sure the FIFOs are flushed. */
  USB_OTG_FlushTxFifo(pusbdev, 0x10 );         /* all Tx FIFOs */
  USB_OTG_FlushRxFifo(pusbdev);
  
  
  /* Clear all pending HC Interrupts */
  for (i = 0; i < pusbdev->cfg.host_channels; i++)
  {
    USB_OTG_WRITE_R32( &pusbdev->regs.HC_REGS[i]->HCINT, 0xFFFFFFFF );
    USB_OTG_WRITE_R32( &pusbdev->regs.HC_REGS[i]->HCINTMASK, 0 );
  }
#ifndef USE_OTG_MODE
  USB_OTG_DriveVbus(pusbdev, 1);
#endif
  
  USB_OTG_EnableHostInt(pusbdev);
  return status;
}

/**
* @brief  USB_OTG_IsEvenFrame 
*         This function returns the frame number for sof packet
* @param  pusbdev : Selected device
* @retval Frame number
*/
uint8_t USB_OTG_IsEvenFrame (USB_OTG_CORE_HANDLE *pusbdev) 
{
  return !(USB_OTG_READ_R32(&pusbdev->regs.HREGS->HFNUM) & 0x1);
}

/**
* @brief  USB_OTG_DriveVbus : set/reset vbus
* @param  pusbdev : Selected device
* @param  state : VBUS state
* @retval None
*/
void USB_OTG_DriveVbus (USB_OTG_CORE_HANDLE *pusbdev, uint8_t state)
{
  USB_OTG_HPRT0_Type     hprt0;
  
  hprt0.u32val = 0;
  
  /* enable disable the external charge pump */
  USB_OTG_BSP_DriveVBUS(pusbdev, state);
  
  /* Turn on the Host port power. */
  hprt0.u32val = USB_OTG_ReadHPRT0(pusbdev);
  if ((hprt0.b.prtpwr == 0 ) && (state == 1 ))
  {
    hprt0.b.prtpwr = 1;
    USB_OTG_WRITE_R32(pusbdev->regs.HPRT0, hprt0.u32val);
  }
  if ((hprt0.b.prtpwr == 1 ) && (state == 0 ))
  {
    hprt0.b.prtpwr = 0;
    USB_OTG_WRITE_R32(pusbdev->regs.HPRT0, hprt0.u32val);
  }
  
  USB_OTG_BSP_mDelay(200);
}
/**
* @brief  USB_OTG_EnableHostInt: Enables the Host mode interrupts
* @param  pusbdev : Selected device
* @retval USB_OTG_STS : status
*/
USB_OTG_STS USB_OTG_EnableHostInt(USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_OTG_STS       status = USB_OTG_OK;
  USB_OTG_GINTMSK_Type  intmsk;
  intmsk.u32val = 0;
  /* Disable all interrupts. */
  USB_OTG_WRITE_R32(&pusbdev->regs.GREGS->GINTMASK, 0);
  
  /* Clear any pending interrupts. */
  USB_OTG_WRITE_R32(&pusbdev->regs.GREGS->GINTSTS, 0xFFFFFFFF);
  
  /* Enable the common interrupts */
  USB_OTG_EnableCommonInt(pusbdev);
  
//  if (pusbdev->cfg.dma_enable == 0)
  {  
    intmsk.b.rxflvlm  = 1;
  }  
  

  intmsk.b.incompisooutm  = 1;
  intmsk.b.hchintm     = 1; 
  intmsk.b.hportintm   = 1;
  USB_OTG_MODIFY_R32(&pusbdev->regs.GREGS->GINTMASK, intmsk.u32val, intmsk.u32val);
 
  intmsk.u32val = 0;
 
  intmsk.b.disconintm = 1;  
  
  intmsk.b.sofm    = 1; 
  USB_OTG_MODIFY_R32(&pusbdev->regs.GREGS->GINTMASK, intmsk.u32val, 0);
  return status;
}

/**
* @brief  USB_OTG_InitFSLSPClkSel : Initializes the FSLSPClkSel field of the 
*         HCFG register on the PHY ept_type
* @param  pusbdev : Selected device
* @param  freq : clock frequency
* @retval None
*/
void USB_OTG_InitFSLSPClkSel(USB_OTG_CORE_HANDLE *pusbdev , uint8_t freq)
{
  USB_OTG_HCFG_Type   hcfg;
  
  hcfg.u32val = USB_OTG_READ_R32(&pusbdev->regs.HREGS->HCFG);
  hcfg.b.fslspclks = freq;
  USB_OTG_WRITE_R32(&pusbdev->regs.HREGS->HCFG, hcfg.u32val);
}


/**
* @brief  USB_OTG_ReadHPRT0 : Reads HPRT0 to modify later
* @param  pusbdev : Selected device
* @retval HPRT0 value
*/
uint32_t USB_OTG_ReadHPRT0(USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_OTG_HPRT0_Type  hprt0;
  
  hprt0.u32val = USB_OTG_READ_R32(pusbdev->regs.HPRT0);
  hprt0.b.prtena = 0;
  hprt0.b.prtcondet = 0;
  hprt0.b.prtenchng = 0;
  hprt0.b.prtovcchng = 0;
  return hprt0.u32val;
}


/**
* @brief  USB_OTG_ReadHostAllChannels_intr : Register PCD Callbacks
* @param  pusbdev : Selected device
* @retval Status
*/
uint32_t USB_OTG_ReadHostAllChannels_intr (USB_OTG_CORE_HANDLE *pusbdev)
{
  return (USB_OTG_READ_R32 (&pusbdev->regs.HREGS->HAINT));
}


/**
* @brief  USB_OTG_ResetPort : Reset Host Port
* @param  pusbdev : Selected device
* @retval status
* @note : (1)The application must wait at least 10 ms (+ 10 ms security)
*   before clearing the reset bit.
*/
uint32_t USB_OTG_ResetPort(USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_OTG_HPRT0_Type  hprt0;
  
  hprt0.u32val = USB_OTG_ReadHPRT0(pusbdev);
  hprt0.b.prtrst = 1;
  USB_OTG_WRITE_R32(pusbdev->regs.HPRT0, hprt0.u32val);
  USB_OTG_BSP_mDelay (100);                                /* See Note #1 */
  hprt0.b.prtrst = 0;
  USB_OTG_WRITE_R32(pusbdev->regs.HPRT0, hprt0.u32val);
  USB_OTG_BSP_mDelay (20);   
  return 1;
}


/**
* @brief  USB_OTG_HCH_Init : Prepares a host channel for transferring packets
* @param  pusbdev : Selected device
* @param  hc_num : channel number
* @retval USB_OTG_STS : status
*/
USB_OTG_STS USB_OTG_HCH_Init(USB_OTG_CORE_HANDLE *pusbdev , uint8_t hc_num)
{
  USB_OTG_STS status = USB_OTG_OK;
  uint32_t intr_enable = 0;
  USB_OTG_HCINTMSK_Type  hcintmsk;
  USB_OTG_GINTMSK_Type    gintmsk;
  USB_OTG_HCCHAR_Type     hcchar;
  USB_OTG_HCINTn_Type     hcint;
  
  
  gintmsk.u32val = 0;
  hcintmsk.u32val = 0;
  hcchar.u32val = 0;
  
  /* Clear old interrupt conditions for this host channel. */
  hcint.u32val = 0xFFFFFFFF;
  USB_OTG_WRITE_R32(&pusbdev->regs.HC_REGS[hc_num]->HCINT, hcint.u32val);
  
  /* Enable channel interrupts required for this transfer. */
  hcintmsk.u32val = 0;
  
  switch (pusbdev->host.hc[hc_num].ept_type) 
  {
  case EPT_TYPE_CTRL:
  case EPT_TYPE_BULK:
    hcintmsk.b.xfercm = 1;
    hcintmsk.b.stallm = 1;
    hcintmsk.b.xacterrm = 1;
    hcintmsk.b.dtgerrm = 1;
    hcintmsk.b.nakm = 1;  
    if (pusbdev->host.hc[hc_num].ept_is_in) 
    {
      hcintmsk.b.bblerrm = 1;
    } 
    else 
    {
      hcintmsk.b.nyetm = 1;
      if (pusbdev->host.hc[hc_num].do_ping) 
      {
        hcintmsk.b.ackm = 1;
      }
    }
    break;
  case EPT_TYPE_INTR:
    hcintmsk.b.xfercm = 1;
    hcintmsk.b.nakm = 1;
    hcintmsk.b.stallm = 1;
    hcintmsk.b.xacterrm = 1;
    hcintmsk.b.dtgerrm = 1;
    hcintmsk.b.frmovrm = 1;
    
    if (pusbdev->host.hc[hc_num].ept_is_in) 
    {
      hcintmsk.b.bblerrm = 1;
    }
    
    break;
  case EPT_TYPE_ISOC:
    hcintmsk.b.xfercm = 1;
    hcintmsk.b.frmovrm = 1;
    hcintmsk.b.ackm = 1;
    
    if (pusbdev->host.hc[hc_num].ept_is_in) 
    {
      hcintmsk.b.xacterrm = 1;
      hcintmsk.b.bblerrm = 1;
    }
    break;
  }
  
  
  USB_OTG_WRITE_R32(&pusbdev->regs.HC_REGS[hc_num]->HCINTMASK, hcintmsk.u32val);
  
  
  /* Enable the top level host channel interrupt. */
  intr_enable = (1 << hc_num);
  USB_OTG_MODIFY_R32(&pusbdev->regs.HREGS->HAINTMASK, 0, intr_enable);
  
  /* Make sure host channel interrupts are enabled. */
  gintmsk.b.hchintm = 1;
  USB_OTG_MODIFY_R32(&pusbdev->regs.GREGS->GINTMASK, 0, gintmsk.u32val);
  
  /* Program the HCCHAR register */
  hcchar.u32val = 0;
  hcchar.b.devaddr = pusbdev->host.hc[hc_num].dev_addr;
  hcchar.b.eptnum   = pusbdev->host.hc[hc_num].ept_num;
  hcchar.b.eptdir   = pusbdev->host.hc[hc_num].ept_is_in;
  hcchar.b.lspddev = (pusbdev->host.hc[hc_num].speed == HPRT0_PRTSPD_LOW_SPEED);
  hcchar.b.eptype  = pusbdev->host.hc[hc_num].ept_type;
  hcchar.b.maxpsize     = pusbdev->host.hc[hc_num].max_packet;
  if (pusbdev->host.hc[hc_num].ept_type == HCCHAR_INTR)
  {
    hcchar.b.oddfrm  = 1;
  }
  USB_OTG_WRITE_R32(&pusbdev->regs.HC_REGS[hc_num]->HCCHAR, hcchar.u32val);
  return status;
}


/**
* @brief  USB_OTG_HCH_StartXfer : Start transfer
* @param  pusbdev : Selected device
* @param  hc_num : channel number
* @retval USB_OTG_STS : status
*/
USB_OTG_STS USB_OTG_HCH_StartXfer(USB_OTG_CORE_HANDLE *pusbdev , uint8_t hc_num)
{
  USB_OTG_STS status = USB_OTG_OK;
  USB_OTG_HCCHAR_Type   hcchar;
  USB_OTG_HCTSIZn_Type  hctsiz;
  USB_OTG_HNPTXSTS_Type hnptxsts; 
  USB_OTG_HPTXSTS_Type  hptxsts; 
  USB_OTG_GINTMSK_Type  intmsk;
  uint16_t                 len_words = 0;   
  
  uint16_t num_packets;
  uint16_t max_hc_pkt_count;
  
  max_hc_pkt_count = 256;
  hctsiz.u32val = 0;
  hcchar.u32val = 0;
  intmsk.u32val = 0;
  
  /* Compute the expected number of packets associated to the transfer */
  if (pusbdev->host.hc[hc_num].xfer_len > 0)
  {
    num_packets = (pusbdev->host.hc[hc_num].xfer_len + \
      pusbdev->host.hc[hc_num].max_packet - 1) / pusbdev->host.hc[hc_num].max_packet;
    
    if (num_packets > max_hc_pkt_count)
    {
      num_packets = max_hc_pkt_count;
      pusbdev->host.hc[hc_num].xfer_len = num_packets * \
        pusbdev->host.hc[hc_num].max_packet;
    }
  }
  else
  {
    num_packets = 1;
  }
  if (pusbdev->host.hc[hc_num].ept_is_in)
  {
    pusbdev->host.hc[hc_num].xfer_len = num_packets * \
      pusbdev->host.hc[hc_num].max_packet;
  }
  /* Initialize the HCTSIZn register */
  hctsiz.b.xfersize = pusbdev->host.hc[hc_num].xfer_len;
  hctsiz.b.pktcnt = num_packets;
  hctsiz.b.dpid = pusbdev->host.hc[hc_num].data_pid;
  USB_OTG_WRITE_R32(&pusbdev->regs.HC_REGS[hc_num]->HCTSIZE, hctsiz.u32val);
  
  hcchar.u32val = USB_OTG_READ_R32(&pusbdev->regs.HC_REGS[hc_num]->HCCHAR);
  hcchar.b.oddfrm = USB_OTG_IsEvenFrame(pusbdev);
  
  /* Set host channel enable */
  hcchar.b.chena = 1;
  hcchar.b.chdis = 0;
  USB_OTG_WRITE_R32(&pusbdev->regs.HC_REGS[hc_num]->HCCHAR, hcchar.u32val);
  
//  if (pusbdev->cfg.dma_enable == 0) /* Slave mode */
  {  
    if((pusbdev->host.hc[hc_num].ept_is_in == 0) && 
       (pusbdev->host.hc[hc_num].xfer_len > 0))
    {
      switch(pusbdev->host.hc[hc_num].ept_type) 
      {
        /* Non periodic transfer */
      case EPT_TYPE_CTRL:
      case EPT_TYPE_BULK:
        
        hnptxsts.u32val = USB_OTG_READ_R32(&pusbdev->regs.GREGS->HNPTXSTS);
        len_words = (pusbdev->host.hc[hc_num].xfer_len + 3) / 4;
        
        /* check if there is enough space in FIFO space */
        if(len_words > hnptxsts.b.nptxfsavail)
        {
          /* need to process data in nptxfemp interrupt */
          intmsk.b.nptxfempm = 1;
          USB_OTG_MODIFY_R32( &pusbdev->regs.GREGS->GINTMASK, 0, intmsk.u32val);  
        }
        
        break;
        /* Periodic transfer */
      case EPT_TYPE_INTR:
      case EPT_TYPE_ISOC:
        hptxsts.u32val = USB_OTG_READ_R32(&pusbdev->regs.HREGS->HPTXSTS);
        len_words = (pusbdev->host.hc[hc_num].xfer_len + 3) / 4;
        /* check if there is enough space in FIFO space */
        if(len_words > hptxsts.b.ptxfspcavl) /* split the transfer */
        {
          /* need to process data in bptxfemp interrupt */
          intmsk.b.ptxfempm = 1;
          USB_OTG_MODIFY_R32( &pusbdev->regs.GREGS->GINTMASK, 0, intmsk.u32val);  
        }
        break;
        
      default:
        break;
      }
      
      /* Write packet into the Tx FIFO. */
      USB_OTG_WritePacket(pusbdev, 
                          pusbdev->host.hc[hc_num].xfer_buff , 
                          hc_num, pusbdev->host.hc[hc_num].xfer_len);
    }
  }
  return status;
}


/**
* @brief  USB_OTG_HCH_Halt : Halt channel
* @param  pusbdev : Selected device
* @param  hc_num : channel number
* @retval USB_OTG_STS : status
*/
USB_OTG_STS USB_OTG_HCH_Halt(USB_OTG_CORE_HANDLE *pusbdev , uint8_t hc_num)
{
  USB_OTG_STS status = USB_OTG_OK;
  USB_OTG_HNPTXSTS_Type            nptxsts;
  USB_OTG_HPTXSTS_Type             hptxsts;
  USB_OTG_HCCHAR_Type              hcchar;
  
  nptxsts.u32val = 0;
  hptxsts.u32val = 0;
  hcchar.u32val = USB_OTG_READ_R32(&pusbdev->regs.HC_REGS[hc_num]->HCCHAR);
  
  hcchar.b.chdis = 1;
  
  /* Check for space in the request queue to issue the halt. */
  if (hcchar.b.eptype == HCCHAR_CTRL || hcchar.b.eptype == HCCHAR_BULK)
  {
    nptxsts.u32val = USB_OTG_READ_R32(&pusbdev->regs.GREGS->HNPTXSTS);
    if (nptxsts.b.nptreqxsavail == 0)
    {
      hcchar.b.chena = 0;
      USB_OTG_WRITE_R32(&pusbdev->regs.HC_REGS[hc_num]->HCCHAR, hcchar.u32val);
    }
  }
  else
  {
    hptxsts.u32val = USB_OTG_READ_R32(&pusbdev->regs.HREGS->HPTXSTS);
    if (hptxsts.b.ptxqspcavl == 0)
    {
      hcchar.b.chena = 0;
      USB_OTG_WRITE_R32(&pusbdev->regs.HC_REGS[hc_num]->HCCHAR, hcchar.u32val);
    }
  }
  hcchar.b.chena = 1;
  USB_OTG_WRITE_R32(&pusbdev->regs.HC_REGS[hc_num]->HCCHAR, hcchar.u32val);
  return status;
}

/**
* @brief  Issue a ping token
* @param  None
* @retval : None
*/
USB_OTG_STS USB_OTG_HCH_DoPing(USB_OTG_CORE_HANDLE *pusbdev , uint8_t hc_num)
{
  USB_OTG_STS               status = USB_OTG_OK;
  USB_OTG_HCCHAR_Type    hcchar;
  USB_OTG_HCTSIZn_Type   hctsiz;  
  
  hctsiz.u32val = 0;
  hctsiz.b.dopng = 1;
  hctsiz.b.pktcnt = 1;
  USB_OTG_WRITE_R32(&pusbdev->regs.HC_REGS[hc_num]->HCTSIZE, hctsiz.u32val);
  
  hcchar.u32val = USB_OTG_READ_R32(&pusbdev->regs.HC_REGS[hc_num]->HCCHAR);
  hcchar.b.chena = 1;
  hcchar.b.chdis = 0;
  USB_OTG_WRITE_R32(&pusbdev->regs.HC_REGS[hc_num]->HCCHAR, hcchar.u32val);
  return status;  
}

/**
* @brief  Stop the device and clean up fifo's
* @param  None
* @retval : None
*/
void USB_OTG_StopHost(USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_OTG_HCCHAR_Type  hcchar;
  uint32_t                i;
  
  USB_OTG_WRITE_R32(&pusbdev->regs.HREGS->HAINTMASK , 0);
  USB_OTG_WRITE_R32(&pusbdev->regs.HREGS->HAINT,      0xFFFFFFFF);
  /* Flush out any leftover queued requests. */
  
  for (i = 0; i < pusbdev->cfg.host_channels; i++)
  {
    hcchar.u32val = USB_OTG_READ_R32(&pusbdev->regs.HC_REGS[i]->HCCHAR);
    hcchar.b.chena = 0;
    hcchar.b.chdis = 1;
    hcchar.b.eptdir = 0;
    USB_OTG_WRITE_R32(&pusbdev->regs.HC_REGS[i]->HCCHAR, hcchar.u32val);
  }
  
  /* Flush the FIFO */
  USB_OTG_FlushRxFifo(pusbdev);
  USB_OTG_FlushTxFifo(pusbdev ,  0x10 );  
}
#endif
#ifdef USE_DEVICE_MODE
/*         PCD Core Layer       */

/**
* @brief  USB_OTG_InitDevSpeed :Initializes the DevSpd field of DCFG register 
*         depending the PHY ept_type and the enumeration speed of the device.
* @param  pusbdev : Selected device
* @retval : None
*/
void USB_OTG_InitDevSpeed(USB_OTG_CORE_HANDLE *pusbdev , uint8_t speed)
{
  USB_OTG_DCFG_Type   dcfg;
  
  dcfg.u32val = USB_OTG_READ_R32(&pusbdev->regs.DREGS->DCFG);
  dcfg.b.devspd = speed;
  USB_OTG_WRITE_R32(&pusbdev->regs.DREGS->DCFG, dcfg.u32val);
}


/**
* @brief  USB_OTG_CoreInitDev : Initializes the USB_OTG controller registers 
*         for device mode
* @param  pusbdev : Selected device
* @retval USB_OTG_STS : status
*/
USB_OTG_STS USB_OTG_CoreInitDev (USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_OTG_STS             status       = USB_OTG_OK;
  USB_OTG_DEPCTL_Type  depctl;
  uint32_t i;
  USB_OTG_DCFG_Type    dcfg;
  USB_OTG_FSIZ_Type    nptxfifosize;
  USB_OTG_FSIZ_Type    txfifosize;
  USB_OTG_DIEPMSK_Type msk;
//  USB_OTG_DTHRCTL_Type dthrctl;  
  
  depctl.u32val = 0;
  dcfg.u32val = 0;
  nptxfifosize.u32val = 0;
  txfifosize.u32val = 0;
  msk.u32val = 0;
  
  /* Restart the Phy Clock */
  USB_OTG_WRITE_R32(pusbdev->regs.PCGCCTL, 0);
  /* Device configuration register */
  dcfg.u32val = USB_OTG_READ_R32( &pusbdev->regs.DREGS->DCFG);
  dcfg.b.perfivl = DCFG_FRAME_INTERVAL_80;
  USB_OTG_WRITE_R32( &pusbdev->regs.DREGS->DCFG, dcfg.u32val );
  
#ifdef USB_OTG_FS_CORE
  if(pusbdev->cfg.coreid == USB_OTG_FS_CORE_ID  )
  {  
    
    /* Set Full speed phy */
    USB_OTG_InitDevSpeed (pusbdev , USB_OTG_SPEED_PARAM_FULL);
    
    /* set Rx FIFO size */
    USB_OTG_WRITE_R32(&pusbdev->regs.GREGS->GRXFSIZE, RX_FIFO_FS_SIZE);
    
    /* EP0 TX*/
    nptxfifosize.b.depth     = TX0_FIFO_FS_SIZE;
    nptxfifosize.b.startaddr = RX_FIFO_FS_SIZE;
    USB_OTG_WRITE_R32( &pusbdev->regs.GREGS->DINEPTTXF0_HNPTXFSIZE, nptxfifosize.u32val );
    
    
    /* EP1 TX*/
    txfifosize.b.startaddr = nptxfifosize.b.startaddr + nptxfifosize.b.depth;
    txfifosize.b.depth = TX1_FIFO_FS_SIZE;
    USB_OTG_WRITE_R32( &pusbdev->regs.GREGS->DINEPTTXF[0], txfifosize.u32val );
    
    
    /* EP2 TX*/
    txfifosize.b.startaddr += txfifosize.b.depth;
    txfifosize.b.depth = TX2_FIFO_FS_SIZE;
    USB_OTG_WRITE_R32( &pusbdev->regs.GREGS->DINEPTTXF[1], txfifosize.u32val );
    
    
    /* EP3 TX*/  
    txfifosize.b.startaddr += txfifosize.b.depth;
    txfifosize.b.depth = TX3_FIFO_FS_SIZE;
    USB_OTG_WRITE_R32( &pusbdev->regs.GREGS->DINEPTTXF[2], txfifosize.u32val );
  }
#endif
  /* Flush the FIFOs */
  USB_OTG_FlushTxFifo(pusbdev , 0x10); /* all Tx FIFOs */
  USB_OTG_FlushRxFifo(pusbdev);
  /* Clear all pending Device Interrupts */
  USB_OTG_WRITE_R32( &pusbdev->regs.DREGS->DINEPTMASK, 0 );
  USB_OTG_WRITE_R32( &pusbdev->regs.DREGS->DOUTEPTMASK, 0 );
  USB_OTG_WRITE_R32( &pusbdev->regs.DREGS->DAINT, 0xFFFFFFFF );
  USB_OTG_WRITE_R32( &pusbdev->regs.DREGS->DAINTMASK, 0 );
  
  for (i = 0; i < pusbdev->cfg.dev_endpoints; i++)
  {
    depctl.u32val = USB_OTG_READ_R32(&pusbdev->regs.INEP_REGS[i]->DINEPTCTRL);
    if (depctl.b.eptena)
    {
      depctl.u32val = 0;
      depctl.b.eptdis = 1;
      depctl.b.snak = 1;
    }
    else
    {
      depctl.u32val = 0;
    }
    USB_OTG_WRITE_R32( &pusbdev->regs.INEP_REGS[i]->DINEPTCTRL, depctl.u32val);
    USB_OTG_WRITE_R32( &pusbdev->regs.INEP_REGS[i]->DINEPTTXSIZE, 0);
    USB_OTG_WRITE_R32( &pusbdev->regs.INEP_REGS[i]->DINEPTINT, 0xFF);
  }
  for (i = 0; i <  pusbdev->cfg.dev_endpoints; i++)
  {
    USB_OTG_DEPCTL_Type  depctl;
    depctl.u32val = USB_OTG_READ_R32(&pusbdev->regs.OUTEP_REGS[i]->DOUTEPTCTRL);
    if (depctl.b.eptena)
    {
      depctl.u32val = 0;
      depctl.b.eptdis = 1;
      depctl.b.snak = 1;
    }
    else
    {
      depctl.u32val = 0;
    }
    USB_OTG_WRITE_R32( &pusbdev->regs.OUTEP_REGS[i]->DOUTEPTCTRL, depctl.u32val);
    USB_OTG_WRITE_R32( &pusbdev->regs.OUTEP_REGS[i]->DOUTEPTTXSIZE, 0);
    USB_OTG_WRITE_R32( &pusbdev->regs.OUTEP_REGS[i]->DOUTEPTINT, 0xFF);
  }
  msk.u32val = 0;
  msk.b.txfifoudn = 1;
  USB_OTG_MODIFY_R32(&pusbdev->regs.DREGS->DINEPTMASK, msk.u32val, msk.u32val);
  
  USB_OTG_EnableDevInt(pusbdev);
  return status;
}


/**
* @brief  USB_OTG_EnableDevInt : Enables the Device mode interrupts
* @param  pusbdev : Selected device
* @retval USB_OTG_STS : status
*/
USB_OTG_STS USB_OTG_EnableDevInt(USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_OTG_STS status = USB_OTG_OK;
  USB_OTG_GINTMSK_Type  intmsk;
  
  intmsk.u32val = 0;
  
  /* Disable all interrupts. */
  USB_OTG_WRITE_R32( &pusbdev->regs.GREGS->GINTMASK, 0);
  /* Clear any pending interrupts */
  USB_OTG_WRITE_R32( &pusbdev->regs.GREGS->GINTSTS, 0xBFFFFFFF);
  /* Enable the common interrupts */
  USB_OTG_EnableCommonInt(pusbdev);
  
//  if (pusbdev->cfg.dma_enable == 0)
  {
    intmsk.b.rxflvlm = 1;
  }
  
  /* Enable interrupts matching to the Device mode ONLY */
  intmsk.b.usbsuspm = 1;
  intmsk.b.usbrstm   = 1;
  intmsk.b.enumdonem   = 1;
  intmsk.b.ieptintm   = 1;
  intmsk.b.oeptintm  = 1;
  intmsk.b.sofm    = 1; 
  
  intmsk.b.incomisoinm    = 1; 
  intmsk.b.incompisooutm    = 1;   
#ifdef VBUS_SENSING_ENABLED
  intmsk.b.sreqintm    = 1; 
  intmsk.b.otgintm    = 1;    
#endif  
  USB_OTG_MODIFY_R32( &pusbdev->regs.GREGS->GINTMASK, intmsk.u32val, intmsk.u32val);
  return status;
}


/**
* @brief  USB_OTG_GetDeviceSpeed
*         Get the device speed from the device status register
* @param  None
* @retval status
*/
enum USB_OTG_SPEED_MODE USB_OTG_GetDeviceSpeed (USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_OTG_DSTS_Type  dsts;
  enum USB_OTG_SPEED_MODE speed = USB_SPEED_UNKNOWN;
  
  
  dsts.u32val = USB_OTG_READ_R32(&pusbdev->regs.DREGS->DSTS);
  
  switch (dsts.b.enumspd)
  {
  case DSTS_ENUMSPD_HS_PHY_30MHZ_OR_60MHZ:
    speed = USB_SPEED_HIGH;
    break;
  case DSTS_ENUMSPD_FS_PHY_30MHZ_OR_60MHZ:
  case DSTS_ENUMSPD_FS_PHY_48MHZ:
    speed = USB_SPEED_FULL;
    break;
    
  case DSTS_ENUMSPD_LS_PHY_6MHZ:
    speed = USB_SPEED_LOW;
    break;
  default:
    speed = USB_SPEED_FULL;
    break; 
  }
  
  return speed;
}

/**
* @brief  enables EP0 OUT to receive SETUP packets and configures EP0
*   for transmitting packets
* @param  None
* @retval USB_OTG_STS : status
*/
USB_OTG_STS  USB_OTG_EPT0Activate(USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_OTG_STS             status = USB_OTG_OK;
  USB_OTG_DSTS_Type    dsts;
  USB_OTG_DEPCTL_Type  diepctl;
  USB_OTG_DCTL_Type    dctl;
  
  dctl.u32val = 0;
  /* Read the Device Status and Endpoint 0 Control registers */
  dsts.u32val = USB_OTG_READ_R32(&pusbdev->regs.DREGS->DSTS);
  diepctl.u32val = USB_OTG_READ_R32(&pusbdev->regs.INEP_REGS[0]->DINEPTCTRL);
  /* Set the MPS of the IN EP based on the enumeration speed */
  switch (dsts.b.enumspd)
  {
  case DSTS_ENUMSPD_HS_PHY_30MHZ_OR_60MHZ:
  case DSTS_ENUMSPD_FS_PHY_30MHZ_OR_60MHZ:
  case DSTS_ENUMSPD_FS_PHY_48MHZ:
    diepctl.b.maxpsize = DEP0CTL_MPS_64;
    break;
  case DSTS_ENUMSPD_LS_PHY_6MHZ:
    diepctl.b.maxpsize = DEP0CTL_MPS_8;
    break;
  default:
    diepctl.b.maxpsize = DEP0CTL_MPS_64;
    break; 
  }
  USB_OTG_WRITE_R32(&pusbdev->regs.INEP_REGS[0]->DINEPTCTRL, diepctl.u32val);
  dctl.b.cgnpinnak = 1;
  USB_OTG_MODIFY_R32(&pusbdev->regs.DREGS->DCTRL, dctl.u32val, dctl.u32val);
  return status;
}


/**
* @brief  USB_OTG_EPTActivate : Activates an EP
* @param  pusbdev : Selected device
* @retval USB_OTG_STS : status
*/
USB_OTG_STS USB_OTG_EPTActivate(USB_OTG_CORE_HANDLE *pusbdev , USB_OTG_EPT *ept)
{
  USB_OTG_STS status = USB_OTG_OK;
  USB_OTG_DEPCTL_Type  depctl;
  USB_OTG_DAINT_TypeDef  daintmsk;
  __IO uint32_t *addr;
  
  
  depctl.u32val = 0;
  daintmsk.u32val = 0;
  /* Read DEPCTLn register */
  if (ept->is_in == 1)
  {
    addr = &pusbdev->regs.INEP_REGS[ept->num]->DINEPTCTRL;
    daintmsk.ept.in = 1 << ept->num;
  }
  else
  {
    addr = &pusbdev->regs.OUTEP_REGS[ept->num]->DOUTEPTCTRL;
    daintmsk.ept.out = 1 << ept->num;
  }
  /* If the EP is already active don't change the EP Control
  * register. */
  depctl.u32val = USB_OTG_READ_R32(addr);
  if (!depctl.b.usbaept)
  {
    depctl.b.maxpsize    = ept->maxpacket;
    depctl.b.eptype = ept->ept_type;
    depctl.b.txfnum = ept->tx_fifo_num;
    depctl.b.setd0pid = 1;
    depctl.b.usbaept = 1;
    USB_OTG_WRITE_R32(addr, depctl.u32val);
  }
  /* Enable the Interrupt for this EP */ 
  USB_OTG_MODIFY_R32(&pusbdev->regs.DREGS->DAINTMASK, 0, daintmsk.u32val);
  return status;
}


/**
* @brief  USB_OTG_EPTDeactivate : Deactivates an EP
* @param  pusbdev : Selected device
* @retval USB_OTG_STS : status
*/
USB_OTG_STS USB_OTG_EPTDeactivate(USB_OTG_CORE_HANDLE *pusbdev , USB_OTG_EPT *ept)
{
  USB_OTG_STS status = USB_OTG_OK;
  USB_OTG_DEPCTL_Type  depctl;
  USB_OTG_DAINT_TypeDef  daintmsk;
  __IO uint32_t *addr;
  
  depctl.u32val = 0;
  daintmsk.u32val = 0;  
  /* Read DEPCTLn register */
  if (ept->is_in == 1)
  {
    addr = &pusbdev->regs.INEP_REGS[ept->num]->DINEPTCTRL;
    daintmsk.ept.in = 1 << ept->num;
  }
  else
  {
    addr = &pusbdev->regs.OUTEP_REGS[ept->num]->DOUTEPTCTRL;
    daintmsk.ept.out = 1 << ept->num;
  }
  depctl.b.usbaept = 0;
  USB_OTG_WRITE_R32(addr, depctl.u32val);
  /* Disable the Interrupt for this EP */
    
  USB_OTG_MODIFY_R32(&pusbdev->regs.DREGS->DAINTMASK, daintmsk.u32val, 0);
  return status;
}


/**
* @brief  USB_OTG_EPTStartXfer : Handle the setup for data xfer for an EP and 
*         starts the xfer
* @param  pusbdev : Selected device
* @retval USB_OTG_STS : status
*/
USB_OTG_STS USB_OTG_EPTStartXfer(USB_OTG_CORE_HANDLE *pusbdev , USB_OTG_EPT *ept)
{
  USB_OTG_STS status = USB_OTG_OK;
  USB_OTG_DEPCTL_Type     depctl;
  USB_OTG_DEPXFRSIZ_Type  deptsiz;
  USB_OTG_DSTS_Type       dsts;    
  uint32_t fifoemptymsk = 0;  
  
  depctl.u32val = 0;
  deptsiz.u32val = 0;
  /* IN endpoint */
  if (ept->is_in == 1)
  {
    depctl.u32val  = USB_OTG_READ_R32(&(pusbdev->regs.INEP_REGS[ept->num]->DINEPTCTRL));
    deptsiz.u32val = USB_OTG_READ_R32(&(pusbdev->regs.INEP_REGS[ept->num]->DINEPTTXSIZE));
    /* Zero Length Packet? */
    if (ept->xfer_len == 0)
    {
      deptsiz.b.xfersize = 0;
      deptsiz.b.pktcnt = 1;
    }
    else
    {
      /* Program the transfer size and packet count
      * as follows: xfersize = N * maxpacket +
      * short_packet pktcnt = N + (short_packet
      * exist ? 1 : 0)
      */
      deptsiz.b.xfersize = ept->xfer_len;
      deptsiz.b.pktcnt = (ept->xfer_len - 1 + ept->maxpacket) / ept->maxpacket;
      
      if (ept->ept_type == EPT_TYPE_ISOC)
      {
        deptsiz.b.mcnt = 1;
      }       
    }
    USB_OTG_WRITE_R32(&pusbdev->regs.INEP_REGS[ept->num]->DINEPTTXSIZE, deptsiz.u32val);
    
    {
      if (ept->ept_type != EPT_TYPE_ISOC)
      {
        /* Enable the Tx FIFO Empty Interrupt for this EP */
        if (ept->xfer_len > 0)
        {
          fifoemptymsk = 1 << ept->num;
          USB_OTG_MODIFY_R32(&pusbdev->regs.DREGS->DINEPTEMPMASK, 0, fifoemptymsk);
        }
      }
    }
    
    
    if (ept->ept_type == EPT_TYPE_ISOC)
    {
      dsts.u32val = USB_OTG_READ_R32(&pusbdev->regs.DREGS->DSTS);
      
      if (((dsts.b.soffn)&0x1) == 0)
      {
        depctl.b.soddfrm = 1;
      }
      else
      {
        depctl.b.setd0pid = 1;
      }
    } 
    
    /* EP enable, IN data in FIFO */
    depctl.b.cnak = 1;
    depctl.b.eptena = 1;
    USB_OTG_WRITE_R32(&pusbdev->regs.INEP_REGS[ept->num]->DINEPTCTRL, depctl.u32val);
    
    if (ept->ept_type == EPT_TYPE_ISOC)
    {
      USB_OTG_WritePacket(pusbdev, ept->xfer_buff, ept->num, ept->xfer_len);   
    }    
  }
  else
  {
    /* OUT endpoint */
    depctl.u32val  = USB_OTG_READ_R32(&(pusbdev->regs.OUTEP_REGS[ept->num]->DOUTEPTCTRL));
    deptsiz.u32val = USB_OTG_READ_R32(&(pusbdev->regs.OUTEP_REGS[ept->num]->DOUTEPTTXSIZE));
    /* Program the transfer size and packet count as follows:
    * pktcnt = N
    * xfersize = N * maxpacket
    */
    if (ept->xfer_len == 0)
    {
      deptsiz.b.xfersize = ept->maxpacket;
      deptsiz.b.pktcnt = 1;
    }
    else
    {
      deptsiz.b.pktcnt = (ept->xfer_len + (ept->maxpacket - 1)) / ept->maxpacket;
      deptsiz.b.xfersize = deptsiz.b.pktcnt * ept->maxpacket;
      ept->xfer_len = deptsiz.b.xfersize ;
    }
    USB_OTG_WRITE_R32(&pusbdev->regs.OUTEP_REGS[ept->num]->DOUTEPTTXSIZE, deptsiz.u32val);
       
    if (ept->ept_type == EPT_TYPE_ISOC)
    {
      if (ept->even_odd_frame)
      {
        depctl.b.soddfrm = 1;
      }
      else
      {
        depctl.b.setd0pid = 1;
      }
    }
    /* EP enable */
    depctl.b.cnak = 1;
    depctl.b.eptena = 1;
    USB_OTG_WRITE_R32(&pusbdev->regs.OUTEP_REGS[ept->num]->DOUTEPTCTRL, depctl.u32val);
  }
  return status;
}


/**
* @brief  USB_OTG_EPT0StartXfer : Handle the setup for a data xfer for EP0 and 
*         starts the xfer
* @param  pusbdev : Selected device
* @retval USB_OTG_STS : status
*/
USB_OTG_STS USB_OTG_EPT0StartXfer(USB_OTG_CORE_HANDLE *pusbdev , USB_OTG_EPT *ept)
{
  USB_OTG_STS                 status = USB_OTG_OK;
  USB_OTG_DEPCTL_Type      depctl;
  USB_OTG_DEP0XFRSIZ_Type  deptsiz;
  USB_OTG_INEPT_REGS          *in_regs;
  uint32_t fifoemptymsk = 0;
  
  depctl.u32val   = 0;
  deptsiz.u32val  = 0;
  /* IN endpoint */
  if (ept->is_in == 1)
  {
    in_regs = pusbdev->regs.INEP_REGS[0];
    depctl.u32val  = USB_OTG_READ_R32(&in_regs->DINEPTCTRL);
    deptsiz.u32val = USB_OTG_READ_R32(&in_regs->DINEPTTXSIZE);
    /* Zero Length Packet? */
    if (ept->xfer_len == 0)
    {
      deptsiz.b.xfersize = 0;
      deptsiz.b.pktcnt = 1;
      
    }
    else
    {
      if (ept->xfer_len > ept->maxpacket)
      {
        ept->xfer_len = ept->maxpacket;
        deptsiz.b.xfersize = ept->maxpacket;
      }
      else
      {
        deptsiz.b.xfersize = ept->xfer_len;
      }
      deptsiz.b.pktcnt = 1;
    }
    USB_OTG_WRITE_R32(&in_regs->DINEPTTXSIZE, deptsiz.u32val);
    
    /* EP enable, IN data in FIFO */
    depctl.b.cnak = 1;
    depctl.b.eptena = 1;
    USB_OTG_WRITE_R32(&in_regs->DINEPTCTRL, depctl.u32val);
    
    
    
//    if (pusbdev->cfg.dma_enable == 0)
    {
      /* Enable the Tx FIFO Empty Interrupt for this EP */
      if (ept->xfer_len > 0)
      {
        {
          fifoemptymsk |= 1 << ept->num;
          USB_OTG_MODIFY_R32(&pusbdev->regs.DREGS->DINEPTEMPMASK, 0, fifoemptymsk);
        }
      }
    }
  }
  else
  {
    /* OUT endpoint */
    depctl.u32val  = USB_OTG_READ_R32(&pusbdev->regs.OUTEP_REGS[ept->num]->DOUTEPTCTRL);
    deptsiz.u32val = USB_OTG_READ_R32(&pusbdev->regs.OUTEP_REGS[ept->num]->DOUTEPTTXSIZE);
    /* Program the transfer size and packet count as follows:
    * xfersize = N * (maxpacket + 4 - (maxpacket % 4))
    * pktcnt = N           */
    if (ept->xfer_len == 0)
    {
      deptsiz.b.xfersize = ept->maxpacket;
      deptsiz.b.pktcnt = 1;
    }
    else
    {
      ept->xfer_len = ept->maxpacket;
      deptsiz.b.xfersize = ept->maxpacket;
      deptsiz.b.pktcnt = 1;
    }
    USB_OTG_WRITE_R32(&pusbdev->regs.OUTEP_REGS[ept->num]->DOUTEPTTXSIZE, deptsiz.u32val);
    /* EP enable */
    depctl.b.cnak = 1;
    depctl.b.eptena = 1;
    USB_OTG_WRITE_R32 (&(pusbdev->regs.OUTEP_REGS[ept->num]->DOUTEPTCTRL), depctl.u32val);
    
  }
  return status;
}


/**
* @brief  USB_OTG_EPTSetStall : Set the EP STALL
* @param  pusbdev : Selected device
* @retval USB_OTG_STS : status
*/
USB_OTG_STS USB_OTG_EPTSetStall(USB_OTG_CORE_HANDLE *pusbdev , USB_OTG_EPT *ept)
{
  USB_OTG_STS status = USB_OTG_OK;
  USB_OTG_DEPCTL_Type  depctl;
  __IO uint32_t *depctl_addr;
  
  depctl.u32val = 0;
  if (ept->is_in == 1)
  {
    depctl_addr = &(pusbdev->regs.INEP_REGS[ept->num]->DINEPTCTRL);
    depctl.u32val = USB_OTG_READ_R32(depctl_addr);
    /* set the disable and stall bits */
    if (depctl.b.eptena)
    {
      depctl.b.eptdis = 1;
    }
    depctl.b.stall = 1;
    USB_OTG_WRITE_R32(depctl_addr, depctl.u32val);
  }
  else
  {
    depctl_addr = &(pusbdev->regs.OUTEP_REGS[ept->num]->DOUTEPTCTRL);
    depctl.u32val = USB_OTG_READ_R32(depctl_addr);
    /* set the stall bit */
    depctl.b.stall = 1;
    USB_OTG_WRITE_R32(depctl_addr, depctl.u32val);
  }
  return status;
}


/**
* @brief  Clear the EP STALL
* @param  pusbdev : Selected device
* @retval USB_OTG_STS : status
*/
USB_OTG_STS USB_OTG_EPTClearStall(USB_OTG_CORE_HANDLE *pusbdev , USB_OTG_EPT *ept)
{
  USB_OTG_STS status = USB_OTG_OK;
  USB_OTG_DEPCTL_Type  depctl;
  __IO uint32_t *depctl_addr;
  
  depctl.u32val = 0;
  
  if (ept->is_in == 1)
  {
    depctl_addr = &(pusbdev->regs.INEP_REGS[ept->num]->DINEPTCTRL);
  }
  else
  {
    depctl_addr = &(pusbdev->regs.OUTEP_REGS[ept->num]->DOUTEPTCTRL);
  }
  depctl.u32val = USB_OTG_READ_R32(depctl_addr);
  /* clear the stall bits */
  depctl.b.stall = 0;
  if (ept->ept_type == EPT_TYPE_INTR || ept->ept_type == EPT_TYPE_BULK)
  {
    depctl.b.setd0pid = 1; /* DATA0 */
  }
  USB_OTG_WRITE_R32(depctl_addr, depctl.u32val);
  return status;
}


/**
* @brief  USB_OTG_ReadDevAllOutEp_itr : returns OUT endpoint interrupt bits
* @param  pusbdev : Selected device
* @retval OUT endpoint interrupt bits
*/
uint32_t USB_OTG_ReadDevAllOutEp_itr(USB_OTG_CORE_HANDLE *pusbdev)
{
  uint32_t v;
  v  = USB_OTG_READ_R32(&pusbdev->regs.DREGS->DAINT);
  v &= USB_OTG_READ_R32(&pusbdev->regs.DREGS->DAINTMASK);
  return ((v & 0xffff0000) >> 16);
}


/**
* @brief  USB_OTG_ReadDevOutEP_itr : returns Device OUT EP Interrupt register
* @param  pusbdev : Selected device
* @param  ept : end point number
* @retval Device OUT EP Interrupt register
*/
uint32_t USB_OTG_ReadDevOutEP_itr(USB_OTG_CORE_HANDLE *pusbdev , uint8_t eptnum)
{
  uint32_t v;
  v  = USB_OTG_READ_R32(&pusbdev->regs.OUTEP_REGS[eptnum]->DOUTEPTINT);
  v &= USB_OTG_READ_R32(&pusbdev->regs.DREGS->DOUTEPTMASK);
  return v;
}


/**
* @brief  USB_OTG_ReadDevAllInEPItr : Get int status register
* @param  pusbdev : Selected device
* @retval int status register
*/
uint32_t USB_OTG_ReadDevAllInEPItr(USB_OTG_CORE_HANDLE *pusbdev)
{
  uint32_t v;
  v = USB_OTG_READ_R32(&pusbdev->regs.DREGS->DAINT);
  v &= USB_OTG_READ_R32(&pusbdev->regs.DREGS->DAINTMASK);
  return (v & 0xffff);
}

/**
* @brief  configures EPO to receive SETUP packets
* @param  None
* @retval : None
*/
void USB_OTG_EPT0_OutStart(USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_OTG_DEP0XFRSIZ_Type  doeptsize0;
  doeptsize0.u32val = 0;
  doeptsize0.b.setupcnt = 3;
  doeptsize0.b.pktcnt = 1;
  doeptsize0.b.xfersize = 8 * 3;
  USB_OTG_WRITE_R32( &pusbdev->regs.OUTEP_REGS[0]->DOUTEPTTXSIZE, doeptsize0.u32val );
}

/**
* @brief  USB_OTG_RemoteWakeup : active remote wakeup signalling
* @param  None
* @retval : None
*/
void USB_OTG_ActiveRemoteWakeup(USB_OTG_CORE_HANDLE *pusbdev)
{
  
  USB_OTG_DCTL_Type     dctl;
  USB_OTG_DSTS_Type     dsts;
  USB_OTG_PCGCCTL_Type  power;  
  
  if (pusbdev->dev.DeviceRemoteWakeup) 
  {
    dsts.u32val = USB_OTG_READ_R32(&pusbdev->regs.DREGS->DSTS);
    if(dsts.b.suspsts == 1)
    {
      if(pusbdev->cfg.low_power)
      {
        /* un-gate USB Core clock */
        power.u32val = USB_OTG_READ_R32(pusbdev->regs.PCGCCTL);
        power.b.gatehclk = 0;
        power.b.stoppclk = 0;
        USB_OTG_WRITE_R32(pusbdev->regs.PCGCCTL, power.u32val);
      }   
      /* active Remote wakeup signaling */
      dctl.u32val = 0;
      dctl.b.rwkupsig = 1;
      USB_OTG_MODIFY_R32(&pusbdev->regs.DREGS->DCTRL, 0, dctl.u32val);
      USB_OTG_BSP_mDelay(5);
      USB_OTG_MODIFY_R32(&pusbdev->regs.DREGS->DCTRL, dctl.u32val, 0 );
    }
  }
}


/**
* @brief  USB_OTG_UngateClock : active USB Core clock
* @param  None
* @retval : None
*/
void USB_OTG_UngateClock(USB_OTG_CORE_HANDLE *pusbdev)
{
  if(pusbdev->cfg.low_power)
  {
    
    USB_OTG_DSTS_Type     dsts;
    USB_OTG_PCGCCTL_Type  power; 
    
    dsts.u32val = USB_OTG_READ_R32(&pusbdev->regs.DREGS->DSTS);
    
    if(dsts.b.suspsts == 1)
    {
      /* un-gate USB Core clock */
      power.u32val = USB_OTG_READ_R32(pusbdev->regs.PCGCCTL);
      power.b.gatehclk = 0;
      power.b.stoppclk = 0;
      USB_OTG_WRITE_R32(pusbdev->regs.PCGCCTL, power.u32val);
      
    }
  }
}

/**
* @brief  Stop the device and clean up fifo's
* @param  None
* @retval : None
*/
void USB_OTG_StopDevice(USB_OTG_CORE_HANDLE *pusbdev)
{
  uint32_t i;
  
  pusbdev->dev.device_status = 1;
  
  for (i = 0; i < pusbdev->cfg.dev_endpoints ; i++)
  {
    USB_OTG_WRITE_R32( &pusbdev->regs.INEP_REGS[i]->DINEPTINT, 0xFF);
    USB_OTG_WRITE_R32( &pusbdev->regs.OUTEP_REGS[i]->DOUTEPTINT, 0xFF);
  }
  
  USB_OTG_WRITE_R32( &pusbdev->regs.DREGS->DINEPTMASK, 0 );
  USB_OTG_WRITE_R32( &pusbdev->regs.DREGS->DOUTEPTMASK, 0 );
  USB_OTG_WRITE_R32( &pusbdev->regs.DREGS->DAINTMASK, 0 );
  USB_OTG_WRITE_R32( &pusbdev->regs.DREGS->DAINT, 0xFFFFFFFF );  
  
  /* Flush the FIFO */
  USB_OTG_FlushRxFifo(pusbdev);
  USB_OTG_FlushTxFifo(pusbdev ,  0x10 );  
}

/**
* @brief  returns the EP Status
* @param  pusbdev : Selected device
*         ept : endpoint structure
* @retval : EP status
*/

uint32_t USB_OTG_GetEPStatus(USB_OTG_CORE_HANDLE *pusbdev ,USB_OTG_EPT *ept)
{
  USB_OTG_DEPCTL_Type  depctl;
  __IO uint32_t *depctl_addr;
  uint32_t Status = 0;  
  
  depctl.u32val = 0;
  if (ept->is_in == 1)
  {
    depctl_addr = &(pusbdev->regs.INEP_REGS[ept->num]->DINEPTCTRL);
    depctl.u32val = USB_OTG_READ_R32(depctl_addr);
    
    if (depctl.b.stall == 1)
    {
      Status = USB_OTG_EPT_TX_STALL;
    }
    else if (depctl.b.naksts == 1)
    {
      Status = USB_OTG_EPT_TX_NAK;
    }
    else 
    {
      Status = USB_OTG_EPT_TX_VALID;     
    }
  }
  else
  {
    depctl_addr = &(pusbdev->regs.OUTEP_REGS[ept->num]->DOUTEPTCTRL);
    depctl.u32val = USB_OTG_READ_R32(depctl_addr);
    if (depctl.b.stall == 1)
    {
      Status = USB_OTG_EPT_RX_STALL;
    }
    else if (depctl.b.naksts == 1)
    {
      Status = USB_OTG_EPT_RX_NAK;
    }
    else 
    {
      Status = USB_OTG_EPT_RX_VALID; 
    }
  } 
  
  /* Return the current status */
  return Status;
}

/**
* @brief  Set the EP Status
* @param  pusbdev : Selected device
*         Status : new Status
*         ept : EP structure
* @retval : None
*/
void USB_OTG_SetEPStatus (USB_OTG_CORE_HANDLE *pusbdev , USB_OTG_EPT *ept , uint32_t Status)
{
  USB_OTG_DEPCTL_Type  depctl;
  __IO uint32_t *depctl_addr;
  
  depctl.u32val = 0;
  
  /* Process for IN endpoint */
  if (ept->is_in == 1)
  {
    depctl_addr = &(pusbdev->regs.INEP_REGS[ept->num]->DINEPTCTRL);
    depctl.u32val = USB_OTG_READ_R32(depctl_addr);
    
    if (Status == USB_OTG_EPT_TX_STALL)  
    {
      USB_OTG_EPTSetStall(pusbdev, ept); return;
    }
    else if (Status == USB_OTG_EPT_TX_NAK)
    {
      depctl.b.snak = 1;
    }
    else if (Status == USB_OTG_EPT_TX_VALID)
    {
      if (depctl.b.stall == 1)
      {  
        ept->even_odd_frame = 0;
        USB_OTG_EPTClearStall(pusbdev, ept);
        return;
      }      
      depctl.b.cnak = 1;
      depctl.b.usbaept = 1; 
      depctl.b.eptena = 1;
    }
    else if (Status == USB_OTG_EPT_TX_DIS)
    {
      depctl.b.usbaept = 0;
    }
    
    else
    {
      /* Do Nothing */
    }
  } 
  else /* Process for OUT endpoint */
  {
    depctl_addr = &(pusbdev->regs.OUTEP_REGS[ept->num]->DOUTEPTCTRL);
    depctl.u32val = USB_OTG_READ_R32(depctl_addr);    
    
    if (Status == USB_OTG_EPT_RX_STALL)  {
      depctl.b.stall = 1;
    }
    else if (Status == USB_OTG_EPT_RX_NAK)
    {
      depctl.b.snak = 1;
    }
    else if (Status == USB_OTG_EPT_RX_VALID)
    {
      if (depctl.b.stall == 1)
      {  
        ept->even_odd_frame = 0;
        USB_OTG_EPTClearStall(pusbdev, ept);
        return;
      }  
      depctl.b.cnak = 1;
      depctl.b.usbaept = 1;    
      depctl.b.eptena = 1;
    }
    else if (Status == USB_OTG_EPT_RX_DIS)
    {
      depctl.b.usbaept = 0;    
    }
    
    else
    {
      /* Do Nothing */
    }
  }
  
  USB_OTG_WRITE_R32(depctl_addr, depctl.u32val); 
}

#endif
/**
* @}
*/ 

/**
* @}
*/ 

/**
* @}
*/

