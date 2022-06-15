/**
  ******************************************************************************
  * File   : usb_dcd_int.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Peripheral Device interrupt subroutines
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usb_dcd_int.h"
/** @addtogroup USB_OTG_DRIVER
* @{
*/

/** @defgroup USB_DCD_INT 
* @brief This file contains the interrupt subroutines for the Device mode.
* @{
*/


/** @defgroup USB_DCD_INT_Private_Defines
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USB_DCD_INT_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 



/** @defgroup USB_DCD_INT_Private_Macros
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USB_DCD_INT_Private_Variables
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USB_DCD_INT_Private_FunctionPrototypes
* @{
*/ 
/* static functions */
static uint32_t USB_device_ReadDevInEP (USB_OTG_CORE_HANDLE *pusbdev, uint8_t bepnum);

/* Interrupt Handlers */
static uint32_t USB_device_handle_InEpt_intsts(USB_OTG_CORE_HANDLE *pusbdev);
static uint32_t USB_device_handle_OutEpt_intsts(USB_OTG_CORE_HANDLE *pusbdev);
static uint32_t USB_device_handle_Sof_intsts(USB_OTG_CORE_HANDLE *pusbdev);

static uint32_t USB_device_handle_RxStatusQueueLevel_intsts(USB_OTG_CORE_HANDLE *pusbdev);
static uint32_t USB_device_WriteEmptyTxFifo(USB_OTG_CORE_HANDLE *pusbdev , uint32_t bepnum);

static uint32_t USB_device_handle_UsbReset_intsts(USB_OTG_CORE_HANDLE *pusbdev);
static uint32_t USB_device_handle_EnumDone_intsts(USB_OTG_CORE_HANDLE *pusbdev);
static uint32_t USB_device_handle_Resume_intsts(USB_OTG_CORE_HANDLE *pusbdev);
static uint32_t USB_device_handle_Suspend_intsts(USB_OTG_CORE_HANDLE *pusbdev);

static uint32_t USB_device_handle_IsoINIncomplete_intsts(USB_OTG_CORE_HANDLE *pusbdev);
static uint32_t USB_device_handle_IsoOUTIncomplete_intsts(USB_OTG_CORE_HANDLE *pusbdev);
#ifdef VBUS_SENSING_ENABLED
static uint32_t USB_device_handle_SessionRequest_intsts(USB_OTG_CORE_HANDLE *pusbdev);
static uint32_t USB_device_handle_OTG_intsts(USB_OTG_CORE_HANDLE *pusbdev);
#endif

/**
* @}
*/ 


/** @defgroup USB_DCD_INT_Private_Functions
* @{
*/ 


/**
* @brief  AT32_USBF_OTG_ISR_Handler
*         handles all USB Interrupts
* @param  pusbdev: device instance
* @retval status
*/
uint32_t USB_DEVICE_OTG_INTSTS_Handler (USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_OTG_GINTSTS_Type  gintr_status;
  uint32_t retval = 0;
  
  if (USB_OTG_IsDeviceMode(pusbdev)) /* ensure that we are in device mode */
  {
    gintr_status.u32val = USB_OTG_ReadCoreInterruptr(pusbdev);
    if (!gintr_status.u32val) /* avoid spurious interrupt */
    {
      return 0;
    }
    
    if (gintr_status.b.oeptint)
    {
      retval |= USB_device_handle_OutEpt_intsts(pusbdev);
    }    
    
    if (gintr_status.b.ieptint)
    {
      retval |= USB_device_handle_InEpt_intsts(pusbdev);
    }
    
    if (gintr_status.b.modmis)
    {
      USB_OTG_GINTSTS_Type  gintsts;
      
      /* Clear interrupt */
      gintsts.u32val = 0;
      gintsts.b.modmis = 1;
      USB_OTG_WRITE_R32(&pusbdev->regs.GREGS->GINTSTS, gintsts.u32val);
    }
    
    if (gintr_status.b.wkupint)
    {
      retval |= USB_device_handle_Resume_intsts(pusbdev);
    }
    
    if (gintr_status.b.usbsusp)
    {
      retval |= USB_device_handle_Suspend_intsts(pusbdev);
    }
    if (gintr_status.b.sof)
    {
      retval |= USB_device_handle_Sof_intsts(pusbdev);
      
    }
    
    if (gintr_status.b.rxflvl)
    {
      retval |= USB_device_handle_RxStatusQueueLevel_intsts(pusbdev);
      
    }
    
    if (gintr_status.b.usbrst)
    {
      retval |= USB_device_handle_UsbReset_intsts(pusbdev);
      
    }
    if (gintr_status.b.enumdone)
    {
      retval |= USB_device_handle_EnumDone_intsts(pusbdev);
    }
    
    if (gintr_status.b.incomisoin)
    {
      retval |= USB_device_handle_IsoINIncomplete_intsts(pusbdev);
    }

    if (gintr_status.b.incompisoout)
    {
      retval |= USB_device_handle_IsoOUTIncomplete_intsts(pusbdev);
    }    
#ifdef VBUS_SENSING_ENABLED
    if (gintr_status.b.sreqint)
    {
      retval |= USB_device_handle_SessionRequest_intsts(pusbdev);
    }

    if (gintr_status.b.otgint)
    {
      retval |= USB_device_handle_OTG_intsts(pusbdev);
    }   
#endif    
  }
  return retval;
}

