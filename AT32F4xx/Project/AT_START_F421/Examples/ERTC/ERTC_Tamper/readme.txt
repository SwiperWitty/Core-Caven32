
/**
  @page ERTC ERTC_Tamper example
  
  @verbatim
  * File   : ERTC/ERTC_Tamper/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the ERTC ERTC_Tamper example.
  ******************************************************************************
   @endverbatim

@par Example Description 

This example shows how to write/read data to/from ERTC Backup data registers and 
demonstrates the Tamper detection feature.

The associated firmware performs the following:
  
1)It configures the ERTC_AF1 pin Tamper to be falling edge, and enables the Tamper 
  interrupt.
     
2)It writes the data to all ERTC Backup data registers, then check whether the 
  data were correctly written. If yes, LED2 turns ON.
     
3)When applying a low level on the ERTC_AF1 pin (PC.13), the ERTC backup data registers
  are reset and the Tamper interrupt is generated. The corresponding ISR then checks
  whether the ERTC Backup data registers are cleared or not. If yes LED3 Toggle.

 */
