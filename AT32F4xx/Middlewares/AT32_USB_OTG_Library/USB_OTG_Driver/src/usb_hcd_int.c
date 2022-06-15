/**
  ******************************************************************************
  * File   : usb_hcd_int.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Host driver interrupt subroutines
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "usb_core.h"
#include "usb_defines.h"
#include "usb_hcd_int.h"

/** @addtogroup USB_OTG_DRIVER
* @{
*/

/** @defgroup USB_HCD_INT 
* @brief This file contains the interrupt subroutines for the Host mode.
* @{
*/


/** @defgroup USB_HCD_INT_Private_Defines
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USB_HCD_INT_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 



/** @defgroup USB_HCD_INT_Private_Macros
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USB_HCD_INT_Private_Variables
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USB_HCD_INT_Private_FunctionPrototypes
* @{
*/ 
static uint32_t USB_host_handle_sof_intsts(USB_OTG_CORE_HANDLE *pusbdev);
static uint32_t USB_host_handle_port_intsts(USB_OTG_CORE_HANDLE *pusbdev);
static uint32_t USB_host_handle_hc_intsts (USB_OTG_CORE_HANDLE *pusbdev);
static uint32_t USB_host_handle_hc_n_In_intsts (USB_OTG_CORE_HANDLE *pusbdev ,
                                                 uint32_t num);
static uint32_t USB_host_handle_hc_n_Out_intsts (USB_OTG_CORE_HANDLE *pusbdev , 
                                                  uint32_t num);
static uint32_t USB_host_handle_rx_qlvl_intsts (USB_OTG_CORE_HANDLE *pusbdev);
static uint32_t USB_host_handle_nptxfempty_intsts (USB_OTG_CORE_HANDLE *pusbdev);
static uint32_t USB_host_handle_ptxfempty_intsts (USB_OTG_CORE_HANDLE *pusbdev);
static uint32_t USB_host_handle_Disconnect_intsts (USB_OTG_CORE_HANDLE *pusbdev);
static uint32_t USB_host_handle_IncompletePeriodicXfer_intsts (USB_OTG_CORE_HANDLE *pusbdev);

/**
* @}
*/ 


/** @defgroup USB_HCD_INT_Private_Functions
* @{
*/ 

/**
* @brief  HOST_Handle_ISR 
*         This function handles all USB Host Interrupts
* @param  pusbdev: Selected device
* @retval status 
*/

uint32_t USB_HOST_OTG_INTSTS_Handler (USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_OTG_GINTSTS_Type  gintsts;
  uint32_t retval = 0;
  
  gintsts.u32val = 0;
  
  /* Check if HOST Mode */
  if (USB_OTG_IsHostMode(pusbdev))
  {
    gintsts.u32val = USB_OTG_ReadCoreInterruptr(pusbdev);
    if (!gintsts.u32val)
    {
      return 0;
    }
    
    if (gintsts.b.sof)
    {
      retval |= USB_host_handle_sof_intsts (pusbdev);
    }
    
    if (gintsts.b.rxflvl)
    {
      retval |= USB_host_handle_rx_qlvl_intsts (pusbdev);
    }
    
    if (gintsts.b.nptxfemp)
    {
      retval |= USB_host_handle_nptxfempty_intsts (pusbdev);
    }
    
    if (gintsts.b.ptxfemp)
    {
      retval |= USB_host_handle_ptxfempty_intsts (pusbdev);
    }    
    
    if (gintsts.b.hchint)
    {
      retval |= USB_host_handle_hc_intsts (pusbdev);
    }
    
    if (gintsts.b.hportint)
    {
      retval |= USB_host_handle_port_intsts (pusbdev);
    }
    
    if (gintsts.b.disconint)
    {
      retval |= USB_host_handle_Disconnect_intsts (pusbdev);  
      
    }
    
    if (gintsts.b.incompisoout)
    {
      retval |= USB_host_handle_IncompletePeriodicXfer_intsts (pusbdev);
    }
    
    
  }
  return retval;
}