#ifdef VBUS_SENSING_ENABLED
/**
* @brief  USB_device_handle_SessionRequest_intsts
*         Indicates that the USB_OTG controller has detected a connection
* @param  pusbdev: device instance
* @retval status
*/
static uint32_t USB_device_handle_SessionRequest_intsts(USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_OTG_GINTSTS_Type  gintsts;  
  USB_DEVICE_DCD_INT_fops->DevConnected (pusbdev);

  /* Clear interrupt */
  gintsts.u32val = 0;
  gintsts.b.sreqint = 1;
  USB_OTG_WRITE_R32 (&pusbdev->regs.GREGS->GINTSTS, gintsts.u32val);   
  return 1;
}

/**
* @brief  USB_device_handle_OTG_intsts
*         Indicates that the USB_OTG controller has detected an OTG event:
*                 used to detect the end of session i.e. disconnection
* @param  pusbdev: device instance
* @retval status
*/
static uint32_t USB_device_handle_OTG_intsts(USB_OTG_CORE_HANDLE *pusbdev)
{

  USB_OTG_GOTGINT_Type  gotgint;

  gotgint.u32val = USB_OTG_READ_R32(&pusbdev->regs.GREGS->GOTGINT);
  
  if (gotgint.b.sendet)
  {
    USB_DEVICE_DCD_INT_fops->DevDisconnected (pusbdev);
  }
  /* Clear OTG interrupt */
  USB_OTG_WRITE_R32(&pusbdev->regs.GREGS->GOTGINT, gotgint.u32val); 
  return 1;
}
#endif
/**
* @brief  USB_device_handle_Resume_intsts
*         Indicates that the USB_OTG controller has detected a resume or
*                 remote Wake-up sequence
* @param  pusbdev: device instance
* @retval status
*/
static uint32_t USB_device_handle_Resume_intsts(USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_OTG_GINTSTS_Type  gintsts;
  USB_OTG_DCTL_Type     devctl;
  USB_OTG_PCGCCTL_Type  power;
  
  if(pusbdev->cfg.low_power)
  {
    /* un-gate USB Core clock */
    power.u32val = USB_OTG_READ_R32(pusbdev->regs.PCGCCTL);
    power.b.gatehclk = 0;
    power.b.stoppclk = 0;
    USB_OTG_WRITE_R32(pusbdev->regs.PCGCCTL, power.u32val);
  }
  
  /* Clear the Remote Wake-up Signaling */
  devctl.u32val = 0;
  devctl.b.rwkupsig = 1;
  USB_OTG_MODIFY_R32(&pusbdev->regs.DREGS->DCTRL, devctl.u32val, 0);
  
  /* Inform upper layer by the Resume Event */
  USB_DEVICE_DCD_INT_fops->Resume (pusbdev);
  
  /* Clear interrupt */
  gintsts.u32val = 0;
  gintsts.b.wkupint = 1;
  USB_OTG_WRITE_R32 (&pusbdev->regs.GREGS->GINTSTS, gintsts.u32val);
  return 1;
}

