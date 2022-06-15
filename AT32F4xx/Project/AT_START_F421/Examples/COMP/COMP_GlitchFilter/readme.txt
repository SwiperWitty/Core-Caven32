/**
  ******************************************************************************
  * File   : COMP/COMP_GlitchFilter/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of how to configure COMP's glitch filter.
  ******************************************************************************
  * @Description
  * 
  *   This example shows how to configure COMP's glitch filter, using TMR1 output(PA8)
  * PWM(freq = 468.7KHz, period = 2.1333us, duty = 50%), connect PA8 to COMP_Input(PA1).
  * And then observe the COMP_Output(PA0) through oscilloscope, it should have a 
  * similar frequency to PWM, but the duty cycle is only 25%. Because 64 PCLK(~0.5333us)
  * have been filtered out.
  *
  ******************************************************************************
  */ 
  
