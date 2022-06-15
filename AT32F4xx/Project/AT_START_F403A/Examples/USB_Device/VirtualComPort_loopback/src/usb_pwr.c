/**
  ******************************************************************************
  * File   : USB_Device/VirtualComPort_loopback/src/usb_pwr.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Connection/disconnection & power management
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <at32f4xx.h>   
#include "usb_lib.h"
#include "usb_conf.h"
#include "usb_pwr.h"
#include "hw_config.h"

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
vu32 bDeviceState = UNCONNECTED; /* USB device status */
volatile bool fSuspendEnabled = TRUE;  /* true when suspend is possible */
/** 
  * @brief resume state and ESOF counter
  */
struct
{
  volatile RESUME_STATE eState;
  volatile u8 bESOFcnt;
}
ResumeS;

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Power on usb moudle.
  * @param  None
  * @retval USB_SUCCESS.
  */
RESULT PowerOn(void)
{
  u16 wRegVal;					   
  /*** CNTR_PWDN = 0 ***/
  wRegVal = CTRL_FRST;
  _SetCTRL(wRegVal);	 
  /*** CNTR_FRES = 0 ***/
  wInterrupt_Mask = 0;
  _SetCTRL(wInterrupt_Mask);
  /*** Clear pending interrupts ***/
  _SetINTSTS(0);
  /*** Set interrupt mask ***/
  wInterrupt_Mask = CTRL_RST_IEN | CTRL_SUSP_IEN | CTRL_WKUP_IEN;
  _SetCTRL(wInterrupt_Mask);	 
  return USB_SUCCESS;
}

/**
  * @brief  Power off usb moudle.
  * @param  None
  * @retval USB_SUCCESS.
  */
RESULT PowerOff()
{
  /* disable all ints and force USB reset */
  _SetCTRL(CTRL_FRST);
  /* clear interrupt status register */
  _SetINTSTS(0);				 
  /* Disable the Pull-Up*/
  //USB_Cable_Config(DISABLE);//Ω˚÷π1.5K…œ¿≠
  /* switch-off device */
  _SetCTRL(CTRL_FRST + CTRL_PDWN);
  /* sw variables reset */
  /* ... */

  return USB_SUCCESS;
}

/**
  * @brief  Set USB enter in suspend mode.
  * @param  None
  * @retval USB_SUCCESS.
  */
void Suspend(void)
{
  u16 wCNTR;
  /* suspend preparation */
  /* ... */

  /* macrocell enters suspend mode */
  wCNTR = _GetCTRL();
  wCNTR |= CTRL_FSUSP;
  _SetCTRL(wCNTR);

  /* ------------------ ONLY WITH BUS-POWERED DEVICES ---------------------- */
  /* power reduction */
  /* ... on connected devices */


  /* force low-power mode in the macrocell */
  wCNTR = _GetCTRL();
  wCNTR |= CTRL_LPWR;
  _SetCTRL(wCNTR);

  /* switch-off the clocks */
  /* ... */
  Enter_LowPowerMode();

}

/**
  * @brief  Handles wake-up restoring normal operations
  * @param  None
  * @retval USB_SUCCESS.
  */
void Resume_Init(void)
{
  u16 wCNTR;

  /* ------------------ ONLY WITH BUS-POWERED DEVICES ---------------------- */
  /* restart the clocks */
  /* ...  */

  /* CNTR_LPMODE = 0 */
  wCNTR = _GetCTRL();
  wCNTR &= (~CTRL_LPWR);
  _SetCTRL(wCNTR);

  /* restore full power */
  /* ... on connected devices */
  Leave_LowPowerMode();

  /* reset FSUSP bit */
  _SetCTRL(IMR_MSK);

  /* reverse suspend preparation */
  /* ... */

}

/**
  * @brief  This is the state machine handling resume operations and
  *         timing sequence. The control is based on the Resume structure
  *         variables and on the ESOF interrupt calling this subroutine
  *         without changing machine state.
  * @param  eResumeSetVal: a state machine value,
  *                        RESUME_ESOF doesn't change ResumeS.eState allowing
  *                        decrementing of the ESOF counter in different states.
  * @retval None.
  */
void Resume(RESUME_STATE eResumeSetVal)
{
  u16 wCNTR;

  if((eResumeSetVal != RESUME_ESOF) && (ResumeS.eState != RESUME_ON))
    ResumeS.eState = eResumeSetVal;

  switch (ResumeS.eState)
  {
    case RESUME_EXTERNAL:
      Resume_Init();
      ResumeS.eState = RESUME_OFF;
      break;
    case RESUME_INTERNAL:
      Resume_Init();
      ResumeS.eState = RESUME_START;
      break;
    case RESUME_LATER:
      ResumeS.bESOFcnt = 2;
      ResumeS.eState = RESUME_WAIT;
      break;
    case RESUME_WAIT:
      ResumeS.bESOFcnt--;
      if (ResumeS.bESOFcnt == 0)
        ResumeS.eState = RESUME_START;
      break;
    case RESUME_START:
      wCNTR = _GetCTRL();
      wCNTR |= CTRL_RESUME;
      _SetCTRL(wCNTR);
      ResumeS.eState = RESUME_ON;
      ResumeS.bESOFcnt = 10;
      break;
    case RESUME_ON:
      ResumeS.bESOFcnt--;
      if (ResumeS.bESOFcnt == 0)
      {
        wCNTR = _GetCTRL();
        wCNTR &= (~CTRL_RESUME);
        _SetCTRL(wCNTR);
        ResumeS.eState = RESUME_OFF;
      }
      break;
    case RESUME_OFF:
    case RESUME_ESOF:
    default:
      ResumeS.eState = RESUME_OFF;
      break;
  }
}

/**
  * @}
  */

/**
  * @}
  */


