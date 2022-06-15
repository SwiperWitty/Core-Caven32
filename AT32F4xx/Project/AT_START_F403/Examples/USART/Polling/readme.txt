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

  This demo  provides a basic communication between USART2 and USART3 using flags.
  First, the USART2 sends TxBuffer to USART3. The USART3 reads the received data and
  store it into RxBuffer.The received data is then compared with the send ones and 
  the result of this comparison is stored in the "TransferStatus" variable. If received
  data right ,the Three leds will turn on.  
  Set-up 
    - Connect USART2 Tx pin (PD.05) to USART3 Rx pin (PC.11)
    - Connect USART2 Rx pin (PD.06) to USART3 Tx pin (PC.10)

 */
