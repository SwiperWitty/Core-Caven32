
/**
  @page ERTC ERTC_StopWatch example
  @verbatim
  * File   : ERTC/ERTC_StopWatch/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the ERTC ERTC_StopWatch example.
  ******************************************************************************
   @endverbatim

@par Example Description 

This example shows how to use the ERTC sub-seconds and Tamper feature (Filter, sampling) 
features. It simulates a precise chronometer with 10 record times possibilities stored 
in the Backup registers:
 - 10 registers for time (second, minutes and hours)
 - 10 registers for subsecond
  
Printf and Push Buttons to allow user to use StopWatch with real time display.

After startup, a default chronometer counter format "00:00:00:000" is displayed 
by printf, it corresponds to [Hours]:[minutes]:[seconds]:[milliseconds].
User can manipulate the chronometer features using the TAMPER and WAKEUP buttons:
 - Press WAKEUP button to stop or start counter.
 - Press TAMPER button to save trials in the backup registers (max 10 actions).

 */
