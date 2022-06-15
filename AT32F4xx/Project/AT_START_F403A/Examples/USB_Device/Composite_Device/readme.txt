/**
  @page USB Composite_Device example
  
  @verbatim
  * File   : USB_Device/Composite_Device/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the USB Composite_Device example.
  ******************************************************************************
   @endverbatim

@par Example Description 

  The examples provide a composite device.
  A composite device is one that has multiple interfaces controlled independently of each other.
  Using composite device, multiple functions are combined into a single device.
  In this Example the independent interfaces are : Virtual COM Port (CDC) and Joystick Mouse (HID)

  Joystick Mouse uses the native PC Host USB-FS-Device HID driver. When push the USER key, AT32 
  will send mouse right button information to PC.
  
  Virtual COM Port is to read data from and write data to USB devices using the CDC protocol.
  It makes the USB device look like a serial port (NO serial cable connectors:You can see the data
  transferred to and from via USB instead of USB-to-USART bridge connection).
  - OUT transfers: receive the data from the PC to AT32
  - IN transfers: to send the data recived from the AT32 to the PC
 

 */