/**
* @brief  USB_host_handle_hc_intsts 
*         This function indicates that one or more host channels has a pending
* @param  pusbdev: Selected device
* @retval status 
*/
static uint32_t USB_host_handle_hc_intsts (USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_OTG_HAINT_Type        haint;
  USB_OTG_HCCHAR_Type       hcchar;
  uint32_t i = 0;
  uint32_t retval = 0;
  
  /* Clear appropriate bits in HCINTn to clear the interrupt bit in
  * GINTSTS */
  
  haint.u32val = USB_OTG_ReadHostAllChannels_intr(pusbdev);
  
  for (i = 0; i < pusbdev->cfg.host_channels ; i++)
  {
    if (haint.b.chaint & (1 << i))
    {
      hcchar.u32val = USB_OTG_READ_R32(&pusbdev->regs.HC_REGS[i]->HCCHAR);
      
      if (hcchar.b.eptdir)
      {
        retval |= USB_host_handle_hc_n_In_intsts (pusbdev, i);
      }
      else
      {
        retval |=  USB_host_handle_hc_n_Out_intsts (pusbdev, i);
      }
    }
  }
  
  return retval;
}

/**
* @brief  USB_OTG_otg_hcd_handle_sof_intr 
*         Handles the start-of-frame interrupt in host mode.
* @param  pusbdev: Selected device
* @retval status 
*/
static uint32_t USB_host_handle_sof_intsts (USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_OTG_GINTSTS_Type      gintsts;
  gintsts.u32val = 0;
  
  USB_HOST_HCD_INT_fops->SOF(pusbdev);
  
  /* Clear interrupt */
  gintsts.b.sof = 1;
  USB_OTG_WRITE_R32(&pusbdev->regs.GREGS->GINTSTS, gintsts.u32val);
  
  return 1;
}

/**
* @brief  USB_host_handle_Disconnect_intsts 
*         Handles bdisconnint event.
* @param  pusbdev: Selected device
* @retval status 
*/
static uint32_t USB_host_handle_Disconnect_intsts (USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_OTG_GINTSTS_Type      gintsts;
  
  gintsts.u32val = 0;
  
  USB_HOST_HCD_INT_fops->DevDisconnected(pusbdev);
  
  /* Clear interrupt */
  gintsts.b.disconint = 1;
  USB_OTG_WRITE_R32(&pusbdev->regs.GREGS->GINTSTS, gintsts.u32val);
  
  return 1;
}
/**
* @brief  USB_host_handle_nptxfempty_intsts 
*         Handles non periodic tx fifo empty.
* @param  pusbdev: Selected device
* @retval status 
*/
static uint32_t USB_host_handle_nptxfempty_intsts (USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_OTG_GINTMSK_Type      intmsk;
  USB_OTG_HNPTXSTS_Type     hnptxsts; 
  uint16_t                     len_words , len; 
  
  hnptxsts.u32val = USB_OTG_READ_R32(&pusbdev->regs.GREGS->HNPTXSTS);
  
  len_words = (pusbdev->host.hc[hnptxsts.b.bnptxqtop.chnum].xfer_len + 3) / 4;
  
  while ((hnptxsts.b.nptxfsavail > len_words)&&
         (pusbdev->host.hc[hnptxsts.b.bnptxqtop.chnum].xfer_len != 0))
  {
    
    len = hnptxsts.b.nptxfsavail * 4;
    
    if (len > pusbdev->host.hc[hnptxsts.b.bnptxqtop.chnum].xfer_len)
    {
      /* Last packet */
      len = pusbdev->host.hc[hnptxsts.b.bnptxqtop.chnum].xfer_len;
      
      intmsk.u32val = 0;
      intmsk.b.nptxfempm = 1;
      USB_OTG_MODIFY_R32( &pusbdev->regs.GREGS->GINTMASK, intmsk.u32val, 0);       
    }
    
    len_words = (pusbdev->host.hc[hnptxsts.b.bnptxqtop.chnum].xfer_len + 3) / 4;
    
    USB_OTG_WritePacket (pusbdev , pusbdev->host.hc[hnptxsts.b.bnptxqtop.chnum].xfer_buff, hnptxsts.b.bnptxqtop.chnum, len);
    
    pusbdev->host.hc[hnptxsts.b.bnptxqtop.chnum].xfer_buff  += len;
    pusbdev->host.hc[hnptxsts.b.bnptxqtop.chnum].xfer_len   -= len;
    pusbdev->host.hc[hnptxsts.b.bnptxqtop.chnum].xfer_count  += len; 
    
    hnptxsts.u32val = USB_OTG_READ_R32(&pusbdev->regs.GREGS->HNPTXSTS);
  }  
  
  return 1;
}
/**
* @brief  USB_host_handle_ptxfempty_intsts 
*         Handles periodic tx fifo empty
* @param  pusbdev: Selected device
* @retval status 
*/
static uint32_t USB_host_handle_ptxfempty_intsts (USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_OTG_GINTMSK_Type      intmsk;
  USB_OTG_HPTXSTS_Type      hptxsts; 
  uint16_t                     len_words , len; 
  
  hptxsts.u32val = USB_OTG_READ_R32(&pusbdev->regs.HREGS->HPTXSTS);
  
  len_words = (pusbdev->host.hc[hptxsts.b.ptxqtop.chnum].xfer_len + 3) / 4;
  
  while ((hptxsts.b.ptxfspcavl > len_words)&&
         (pusbdev->host.hc[hptxsts.b.ptxqtop.chnum].xfer_len != 0))    
  {
    
    len = hptxsts.b.ptxfspcavl * 4;
    
    if (len > pusbdev->host.hc[hptxsts.b.ptxqtop.chnum].xfer_len)
    {
      len = pusbdev->host.hc[hptxsts.b.ptxqtop.chnum].xfer_len;
      /* Last packet */
      intmsk.u32val = 0;
      intmsk.b.ptxfempm = 1;
      USB_OTG_MODIFY_R32( &pusbdev->regs.GREGS->GINTMASK, intmsk.u32val, 0); 
    }
    
    len_words = (pusbdev->host.hc[hptxsts.b.ptxqtop.chnum].xfer_len + 3) / 4;
    
    USB_OTG_WritePacket (pusbdev , pusbdev->host.hc[hptxsts.b.ptxqtop.chnum].xfer_buff, hptxsts.b.ptxqtop.chnum, len);
    
    pusbdev->host.hc[hptxsts.b.ptxqtop.chnum].xfer_buff  += len;
    pusbdev->host.hc[hptxsts.b.ptxqtop.chnum].xfer_len   -= len;
    pusbdev->host.hc[hptxsts.b.ptxqtop.chnum].xfer_count  += len; 
    
    hptxsts.u32val = USB_OTG_READ_R32(&pusbdev->regs.HREGS->HPTXSTS);
  }  
  
  return 1;
}

