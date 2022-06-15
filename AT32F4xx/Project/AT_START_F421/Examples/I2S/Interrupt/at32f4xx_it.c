/**
  **************************************************************************
  * File   : at32f4xx_it.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : at32f4xx interrupt service routines.
  **************************************************************************
  */ 


/* Includes ------------------------------------------------------------------*/
#include "at32f4xx_it.h"

/** @addtogroup AT32F421_StdPeriph_Examples
  * @{
  */

/** @addtogroup I2S_Interrupt
  * @{
  */

extern const uint16_t I2S1_Buffer_Tx[32];
extern uint16_t I2S2_Buffer_Rx[32];
extern  __IO uint32_t TxIdx, RxIdx;


void TMRingDelay_Decrement(void);

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}


void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
//  TMRingDelay_Decrement();
}

/**
  * @brief  This function handles SPI1 global interrupt request.
  * @param  None
  * @retval None
  */
void SPI1_IRQHandler(void)
{
  /* Check the interrupt source */
  if (SPI_I2S_GetITStatus(SPI1, SPI_I2S_INT_TE) == SET)
  {
    /* Send a data from I2S1 */
    SPI_I2S_TxData(SPI1, I2S1_Buffer_Tx[TxIdx++]);
  }

  /* Check the end of buffer transfer */
  if (RxIdx == 32)
  {
    /* Disable the I2S1 TXE interrupt to end the communication */
    SPI_I2S_INTConfig(SPI1, SPI_I2S_INT_TE, DISABLE);
  }
}

/**
  * @brief  This function handles SPI2 global interrupt request.
  * @param  None
  * @retval None
  */
void SPI2_IRQHandler(void)
{
  /* Check the interrupt source */
  if (SPI_I2S_GetITStatus(SPI2, SPI_I2S_INT_RNE) == SET)
  {
    /* Store the I2S2 received data in the relative data table */
    I2S2_Buffer_Rx[RxIdx++] = SPI_I2S_RxData(SPI2);
  }
}

/******************************************************************************/
/*                 AT32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_at32f413_xx.s).                                            */
/******************************************************************************/
/**
  * @}
  */ 

/**
  * @}
  */
  
