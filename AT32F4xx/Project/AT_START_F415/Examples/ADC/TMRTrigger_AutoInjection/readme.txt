/**
  @page ADC_TMRTrigger_AutoInjection ADC TMR trigger and auto-injection example
  
  @verbatim
  * File   : ADC/TMRTrigger_AutoInjection/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the ADC TMR trigger and auto-injection example.
  ******************************************************************************
   @endverbatim

@par Example Description

This example describes how to convert ADC regular group channels continuously using
TMR1 external trigger and injected group channels using the auto-injected feature.

ADC1 is configured to convert on each TMR1 capture compare event ADC channel14.
Each time an end of regular conversion occurs the DMA transfers the converted data 
from ADC1 DR register to the ADC_RegularConvertedValueTab table. 
Enabling the auto-injected feature, allows to convert automatically the injected 
channel ADC channel11 after the end of the regular channel14. An ADC interrupt is 
generated then by JEOC flag at the end of the injected channel conversion and in 
the interrupt routine the result is stored in the ADC_InjectedConvertedValueTab table.
The procedure is repeated 32 times then the TMR1 peripheral is disabled and thus, 
no conversion will be done neither regular nor injected.
TMR1 start conversion triggers can be visualized on oscilloscope on PA.08 and at the
same time the toggle of pin PC.06 which indicates the automatic auto-injection conversion.
The ADCs clock is set to 18 MHz.

 */
 