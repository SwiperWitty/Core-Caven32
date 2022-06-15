/**
 @page  CAN Normal mode Transmit and receive data example
 @verbatim
  * File   : CAN/CAN_Normal/readme.txt
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the CAN Normal mode Transmit and receive data example.
  ********************************************************************************************
 @endverbatiom

@par Example Desctiption
  This demo is based on the AT-START-F415 board,in this demo,CAN work in Normal mode.
  LED4 toggle when transmit CAN data.LED2(CAN1 used) toggle when receive CAN data
  User can use USBCAN to transmit and receive data.
  LED2-->GPIOC Pin2
  LED3-->GPIOC Pin3
  LED4-->GPIOC Pin5
 
@note
 ¡î¡î¡î	AT32_F413 contain only CAN1.
 ¡î¡î¡î	In main.c line 46:The following one of the define AT32_USE_CAN1 must be choose
 ¡î¡î¡î	In main.c line 51:The following one of the define CAN1_NOREMAP and CAN1_REMAP10 must be choose
 
  */

