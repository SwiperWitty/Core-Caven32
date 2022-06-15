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
  This demo is based on the AT-START-F403A board,in this demo,CAN work in Normal mode.
  LED4 toggle when transmit CAN data.LED2 toggle when receive CAN data
  User can use USBCAN to transmit and receive data.
 
@note
 ¡î¡î¡î	In main.c line 46:The following one of the define AT32_USE_CAN1 must be choose
 ¡î¡î¡î	In main.c line 51:The following one of the define CAN1_NOREMAP and CAN1_REMAP10 and CAN1_REMAP11 must be choose
 
  */

