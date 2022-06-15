/**
  ******************************************************************************
  * File   : RCC/NewPLLConfig/PLLconfig.c 
  * Version: V1.x.x
  * Date   : 2020-xx-xx
  * Brief  : New PLL config program, Only for AT32F415 series
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "PLLconfig.h"

/** @addtogroup AT32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup RCC_NewPLLConfig
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
OscInitType OscInitStruct;
SysClkInitType ClockInitStruct;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Structure Initialization.
  * @param  None
  * @retval None
  */
static void StructInit(void)
{
  OscInitStruct.LSEState = RCC_LSE_DISABLE;
  OscInitStruct.HSIState = RCC_HSI_ENABLE;
  OscInitStruct.HSEState = RCC_HSE_DISABLE;
  OscInitStruct.PLLState = RCC_PLL_ENABLE;
  OscInitStruct.PLLConfigMode = NORMAL_MODE;
  OscInitStruct.PLLRC = RCC_CFG_PLLRC_HSI_DIV2;
  OscInitStruct.PLLSourceHSEPsc = RCC_CFG_PLLHSEPSC_HSE;
  OscInitStruct.PLLMUL = RCC_CFG_PLLMULT18;
  OscInitStruct.PLLNS = 31;
  OscInitStruct.PLLMS = 1;
  OscInitStruct.PLLFR = RCC_PLL_PLLFR_1;

  ClockInitStruct.SYSCLKSel = RCC_CFG_SYSCLKSEL_PLL;
  ClockInitStruct.Flatency = FLASH_ACR_LATENCY_2;
  ClockInitStruct.AHBClkDivider = RCC_CFG_AHBPSC_DIV1;
  ClockInitStruct.APB2ClkDivider = RCC_CFG_APB2PSC_DIV2;
  ClockInitStruct.APB1ClkDivider = RCC_CFG_APB1PSC_DIV2;
  ClockInitStruct.USBDivider = RCC_USBCLKSelection_PLL_Div1;
  ClockInitStruct.CLKOUTSel = RCC_CLKOUT_NOCLK;
  ClockInitStruct.CLKOUTPre = RCC_MCOPRE_1;
  ClockInitStruct.HSI48MToSYSCLK = HSI8M_FOR_SYSCLK_ENABLE;
  ClockInitStruct.HSI48MToUSB = HSI_FOR_USB_ENABLE;
}

/**
  * @brief  Auto clock calibration configuration program
  * @param  None
  * @retval None
  */
static void ACCEnable(void)
{
  uint32_t ACC_C2_value=0;
  /* Enable ACC clock */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_ACC, ENABLE);

  /*Update the C1\C2\C3 value*/
  ACC_C2_value = 7999;
  ACC_WriteC1(ACC_C2_value-10);
  ACC_WriteC2(ACC_C2_value);  
  ACC_WriteC3(ACC_C2_value+10);

  /* Enable TRIM calibration */
  ACC_EnterCALMode(ACC_TRIM_ON,ENABLE);
}

/**
  * @brief  Reset RCC configuration.
  * @param  None
  * @retval None
  */
static void ResetRCCConfig(void)
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

  /* Reset RCC PLL register */
  BIT_CLEAR(RCC->MISC, RCC_PLL_PLLFR | RCC_PLL_PLLMS | RCC_PLL_PLLNS | \
            RCC_PLL_PLLFREF | RCC_PLL_PLLCFGEN);

  /* Reset USB768B, CLKOUT[3], HSICAL_KEY[7:0] */
  BIT_CLEAR(RCC->MISC, 0x010100FF);

  /* Disable all interrupts and clear pending bits  */
  RCC->CLKINT = RCC_CLKINT_LSISTBLFC | RCC_CLKINT_LSESTBLFC | \
                RCC_CLKINT_HSISTBLFC | RCC_CLKINT_HSESTBLFC | \
                RCC_CLKINT_PLLSTBLFC | RCC_CLKINT_HSECFDFC;
}

/**
  * @brief  RCC OSC configuration.
  * @param  OscInitStruct: pointer of osc init struct
  * @retval status: Ok or Error
  */
