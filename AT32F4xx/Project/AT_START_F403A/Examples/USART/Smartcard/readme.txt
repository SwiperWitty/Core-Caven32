/**
  @page USART Smartcard example
  
  @verbatim
  * File   : USART/Smartcard/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the USART/Smartcard example.
  ******************************************************************************
   @endverbatim

@par Example Description 

  This demo provides a description of how  to use the USART in Smartcard mode. 
  The example gives only the possibility to read the ATR and decode it into 
  predefined buffer.If test pass,the Three leds will turn on.
  
  The used Smartcard should be ISO7816-3 T=0 compatible.
  SC_USART configured as follow:
  - Word Length = 9 Bits
  - 0.5 Stop Bit
  - Even parity
  - BaudRate = 12096 baud
  - Hardware flow control disabled (RTS and CTS signals)
  - Tx and Rx enabled
  - USART Clock enabled
  - USART CPOL: Clock is active low
  - USART CPHA: Data is captured on the second edge 
  - USART LastBit: The clock pulse of the last data bit is not output to 
                   the SCLK pin

 */