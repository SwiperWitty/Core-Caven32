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

/** @addtogroup AT32F421_StdPeriph_Examples
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
void I2C2_TX_IRQHandler(void)
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
void I2C2_RX_IRQHandler(void)
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
void I2C1_TX_IRQHandler(void)
{
  uint32_t timeout = 0x00ffffff;

  if (DMA_GetFlagStatus(DMA1_FLAG_TC2) != RESET)
  {
    if (DMA_GetITStatus(DMA1_INT_TC2) != RESET)
    {
      /* Disable the transfer complete interrupt */
      DMA_INTConfig(DMA1_Channel2, DMA_INT_TC, DISABLE);
      
      /* Clear the transfer complete flag */
      DMA_ClearFlag(DMA1_FLAG_TC2);
      
      DMA_ClearITPendingBit(DMA1_INT_TC2);

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
void I2C1_RX_IRQHandler(void)
{
  if (DMA_GetFlagStatus(DMA1_FLAG_TC3) != RESET)
  {
    if (DMA_GetITStatus(DMA1_INT_TC3) != RESET)
    {
      /* Disable the transfer complete interrupt */
      DMA_INTConfig(DMA1_Channel3, DMA_INT_TC, DISABLE);
      
      /* Clear the transfer complete flag */
      DMA_ClearFlag(DMA1_FLAG_TC3);
      
      DMA_ClearITPendingBit(DMA1_INT_TC3);

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
void DMA1_Channel3_2_IRQHandler(void)
{
  I2C1_TX_IRQHandler();
  I2C1_RX_IRQHandler();
}

/**
  * @brief  This function handles I2C2 Event interrupt request.
  * @param  None
  * @retval None
  */
void DMA1_Channel7_4_IRQHandler(void)
{
  I2C2_TX_IRQHandler();
  I2C2_RX_IRQHandler();
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

