/**
  @page OTG_Device VCP_Loopback VCP example
  
  @verbatim
  * File   : readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the USB Device VCP (Virtual Com Port) example
  ******************************************************************************
   @endverbatim

   
@par Example Description 

This Demo presents the implementation of a Virtual Com Port (VCP) capability in 
the AT32F415xx devices. 

The aim of this example is to read data from and write data to USB devices using the CDC protocol. 
It makes the USB device look like a serial port (NO serial cable connectors:You can see the data
transferred to and from via USB instead of USB-to-USART bridge connection). 
This example loops back the contents of a text file over usb port. To run the example, Type a message using the Pc's keyboard.
Any data that shows in HyperTerminal is received from the device. Here are the modifiction done versus the VCP example:		
    
 */
