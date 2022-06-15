
/**
  @page ERTC ERTC_Timer example
  
  @verbatim
  * File   : ERTC/ERTC_Timer/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the ERTC ERTC_Timer example.
  ******************************************************************************
   @endverbatim

@par Example Description 

This example shows how to use the ERTC peripheral with Alarm sub seconds 
feature to simulate a timer with refresh time equal to 250 ms  (1 second/ 8) * 2).
The ERTC is configured to generate sub seconds interrupt each 125ms (will have
8 interrupt per 1 second).

After 480 interrupts (60 * 8) the progress bar will be full.

User can manipulate the chronometer features using the TAMPER and WAKEUP buttons:
  - Press KEY button to Start\Stop the timer.
  - Press TAMPER button to Reset the timer.
  
  
 */
