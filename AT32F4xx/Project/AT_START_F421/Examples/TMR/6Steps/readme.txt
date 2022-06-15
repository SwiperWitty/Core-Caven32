/**
  @page TMR_6Steps example
  
  @verbatim
  * File   : TMR/6Steps/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the TMR_6Steps example.
  ******************************************************************************
   @endverbatim

@par Example Description 
  This demo shows how to configure the TMR1 peripheral to generate 6 Steps.
  In this example, a software COM event is generated each 100 ms: using the SysTick 
  interrupt.
  The break Polarity is used at High level.

  The following Table describes the TMR1 Channels states:
              -----------------------------------------------
             | Step1 | Step2 | Step3 | Step4 | Step5 | Step6 |
   ----------------------------------------------------------
  |Channel1  |   1   |   0   |   0   |   0   |   0   |   1   |
   ----------------------------------------------------------
  |Channel1N |   0   |   0   |   1   |   1   |   0   |   0   |
   ----------------------------------------------------------
  |Channel2  |   0   |   0   |   0   |   1   |   1   |   0   |
   ----------------------------------------------------------
  |Channel2N |   1   |   1   |   0   |   0   |   0   |   0   |
   ----------------------------------------------------------
  |Channel3  |   0   |   1   |   1   |   0   |   0   |   0   |
   ----------------------------------------------------------
  |Channel3N |   0   |   0   |   0   |   0   |   1   |   1   |
   ----------------------------------------------------------
   
  Set-up 
    - Connect the TMR1 pins to an oscilloscope to monitor the different waveforms:
      - TMR1_CH3  pin (PA.10)
      - TMR1_CH1N pin (PB.13)  
      - TMR1_CH2  pin (PA.09)  
      - TMR1_CH3N pin (PB.15)
      - TMR1_CH1  pin (PA.08)  
      - TMR1_CH2N pin (PB.14) 
      
      PB12 connect to ground.
  
 */
