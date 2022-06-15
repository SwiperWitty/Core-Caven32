/**
 @page  CAN Loopback mode Transmit data example
 @verbatim
  * File   : CAN/CAN_Loopback/readme.txt
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the CAN Loopback mode Transmit data example.
  ********************************************************************************************
 @endverbatiom

@par Example Desctiption
 This demo is based on the AT-START-F415 board,in this demo,CAN work in LoopBack mode.
 LED4 toggle when transmit CAN data.
 User can use USBCAN to receive data.
 User can check register CAN->ESTS and CAN->TSTS to affirm no error causing when disconnect CAN_RX pin.
 LED2-->GPIOC Pin2
 LED3-->GPIOC Pin3
 LED4-->GPIOC Pin5
 
@note
 ¡î¡î¡î	AT32_F415 contain only CAN1.
 ¡î¡î¡î	In main.c line 46:The following one of the define AT32_USE_CAN1 must be choose
 ¡î¡î¡î	In main.c line 51:The following one of the define CAN1_NOREMAP and CAN1_REMAP10 must be choose
 
  */
 