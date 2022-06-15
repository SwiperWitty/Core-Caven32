/**
  @page TMR_Cascade_Synchro example
  
  @verbatim
  * File   : TMR/Cascade_Synchro/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the TMR_Cascade_Synchro example.
  ******************************************************************************
   @endverbatim

@par Example Description
  This demo shows how to synchronize TMR peripherals in cascade mode.
  In this example three timers are used:
  
  Timers synchronisation in cascade mode:
  1/TMR1 is configured as Master Timer:
  - PWM Mode is used
  - The TMR1 Update event is used as Trigger Output  

  2/TMR3 is slave for TMR1 and Master for TMR15,
  - PWM Mode is used
  - The ITR1(TMR1) is used as input trigger 
  - Gated mode is used, so start and stop of slave counter
    are controlled by the Master trigger output signal(TMR1 update event).
  - The TMR3 Update event is used as Trigger Output.  

  3/TMR15 is slave for TMR3,
  - PWM Mode is used
  - The ITR2(TMR3) is used as input trigger
  - Gated mode is used, so start and stop of slave counter
    are controlled by the Master trigger output signal(TMR3 update event).
  
  The Master Timer TMR1 is running at TMR1 frequency :
  TMR1 frequency = (TMR1 counter clock)/ (TMR1 period + 1) = 468.75 KHz 
  and the duty cycle = TMR2_CCR1/(TMR2_ARR + 1) = 25%.

  The TMR3 is running at:
  (TMR1 frequency)/ (TMR3 period + 1) = 117.19 KHz and a duty cycle equal 
  to TMR3_CCR1/(TMR3_ARR + 1) = 25%

  The TMR15 is running at:
  (TMR3 frequency)/ (TMR15 period + 1) = 29.29 KHz and a duty cycle equal 
  to TMR4_CCR1/(TMR4_ARR + 1) = 25%  

  Set-up 
    - Connect the following pins to an oscilloscope to monitor the different 
      waveforms:
        - TMR1 CH1  (PA.08) 
        - TMR3 CH1  (PA.06)
        - TMR15 CH1 (PA.02)
 */



