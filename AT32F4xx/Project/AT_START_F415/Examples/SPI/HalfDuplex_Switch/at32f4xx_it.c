/**
  ******************************************************************************
  * File   : SPI/HalfDuplex_Switch/at32f4xx_it.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "at32f4xx_it.h"

/** @addtogroup AT32F415_StdPeriph_Examples
  * @{
  */

/** @addtogroup SPI_HalfDuplex_Switch
  * @{
  */ 
  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#define SPI_MASTER                   SPI1
#define SPI_MASTER_CLK               RCC_APB2PERIPH_SPI1
#define SPI_MASTER_GPIO              GPIOA
#define SPI_MASTER_GPIO_CLK          RCC_APB2PERIPH_GPIOA  
#define SPI_MASTER_PIN_SCK           GPIO_Pins_5
#define SPI_MASTER_PIN_MISO          GPIO_Pins_6
#define SPI_MASTER_PIN_MOSI          GPIO_Pins_7
#define SPI_MASTER_IRQn              SPI1_IRQn

#define SPI_SLAVE                    SPI2
#define SPI_SLAVE_CLK                RCC_APB1PERIPH_SPI2
#define SPI_SLAVE_GPIO               GPIOB
#define SPI_SLAVE_GPIO_CLK           RCC_APB2PERIPH_GPIOB 
#define SPI_SLAVE_PIN_SCK            GPIO_Pins_13
#define SPI_SLAVE_PIN_MISO           GPIO_Pins_14
#define SPI_SLAVE_PIN_MOSI           GPIO_Pins_15 
#define SPI_SLAVE_IRQn               SPI2_IRQn

#define BufferSize 32

extern uint8_t SPI_MASTER_Buffer_Tx[BufferSize];
extern uint8_t SPI_SLAVE_Buffer_Rx[BufferSize];
extern uint8_t SPI_SLAVE_Buffer_Tx[BufferSize];
extern uint8_t SPI_MASTER_Buffer_Rx[BufferSize];
extern __IO uint8_t TxIdx , RxIdx ;

void TMRingDelay_Decrement(void);
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
//  TMRingDelay_Decrement();
}

/**
  * @brief  This function handles SPI1 or SPI3 global interrupt request.
  * @param  None
  * @retval None
  */

 void SPI1_IRQHandler(void)
{
  if (SPI_I2S_GetITStatus(SPI_MASTER, SPI_I2S_INT_TE) != RESET)
  {
    if(SPI_I2S_GetFlagStatus(SPI_MASTER, SPI_I2S_FLAG_TE)!=RESET)
    {
      /* Send SPI_MASTER data */
      SPI_I2S_TxData(SPI_MASTER, SPI_MASTER_Buffer_Tx[TxIdx++]);

      /* Disable SPI_MASTER TXE interrupt */
      if (TxIdx == BufferSize)
      {
        SPI_I2S_INTConfig(SPI_MASTER, SPI_I2S_INT_TE, DISABLE);
      }
    }
  }
  /* Store SPI_SLAVE received data */  
  if (SPI_I2S_GetITStatus(SPI_MASTER, SPI_I2S_INT_RNE) != RESET)
  {    
    if(SPI_I2S_GetFlagStatus(SPI_MASTER, SPI_I2S_FLAG_RNE)!=RESET)
    {
      SPI_Enable(SPI_MASTER, DISABLE);      
      SPI_MASTER_Buffer_Rx[RxIdx++] = SPI_I2S_RxData(SPI_MASTER);  
      SPI_Enable(SPI_MASTER, ENABLE);       
      if(RxIdx == BufferSize)
      {
        SPI_I2S_INTConfig(SPI_MASTER, SPI_I2S_INT_RNE, DISABLE);
      }
    }  
  }  
}

/**
  * @brief  This function handles SPI2 global interrupt request.
  * @param  None
  * @retval None
  */
void SPI2_IRQHandler(void)
{
  if (SPI_I2S_GetITStatus(SPI_SLAVE, SPI_I2S_INT_TE) != RESET)
  {
    if(SPI_I2S_GetFlagStatus(SPI_SLAVE, SPI_I2S_FLAG_TE)!=RESET)
    {    
      /* Send SPI_MASTER data */
      SPI_I2S_TxData(SPI_SLAVE, SPI_SLAVE_Buffer_Tx[TxIdx++]);

      /* Disable SPI_MASTER TXE interrupt */
      if (TxIdx == BufferSize)
      {
        SPI_I2S_INTConfig(SPI_SLAVE, SPI_I2S_INT_TE, DISABLE);
      }
    }
  }  
  /* Store SPI_SLAVE received data */
  if (SPI_I2S_GetITStatus(SPI_SLAVE, SPI_I2S_INT_RNE) != RESET)
  {    
    if(SPI_I2S_GetFlagStatus(SPI_SLAVE, SPI_I2S_FLAG_RNE)!=RESET)
    {    
      SPI_SLAVE_Buffer_Rx[RxIdx++] = SPI_I2S_RxData(SPI_SLAVE);
    }
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
  
