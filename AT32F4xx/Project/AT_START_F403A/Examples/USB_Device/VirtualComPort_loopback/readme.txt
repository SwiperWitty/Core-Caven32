/**
  @page USB VirtualComPort_loopback example
  
  @verbatim
  * File   : USB_Device/VirtualComPort_loopback/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the USB VirtualComPort_loopback example.
  ******************************************************************************
   @endverbatim

@par Example Description 

  The aim of this example is to read data from and write data to USB devices using the CDC protocol.
  It makes the USB device look like a serial port (NO serial cable connectors:You can see the data
  transferred to and from via USB instead of USB-to-USART bridge connection).
  - OUT transfers: receive the data from the PC to AT32
  - IN transfers: to send the data recived from the AT32 to the PC
 

 */