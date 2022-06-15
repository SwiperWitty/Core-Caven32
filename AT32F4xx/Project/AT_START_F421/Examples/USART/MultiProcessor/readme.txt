/**
  @page USART MultiProcessor example
  
  @verbatim
  * File   : USART/MultiProcessor/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the USART MultiProcessor example.
  ******************************************************************************
   @endverbatim

@par Example Description 

  This demo provides a description of how to use the USART in multi-processor mode.
  
  First, the USART1 and USART2 address are set to 0x1 and 0x2. The USART1 send 
  continuously the character 0x33 to the USART3. The board toggle LED2, LED3 and
  LED4 pins while receiving 0x33.
  
  When a falling edge is applied on EXTI Line8, an interrupt is generatedand in 
  the EXTI9_5_IRQHandler routine, the USART3 is entered in mute mode and still
  in this mode (no LED toggling) until a rising edge is applied on BUTTON_WAKEUP 
  EXTI Line 0.
  
  In this interrupt routine the USART1 send the character of address mark (0x102)
  to wakeup USART2. The LED restart toggling.
  
  USART1 and USART2 configured as follow:
  - BaudRate = 9600 baud  
  - Word Length = 9 Bits
  - One Stop Bit
  - No parity
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled
  
  Set-up 
    - Connect USART1 Tx pin (PA.09) to USART2 Rx pin (PA.03)
    - Connect USART1 Rx pin (PA.10) to USART2 Tx pin (PA.02)
    - Use Key push-button connected to pin PA.08 (EXTI Line8)
    - Use Wakeup push-button connected to pin PA.00 (EXTI Line0) 

 */