/**
* @brief  USB_OTG_HandleUSBSuspend_ISR
*         Indicates that SUSPEND state has been detected on the USB
* @param  pusbdev: device instance
* @retval status
*/
static uint32_t USB_device_handle_Suspend_intsts(USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_OTG_GINTSTS_Type  gintsts;
  USB_OTG_PCGCCTL_Type  power;
  USB_OTG_DSTS_Type     dsts;
  __IO uint8_t prev_status = 0;
  
  prev_status = pusbdev->dev.device_status;
  USB_DEVICE_DCD_INT_fops->Suspend (pusbdev);      
  
  dsts.u32val = USB_OTG_READ_R32(&pusbdev->regs.DREGS->DSTS);
    
  /* Clear interrupt */
  gintsts.u32val = 0;
  gintsts.b.usbsusp = 1;
  USB_OTG_WRITE_R32(&pusbdev->regs.GREGS->GINTSTS, gintsts.u32val);
  
  if((pusbdev->cfg.low_power) && (dsts.b.suspsts == 1)  && 
    (pusbdev->dev.connection_status == 1) && 
    (prev_status  == USB_OTG_CONFIGURED))
  {
	/*  switch-off the clocks */
    power.u32val = 0;
    power.b.stoppclk = 1;
    USB_OTG_MODIFY_R32(pusbdev->regs.PCGCCTL, 0, power.u32val);  
    
    power.b.gatehclk = 1;
    USB_OTG_MODIFY_R32(pusbdev->regs.PCGCCTL, 0, power.u32val);
    
    /* Request to enter Sleep mode after exit from current ISR */
//    SCB->SCR |= (SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk);
  }
  return 1;
}

/**
* @brief  USB_device_handle_InEpt_intsts
*         Indicates that an IN EP has a pending Interrupt
* @param  pusbdev: device instance
* @retval status
*/
static uint32_t USB_device_handle_InEpt_intsts(USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_OTG_DIEPINTn_Type  diepint;
  
  uint32_t ep_intr;
  uint32_t bepnum = 0;
  uint32_t fifoemptymsk;
  diepint.u32val = 0;
  ep_intr = USB_OTG_ReadDevAllInEPItr(pusbdev);
  
  while ( ep_intr )
  {
    if ((ep_intr & 0x1) == 0x01) /* In ITR */
    {
      diepint.u32val = USB_device_ReadDevInEP(pusbdev , bepnum); /* Get In ITR status */
      if ( diepint.b.xferc )
      {
        fifoemptymsk = 0x1 << bepnum;
        USB_OTG_MODIFY_R32(&pusbdev->regs.DREGS->DINEPTEMPMASK, fifoemptymsk, 0);
        CLEAR_IN_EP_INTR(bepnum, xferc);
        /* TX COMPLETE */
        USB_DEVICE_DCD_INT_fops->DataInStage(pusbdev , bepnum);
                
      }
      if ( diepint.b.timeout )
      {
        CLEAR_IN_EP_INTR(bepnum, timeout);
      }
      if (diepint.b.inttxfemp)
      {
        CLEAR_IN_EP_INTR(bepnum, inttxfemp);
      }
      if (diepint.b.ineptnak)
      {
        CLEAR_IN_EP_INTR(bepnum, ineptnak);
      }
      if ( diepint.b.eptdis )
      {
        CLEAR_IN_EP_INTR(bepnum, eptdis);
      }       
      if (diepint.b.emptyint)
      {
        USB_device_WriteEmptyTxFifo(pusbdev , bepnum);
      }
    }
    bepnum++;
    ep_intr >>= 1;
  }
  
  return 1;
}

