/**
 @page  PWR_PVD example
 @verbatim
  * File   : PWR/PVD/readme.txt
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the PWR_PVD example.
  ********************************************************************************************
 @endverbatiom

@par Example Desctiption
  This demo is based on the AT-START-F403 board.
  This example shows how to configure the programmable voltage detector usingan external interrupt line.
  In this example, EXTI line 16 is configured to generate an interrupt on each rising or falling edge 
  of the PVD output signal (which indicates that the Vdd voltage is below the PVD threshold).
  In the interrupt routine a led connected to a specific GPIO pin is toggled every time the voltage drops 
  below or  the target threshold.
  LED2-->GPIOD Pin13
  LED3-->GPIOD Pin14
  LED4-->GPIOD Pin15
 
  */


