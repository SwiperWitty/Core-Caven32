/**
  @page USB Custom HID example
  
  @verbatim
  * File   : USB_Device/Custom_HID/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the USB Custom_HID example.
  ******************************************************************************
   @endverbatim

@par Example Description 

This Demo provides a description of how to use the USB-FS-Device on the AT32.
The AT32 device is enumerated as an USB-FS-Device HID , that uses the native 
PC Host USB-FS-Device HID driver.

To test the example, you need to:

  2)- The device should be detected and shown in the Device capabilities
  3)- Communicaton view: LED2 ID(0x2),LED3 ID(0x3), LED4 ID(0x4), BUTTON1_ID(0x5) and Input\Output Report.
  5)- Push the USER key button in AT-START => AT32 will sent SET_REPORT to PC
      transmit by AT32

 */