/**
* @brief  USB_device_handle_OutEpt_intsts
*         Indicates that an OUT EP has a pending Interrupt
* @param  pusbdev: device instance
* @retval status
*/
static uint32_t USB_device_handle_OutEpt_intsts(USB_OTG_CORE_HANDLE *pusbdev)
{
  uint32_t ep_intr;
  USB_OTG_DOEPINTn_Type  doepint;
//  USB_OTG_DEPXFRSIZ_Type  deptsiz;
  uint32_t bepnum = 0;
  
  doepint.u32val = 0;
  
  /* Read in the device interrupt bits */
  ep_intr = USB_OTG_ReadDevAllOutEp_itr(pusbdev);
  
  while ( ep_intr )
  {
    if (ep_intr&0x1)
    {
      
      doepint.u32val = USB_OTG_ReadDevOutEP_itr(pusbdev, bepnum);
      
      /* Transfer complete */
      if ( doepint.b.xferc )
      {
        /* Clear the bit in DOEPINTn for this interrupt */
        CLEAR_OUT_EP_INTR(bepnum, xferc);
        /* Inform upper layer: data ready */
        /* RX COMPLETE */
        USB_DEVICE_DCD_INT_fops->DataOutStage(pusbdev , bepnum);
              
      }
      /* Endpoint disable  */
      if ( doepint.b.eptdis )
      {
        /* Clear the bit in DOEPINTn for this interrupt */
        CLEAR_OUT_EP_INTR(bepnum, eptdis);
      }
      /* Setup Phase Done (control EPs) */
      if ( doepint.b.setup )
      {
        
        /* inform the upper layer that a setup packet is available */
        /* SETUP COMPLETE */
        USB_DEVICE_DCD_INT_fops->SetupStage(pusbdev);
        CLEAR_OUT_EP_INTR(bepnum, setup);
      }
    }
    bepnum++;
    ep_intr >>= 1;
  }
  return 1;
}

/**
* @brief  USB_device_handle_Sof_intsts
*         Handles the SOF Interrupts
* @param  pusbdev: device instance
* @retval status
*/
static uint32_t USB_device_handle_Sof_intsts(USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_OTG_GINTSTS_Type  GINTSTS;
  
  
  USB_DEVICE_DCD_INT_fops->SOF(pusbdev);
  
  /* Clear interrupt */
  GINTSTS.u32val = 0;
  GINTSTS.b.sof = 1;
  USB_OTG_WRITE_R32 (&pusbdev->regs.GREGS->GINTSTS, GINTSTS.u32val);
  
  return 1;
}

/**
* @brief  USB_device_handle_RxStatusQueueLevel_intsts
*         Handles the Rx Status Queue Level Interrupt
* @param  pusbdev: device instance
* @retval status
*/
static uint32_t USB_device_handle_RxStatusQueueLevel_intsts(USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_OTG_GINTMSK_Type  int_mask;
  USB_OTG_DRXSTS_Type   status;
  USB_OTG_EPT *ept;
  
  /* Disable the Rx Status Queue Level interrupt */
  int_mask.u32val = 0;
  int_mask.b.rxflvlm = 1;
  USB_OTG_MODIFY_R32( &pusbdev->regs.GREGS->GINTMASK, int_mask.u32val, 0);
  
  /* Get the Status from the top of the FIFO */
  status.u32val = USB_OTG_READ_R32( &pusbdev->regs.GREGS->GRXSTSP );
  
  ept = &pusbdev->dev.out_ept[status.b.eptnum];
  
  switch (status.b.pktsts)
  {
  case STS_GOUT_NAK:
    break;
  case STS_DATA_UPDT:
    if (status.b.bcnt)
    {
      USB_OTG_ReadPacket(pusbdev,ept->xfer_buff, status.b.bcnt);
      ept->xfer_buff += status.b.bcnt;
      ept->xfer_count += status.b.bcnt;
    }
    break;
  case STS_XFER_COMP:
    break;
  case STS_SETUP_COMP:
    break;
  case STS_SETUP_UPDT:
    /* Copy the setup packet received in FIFO into the setup buffer in RAM */
    USB_OTG_ReadPacket(pusbdev , pusbdev->dev.setup_packet, 8);
    ept->xfer_count += status.b.bcnt;
    break;
  default:
    break;
  }
  
  /* Enable the Rx Status Queue Level interrupt */
  USB_OTG_MODIFY_R32( &pusbdev->regs.GREGS->GINTMASK, 0, int_mask.u32val);
  
  return 1;
}

