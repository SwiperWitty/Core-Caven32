/**
  @page ERTC ERTC_BKPDomain example
  
  @verbatim
  * File   : ERTC/ERTC_BKPDomain/readme.txt
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the ERTC ERTC_BKPDomain example.
  ******************************************************************************
   @endverbatim

@par Example Description

This example shows how to setup the ERTC hardware calendar,
ERTC Backup Data registers read/write  

One from the following clock can be used as ERTC clock source (uncomment the
corresponding define in main.c file): 
  - LSE oscillator clock usually delivered by a 32.768 kHz quartz.
  - LSI oscillator clock
  
The Backup (BKP) domain, still powered by VBAT when VDD is switched off, so its
content is not lost if a battery is connected to the VBAT pin. 
A key value is written in backup data register 0 to indicate if the Backup Domain
is already configured.When an ERTC Wakeup event occurs the ERTC Wakeup interrupt is 
generated and the current Time/Date is refreshed by USART printf(BaudRate:115200).
   
 */
 