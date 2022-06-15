/**
  ******************************************************************************
  * File   : USB_Device/VirtualComPort_loopback/src/usb_istr.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : ISTS events interrupt service routines
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_prop.h"
#include "usb_pwr.h"
#include "usb_istr.h"


/** @addtogroup AT32F403A_StdPeriph_Examples
  * @{
  */

/** @addtogroup USB_VirtualComPort_loopback
  * @{
  */ 
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile u16 wIstr;  /* ISTR register last read value */
volatile u8 bIntPackSOF = 0;  /* SOFs received between 2 consecutive packets */

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/** 
  * @brief  function pointers to non-control IN endpoints service routines 
  */

void (*pEpInt_IN[7])(void) =
  {
    EP1_IN_Callback, ///Endpoint 1 IN Callback
    EP2_IN_Callback, ///Endpoint 2 IN Callback
    EP3_IN_Callback, ///Endpoint 3 IN Callback
    EP4_IN_Callback, ///Endpoint 4 IN Callback
    EP5_IN_Callback, ///Endpoint 5 IN Callback
    EP6_IN_Callback, ///Endpoint 6 IN Callback
    EP7_IN_Callback, ///Endpoint 7 IN Callback
  };
/** 
  * @brief  function pointers to non-control OUT endpoints service routines 
  */
void (*pEpInt_OUT[7])(void) =
  {
    EP1_OUT_Callback, ///Endpoint 1 OUT Callback
    EP2_OUT_Callback, ///Endpoint 2 OUT Callback
    EP3_OUT_Callback, ///Endpoint 3 OUT Callback
    EP4_OUT_Callback, ///Endpoint 4 OUT Callback
    EP5_OUT_Callback, ///Endpoint 5 OUT Callback
    EP6_OUT_Callback, ///Endpoint 6 OUT Callback
    EP7_OUT_Callback, ///Endpoint 7 OUT Callback
  };



/**
  * @brief  STS events interrupt service routine
  * @param  None.            
  * @retval None.
  */
void USB_Istr(void)
{		  
  wIstr = _GetINTSTS();

#if (IMR_MSK & INTSTS_RSTF)
  if (wIstr & INTSTS_RSTF & wInterrupt_Mask)
  {
    _SetINTSTS((u16)CLR_RSTF);
    Device_Property.Reset();
#ifdef RESET_CALLBACK
    RESET_Callback();
#endif
  }
#endif
  /*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#if (IMR_MSK & INTSTS_DOVR)
  if (wIstr & INTSTS_DOVR & wInterrupt_Mask)
  {
    _SetINTSTS((u16)CLR_DOVR);
#ifdef DOVR_CALLBACK
    DOVR_Callback();
#endif
  }
#endif
  /*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#if (IMR_MSK & INTSTS_ERRF)
  if (wIstr & INTSTS_ERRF & wInterrupt_Mask)
  {
    _SetINTSTS((u16)CLR_ERRF);
#ifdef ERR_CALLBACK
    ERR_Callback();
#endif
  }
#endif
  /*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#if (IMR_MSK & INTSTS_WKUPF)
  if (wIstr & INTSTS_WKUPF & wInterrupt_Mask)
  {
    _SetINTSTS((u16)CLR_WKUPF);
    Resume(RESUME_EXTERNAL);
#ifdef WKUP_CALLBACK
    WKUP_Callback();
#endif
  }
#endif
  /*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#if (IMR_MSK & INTSTS_SUSPF)
  if (wIstr & INTSTS_SUSPF & wInterrupt_Mask)
  {

    /* check if SUSPEND is possible */
    if (fSuspendEnabled)
    {
      Suspend();
    }
    else
    {
      /* if not possible then resume after xx ms */
      Resume(RESUME_LATER);
    }
    /* clear of the INTSTS bit must be done after setting of CNTR_FSUSP */
    _SetINTSTS((u16)CLR_SUSPF);
#ifdef SUSP_CALLBACK
    SUSP_Callback();
#endif
  }
#endif
  /*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#if (IMR_MSK & INTSTS_SOFF)
  if (wIstr & INTSTS_SOFF & wInterrupt_Mask)
  {
    _SetINTSTS((u16)CLR_SOFF);
    bIntPackSOF++;

#ifdef SOF_CALLBACK
    SOF_Callback();
#endif
  }
#endif
  /*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#if (IMR_MSK & INTSTS_ESOF)
  if (wIstr & INTSTS_ESOF & wInterrupt_Mask)
  {
    _SetINTSTS((u16)CLR_ESOF);
    /* resume handling timing is made with ESOFs */
    Resume(RESUME_ESOF); /* request without change of the machine state */

#ifdef ESOF_CALLBACK
    ESOF_Callback();
#endif
  }
#endif
  /*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#if (IMR_MSK & INTSTS_CTFR)
  if (wIstr & INTSTS_CTFR & wInterrupt_Mask)
  {
    /* servicing of the endpoint correct transfer interrupt */
    /* clear of the CTR flag into the sub */
    CTR_LP();
#ifdef CTR_CALLBACK
    CTR_Callback();
#endif
  }
#endif
} /* USB_Istr */


/**
  * @}
  */

/**
  * @}
  */


