/**
  ******************************************************************************
  * File   : I2C/I2Cx_DMA_INT/at32f4xx_it.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx_it.h"
#include "i2c.h"

/** @addtogroup AT32F403A_StdPeriph_Examples
  * @{
  */

/** @addtogroup I2Cx_DMA_INT
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern uint8_t recv_1byte_flag;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{

}

/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  * @Note   This function is redefined in "main.h" and related to DMA Channel
  *         used for I2C data transmission
  */
void DMA1_Channel4_IRQHandler(void)
{
  uint32_t timeout = 0x00ffffff;

  if (DMA_GetFlagStatus(DMA1_FLAG_TC4) != RESET)
  {
    if (DMA_GetITStatus(DMA1_INT_TC4) != RESET)
    {
      /* Disable the transfer complete interrupt */
      DMA_INTConfig(DMA1_Channel4, DMA_INT_TC, DISABLE);
      
      /* Clear the transfer complete flag */
      DMA_ClearFlag(DMA1_FLAG_TC4);
      
      DMA_ClearITPendingBit(DMA1_INT_TC4);

      /* Disable DMA Request */
      BIT_CLEAR(I2C2->CTRL2, I2C_CTRL2_DMAEN);

      /* Wait until BTF flag is reset */
      while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BTFF) != SET)
      {
        if(--timeout == 0)
        {
          break;
        }
      }

      /* Generate Stop */
      I2C_GenerateSTOP(I2C2, ENABLE);
    }
  }
}

/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  * @Note   This function is redefined in "main.h" and related to DMA Channel
  *         used for I2C data reception
  */
void DMA1_Channel5_IRQHandler(void)
{
  if (DMA_GetFlagStatus(DMA1_FLAG_TC5) != RESET)
  {
    if (DMA_GetITStatus(DMA1_INT_TC5) != RESET)
    {
      /* Disable the transfer complete interrupt */
      DMA_INTConfig(DMA1_Channel5, DMA_INT_TC, DISABLE);
      
      /* Clear the transfer complete flag */
      DMA_ClearFlag(DMA1_FLAG_TC5);
      
      DMA_ClearITPendingBit(DMA1_INT_TC5);

      /* Disable Last DMA */
      BIT_CLEAR(I2C2->CTRL2, I2C_CTRL2_DMALAST);

      /* Disable DMA Request */
      BIT_CLEAR(I2C2->CTRL2, I2C_CTRL2_DMAEN);

      if(!recv_1byte_flag)
      {
        /* Disable Acknowledge */
        BIT_CLEAR(I2C2->CTRL1, I2C_CTRL1_ACKEN);

        /* Generate Stop */
        BIT_SET(I2C2->CTRL1, I2C_CTRL1_STOPGEN);
      }
    }
  }
}

/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  * @Note   This function is redefined in "main.h" and related to DMA Channel
  *         used for I2C data transmission
  */
void DMA1_Channel6_IRQHandler(void)
{
  uint32_t timeout = 0x00ffffff;

  if (DMA_GetFlagStatus(DMA1_FLAG_TC6) != RESET)
  {
    if (DMA_GetITStatus(DMA1_INT_TC6) != RESET)
    {
      /* Disable the transfer complete interrupt */
      DMA_INTConfig(DMA1_Channel6, DMA_INT_TC, DISABLE);
      
      /* Clear the transfer complete flag */
      DMA_ClearFlag(DMA1_FLAG_TC6);
      
      DMA_ClearITPendingBit(DMA1_INT_TC6);

      /* Disable DMA Request */
      BIT_CLEAR(I2C1->CTRL2, I2C_CTRL2_DMAEN);

      /* Wait until BTF flag is reset */
      while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BTFF) != SET)
      {
        if(--timeout == 0)
        {
          break;
        }
      }

      /* Generate Stop */
      I2C_GenerateSTOP(I2C1, ENABLE);
    }
  }
}

/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  * @Note   This function is redefined in "main.h" and related to DMA Channel
  *         used for I2C data reception
  */
void DMA1_Channel7_IRQHandler(void)
{
  if (DMA_GetFlagStatus(DMA1_FLAG_TC7) != RESET)
  {
    if (DMA_GetITStatus(DMA1_INT_TC7) != RESET)
    {
      /* Disable the transfer complete interrupt */
      DMA_INTConfig(DMA1_Channel7, DMA_INT_TC, DISABLE);
      
      /* Clear the transfer complete flag */
      DMA_ClearFlag(DMA1_FLAG_TC7);
      
      DMA_ClearITPendingBit(DMA1_INT_TC7);

      /* Disable Last DMA */
      BIT_CLEAR(I2C1->CTRL2, I2C_CTRL2_DMALAST);

      /* Disable DMA Request */
      BIT_CLEAR(I2C1->CTRL2, I2C_CTRL2_DMAEN);

      if(!recv_1byte_flag)
      {
        /* Disable Acknowledge */
        BIT_CLEAR(I2C1->CTRL1, I2C_CTRL1_ACKEN);

        /* Generate Stop */
        BIT_SET(I2C1->CTRL1, I2C_CTRL1_STOPGEN);
      }
    }
  }
}

/**
  * @brief  This function handles I2C1 Event interrupt request.
  * @param  None
  * @retval None
  */
//void I2C1_EV_IRQHandler(void)
//{
//  I2C1_Evt_handle();
//}

/**
  * @brief  This function handles I2C1 Error interrupt request.
  * @param  None
  * @retval None
  */
//void I2C1_ER_IRQHandler(void)
//{
//  I2C1_Err_handle();
//}

/**
  * @}
  */

/**
  * @}
  */

