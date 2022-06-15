/**
  @page ADC_3ADCs_DMA 3 ADCs DMA example
  
  @verbatim
  * File   : ADC/3ADCs_DMA/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the 3ADCs DMA example.
  ******************************************************************************
   @endverbatim

@par Example Description 

This example describes how to use the 3 ADCs in independent conversions. 
Two of them; ADC1 and ADC3 are transferring continuously converted data using DMA
while ADC2 converted data are stored using End of conversion interrupt.

ADC1 is configured to convert continuously ADC channel14. Each time an end of
conversion occurs the DMA1 transfers, in circular mode, the converted data from 
ADC1 DR register to the ADC1_ConvertedValue variable.

ADC2 is configured to convert continuously ADC channel13. Each time an end of
conversion occurs an end of conversion interrupt is generated and inside the
interrupt routine the converted data are read from ADC2 DR register and stored
into the ADC2_ConvertedValue variable.

ADC3 is configured to convert continuously ADC channel12. Each time an end of
conversion occurs the DMA2 transfers, in circular mode, the converted data from
ADC3 DR register to the ADC3_ConvertedValue variable.

The ADCs clocks are set to 24 MHz.

The result of ADC1, ADC2 and ADC3 conversion is monitored through the three
variables: ADC1ConvertedValue, ADC2ConvertedValue and ADC3ConvertedValue.

 */
