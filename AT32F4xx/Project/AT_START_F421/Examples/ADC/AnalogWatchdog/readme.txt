/**
  @page ADC_AnalogWatchdog ADC analog watchdog example
  
  @verbatim
  * File   : ADC/AnalogWatchdog/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the ADC analog watchdog example.
  ******************************************************************************
   @endverbatim

@par Example Description

This example describes how to use the ADC analog watchdog to guard continuously  
an ADC channel.
The ADC1 is configured to convert continuously ADC channel14. The analog watchdog
is configured and enabled to guard a single regular channel.
Each time the channel14 converted value exceeds programmed analog watchdog high 
threshold (value 0x0B00) or goes down analog watchdog low threshold (value 0x0300)
an AWD interrupt is generated and the output pin connected to LED2, LED3 and LED4 
are toggled. The LEDs will bright as long as the AWD interrupt is generated which 
means that the converted value of regular ADC channel14 is outside the range 
limited by high and low analog watchdog thresholds.

 */
