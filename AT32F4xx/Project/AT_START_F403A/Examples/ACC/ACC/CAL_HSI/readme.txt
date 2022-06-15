/**
  @page ACC CAL_HSI example
  
  @verbatim
  * File   : ACC/CAL_HSI/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the ACC CAL_HSI example.
  ******************************************************************************
   @endverbatim

@par Example Description

  This demo is based on USB VirtualComPort example(via USB instead of USB-to-USART bridge
  connection).In the USB communication, ACC use the SOF signal to calbration HSI.The ACC 
  demo can Calibration HSI to 8Mhz(¡À0.25%).Can change define to test different HSI accuracy.
  For example,ACC_CAL step is 40Khz while ACC_TRIM step is 20Khz.

  --------GPIO Connect-------------
  -------USB Connect---------------
  PA11  -------------->USB -
  PA12  -------------->USB +

  ------HSI clock test -------------
  PA8  ---------------> Oscilloscope
 
 */