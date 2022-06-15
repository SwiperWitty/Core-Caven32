/**
  @page USART Swap example
  
  @verbatim
  * File   : USART/Swap/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the USART swao example.
  ******************************************************************************
   @endverbatim

@par Example Description

  This demo provides a description of how to use the USART Transmit and Receive 
  interrupts to communicate with the hyperterminal.
  Notice that if you use EZ to download program, you can't use AT-Link pin to receive
  data, and vice versa.
  Notice that TX/RX will be swapped, so TX is PA10, RX is PA9.

  The USART is configured as follow:
    - BaudRate = 115200 baud  
    - Word Length = 8 Bits (7 data bit + 1 parity bit)
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled

 */
