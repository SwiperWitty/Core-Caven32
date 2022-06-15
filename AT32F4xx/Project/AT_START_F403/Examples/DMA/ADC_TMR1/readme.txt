/**
  @page DMA ADC_TMR1 example
  
  @verbatim
  * File   : DMA/ADC_TMR1/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the DMA ADC_TMR1 example.
  ******************************************************************************
   @endverbatim

@par Example Description 
  This demo provides a description of how to use a DMA channel to transfer continuously 
  a data from a peripheral (ADC1) to another peripheral (TIM1) supporting DMA transfer.
  The ADC channel14 is configured to be converted continuously. TIM1_CH1 is configured
  to generate a PWM signal on its output.The duty cycle variation can be visualized on 
  oscilloscope on the TIM1_CH1 pin PA.08 while changing the analog input on ADC channel14 
  using the potentiometer.
  
 */
