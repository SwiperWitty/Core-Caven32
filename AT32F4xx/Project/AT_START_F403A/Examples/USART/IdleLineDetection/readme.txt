/**
  @page USART IdleLineDetection example
  
  @verbatim
  * File   : USART/IdleLineDetection/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the USART IdleLineDetection example.
  ******************************************************************************
   @endverbatim

@par Example Description 

  This demo provides a description of how to use the USART in multi-processor mode.
  
  First, we need two developed board, one as master, the other one as slave.
  Second, we need connect a button to PE3 to trigger transmit data on master board.
  After send data, slave received 0x7F and enter mute mode, then discard all data
  that from master until detect idle frame, all recoginized data will be transmitted
  by slave, that can be watched by LA or trasmit assistant.
  
  USART1 configured as follow:
  - BaudRate = 115200 baud  
  - Word Length = 8 Bits
  - One Stop Bit
  - No parity
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled
  
  Set-up 
    - Connect button to KEY1 (PE.03)
    - Connect MASTER USART1 Tx pin (PA.09) to SLAVE USART1 Rx pin (PA.10)
    - Connect SLAVE USART1 Tx pin (PA.09) to LA or transmit assistant

 */