/**
* @brief  USB_device_WriteEmptyTxFifo
*         check FIFO for the next packet to be loaded
* @param  pusbdev: device instance
* @retval status
*/
static uint32_t USB_device_WriteEmptyTxFifo(USB_OTG_CORE_HANDLE *pusbdev, uint32_t bepnum)
{
  USB_OTG_DTXFSTSn_Type  txstatus;
  USB_OTG_EPT *ept;
  uint32_t len = 0;
  uint32_t len32b;
  uint32_t fifoemptymsk;
  txstatus.u32val = 0;

  
  ept = &pusbdev->dev.in_ept[bepnum];    
  
  len = ept->xfer_len - ept->xfer_count;
  
  if (len > ept->maxpacket)
  {
    len = ept->maxpacket;
  }
  
  len32b = (len + 3) / 4;
  txstatus.u32val = USB_OTG_READ_R32( &pusbdev->regs.INEP_REGS[bepnum]->DTXFSTS);
  
  while  (txstatus.b.txfspcavail > len32b &&
          ept->xfer_count < ept->xfer_len &&
            ept->xfer_len != 0)
  {
    /* Write the FIFO */
    len = ept->xfer_len - ept->xfer_count;
    
    if (len > ept->maxpacket)
    {
      len = ept->maxpacket;
    }
    len32b = (len + 3) / 4;
    
    USB_OTG_WritePacket (pusbdev , ept->xfer_buff, bepnum, len);
    
    ept->xfer_buff  += len;
    ept->xfer_count += len;
    
    txstatus.u32val = USB_OTG_READ_R32(&pusbdev->regs.INEP_REGS[bepnum]->DTXFSTS);
    
    /* Mask the TxFIFOEmpty interrupt  */
    if (ept->xfer_len == ept->xfer_count)
    {
      fifoemptymsk = 0x1 << ept->num;  
      USB_OTG_MODIFY_R32(&pusbdev->regs.DREGS->DINEPTEMPMASK, 
                           fifoemptymsk, 0); 
    }
  }
  
  return 1;
}

/**
* @brief  USB_device_handle_UsbReset_intsts
*         This interrupt occurs when a USB Reset is detected
* @param  pusbdev: device instance
* @retval status
*/
static uint32_t USB_device_handle_UsbReset_intsts(USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_OTG_DAINT_TypeDef    daintmsk;
  USB_OTG_DOEPMSK_Type  doepmsk;
  USB_OTG_DIEPMSK_Type  diepmsk;
  USB_OTG_DCFG_Type     dcfg;
  USB_OTG_DCTL_Type     dctl;
  USB_OTG_GINTSTS_Type  gintsts;
  uint32_t i;
  
  dctl.u32val = 0;
  daintmsk.u32val = 0;
  doepmsk.u32val = 0;
  diepmsk.u32val = 0;
  dcfg.u32val = 0;
  gintsts.u32val = 0;
  
  /* Clear the Remote Wake-up Signaling */
  dctl.b.rwkupsig = 1;
  USB_OTG_MODIFY_R32(&pusbdev->regs.DREGS->DCTRL, dctl.u32val, 0 );
  
  /* Flush the Tx FIFO */
  USB_OTG_FlushTxFifo(pusbdev ,  0 );
  
  for (i = 0; i < pusbdev->cfg.dev_endpoints ; i++)
  {
    USB_OTG_WRITE_R32( &pusbdev->regs.INEP_REGS[i]->DINEPTINT, 0xFF);
    USB_OTG_WRITE_R32( &pusbdev->regs.OUTEP_REGS[i]->DOUTEPTINT, 0xFF);
  }
  USB_OTG_WRITE_R32( &pusbdev->regs.DREGS->DAINT, 0xFFFFFFFF );
  
  daintmsk.ept.in = 1;
  daintmsk.ept.out = 1;
  USB_OTG_WRITE_R32( &pusbdev->regs.DREGS->DAINTMASK, daintmsk.u32val );
  
  doepmsk.b.setup = 1;
  doepmsk.b.xferc = 1;
  doepmsk.b.eptdis = 1;
  USB_OTG_WRITE_R32( &pusbdev->regs.DREGS->DOUTEPTMASK, doepmsk.u32val );

  diepmsk.b.xferc = 1;
  diepmsk.b.timeout = 1;
  diepmsk.b.eptdis = 1;

  USB_OTG_WRITE_R32( &pusbdev->regs.DREGS->DINEPTMASK, diepmsk.u32val );
  /* Reset Device Address */
  dcfg.u32val = USB_OTG_READ_R32( &pusbdev->regs.DREGS->DCFG);
  dcfg.b.devaddr = 0;
  USB_OTG_WRITE_R32( &pusbdev->regs.DREGS->DCFG, dcfg.u32val);
  
  
  /* setup EP0 to receive SETUP packets */
  USB_OTG_EPT0_OutStart(pusbdev);
  
  /* Clear interrupt */
  gintsts.u32val = 0;
  gintsts.b.usbrst = 1;
  USB_OTG_WRITE_R32 (&pusbdev->regs.GREGS->GINTSTS, gintsts.u32val);
  
  /*Reset internal state machine */
  USB_DEVICE_DCD_INT_fops->Reset(pusbdev);
  return 1;
}

