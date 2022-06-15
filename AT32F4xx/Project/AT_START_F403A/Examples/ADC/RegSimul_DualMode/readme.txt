/**
  @page ADC_RegSimul_DualMode ADC regular simultaneous dual mode example
  
  @verbatim
  * File   : ADC/RegSimul_DualMode/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the ADC regular simultaneous dual mode example.
  ******************************************************************************
   @endverbatim

@par Example Description 

This example describes how to use ADC1 and ADC2 in regular simultaneous dual mode.
ADC1 is configured to convert channel14 and channel17 regular channels continuously.
ADC2 is configured to convert channel11 and channel12 regular channels continuously.
The connection between internal Vref and channel17 is enabled for ADC1.

Once the regular channels conversion is started by software, channel14 is converted
on ADC1 and channel11 is converted on ADC2 on the same time. The 32bits conversion 
result is then stored on ADC1 DR register. The DMA will transfer this data which
will be stored ADC_DualConvertedValueTab table. Consecutively to those conversion,
channel17 is converted on ADC1 and channel12 on ADC2. The combined conversion
result is also transferred by DMA to the same destination buffer. 

The same procedure is repeated until the specified number of data to be transferred
by DMA is reached. The ADCs clock is set to 24 MHz.

