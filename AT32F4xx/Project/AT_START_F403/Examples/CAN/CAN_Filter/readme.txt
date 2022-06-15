/**
 @page  CAN Receive Filter example
 @verbatim
  * File   : CAN/CAN_Filter/readme.txt
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the CAN Filter the identifier.
  ********************************************************************************************
 @endverbatiom

@par Example Desctiption
  This demo is based on the AT-START-F407 board,in this demo,CAN work in Normal mode
  and config Extended identfier and Standard identfier filter.
  LED4 toggle when transmit CAN data.LED2 toggle when receive CAN data
  User can use USBCAN to transmit and receive data.
  LED2-->GPIOD Pin13
  LED3-->GPIOD Pin14
  LED4-->GPIOD Pin15
 
@note
 ¡î¡î¡î	AT32_F403 contain only CAN1.
 ¡î¡î¡î	In main.c line 46:The following one of the define AT32_USE_CAN1 must be choose
 ¡î¡î¡î	In main.c line 51:The following one of the define CAN1_NOREMAP and CAN1_REMAP10 and CAN1_REMAP11 must be choose
 
  */
