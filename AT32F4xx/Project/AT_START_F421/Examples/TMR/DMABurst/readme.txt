/**
  @page TMR_DMABurst example
  
  @verbatim
  * File   : TMR/TMR_DMABurst/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the TMR_DMABurst example.
  ******************************************************************************
   @endverbatim

@par Example Description
  This demo shows how to update the TMR1 channel1 period and the duty cycle 
  using the TMR1 DMA burst feature.
  
  Every update DMA request, the DMA will do 3 transfers of half words into Timer 
  registers beginning from ARR register.
  On the DMA update request, 0x0FFF will be transferred into ARR, 0x0000 
  will be transferred into RCR, 0x0555 will be transferred into CCR1. 

  TMR1 Configuration: generate 1 PWM signal using the DMA burst mode:
  The TMR1CLK frequency is set to SystemCoreClock (Hz), to get TMR1 counter
  clock at 24 MHz the Prescaler is computed as following:
  - Prescaler = (TIM1CLK / TMR1 counter clock) - 1
  SystemCoreClock is set to 120 MHz .
  The TMR1 period is 5.86 KHz: TMR1 Frequency = TMR1 counter clock/(ARR + 1)
                                               = 24 MHz / 4096 = 5.86KHz KHz
  TMR1 Channel1 duty cycle = (TIM1_CCR1/ TMR1_ARR)* 100 = 33.33%
    
  Set-up 
    - Connect the following pins to an oscilloscope to monitor the different 
      waveforms:  
        - TIM1 CH1  (PA.08) 
 */