static Status OscConfig(OscInitType *OscInitStruct)
{
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0, HSIStatus = 0;

  /* reset pll config at RCC_CFG register */
  RCC->CFG &= RCC_CFG_PLLCFG_MASK;

  if(RCC_LSE_ENABLE == OscInitStruct->LSEState)
  {
    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_PWR, ENABLE);
    PWR_BackupAccessCtrl(ENABLE);
    RCC_BackupResetCmd(ENABLE);
    RCC_BackupResetCmd(DISABLE);

    /* Enable LSE */
    RCC_LSEConfig(OscInitStruct->LSEState);

    PWR_BackupAccessCtrl(DISABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_PWR, DISABLE);
  }

  if(RCC_HSI_ENABLE == OscInitStruct->HSIState)
  {
    /* Enable HSI */
    RCC->CTRL |= ((uint32_t)RCC_CTRL_HSIEN);
    /* Wait till HSI is ready and if Time out is reached exit */
    do
    {
      HSIStatus = RCC->CTRL & RCC_CTRL_HSISTBL;
      StartUpCounter++;
    }
    while((HSIStatus == 0) && (StartUpCounter != 0xFFFF));
  }

  if(RCC_HSE_ENABLE == OscInitStruct->HSEState)
  {
    /* Enable HSE */
    RCC->CTRL |= ((uint32_t)RCC_CTRL_HSEEN);
    /* Wait till HSE is ready and if Time out is reached exit */
    do
    {
      HSEStatus = RCC->CTRL & RCC_CTRL_HSESTBL;
      StartUpCounter++;
    }
    while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));
  }

  if(RCC_PLL_ENABLE == OscInitStruct->PLLState)
  {
    /* config PLL clock resource */
    RCC->CFG |= (uint32_t)OscInitStruct->PLLRC;

    if(RCC_CFG_PLLRC_HSE == OscInitStruct->PLLRC)
    {
      /* config hse divider */
      RCC->CFG |= (uint32_t)(OscInitStruct->PLLSourceHSEPsc);
    }

    if(NORMAL_MODE == OscInitStruct->PLLConfigMode)
    {
      /* pll config with normal mode */
      RCC->CFG |= (uint32_t)(OscInitStruct->PLLMUL);

      /* Config PLL clock resource frequency */
      RCC_PLLFrefTableConfig(HSE_VALUE);

    }else if(FLEXIBLE_MODE == OscInitStruct->PLLConfigMode)
    {
      /* pll config with flexible mode */
      RCC_PLLconfig2(OscInitStruct->PLLNS, OscInitStruct->PLLMS, \
      OscInitStruct->PLLFR);
    }

    /* Enable PLL */
    RCC->CTRL |= RCC_CTRL_PLLEN;
    /* Wait till PLL is ready */
    while((RCC->CTRL & RCC_CTRL_PLLSTBL) == 0)
    {
    }
  }

  return Ok;
}

/**
  * @brief  RCC PLL configuration.
  * @param  SysClkInitStruct: pointer of system clock init struct
  * @retval status: Ok or Error
  */
