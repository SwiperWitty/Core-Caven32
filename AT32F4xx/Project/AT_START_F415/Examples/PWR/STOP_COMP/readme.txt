/**
 @page  PWR_STOP_COMP example
 @verbatim
  * File   : PWR/STOP_COMP/readme.txt
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the PWR_STOP_COMP mode example.
  ********************************************************************************************
 @endverbatiom

@par Example Desctiption
  This demo is based on the AT-START-F415 board.
  The system enters and exits STOP mode as following:
    The COMP is configured to generate an EXTI event on PA1 rising then the system enters STOP mode. 
    The  PA1rising will wake-up the system. After exit from STOP the system clock is reconfigured
    to its previous state (as HSE and PLL are disabled in STOP mode).
    Then after a delay the system will enter again in STOP mode and exit in the way
    described above. This behavior is repeated in an infinite loop.

  Three leds LED2,LED3 and LED4 are used to monitor the system state as following:
    - LED2 on: system in RUN mode
    - LED2 off: system in STOP mode
    - LED3 is toggled if EXTI line19(COMP1) is used to exit from STOP 
 
  */


