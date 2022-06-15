/**
  ******************************************************************************
  * File   : usbh_usr_show.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file includes the declarations for user routines 
  ******************************************************************************
  */   

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBH_USR_SHOW_H
#define __USBH_USR_SHOW_H

/* Includes ------------------------------------------------------------------*/
#include "usbh_hid_core.h"

/** @addtogroup USBH_USER
  * @{
  */
  
/** @defgroup USBH_USR_SHOW 
  * @brief This file is the header file for user routines
  * @{
  */ 

/** @defgroup USBH_USR_Show_Exported_Types
  * @{
  */ 


/**
  * @}
  */ 

/** @defgroup USBH_USR_Show_Exported_Defines
  * @{
  */ 



/**
  * @}
  */ 

/** @defgroup USBH_USR_Show_Exported_Constants
  * @{
  */ 
/*Left Button : Report data :0x01*/
#define HID_MOUSE_BUTTON1                0x01 
/*Right Button : Report data :0x02*/
#define HID_MOUSE_BUTTON2                0x02 
/*Middle Button : Report data : 0x04*/
#define HID_MOUSE_BUTTON3                0x04 

/* Mouse directions */
#define MOUSE_TOP_DIR                   0x80
#define MOUSE_BOTTOM_DIR                0x00
#define MOUSE_LEFT_DIR                  0x80
#define MOUSE_RIGHT_DIR                 0x00

#define MOUSE_WINDOW_X                  100
#define MOUSE_WINDOW_Y                  220
#define MOUSE_WINDOW_HEIGHT             90
#define MOUSE_WINDOW_WIDTH              128

#define HID_MOUSE_BUTTON_HEIGHT         10
#define HID_MOUSE_BUTTON_WIDTH          24
#define HID_MOUSE_BUTTON_XCHORD         201
#define HID_MOUSE_BUTTON1_YCHORD        220
#define HID_MOUSE_BUTTON2_YCHORD        116
#define HID_MOUSE_BUTTON3_YCHORD        166



#define MOUSE_LEFT_MOVE                  1
#define MOUSE_RIGHT_MOVE                 2
#define MOUSE_UP_MOVE                    3
#define MOUSE_DOWN_MOVE                  4

#define HID_MOUSE_HEIGHTLSB              2
#define HID_MOUSE_WIDTHLSB               2
#define HID_MOUSE_RES_X                  4  
#define HID_MOUSE_RES_Y                  4

#define SMALL_FONT_COLUMN_WIDTH          8
#define SMALL_FONT_LINE_WIDTH            12

/**
  * @}
  */ 

/** @defgroup USBH_USR_Show_Exported_Variables
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBH_USR_Show_Exported_Functions
  * @{
  */ 
void HID_MOUSE_UpdatePosition(int8_t x ,int8_t y);
void HID_MOUSE_ButtonReleased(uint8_t button_idx);
void HID_MOUSE_ButtonPressed (uint8_t button_idx);

/**
  * @}
  */ 

#endif /* __USBH_USR_Show_H */

/**
  * @}
  */ 

/**
  * @}
  */ 
