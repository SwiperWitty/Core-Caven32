/**
  @page TMR_Parallel_Synchro example
  
  @verbatim
  * File   : TMR/Parallel_Synchro/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the TMR_Parallel_Synchro example.
  ******************************************************************************
   @endverbatim

@par Example Description
    This demo is based on the AT-START-F421 board,in this demo,PA6 ,PA0 and PB6 output pwm waveform.
     1/TMR15 is configured as Master TMRer:
     - PWM Mode is used
     - The TMR15 Update event is used as Trigger Output
     2/TMR3 and TMR1 are slaves for TMR15,
     - PWM Mode is used
     - The ITR1(TMR15) is used as input trigger for both slaves
     - Gated mode is used, so starts and stops of slaves counters
      are controlled by the Master trigger output signal(update event).

     The TMRxCLK is fixed to 120 MHz, the TMR15 counter clock is 120 MHz.
     The Master TMRer TMR15 is running at 500 KHz and the duty cycle
     is equal to 25%
     The TMR3 is running:
     - At (TMR15 frequency)/ (TMR3 period + 1) = 50 KHz and a duty cycle
     equal to TMR3_CCR1/(TMR3_ARR + 1) = 30%
     The TMR1 is running:
     - At (TMR15 frequency)/ (TMR1 period + 1) = 100 KHz and a duty cycle
     equal to TMR1_CCR1/(TMR1_ARR + 1) = 60%

 */



