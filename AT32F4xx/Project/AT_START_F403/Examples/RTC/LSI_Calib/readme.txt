
/**
  @page RTC_LSI_Calib RTC LSI_Calib example
  
  @verbatim
  * File   : RTC/LSI_Calib/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the RTC LSI_Calib example.
  ******************************************************************************
   @endverbatim

@par Example Description 

This example demonstrates and explains how to use the LSI clock source auto
calibration to get a precise RTC clock. 
As an application example, it demonstrates how to configure the TIM5 timer
internally connected to LSI clock output, in order to adjust the RTC prescaler. 

The Low Speed External (LSI) clock is used as RTC clock source.
After reset, the RTC prescaler is set with the default value (40000). 
The inaccuracy of the LSI clock causes the RTC Second signal to be inaccurate. This
signal is output on the Tamper pin (PC.13) and can be measured by on oscilloscope
or a frequencymeter.
    
 */