/**
* @brief  USB_device_handle_EnumDone_intsts
*         Read the device status register and set the device speed
* @param  pusbdev: device instance
* @retval status
*/
static uint32_t USB_device_handle_EnumDone_intsts(USB_OTG_CORE_HANDLE *pusbdev)
{
  uint32_t hclk = 168000000;
  
  USB_OTG_GINTSTS_Type  gintsts;
  USB_OTG_GUSBCFG_Type  gusbcfg;
  RCC_ClockType RCC_Clocks;
  USB_OTG_EPT0Activate(pusbdev);
  
  /* Get HCLK frequency */
  RCC_GetClocksFreq(&RCC_Clocks);
  hclk = RCC_Clocks.AHBCLK_Freq;

  /* Clear default TRDT value and Set USB turn-around time based on device speed and PHY interface. */
  gusbcfg.u32val = USB_OTG_READ_R32(&pusbdev->regs.GREGS->GUSBCFG);
  gusbcfg.b.trdtim = 0;
  USB_OTG_WRITE_R32(&pusbdev->regs.GREGS->GUSBCFG, gusbcfg.u32val);
  
  /* Full or High speed */
  if ( USB_OTG_GetDeviceSpeed(pusbdev) == USB_SPEED_HIGH)
  {
    pusbdev->cfg.speed            = USB_OTG_SPEED_HIGH;
    pusbdev->cfg.maxpsize              = USB_OTG_HS_MAX_PACKET_SIZE ;    
    
    /*USBTRD min For HS device*/
    gusbcfg.b.trdtim = 9;
  }
  else
  {
    pusbdev->cfg.speed            = USB_OTG_SPEED_FULL;
    pusbdev->cfg.maxpsize              = USBFS_OTG_MAX_PACKET_SIZE ; 
    
    /* The USBTRD is configured according to the tables below, depending on AHB frequency 
    used by application. In the low AHB frequency range it is used to stretch enough the USB response 
    time to IN tokens, the USB turnaround time, so to compensate for the longer AHB read access 
    latency to the Data FIFO */
    
    if((hclk >= 15000000)&&(hclk < 16000000))
    {
      /* hclk Clock Range between 15-16 MHz */
      gusbcfg.b.trdtim = 0xE;
    }
    
    else if((hclk >= 16000000)&&(hclk < 17100000))
    {
      /* hclk Clock Range between 16-17.1 MHz */
      gusbcfg.b.trdtim = 0xD;
    }
    
    else if((hclk >= 17100000)&&(hclk < 18400000))
    {
      /* hclk Clock Range between 17-18.4 MHz */
      gusbcfg.b.trdtim = 0xC;
    }
    
    else if((hclk >= 18400000)&&(hclk < 20000000))
    {
      /* hclk Clock Range between 18.4-20 MHz */
      gusbcfg.b.trdtim = 0xB;
    }
    
    else if((hclk >= 20000000)&&(hclk < 21800000))
    {
      /* hclk Clock Range between 20-21.8 MHz */
      gusbcfg.b.trdtim = 0xA;
    }
    
    else if((hclk >= 21800000)&&(hclk < 24000000))
    {
      /* hclk Clock Range between 21.8-24 MHz */
      gusbcfg.b.trdtim = 0x9;
    }
    
    else if((hclk >= 24000000)&&(hclk < 26600000))
    {
      /* hclk Clock Range between 24-26.6 MHz */
      gusbcfg.b.trdtim = 0x8;
    }
    
    else if((hclk >= 26600000)&&(hclk < 30000000))
    {
      /* hclk Clock Range between 26.6-30 MHz */
      gusbcfg.b.trdtim = 0x7;
    }
    
    else if((hclk >= 30000000)&&(hclk < 34300000))
    {
      /* hclk Clock Range between 30-34.3 MHz */
      gusbcfg.b.trdtim= 0x6;
    } 
    
    else /* if(hclk >= 34300000) */
    {
      /* hclk Clock Range between 34.3-168 MHz */
      gusbcfg.b.trdtim = 0x5;
    }
  }

  USB_OTG_WRITE_R32(&pusbdev->regs.GREGS->GUSBCFG, gusbcfg.u32val);
  
  /* Clear interrupt */
  gintsts.u32val = 0;
  gintsts.b.enumdone = 1;
  USB_OTG_WRITE_R32( &pusbdev->regs.GREGS->GINTSTS, gintsts.u32val );
  return 1;
}


