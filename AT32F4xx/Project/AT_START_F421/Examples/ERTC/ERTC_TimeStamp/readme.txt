
/**
  @page ERTC ERTC_TimeStamp example
  
  @verbatim
  * File   : ERTC/ERTC_TimeStamp/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the ERTC ERTC_TimeStamp example.
  ******************************************************************************
   @endverbatim

@par Example Description 

This example shows how to use the ERTC peripheral and the Time Stamp feature.

The program behaves as following:

1)At startup the program checks the backup data register 0 value:
  - BKP_DT0 value not correct: (ERTC_BKP_DT0 value is not correct or has not yet
    been programmed when the program is executed for the first time) the ERTC is
    configured,the time  is set to 08:10:00 and the date is set to 01-01-19
       
  - BKP_DT0 value correct: this means that the ERTC is configured and the time
    date and timestamp (time and date) are displayed.

2)When an External Reset occurs the BKP domain is not reset and the ERTC configuration
  is not lost.

3)When power on reset occurs:
  - If a battery is connected to the VBAT pin : 
    the BKP domain is not reset and the ERTC configuration is not lost.
  - If no battery is connected to the VBAT pin : 
    the BKP domain is reset and the ERTC configuration is lost.

4)It configures the ERTC TimeStamp pin to be falling edge and enables the
  TimeStamp detection.

5)Applying a low level on the TimeStamp pin (PC.13) by input low level,the calendar 
  is saved in the time-stamp registers thanks to the timestamp event detection.
   
The example uses printf to display the current time and timestamp registers contents:
  - Input low level in PC13, saves the current time and date in ERTC TSTM
    and TSDT registers and  displays the current ERTC Calendar (Time and date) and
    ERTC TimeStamp Calendar (Time and date). 
  - Pressing WakeUp push-button, clears the TimeStamp Calendar.
    
 */
