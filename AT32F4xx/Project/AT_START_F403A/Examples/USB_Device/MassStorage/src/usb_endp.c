/**
  ******************************************************************************
  * File   : USB_Device/MassStorage/src/usb_endp.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Endpoint routines
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_bot.h"
#include "usb_istr.h"
/** @addtogroup AT32F403A_StdPeriph_Examples
  * @{
  */

/** @addtogroup USB_MassStorage
  * @{
  */ 
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  EP1 IN Callback Routine.
  * @param  None.
  * @retval None.
  */
void EP1_IN_Callback(void)
{
  Mass_Storage_In();
}

/**
  * @brief  EP2 OUT Callback Routine.
  * @param  None.
  * @retval None.
  */
void EP2_OUT_Callback(void)
{
  Mass_Storage_Out();
}
/**
  * @}
  */

/**
  * @}
  */
  

