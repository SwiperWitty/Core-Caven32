/**
  ******************************************************************************
  * File   : USB_Device/USB_Audio_Demo/src/usb_istr.c
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

/** @addtogroup AT32F413_StdPeriph_Examples
  * @{
  */

/** @addtogroup USB_Audio_Demo
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
/* function pointers to non-control endpoints service routines */
void (*pEpInt_IN[7])(void) =
  {
    EP1_IN_Callback,
    EP2_IN_Callback,
    EP3_IN_Callback,
    EP4_IN_Callback,
    EP5_IN_Callback,
    EP6_IN_Callback,
    EP7_IN_Callback,
  };

void (*pEpInt_OUT[7])(void) =
  {
    EP1_OUT_Callback,
    EP2_OUT_Callback,
    EP3_OUT_Callback,
    EP4_OUT_Callback,
    EP5_OUT_Callback,
    EP6_OUT_Callback,
    EP7_OUT_Callback,
  };


/*******************************************************************************
* Function Name  : USB_Istr
* Description    : STR events interrupt service routine
* Input          :
* Output         :
* Return         :
*******************************************************************************/
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

/*******************************************************************************
* Function Name  : USB_Istr
* Description    : Start of frame callback function.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void SOF_Callback(void)
{
   
}

/**
  * @}
  */

/**
  * @}
  */