/**
* @brief  USB_host_handle_port_intsts 
*         This function determines which interrupt conditions have occurred
* @param  pusbdev: Selected device
* @retval status 
*/
static uint32_t USB_host_handle_port_intsts (USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_OTG_HPRT0_Type  hprt0;
  USB_OTG_HPRT0_Type  hprt0_dup;
  USB_OTG_HCFG_Type   hcfg;    
  uint32_t retval = 0;
  USB_OTG_GINTMSK_Type  intmsk;
  
  intmsk.u32val = 0;
  hcfg.u32val = 0;
  hprt0.u32val = 0;
  hprt0_dup.u32val = 0;
  
  hprt0.u32val = USB_OTG_READ_R32(pusbdev->regs.HPRT0);
  hprt0_dup.u32val = USB_OTG_READ_R32(pusbdev->regs.HPRT0);
  
  /* Clear the interrupt bits in GINTSTS */
  
  hprt0_dup.b.prtena = 0;
  hprt0_dup.b.prtcondet = 0;
  hprt0_dup.b.prtenchng = 0;
  hprt0_dup.b.prtovcchng = 0;
  
  /* Port Connect Detected */
  if (hprt0.b.prtcondet)
  {
    hprt0_dup.b.prtcondet = 1;
    USB_HOST_HCD_INT_fops->DevConnected(pusbdev);
    retval |= 1;   
  }
  
  /* Port Enable Changed */
  if (hprt0.b.prtenchng)
  {
    hprt0_dup.b.prtenchng = 1;
    
    if (hprt0.b.prtena == 1)
    {
      if ((hprt0.b.prtspd == HPRT0_PRTSPD_LOW_SPEED) ||
          (hprt0.b.prtspd == HPRT0_PRTSPD_FULL_SPEED))
      { 
        hcfg.u32val = USB_OTG_READ_R32(&pusbdev->regs.HREGS->HCFG);
        
        if (hprt0.b.prtspd == HPRT0_PRTSPD_LOW_SPEED)
        {
          USB_OTG_WRITE_R32(&pusbdev->regs.HREGS->HFIR, 6000 );
          if (hcfg.b.fslspclks != HCFG_6_MHZ)
          {
            if(pusbdev->cfg.phy_itface  == USB_OTG_EMBEDDED_PHY)
            {
              USB_OTG_InitFSLSPClkSel(pusbdev , HCFG_6_MHZ);
            }
            
            else
            {
              USB_OTG_WRITE_R32(&pusbdev->regs.HREGS->HFIR, 48000 );            
              if (hcfg.b.fslspclks != HCFG_48_MHZ)
              {
                USB_OTG_InitFSLSPClkSel(pusbdev ,HCFG_48_MHZ );
              }
            } 
          }
        }
      }
      
      USB_HOST_HCD_INT_fops->DevPortEnabled(pusbdev);  
      
      /*unmask disconintm interrupt */
      intmsk.u32val = 0;
      intmsk.b.disconintm = 1;  
      USB_OTG_MODIFY_R32(&pusbdev->regs.GREGS->GINTMASK, intmsk.u32val, intmsk.u32val);
    }
    else
    {
      USB_HOST_HCD_INT_fops->DevPortDisabled(pusbdev);
      
    }
  }
  
  /* Overcurrent Change Interrupt */
  if (hprt0.b.prtovcchng)
  {
    hprt0_dup.b.prtovcchng = 1;
    retval |= 1;
  }
  
  /* Clear Port Interrupts */
  USB_OTG_WRITE_R32(pusbdev->regs.HPRT0, hprt0_dup.u32val);
  
  return retval;
}
/**
* @brief  USB_host_handle_hc_n_Out_intsts 
*         Handles interrupt for a specific Host Channel
* @param  pusbdev: Selected device
* @param  hc_num: Channel number
* @retval status 
*/
uint32_t USB_host_handle_hc_n_Out_intsts (USB_OTG_CORE_HANDLE *pusbdev , uint32_t num)
{
  
  USB_OTG_HCINTn_Type     hcint;
  USB_OTG_HCINTMSK_Type  hcintmsk;
  USB_OTG_HOSTCH_REGS *hcreg;
  USB_OTG_HCCHAR_Type     hcchar; 
  
  hcreg = pusbdev->regs.HC_REGS[num];
  hcint.u32val = USB_OTG_READ_R32(&hcreg->HCINT);
  hcintmsk.u32val = USB_OTG_READ_R32(&hcreg->HCINTMASK);
  hcint.u32val = hcint.u32val & hcintmsk.u32val;
  
  hcchar.u32val = USB_OTG_READ_R32(&pusbdev->regs.HC_REGS[num]->HCCHAR);
  
  if (hcint.b.ahberr)
  {
    CLEAR_HCH_INT(hcreg ,ahberr);
    UNMASK_HOST_INT_CHH (num);
  } 
  else if (hcint.b.ack)
  {
    CLEAR_HCH_INT(hcreg , ack);
  }
  else if (hcint.b.frmovr)
  {
    UNMASK_HOST_INT_CHH (num);
    USB_OTG_HCH_Halt(pusbdev, num);
    CLEAR_HCH_INT(hcreg ,frmovr);
  }
  else if (hcint.b.xferc)
  {
    pusbdev->host.ErrCnt[num] = 0;
    UNMASK_HOST_INT_CHH (num);
    USB_OTG_HCH_Halt(pusbdev, num);
    CLEAR_HCH_INT(hcreg , xferc);
    pusbdev->host.HCH_Status[num] = HC_XFRC;            
  }
  
  else if (hcint.b.stall)
  {
    CLEAR_HCH_INT(hcreg , stall);
    UNMASK_HOST_INT_CHH (num);
    USB_OTG_HCH_Halt(pusbdev, num);
    pusbdev->host.HCH_Status[num] = HC_STALL;      
  }
  
  else if (hcint.b.nak)
  {
    pusbdev->host.ErrCnt[num] = 0;
    UNMASK_HOST_INT_CHH (num);
//    if (pusbdev->cfg.dma_enable == 0)
    {
      USB_OTG_HCH_Halt(pusbdev, num);
    }
    CLEAR_HCH_INT(hcreg , nak);
    pusbdev->host.HCH_Status[num] = HC_NAK;      
  }
  
  else if (hcint.b.xacterr)
  {
    UNMASK_HOST_INT_CHH (num);
    USB_OTG_HCH_Halt(pusbdev, num);
    pusbdev->host.HCH_Status[num] = HC_XACTERR;
    CLEAR_HCH_INT(hcreg , xacterr);
  }
  else if (hcint.b.nyet)
  {
    pusbdev->host.ErrCnt[num] = 0;
    UNMASK_HOST_INT_CHH (num);
//    if (pusbdev->cfg.dma_enable == 0)
    {
      USB_OTG_HCH_Halt(pusbdev, num);
    }
    CLEAR_HCH_INT(hcreg , nyet);
    pusbdev->host.HCH_Status[num] = HC_NYET;    
  }
  else if (hcint.b.dtgerr)
  {
    UNMASK_HOST_INT_CHH (num);
    USB_OTG_HCH_Halt(pusbdev, num);
    CLEAR_HCH_INT(hcreg , nak);   
    pusbdev->host.HCH_Status[num] = HC_DATATGLERR;
    
    CLEAR_HCH_INT(hcreg , dtgerr);
  }  
  else if (hcint.b.chhlt)
  {
    MASK_HOST_INT_CHH (num);
    
    if(pusbdev->host.HCH_Status[num] == HC_XFRC)
    {
      pusbdev->host.URB_State[num] = URB_DONE;  
      
      if (hcchar.b.eptype == EPT_TYPE_BULK)
      {
        pusbdev->host.hc[num].toggle_out ^= 1; 
      }
    }
    else if(pusbdev->host.HCH_Status[num] == HC_NAK)
    {
      pusbdev->host.URB_State[num] = URB_NOTREADY;      
    }    
    else if(pusbdev->host.HCH_Status[num] == HC_NYET)
    {
      if(pusbdev->host.hc[num].do_ping == 1)
      {
        USB_OTG_HCH_DoPing(pusbdev, num);
      }
      pusbdev->host.URB_State[num] = URB_NOTREADY;      
    }      
    else if(pusbdev->host.HCH_Status[num] == HC_STALL)
    {
      pusbdev->host.URB_State[num] = URB_STALL;      
    }  
    else if(pusbdev->host.HCH_Status[num] == HC_XACTERR)
    {
      {
        pusbdev->host.URB_State[num] = URB_ERROR;  
      }
    }
    CLEAR_HCH_INT(hcreg , chhlt);    
  }
  
  
  return 1;
}
/**
* @brief  USB_host_handle_hc_n_In_intsts 
*         Handles interrupt for a specific Host Channel
* @param  pusbdev: Selected device
* @param  hc_num: Channel number
* @retval status 
*/
uint32_t USB_host_handle_hc_n_In_intsts (USB_OTG_CORE_HANDLE *pusbdev , uint32_t num)
{
  USB_OTG_HCINTn_Type     hcint;
  USB_OTG_HCINTMSK_Type  hcintmsk;
  USB_OTG_HCCHAR_Type     hcchar; 
//  USB_OTG_HCTSIZn_Type  hctsiz;
  USB_OTG_HOSTCH_REGS *hcreg;
  
  hcreg = pusbdev->regs.HC_REGS[num];
  hcint.u32val = USB_OTG_READ_R32(&hcreg->HCINT);
  hcintmsk.u32val = USB_OTG_READ_R32(&hcreg->HCINTMASK);
  hcint.u32val = hcint.u32val & hcintmsk.u32val;
  hcchar.u32val = USB_OTG_READ_R32(&pusbdev->regs.HC_REGS[num]->HCCHAR);
  hcintmsk.u32val = 0;
  
  if (hcint.b.ahberr)
  {
    CLEAR_HCH_INT(hcreg ,ahberr);
    UNMASK_HOST_INT_CHH (num);
  }  
  else if (hcint.b.ack)
  {
    CLEAR_HCH_INT(hcreg ,ack);
  }
  
  else if (hcint.b.stall)  
  {
    UNMASK_HOST_INT_CHH (num);
    pusbdev->host.HCH_Status[num] = HC_STALL; 
    CLEAR_HCH_INT(hcreg , nak);   /* Clear the NAK Condition */
    CLEAR_HCH_INT(hcreg , stall); /* Clear the STALL Condition */
    hcint.b.nak = 0;           /* NOTE: When there is a 'stall', reset also nak, 
                                  else, the pusbdev->host.HCH_Status = HC_STALL
    will be overwritten by 'nak' in code below */
    USB_OTG_HCH_Halt(pusbdev, num);    
  }
  else if (hcint.b.dtgerr)
  {
    UNMASK_HOST_INT_CHH (num);
    USB_OTG_HCH_Halt(pusbdev, num);
    CLEAR_HCH_INT(hcreg , nak);   
    pusbdev->host.HCH_Status[num] = HC_DATATGLERR; 
    CLEAR_HCH_INT(hcreg , dtgerr);
  }    
  
  if (hcint.b.frmovr)
  {
    UNMASK_HOST_INT_CHH (num);
    USB_OTG_HCH_Halt(pusbdev, num);
    CLEAR_HCH_INT(hcreg ,frmovr);
  }
  
  else if (hcint.b.xferc)
  {
//    if (pusbdev->cfg.dma_enable == 1)
//    {
//      hctsiz.u32val = USB_OTG_READ_R32(&pusbdev->regs.HC_REGS[num]->HCTSIZE);
//      pusbdev->host.XferCnt[num] =  pusbdev->host.hc[num].xfer_len - hctsiz.b.xfersize;
//    }
    
    pusbdev->host.HCH_Status[num] = HC_XFRC;     
    pusbdev->host.ErrCnt [num]= 0;
    CLEAR_HCH_INT(hcreg , xferc);
    
    if ((hcchar.b.eptype == EPT_TYPE_CTRL)||
        (hcchar.b.eptype == EPT_TYPE_BULK))
    {
      UNMASK_HOST_INT_CHH (num);
      USB_OTG_HCH_Halt(pusbdev, num);
      CLEAR_HCH_INT(hcreg , nak); 
      pusbdev->host.hc[num].toggle_in ^= 1;
      
    }
    else if(hcchar.b.eptype == EPT_TYPE_INTR)
    {
      hcchar.b.oddfrm  = 1;
      USB_OTG_WRITE_R32(&pusbdev->regs.HC_REGS[num]->HCCHAR, hcchar.u32val); 
      pusbdev->host.URB_State[num] = URB_DONE;  
    } 
  }
  else if (hcint.b.chhlt)
  {
    MASK_HOST_INT_CHH (num);
    
    if(pusbdev->host.HCH_Status[num] == HC_XFRC)
    {
      pusbdev->host.URB_State[num] = URB_DONE;      
    }
    
    else if (pusbdev->host.HCH_Status[num] == HC_STALL) 
    {
      pusbdev->host.URB_State[num] = URB_STALL;
    }   
    
    else if((pusbdev->host.HCH_Status[num] == HC_XACTERR) ||
            (pusbdev->host.HCH_Status[num] == HC_DATATGLERR))
    {
      pusbdev->host.ErrCnt[num] = 0;
      pusbdev->host.URB_State[num] = URB_ERROR;  
      
    }
    else if(hcchar.b.eptype == EPT_TYPE_INTR)
    {
      pusbdev->host.hc[num].toggle_in ^= 1;
    }
    
    CLEAR_HCH_INT(hcreg , chhlt);    
    
  }    
  else if (hcint.b.xacterr)
  {
    UNMASK_HOST_INT_CHH (num);
    pusbdev->host.HCH_Status[num] = HC_XACTERR;
    USB_OTG_HCH_Halt(pusbdev, num);
    CLEAR_HCH_INT(hcreg , xacterr);    
  }
  else if (hcint.b.nak)  
  {  
    if(hcchar.b.eptype == EPT_TYPE_INTR)
    {
      UNMASK_HOST_INT_CHH (num);
//      if (pusbdev->cfg.dma_enable == 0)
      {
        USB_OTG_HCH_Halt(pusbdev, num);
      }
    }
    
    pusbdev->host.HCH_Status[num] = HC_NAK;
    CLEAR_HCH_INT(hcreg , nak);  
    
    if  ((hcchar.b.eptype == EPT_TYPE_CTRL)||
              (hcchar.b.eptype == EPT_TYPE_BULK))
    {
      /* re-activate the channel  */
      hcchar.b.chena = 1;
      hcchar.b.chdis = 0;
      USB_OTG_WRITE_R32(&pusbdev->regs.HC_REGS[num]->HCCHAR, hcchar.u32val); 
    }
  }
  
  
  return 1;
  
}