static Status SysClockConfig(SysClkInitType *SysClkInitStruct)
{
  /* Enable Prefetch Buffer */
  FLASH->ACR |= (FLASH_ACR_PRFTBE);

  /* Flash wait state */
  FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
  FLASH->ACR |= (uint32_t)(SysClkInitStruct->Flatency);

  /* HCLK = SYSCLK */
  RCC->CFG |= (uint32_t)(SysClkInitStruct->AHBClkDivider);

  /* config PCLK2 */
  RCC->CFG &= 0xFFFFC7FF;
  RCC->CFG |= (uint32_t)(SysClkInitStruct->APB2ClkDivider);

  /* config PCLK1*/
  RCC->CFG &= 0xFFFFF8FF;
  RCC->CFG |= (uint32_t)(SysClkInitStruct->APB1ClkDivider);

  /* reset pll config at RCC_CFG register */
  RCC->CFG &= RCC_CFG_PLLCFG_MASK;

  if(RCC_CFG_SYSCLKSEL_HSI == SysClkInitStruct->SYSCLKSel)
  {
    if(HSI8M_FOR_SYSCLK_ENABLE == SysClkInitStruct->HSI48MToSYSCLK)
    {
      RCC->MISC2 &= ~RCC_MISC2_HSI_SYS_CTRL;
    }else if(HSI48M_FOR_SYSCLK_ENABLE == SysClkInitStruct->HSI48MToSYSCLK)
    {
      RCC->MISC  |= RCC_MISC_HSI_DIV_EN;
      RCC->MISC2 |= RCC_MISC2_HSI_SYS_CTRL;
    }

    /* Select HSI as system clock source */
    RCC->CFG &= (uint32_t)((uint32_t)~(RCC_CFG_SYSCLKSEL));
    RCC->CFG |= (uint32_t)RCC_CFG_SYSCLKSEL_HSI;

    /* Wait till HSE is used as system clock source */
    while ((RCC->CFG & (uint32_t)RCC_CFG_SYSCLKSTS) != RCC_CFG_SYSCLKSTS_HSI)
    {
    }

  } else if(RCC_CFG_SYSCLKSEL_HSE == SysClkInitStruct->SYSCLKSel)
  {
    /* Select HSE as system clock source */
    RCC->CFG &= (uint32_t)((uint32_t)~(RCC_CFG_SYSCLKSEL));
    RCC->CFG |= (uint32_t)RCC_CFG_SYSCLKSEL_HSE;

    /* Wait till HSE is used as system clock source */
    while ((RCC->CFG & (uint32_t)RCC_CFG_SYSCLKSTS) != RCC_CFG_SYSCLKSTS_HSE)
    {
    }
  } else if(RCC_CFG_SYSCLKSEL_PLL == SysClkInitStruct->SYSCLKSel)
  {
    /* enable step mode */
    RCC_StepModeCmd(ENABLE);

    /* Select PLL as system clock source */
    RCC->CFG &= (uint32_t)((uint32_t)~(RCC_CFG_SYSCLKSEL));
    RCC->CFG |= (uint32_t)RCC_CFG_SYSCLKSEL_PLL;

    /* Wait till PLL is used as system clock source */
    while ((RCC->CFG & (uint32_t)RCC_CFG_SYSCLKSTS) != RCC_CFG_SYSCLKSTS_PLL)
    {
    }

    /* disable step mode */
    RCC_StepModeCmd(DISABLE);
  }

  if(HSI_FOR_USB_DISABLE == SysClkInitStruct->HSI48MToUSB)
  {
    RCC->MISC2 &= ~RCC_MISC2_HSI_FOR_USB;

    RCC_USBCLKConfig(SysClkInitStruct->USBDivider);
  }else if(HSI_FOR_USB_ENABLE == SysClkInitStruct->HSI48MToUSB)
  {
    RCC->MISC  |= RCC_MISC_HSI_DIV_EN;
    RCC->MISC2 |= RCC_MISC2_HSI_FOR_USB;

    /* Enable ACC */
    ACCEnable();
  }

  return Ok;
}

/**
  * @brief  MCO configuration program
  * @param  SysClkInitStruct: pointer of system clock init struct
  * @retval None
  */
static void MCO_config(SysClkInitType *SysClkInitStruct)
{
  if(SysClkInitStruct->CLKOUTSel != RCC_CLKOUT_NOCLK)
  {
    GPIO_InitType GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA, ENABLE);

    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_8; 
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    RCC_CLKOUTConfig(SysClkInitStruct->CLKOUTSel, SysClkInitStruct->CLKOUTPre);
  }
}

/**
  * @brief  New PLL config program
  * @param  None
  * @retval None
  *
  * Note: Configuration result
  *   Clock Resource :      HSI
  *   SYSCLK :              72.000 MHz
  *   AHBCLK :              72.000 MHz
  *   APB2CLK :             36.000 MHz
  *   APB1CLK :             36.000 MHz
  *   CLKOUT :              NOCLK
  *   CLKOUT_Div :          /1
  *
  */
void NewPLLconfig(void)
{
  /* Structure init */
  StructInit();

  /* Reset RCC configuration */
  ResetRCCConfig();

  /* Config Osc */
  OscConfig(&OscInitStruct);

  /* Config sysclk */
  SysClockConfig(&ClockInitStruct);

  /* Config MCO */
  MCO_config(&ClockInitStruct);
}

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2020 ArteryTek *****END OF FILE****/ 
