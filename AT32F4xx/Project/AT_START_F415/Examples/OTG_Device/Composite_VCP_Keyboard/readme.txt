/**
  @page OTG USB Device Composite example
  
  @verbatim
  * File   : readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the USB Device Composite (HID+CDC) example
  ******************************************************************************
   @endverbatim

   
@par Example Description 

The examples provide a composite device.
A composite device is one that has multiple interfaces controlled independently of each other.
Using composite device, multiple functions are combined into a single device.
In this Example the independent interfaces are : HID and VCP (CDC).

The implementation of the composite CDC-HID device is done by creating a wrapping
class (see folder hid_cdc_wrapper ) that makes calls to the separate CDC and HID class layer
handlers (files usb_cdc_core.c and usb_hid_keyboard.c files).

To test the demo, you can run HID mouse example and in same time you can
do a USB to USART communication by opening a serial terminal window (like hyperterminal)
and selecting the AT32 Composite virtual comport (you need to follow same steps as  the CDC virtual comport example).
    
 */
