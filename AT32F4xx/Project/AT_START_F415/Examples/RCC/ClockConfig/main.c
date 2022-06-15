/**
  ******************************************************************************
  * File   : RCC/ClockConfig/main.c 
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

/** @addtogroup RCC_ClockConfig
  * @{
  */ 

  
/* Private typedef -----------------------------------------------------------*/
typedef struct ClockConfigStruct
{
  uint32_t Sysclk;
  uint32_t APB1_Div;
  uint32_t APB2_Div;
  uint32_t Mult;
  uint32_t PllRange;
}ClkConfStruct;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
enum PLLClkSrc{
  PLL_HSI = 0,
  PLL_HSE,
  PLL_end
};

ClkConfStruct ClockTable[] =
{
  {24 , RCC_CFG_APB1PSC_DIV1, RCC_CFG_APB2PSC_DIV1, RCC_CFG_PLLMULT6 , RCC_CFG_PLLRANGE_LE72MHZ},
  {36 , RCC_CFG_APB1PSC_DIV1, RCC_CFG_APB2PSC_DIV1, RCC_CFG_PLLMULT9 , RCC_CFG_PLLRANGE_LE72MHZ},
  {48 , RCC_CFG_APB1PSC_DIV1, RCC_CFG_APB2PSC_DIV1, RCC_CFG_PLLMULT12, RCC_CFG_PLLRANGE_LE72MHZ},
  {56 , RCC_CFG_APB1PSC_DIV1, RCC_CFG_APB2PSC_DIV1, RCC_CFG_PLLMULT14, RCC_CFG_PLLRANGE_LE72MHZ},
  {72 , RCC_CFG_APB1PSC_DIV1, RCC_CFG_APB2PSC_DIV1, RCC_CFG_PLLMULT18, RCC_CFG_PLLRANGE_LE72MHZ},
  {96 , RCC_CFG_APB1PSC_DIV2, RCC_CFG_APB2PSC_DIV2, RCC_CFG_PLLMULT24, RCC_CFG_PLLRANGE_GT72MHZ},
  {108, RCC_CFG_APB1PSC_DIV2, RCC_CFG_APB2PSC_DIV2, RCC_CFG_PLLMULT27, RCC_CFG_PLLRANGE_GT72MHZ},
  {120, RCC_CFG_APB1PSC_DIV2, RCC_CFG_APB2PSC_DIV2, RCC_CFG_PLLMULT30, RCC_CFG_PLLRANGE_GT72MHZ},
  {144, RCC_CFG_APB1PSC_DIV2, RCC_CFG_APB2PSC_DIV2, RCC_CFG_PLLMULT36, RCC_CFG_PLLRANGE_GT72MHZ},
};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Search the destination frequency program
  * @param  DestClkFreq: 
  * @retval success return index, error return (-1)
  */
int32_t SearchIndex(uint32_t DestClkFreq)
{
  int32_t index = 0;
  for(index = 0; index < sizeof(ClockTable)/sizeof(ClkConfStruct); index ++)
  {
    if(ClockTable[index].Sysclk == DestClkFreq)
    {
      return index;
    }
  }
  return -1;
}

/**
  * @brief  Clock reset program
  * @param  None
  * @retval None
  */
void ClockReset(void)
{
  /* Reset the RCC clock configuration to the default reset state(for debug purpose) */
  /* Set HSIEN bit */
  BIT_SET(RCC->CTRL, RCC_CTRL_HSIEN);

  /* Reset SW, AHBPSC, APB1PSC, APB2PSC, ADCPSC and CLKOUT bits */
  BIT_CLEAR(RCC->CFG, RCC_CFG_SYSCLKSEL | RCC_CFG_AHBPSC | \
            RCC_CFG_APB1PSC | RCC_CFG_APB2PSC | \
            RCC_CFG_ADCPSC | RCC_CFG_CLKOUT);

  /* Reset HSEEN, HSECFDEN and PLLEN bits */
  BIT_CLEAR(RCC->CTRL, RCC_CTRL_HSEEN | RCC_CTRL_HSECFDEN | \
            RCC_CTRL_PLLEN);

  /* Reset HSEBYPS bit */
  BIT_CLEAR(RCC->CTRL, RCC_CTRL_HSEBYPS);

  /* Reset PLLRC, PLLHSEPSC, PLLMUL, USBPSC and PLLRANGE bits */
  BIT_CLEAR(RCC->CFG, RCC_CFG_PLLRC | RCC_CFG_PLLHSEPSC | \
            RCC_CFG_PLLMULT | RCC_CFG_USBPSC | RCC_CFG_PLLRANGE);

  /* Reset USB768B, CLKOUT[3], HSICAL_KEY[7:0] */
  BIT_CLEAR(RCC->MISC, 0x010100FF);

  /* Disable all interrupts and clear pending bits  */
  RCC->CLKINT = RCC_CLKINT_LSISTBLFC | RCC_CLKINT_LSESTBLFC | \
                RCC_CLKINT_HSISTBLFC | RCC_CLKINT_HSESTBLFC | \
                RCC_CLKINT_PLLSTBLFC | RCC_CLKINT_HSECFDFC;
}

