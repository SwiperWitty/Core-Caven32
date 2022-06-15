/**
  ******************************************************************************
  * File   : GPIO/JTAG_Remap/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the JTAG/SWD IOs as standard GPIOs.
  ******************************************************************************
  * @Description
  * 
  *   This example provides a short description of how to use the JTAG/SWD IOs
  * as standard GPIOs and gives a configuration sequence. 
  *   First, the SWJ-DP is enabled, five SWD-DP pins are not toggled. If the 
  * BUTTON_WAKEUP(PA0) is pressed the SWJ-DP will be disabled. The SWJ-DP pins are
  * configured as output push-pull. The PA.13(JTMS/SWDAT) and PA.14(JTCK/SWCLK)
  * pins are toggled in an infinite loop. 
  * 
  ******************************************************************************
  */ 
  
