/**
  @page USART HyperTerminal Interrupt example
  
  @verbatim
  * File   : USART/HyperTerminal_Interrupt/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the USART HyperTerminal Interrupt example.
  ******************************************************************************
   @endverbatim

@par Example Description

  This demo provides a description of how to use the USART Transmit and Receive 
  interrupts to communicate with the hyperterminal.
  Notice that if you use EZ to download program, you can't use AT-Link pin to receive
  data, and vice versa.

  The USART is configured as follow:
    - BaudRate = 9600 baud  
    - Word Length = 8 Bits (7 data bit + 1 parity bit)
    - Two Stop Bit
    - Odd parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled

 */
