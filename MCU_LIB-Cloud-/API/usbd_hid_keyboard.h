/**
  ******************************************************************************
  * File   : usbd_hid_keyboard.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : header file for the usbd_hid_core.c file.
  ******************************************************************************
  */  
/* Includes ------------------------------------------------------------------*/

#ifndef __USB_HID_CORE_H_
#define __USB_HID_CORE_H_


/** @addtogroup AT32_USB_OTG_DEVICE_LIBRARY
  * @{
  */
  
/** @defgroup USBD_HID
  * @brief This file is the Header file for USBD_msc.c
  * @{
  */ 


/** @defgroup USBD_HID_Exported_Defines
  * @{
  */ 
#define USB_HID_CONFIG_DESC_SIZ       34
#define USB_HID_DESC_SIZ              9
#define HID_KEYBOARD_REPORT_DESC_SIZE    63

#define HID_DESCRIPTOR_TYPE           0x21
#define HID_REPORT_DESC               0x22

#define HID_HS_BINTERVAL              0x07
#define HID_FS_BINTERVAL              0x0A

#define HID_REQ_SET_PROTOCOL          0x0B
#define HID_REQ_GET_PROTOCOL          0x03

#define HID_REQ_SET_IDLE              0x0A
#define HID_REQ_GET_IDLE              0x02

#define HID_REQ_SET_REPORT            0x09
#define HID_REQ_GET_REPORT            0x01
/**
  * @}
  */ 


/** @defgroup USBD_CORE_Exported_TypesDefinitions
  * @{
  */


/**
  * @}
  */ 



/** @defgroup USBD_CORE_Exported_Macros
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup USBD_CORE_Exported_Variables
  * @{
  */ 
#define  HID_A                0x04
#define  HID_B                0x05
#define  HID_C                0x06
#define  HID_D                0x07
#define  HID_E                0x08
#define  HID_F                0x09
#define  HID_G                0x0A
#define  HID_H                0x0B
#define  HID_I                0x0C
#define  HID_J                0x0D
#define  HID_K                0x0E
#define  HID_L                0x0F
#define  HID_M                0x10
#define  HID_N                0x11
#define  HID_O                0x12
#define  HID_P                0x13
#define  HID_Q                0x14
#define  HID_R                0x15
#define  HID_S                0x16
#define  HID_T                0x17
#define  HID_U                0x18
#define  HID_V                0x19
#define  HID_W                0x1A
#define  HID_X                0x1B
#define  HID_Y                0x1C
#define  HID_Z                0x1D
#define  HID_1                30
#define  HID_2                31
#define  HID_3                32
#define  HID_4                33
#define  HID_5                34
#define  HID_6                35
#define  HID_7                36
#define  HID_8                37
#define  HID_9                38
#define  HID_0                39
#define  HID_ENTER            40
#define  HID_ESCAPE           41
#define  HID_BACKSPACE        42
#define  HID_TAB              43
#define  HID_SPACEBAR         44
#define  HID_UNDERSCORE       45
#define  HID_PLUS             46
#define  HID_OPEN_BRACKET     47 // {
#define  HID_CLOSE_BRACKET    48 // }
#define  HID_BACKSLASH        49
#define  HID_ASH              50 // # ~
#define  HID_COLON            51 // ; :
#define  HID_QUOTE            52 // ' "
#define  HID_TILDE            53
#define  HID_COMMA            54
#define  HID_DOT              55
#define  HID_SLASH            56
#define  HID_CAPS_LOCK        57
#define  HID_F1               58
#define  HID_F2               59
#define  HID_F3               60
#define  HID_F4               61
#define  HID_F5               62
#define  HID_F6               63
#define  HID_F7               64
#define  HID_F8               65
#define  HID_F9               66
#define  HID_F10              67
#define  HID_F11              68
#define  HID_F12              69
#define  HID_PRINTSCREEN      70
#define  HID_SCROLL_LOCK      71
#define  HID_PAUSE            72
#define  HID_INSERT           73
#define  HID_HOME             74
#define  HID_PAGEUP           75
#define  HID_DELETE           76
#define  HID_END              77
#define  HID_PAGEDOWN         78
#define  HID_RIGHT            79
#define  HID_LEFT             80
#define  HID_DOWN             81
#define  HID_UP               82
#define  HID_KEYPAD_NUM_LOCK  83
#define  HID_KEYPAD_DIVIDE    84
#define  HID_KEYPAD_AT        85
#define  HID_KEYPAD_MULTIPLY  85
#define  HID_KEYPAD_MINUS     86
#define  HID_KEYPAD_PLUS      87
#define  HID_KEYPAD_ENTER     88
#define  HID_KEYPAD_1         89
#define  HID_KEYPAD_2         90
#define  HID_KEYPAD_3         91
#define  HID_KEYPAD_4         92
#define  HID_KEYPAD_5         93
#define  HID_KEYPAD_6         94
#define  HID_KEYPAD_7         95
#define  HID_KEYPAD_8         96
#define  HID_KEYPAD_9         97
#define  HID_KEYPAD_0         98
#define  HID_KEYPAD_A         0XBC
#define  HID_KEYPAD_B         0XBD
#define  HID_KEYPAD_C         0XBE
#define  HID_KEYPAD_D         0XBF
#define  HID_KEYPAD_E         0XC0
#define  HID_KEYPAD_F         0XC1



/**
  * @}
  */ 

/** @defgroup USB_CORE_Exported_Functions
  * @{
  */ 

//uint32_t usb_device_hid_getpollinginterval (USB_OTG_CORE_HANDLE *pusbdev);
//
//uint16_t USB_Keyboard_Send_Char (uint8_t u8Senddata);
//
//void udi_hid_kbd_print_char(USB_OTG_CORE_HANDLE *pusbdev,uint8_t ASCII_Data,uint8_t flag);
//void udi_hid_kbd_print_null(USB_OTG_CORE_HANDLE *pusbdev);
//void udi_hid_kbd_down(USB_OTG_CORE_HANDLE *pusbdev,uint8_t key_id);

/**
  * @}
  */ 

#endif  /*  __USB_HID_CORE_H_ */
/**
  * @}
  */ 

/**
  * @}
  */ 
  