/**
* @brief  USB_device_handle_IsoINIncomplete_intsts
*         handle the ISO IN incomplete interrupt
* @param  pusbdev: device instance
* @retval status
*/
static uint32_t USB_device_handle_IsoINIncomplete_intsts(USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_OTG_GINTSTS_Type gintsts;  
  
  gintsts.u32val = 0;

  USB_DEVICE_DCD_INT_fops->IsoINIncomplete (pusbdev); 
  
  /* Clear interrupt */
  gintsts.b.incomisoin = 1;
  USB_OTG_WRITE_R32(&pusbdev->regs.GREGS->GINTSTS, gintsts.u32val);
  
  return 1;
}

/**
* @brief  USB_device_handle_IsoOUTIncomplete_intsts
*         handle the ISO OUT incomplete interrupt
* @param  pusbdev: device instance
* @retval status
*/
static uint32_t USB_device_handle_IsoOUTIncomplete_intsts(USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_OTG_GINTSTS_Type gintsts;  
  
  gintsts.u32val = 0;

  USB_DEVICE_DCD_INT_fops->IsoOUTIncomplete (pusbdev); 
  
  /* Clear interrupt */
  gintsts.b.incompisoout = 1;
  USB_OTG_WRITE_R32(&pusbdev->regs.GREGS->GINTSTS, gintsts.u32val);
  return 1;
}
/**
* @brief  USB_device_ReadDevInEP
*         Reads ept flags
* @param  pusbdev: device instance
* @retval status
*/
static uint32_t USB_device_ReadDevInEP (USB_OTG_CORE_HANDLE *pusbdev, uint8_t bepnum)
{
  uint32_t v, msk, emp;
  msk = USB_OTG_READ_R32(&pusbdev->regs.DREGS->DINEPTMASK);
  emp = USB_OTG_READ_R32(&pusbdev->regs.DREGS->DINEPTEMPMASK);
  msk |= ((emp >> bepnum) & 0x1) << 7;
  v = USB_OTG_READ_R32(&pusbdev->regs.INEP_REGS[bepnum]->DINEPTINT) & msk;
  return v;
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

