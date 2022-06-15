/**
  @page TMR_OCInactive example
  
  @verbatim
  * File   : TMR/OCInactive/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the TMR_OCInactive example.
  ******************************************************************************
   @endverbatim

@par Example Description 
  This demo shows how to configure the TMR peripheral in Output Compare Inactive 
  mode with the corresponding Interrupt requests for each channel.
  
  TMR2 Configuration: 
    TMR2CLK = SystemCoreClock / 4,
    The objective is to get TMR2 counter clock at 1 KHz:
     - Prescaler = (TIM2CLK / TMR2 counter clock) - 1
    And generate 4 signals with 4 different delays:
    TMR2_CH1 delay = CCR1_Val/TMR2 counter clock = 1000 ms
    TMR2_CH2 delay = CCR2_Val/TMR2 counter clock = 500 ms
    TMR2_CH3 delay = CCR3_Val/TMR2 counter clock = 250 ms
    TMR2_CH4 delay = CCR4_Val/TMR2 counter clock = 125 ms
    
  Set-up 
    - Connect the following pins to an oscilloscope to monitor the different 
      waveforms:
        - PC.06
        - PC.07
        - PC.08 
        - PC.09
 */
