/**
  @page TMR_OCActive example
  
  @verbatim
  * File   : TMR/OCActive/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the TMR_OCActive example.
  ******************************************************************************
   @endverbatim

@par Example Description
  This demo shows how to configure the TIM peripheral to generate four different 
  signals with four different delays.
  
  TMR3 Configuration: 
    TMR3CLK = SystemCoreClock / 4,
    The objective is to get TMR3 counter clock at 1 KHz:
     - Prescaler = (TIM3CLK / TMR3 counter clock) - 1
    And generate 4 signals with 4 different delays:
    TMR3_CH1 delay = CCR1_Val/TMR3 counter clock = 1000 ms
    TMR3_CH2 delay = CCR2_Val/TMR3 counter clock = 500 ms
    TMR3_CH3 delay = CCR3_Val/TMR3 counter clock = 250 ms
    TMR3_CH4 delay = CCR4_Val/TMR3 counter clock = 125 ms
    
  Set-up 
    - Connect the following pins to an oscilloscope to monitor the different 
      waveforms:
       - PC.06
       - PA.06 (TMR3_CH1)
       - PA.07 (TMR3_CH2)
       - PB.00 (TMR3_CH3)
       - PB.01 (TMR3_CH4)
 */