/**
  ******************************************************************************
  * File   : RCC/ClockFailureDetect/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "at32f4xx.h"
#include "at32_board.h"

/** @addtogroup AT32F415_StdPeriph_Examples
  * @{
  */

/** @addtogroup RCC_ClockFailureDetect
  * @{
  */ 


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Sets System clock frequency to 144MHz from HSI and configure HCLK, PCLK2
  *         and PCLK1 prescalers.
  * @note   This function should be used only after reset.
  * @param  None
  * @retval None
  */
static void SetSysClockTo144MHSI(void)
{
  __IO uint32_t StartUpCounter = 0, HSIStatus = 0;

  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/
  /* Enable HSI */
  RCC->CTRL |= ((uint32_t)RCC_CTRL_HSIEN);

  /* Wait till HSI is ready and if Time out is reached exit */
  do
  {
    HSIStatus = RCC->CTRL & RCC_CTRL_HSISTBL;
    StartUpCounter++;
  }
  while((HSIStatus == 0) && (StartUpCounter != 0xFFFF));

  if ((RCC->CTRL & RCC_CTRL_HSISTBL) != RESET)
  {
    HSIStatus = (uint32_t)0x01;
  }
  else
  {
    HSIStatus = (uint32_t)0x00;
  }

  if (HSIStatus == (uint32_t)0x01)
  {
    /* Enable Prefetch Buffer */
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    /* Flash 1 wait state */
    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_4;    

    /* HCLK = SYSCLK */
    RCC->CFG |= (uint32_t)RCC_CFG_AHBPSC_DIV1;

    /* PCLK2 = HCLK/2 */
    RCC->CFG &= 0xFFFFC7FF;
    RCC->CFG |= (uint32_t)RCC_CFG_APB2PSC_DIV2;

    /* PCLK1 = HCLK/2 */
    RCC->CFG &= 0xFFFFF8FF;
    RCC->CFG |= (uint32_t)RCC_CFG_APB1PSC_DIV2;

    /*  PLL configuration: PLLCLK = (HSI/2) * 36 = 144 MHz */
    RCC->CFG &= RCC_CFG_PLLCFG_MASK;

    RCC->CFG |= (uint32_t)(RCC_CFG_PLLRC_HSI_DIV2 | RCC_CFG_PLLMULT36);

    RCC_PLLFrefTableConfig(HSE_VALUE);

    /* Enable PLL */
    RCC->CTRL |= RCC_CTRL_PLLEN;

    /* Wait till PLL is ready */
    while((RCC->CTRL & RCC_CTRL_PLLSTBL) == 0)
    {
    }

    RCC_StepModeCmd(ENABLE);

    /* Select PLL as system clock source */
    RCC->CFG &= (uint32_t)((uint32_t)~(RCC_CFG_SYSCLKSEL));
    RCC->CFG |= (uint32_t)RCC_CFG_SYSCLKSEL_PLL;

    /* Wait till PLL is used as system clock source */
    while ((RCC->CFG & (uint32_t)RCC_CFG_SYSCLKSTS) != RCC_CFG_SYSCLKSTS_PLL)
    {
    }

    RCC_StepModeCmd(DISABLE);
  }
}

/**
  * @brief  Clock Failure Detect interrupt handler
  * @param  None
  * @retval None
  */
void CFD_Handler(void)
{
  if(RCC_GetINTStatus(RCC_INT_HSECFD) == SET)
  {
    /* Clear HSECFD Int flag */
    RCC_ClearINTPendingBit(RCC_INT_HSECFD);
    /* Disable clock failure detect*/
    RCC_HSEClockFailureDetectorCmd(DISABLE);
    /* Config sysclk to 144m from hsi */
    SetSysClockTo144MHSI();
    /* Config uart, and print result */
    UART_Print_Init(115200);
    printf("--- CFD has occured ---\r\n");
  }
}

/**
  * @brief  TMR1 break IRQ
  * @param  None
  * @retval None
  */
void TMR1_BRK_TMR9_IRQHandler(void)
{
  if(TMR_GetINTStatus(TMR1,TMR_INT_Break) == SET)
  {
    printf("--- TMR1 break has occured ---\r\n");
    TMR_ClearITPendingBit(TMR1,TMR_INT_Break);
  }
}

/**
  * @brief  TMR1 break init.
  * @param  None
  * @retval None
  */
void TMR_Brk_Init(void)
{
  GPIO_InitType  GPIO_InitStructure;
  TMR_TimerBaseInitType  TMR_TimeBaseStructure;
  TMR_BRKDTInitType  TMR_BRKDTInitStructure;
  NVIC_InitType NVIC_InitStructure;
  
  /* Open timer's&GPIOA/GPIOB clock */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_TMR1|RCC_APB2PERIPH_GPIOB,ENABLE);
  
  /* GPIOB.12 configuration for timer1's break Pin */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  /* Enable the TMR1 break Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TMR1_BRK_TMR9_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Time Base configuration */
  TMR_TimeBaseStructInit(&TMR_TimeBaseStructure);
  TMR_TimeBaseStructure.TMR_DIV = 0;
  TMR_TimeBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;
  TMR_TimeBaseStructure.TMR_Period = 0xffff;
  TMR_TimeBaseStructure.TMR_ClockDivision = 0;
  TMR_TimeBaseStructure.TMR_RepetitionCounter = 0;

  TMR_TimeBaseInit(TMR1, &TMR_TimeBaseStructure);
  
  /* Timer1 break Configuration */
  TMR_BRKDTInitStructure.TMR_AutomaticOutput = TMR_AutomaticOutput_Enable;
  TMR_BRKDTInitStructure.TMR_Break = TMR_Break_Enable;
  TMR_BRKDTInitStructure.TMR_BreakPolarity  = TMR_BreakPolarity_Low;
  TMR_BRKDTInitStructure.TMR_DeadTime = 0x0;
  TMR_BRKDTInitStructure.TMR_LOCKgrade = TMR_LOCKgrade_3;
  TMR_BRKDTInitStructure.TMR_OSIMIState = TMR_OSIMIState_Disable;
  TMR_BRKDTInitStructure.TMR_OSIMRState = TMR_OSIMRState_Disable;
  
  TMR_BRKDTConfig(TMR1,&TMR_BRKDTInitStructure);
  
  /* Enabe Timer's break interrupt */
  TMR_INTConfig(TMR1,TMR_INT_Break,ENABLE);

  /* TMR1 counter enable */
  TMR_Cmd(TMR1, ENABLE);
}
/**
  * @brief  Clock Failure Detect configuration program
  * @param  None
  * @retval None
  */
void ClockFailureDetectConfig(void)
{
  RCC_HSEClockFailureDetectorCmd(ENABLE);
}

/**
  * @brief  MCO configuration program
  * @param  None
  * @retval None
  */
void MCO_config(void)
{
  GPIO_InitType GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA,ENABLE); 
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_8; 
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  RCC_CLKOUTConfig(RCC_CLKOUT_PLL_Div4, RCC_MCOPRE_1);
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  AT32_Board_Init();
  
  UART_Print_Init(115200);

  MCO_config();
  
  TMR_Brk_Init();

  ClockFailureDetectConfig();

  for(;;)
  {
    AT32_LEDn_Toggle(LED4);
    Delay_ms(200);
  }
}

/**
  * @}
  */

/**
  * @}
  */

