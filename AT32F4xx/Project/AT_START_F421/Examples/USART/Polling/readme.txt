/**
  @page USART Polling example
  
  @verbatim
  * File   : USART/Polling/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the USART Polling example.
  ******************************************************************************
   @endverbatim

@par Example Description 

  This demo  provides a basic communication between USART1 and USART2 using flags.
  First, the USART1 sends TxBuffer to USART2. The USART3 reads the received data and
  store it into RxBuffer.The received data is then compared with the send ones and 
  the result of this comparison is stored in the "TransferStatus" variable. If received
  data right ,the Three leds will turn on.  
  Set-up 
    - Connect USART1 Tx pin (PA.09) to USART2 Rx pin (PA.03)
    - Connect USART1 Rx pin (PA.10) to USART2 Tx pin (PA.02)

 */
