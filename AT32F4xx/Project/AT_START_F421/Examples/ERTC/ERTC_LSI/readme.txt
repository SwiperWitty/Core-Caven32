
/**
  @page ERTC ERTC_LSI example
  
  @verbatim
  * File   : ERTC/ERTC_LSI/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the ERTC ERTC_LSI example.
  ******************************************************************************
   @endverbatim

@par Example Description 

This example shows how to use the LSI clock source auto calibration to get 
a precise ERTC clock. 
As an application example, it demonstrates how to configure the TMR5 timer
internally connected to LSI clock output, in order to adjust the ERTC prescaler. 
 
The ERTC WakeUp is configured to generate an interrupt each 1s. The WakeUp counter
is clocked by the ERTC CK_SPRE signal (1Hz) and its counter is set to 0.
LED2 is toggled inside the ERTC WakeUp interrupt each 1s.

The inaccuracy of the LSI clock causes the ERTC WakeUp Interrupt to be inaccurate.
The ERTC CK_SPRE signal can be monitored by LED2 which is toggled into the ERTC 
Wakeup interrupt service routine.

 */
