/**
 @page  PWR_SLEEP_USART example
 @verbatim
  * File   : PWR/SLEEP_USART/readme.txt
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the PWR_SLEEP_USART mode example.
  ********************************************************************************************
 @endverbatiom

@par Example Desctiption
  This demo is based on the AT-START-F421 board.
  The system enters and exits SLEEP mode as following:
    The USART1 is configured to generate an Receive interrupt when data receive then the system enters SLEEP mode. 
    The  data receive will wake-up the system . 
    Then after a delay the system will enter again in SLEEP mode and exit in the way
    described above. This behavior is repeated in an infinite loop.

  Three leds LED2,LED3 and LED4 are used to monitor the system state as following:
    - LED2 on: system in RUN mode
    - LED2 off: system in Sleep mode
    - LED4 is toggled if data receive
 
  */


