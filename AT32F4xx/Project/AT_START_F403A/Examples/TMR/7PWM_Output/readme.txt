/**
  @page TMR_7PWM_OUTPUT example
  
  @verbatim
  * File   : TMR/7PWM_OUTPUT/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the TMR_7PWM_OUTPUT example.
  ******************************************************************************
   @endverbatim

@par Example Description 
  This demo shows how to configure the TMR1 peripheral to generate 7 PWM signals 
  with 4 different duty cycles (50%, 37.5%, 25% and 12.5%).
  
  The objective is to generate 7 PWM signal at 17.57 KHz:
  - TIM1_Period = (SystemCoreClock / 17570) - 1
  The channel 1 and channel 1N duty cycle is set to 50%
  The channel 2 and channel 2N duty cycle is set to 37.5%
  The channel 3 and channel 3N duty cycle is set to 25%
  The channel 4 duty cycle is set to 12.5%
  The Timer pulse is calculated as follows:
    - ChannelxPulse = DutyCycle * (TIM1_Period - 1) / 100
  The TIM1 waveform can be displayed using an oscilloscope.

  Set-up 
    - Connect the TMR1 pins to an oscilloscope to monitor the different waveforms:
      - TMR1_CH1  pin (PA.08)  
      - TMR1_CH1N pin (PB.13)  
      - TMR1_CH2  pin (PA.09)  
      - TMR1_CH2N pin (PB.14)  
      - TMR1_CH3  pin (PA.10)  
      - TMR1_CH3N pin (PB.15)
      - TMR1_CH4  pin (PA.11)  
 */
