/**
  @page USART Synchronous example
  
  @verbatim
  * File   : USART/Synchronous/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the USART Synchronous example.
  ******************************************************************************
   @endverbatim

@par Example Description 

  This demo provides a basic communication between USART1 (Synchronous mode) 
  and SPI1 using flags.If received data right ,the Three leds will turn on.
  
  USART1 configured as follow:
  - BaudRate = 115200 baud  
  - Word Length = 8 Bits
  - One Stop Bit
  - No parity
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled
  - USART Clock enabled
  - USART CPOL: Clock is active high
  - USART CPHA: Data is captured on the second edge 
  - USART LastBit: The clock pulse of the last data bit is output to the SCLK pin

  SPI1 configured as follow:
  - Direction = 2 Lines FullDuplex
  - Mode = Slave Mode
  - DataSize = 8 Bits
  - CPOL = Clock is active high
  - CPHA = Data is captured on the second edge 
  - NSS = NSS Software
  - First Bit = First Bit is the LSB  
  Set-up 
  - Connect USART1_Tx(PA.09) to SPI1_MOSI(PA.07), USART1_Rx(PA.10) to 
    SPI1_MISO(PA.06) and USART1_CK(PA.08) to SPI1_SCK(PA.05).   

 */