/**
  * @brief  Sysclk configuration program
  * @param  Src : PLL source clock (HSE or HSI).
  *         DestClkFreq: Destination clock frequency
  * @retval ErrorStatus: ERROR or SUCCESS
  */
ErrorStatus SysclkConfig(enum PLLClkSrc Src, uint32_t DestClkFreq)
{
  __IO uint32_t StartUpCounter = 0, Stabled = 0;
  __IO int32_t index = 0;

  /* Check the DestClkFreq*/
  index = SearchIndex(DestClkFreq);
  if(index == -1)
  {
    return ERROR;
  }

  /* Reset all RCC configuration */
  ClockReset();

  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/
  if(Src == PLL_HSE)
  {
    /* Enable HSE */
    RCC->CTRL |= ((uint32_t)RCC_CTRL_HSEEN);
    /* Wait till HSE is ready and if Time out is reached exit */
    do
    {
      Stabled = RCC->CTRL & RCC_CTRL_HSESTBL;
      StartUpCounter++;
    }
    while((Stabled == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

    if ((RCC->CTRL & RCC_CTRL_HSESTBL) != RESET)
    {
      Stabled = (uint32_t)0x01;
    }
  }
  else if(Src == PLL_HSI)
  {
    /* Enable HSI */
    RCC->CTRL |= ((uint32_t)RCC_CTRL_HSIEN);
    /* Wait till HSI is ready and if Time out is reached exit */
    do
    {
      Stabled = RCC->CTRL & RCC_CTRL_HSISTBL;
      StartUpCounter++;
    }
    while((Stabled == 0) && (StartUpCounter != 0xFFFF));
    if ((RCC->CTRL & RCC_CTRL_HSISTBL) != RESET)
    {
      Stabled = (uint32_t)0x01;
    }
  }

  if (Stabled == (uint32_t)0x01)
  {
    /* HCLK = SYSCLK */
    RCC->CFG |= (uint32_t)RCC_CFG_AHBPSC_DIV1;

    /* PCLK2 = HCLK/2 */
    RCC->CFG &= 0xFFFFC7FF;
    RCC->CFG |= (uint32_t)ClockTable[index].APB2_Div;

    /* PCLK1 = HCLK/2 */
    RCC->CFG &= 0xFFFFF8FF;
    RCC->CFG |= (uint32_t)ClockTable[index].APB1_Div;

    /*  PLL configuration */
    RCC->CFG &= RCC_CFG_PLLCFG_MASK;

    if(Src == PLL_HSE)
    {
      RCC->CFG |= (uint32_t)(RCC_CFG_PLLRC_HSE | RCC_CFG_PLLHSEPSC_HSE_DIV2 | ClockTable[index].Mult);
    }else if(Src == PLL_HSI)
    {
      RCC->CFG |= (uint32_t)(RCC_CFG_PLLRC_HSI_DIV2 | ClockTable[index].Mult);
    }

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

  /* Update system clock */
  SystemCoreClockUpdate();

  return SUCCESS;
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
 
  RCC_CLKOUTConfig(RCC_CLKOUT_SYSCLK, RCC_MCOPRE_4);
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  uint32_t index = 0;
  enum PLLClkSrc ClkSrc = PLL_HSI;

  while(1)
  {
    for(ClkSrc = PLL_HSI; ClkSrc < PLL_end; ClkSrc ++)
    {
      for(index = 0; index < sizeof(ClockTable)/sizeof(ClkConfStruct); index ++)
      {
        SysclkConfig(ClkSrc, ClockTable[index].Sysclk);
        MCO_config();
        AT32_Board_Init();
        UART_Print_Init(115200);
        printf("switch sysclk to %dMhz \r\n", ClockTable[index].Sysclk);
        Delay_ms(5000);
      }
    }
  }
}


/**
  * @}
  */

/**
  * @}
  */