/**
* @brief  USB_host_handle_rx_qlvl_intsts 
*         Handles the Rx Status Queue Level Interrupt
* @param  pusbdev: Selected device
* @retval status 
*/
static uint32_t USB_host_handle_rx_qlvl_intsts (USB_OTG_CORE_HANDLE *pusbdev)
{
  USB_OTG_GRXFSTS_Type       grxsts;
  USB_OTG_GINTMSK_Type       intmsk;
  USB_OTG_HCTSIZn_Type       hctsiz; 
  USB_OTG_HCCHAR_Type        hcchar;
  __IO uint8_t                  channelnum =0;  
  uint32_t                      count;    
  
  /* Disable the Rx Status Queue Level interrupt */
  intmsk.u32val = 0;
  intmsk.b.rxflvlm = 1;
  USB_OTG_MODIFY_R32( &pusbdev->regs.GREGS->GINTMASK, intmsk.u32val, 0);
  
  grxsts.u32val = USB_OTG_READ_R32(&pusbdev->regs.GREGS->GRXSTSP);
  channelnum = grxsts.b.chnum;  
  hcchar.u32val = USB_OTG_READ_R32(&pusbdev->regs.HC_REGS[channelnum]->HCCHAR);
  
  switch (grxsts.b.pktsts)
  {
  case GRXSTS_PKTSTS_IN:
    /* Read the data into the host buffer. */
    if ((grxsts.b.bcnt > 0) && (pusbdev->host.hc[channelnum].xfer_buff != (void  *)0))
    {  
      
      USB_OTG_ReadPacket(pusbdev, pusbdev->host.hc[channelnum].xfer_buff, grxsts.b.bcnt);
      /*manage multiple Xfer */
      pusbdev->host.hc[grxsts.b.chnum].xfer_buff += grxsts.b.bcnt;           
      pusbdev->host.hc[grxsts.b.chnum].xfer_count  += grxsts.b.bcnt;
      
      
      count = pusbdev->host.hc[channelnum].xfer_count;
      pusbdev->host.XferCnt[channelnum]  = count;
      
      hctsiz.u32val = USB_OTG_READ_R32(&pusbdev->regs.HC_REGS[channelnum]->HCTSIZE);
      if(hctsiz.b.pktcnt > 0)
      {
        /* re-activate the channel when more packets are expected */
        hcchar.b.chena = 1;
        hcchar.b.chdis = 0;
        USB_OTG_WRITE_R32(&pusbdev->regs.HC_REGS[channelnum]->HCCHAR, hcchar.u32val);
      }
    }
    break;
    
  case GRXSTS_PKTSTS_IN_XFER_COMP:
    
  case GRXSTS_PKTSTS_DATA_TOGGLE_ERR:
  case GRXSTS_PKTSTS_CH_HALTED:
  default:
    break;
  }
  
  /* Enable the Rx Status Queue Level interrupt */
  intmsk.b.rxflvlm = 1;
  USB_OTG_MODIFY_R32(&pusbdev->regs.GREGS->GINTMASK, 0, intmsk.u32val);
  return 1;
}

/**
* @brief  USB_host_handle_IncompletePeriodicXfer_intsts 
*         Handles the incomplete Periodic transfer Interrupt
* @param  pusbdev: Selected device
* @retval status 
*/
static uint32_t USB_host_handle_IncompletePeriodicXfer_intsts (USB_OTG_CORE_HANDLE *pusbdev)
{
  
  USB_OTG_GINTSTS_Type       gintsts;
  USB_OTG_HCCHAR_Type        hcchar; 
  
  
  
  
  hcchar.u32val = USB_OTG_READ_R32(&pusbdev->regs.HC_REGS[0]->HCCHAR);
  hcchar.b.chena = 1;
  hcchar.b.chdis = 1;
  USB_OTG_WRITE_R32(&pusbdev->regs.HC_REGS[0]->HCCHAR, hcchar.u32val);  
  
  gintsts.u32val = 0;
  /* Clear interrupt */
  gintsts.b.incompisoout = 1;
  USB_OTG_WRITE_R32(&pusbdev->regs.GREGS->GINTSTS, gintsts.u32val);
  
  return 1;
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


