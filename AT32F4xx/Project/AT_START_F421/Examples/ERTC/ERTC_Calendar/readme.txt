
/**
  @page ERTC ERTC_Calendar example
  
  @verbatim
  * File   : ERTC/ERTC_Calendar/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the ERTC ERTC_Calendar example.
  ******************************************************************************
   @endverbatim

@par Example Description 

This example shows how to use the ERTC peripheral.
As an application example, it demonstrates how to setup the ERTC peripheral, in 
terms of prescaler and interrupts, to be used to keep time and to generate 
alarm interrupt. 

One from the following clock can be used as ERTC clock source (uncomment the
corresponding define in main.c): 
  - LSE oscillator clock usually delivered by a 32.768 kHz quartz.
  - LSI oscillator clock
  
The ERTC is in the backup (BKP) domain, still powered by VBAT when VDD is switched off,
so the ERTC configuration is not lost if a battery is connected to the VBAT pin. 
A key value is written in backup data register 0 to indicate if the ERTC is already
configured.

 */
