/**
  **************************************************************************
  * @file     at32f415_clock.c
  * @brief    system clock config program
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to 
  * download from Artery official website is the copyrighted work of Artery. 
  * Artery authorizes customers to use, copy, and distribute the BSP 
  * software and its related documentation for the purpose of design and 
  * development in conjunction with Artery microcontrollers. Use of the 
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */

/* includes ------------------------------------------------------------------*/
#include "at32f415_clock.h"


/**
  * @brief  system clock config program
  * @note   the system clock is configured as follow:
  *         system clock (sclk)   = hick / 12 * pll_mult
  *         system clock source   = HICK_VALUE
  *         - sclk                = 144000000
  *         - ahbdiv              = 1
  *         - ahbclk              = 144000000
  *         - apb1div             = 2
  *         - apb1clk             = 72000000
  *         - apb2div             = 2
  *         - apb2clk             = 72000000
  *         - pll_mult            = 36
  *         - flash_wtcyc         = 4 cycle
  * @param  none
  * @retval none
  */
void system_clock_hick_config(void)
{
  /* reset crm */
  crm_reset();

  /* config flash psr register */
  flash_psr_set(FLASH_WAIT_CYCLE_4);

  /* enable lick */
  crm_clock_source_enable(CRM_CLOCK_SOURCE_LICK, TRUE);

  /* wait till lick is ready */
  while(crm_flag_get(CRM_LICK_STABLE_FLAG) != SET)
  {
  }

  /* enable hick */
  crm_clock_source_enable(CRM_CLOCK_SOURCE_HICK, TRUE);

  /* wait till hick is ready */
  while(crm_flag_get(CRM_HICK_STABLE_FLAG) != SET)
  {
  }

  /* config pll clock resource */
  crm_pll_config(CRM_PLL_SOURCE_HICK, CRM_PLL_MULT_36);

  /* enable pll */
  crm_clock_source_enable(CRM_CLOCK_SOURCE_PLL, TRUE);

  /* wait till pll is ready */
  while(crm_flag_get(CRM_PLL_STABLE_FLAG) != SET)
  {
  }

  /* config ahbclk */
  crm_ahb_div_set(CRM_AHB_DIV_1);

  /* config apb2clk, the maximum frequency of APB2 clock is 75 MHz  */
  crm_apb2_div_set(CRM_APB2_DIV_2);

  /* config apb1clk, the maximum frequency of APB1 clock is 75 MHz  */
  crm_apb1_div_set(CRM_APB1_DIV_2);

  /* enable auto step mode */
  crm_auto_step_mode_enable(TRUE);

  /* select pll as system clock source */
  crm_sysclk_switch(CRM_SCLK_PLL);

  /* wait till pll is used as system clock source */
  while(crm_sysclk_switch_status_get() != CRM_SCLK_PLL)
  {
  }

  /* disable auto step mode */
  crm_auto_step_mode_enable(FALSE);

  /* update system_core_clock global variable */
  system_core_clock_update();

  /* config usbclk from pll */
  crm_usb_clock_source_select(CRM_USB_CLOCK_SOURCE_PLL);
  crm_usb_clock_div_set(CRM_USB_DIV_3);

}

/** 12Mhz
  * @brief  system clock config program
  * @note   the system clock is configured as follow:
  *         - system clock        = hext * pll_mult
  *         - system clock source = pll (hext)
  *         - hext                = 12000000    //12Mhz
  *         - sclk                = 144000000
  *         - ahbdiv              = 1
  *         - ahbclk              = 144000000
  *         - apb1div             = 2
  *         - apb1clk             = 72000000
  *         - apb2div             = 2
  *         - apb2clk             = 72000000
  *         - pll_mult            = 12
  *         - flash_wtcyc         = 4 cycle
  * @param  none
  * @retval none
  */
  
void system_clock_hext_config(void)
{
  /* config flash psr register */
  flash_psr_set(FLASH_WAIT_CYCLE_4);

 
  /* reset crm */
  crm_reset();

  /* enable hick */
  crm_clock_source_enable(CRM_CLOCK_SOURCE_HICK, TRUE);

   /* wait till hick is ready */
  while(crm_flag_get(CRM_HICK_STABLE_FLAG) != SET)
  {
  }

  /* config pll clock resource */
  crm_pll_config(CRM_PLL_SOURCE_HICK, CRM_PLL_MULT_36);

  /* enable pll */
  crm_clock_source_enable(CRM_CLOCK_SOURCE_PLL, TRUE);

  /* wait till pll is ready */
  while(crm_flag_get(CRM_PLL_STABLE_FLAG) != SET)
  {
  }

  /* config ahbclk */
  crm_ahb_div_set(CRM_AHB_DIV_1);

  /* config apb2clk */
  crm_apb2_div_set(CRM_APB2_DIV_2);

  /* config apb1clk */
  crm_apb1_div_set(CRM_APB1_DIV_2);

  /* enable auto step mode */
  crm_auto_step_mode_enable(TRUE);

  /* select pll as system clock source */
  crm_sysclk_switch(CRM_SCLK_PLL);

  /* wait till pll is used as system clock source */
  while(crm_sysclk_switch_status_get() != CRM_SCLK_PLL)
  {
  }

  /* disable auto step mode */
  crm_auto_step_mode_enable(FALSE);

  /* config usbclk from pll */
  crm_usb_clock_div_set(CRM_USB_DIV_3);
  crm_usb_clock_source_select(CRM_USB_CLOCK_SOURCE_PLL);

  /* update system_core_clock global variable */
  system_core_clock_update();
}

void system_clock_config(void)
{
#ifdef EXT_HSCLK
    system_clock_hext_config();     // EXT_HSCLK
#else 
    system_clock_hick_config();     // 默认内部高速时钟
#endif
}
