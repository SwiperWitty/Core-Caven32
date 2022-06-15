/**
  ******************************************************************************
  * File   : usbh_usr_show.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file includes the some user routines 
  ******************************************************************************
  */    

/* Includes ------------------------------------------------------------------*/
#include "usbh_usr_show.h"
#include "usb_conf.h"

/** @addtogroup USBH_USER
* @{
*/


/** @defgroup USBH_USR_SHOW 
* @brief    This file includes the some user routines
* @{
*/ 

/** @defgroup USBH_USR_SHOW_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_USR_SHOW_Private_Defines
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_USR_SHOW_Private_Macros
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_USR_SHOW_Private_Variables
* @{
*/ 
int16_t  x_loc  = 0, y_loc  = 0; 
int16_t  prev_x = 0, prev_y = 0;
/**
* @}
*/ 

/** @defgroup USBH_USR_SHOW_Exported_Variables
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_USR_SHOW_Private_FunctionPrototypes
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_USR_SHOW_Private_Functions
* @{
*/ 

/**
* @brief  HID_MOUSE_UpdatePosition
*         The function is to handle mouse scroll to update the mouse position 
*         on display window
* @param   x : USB HID Mouse X co-ordinate
* @param   y :  USB HID Mouse Y co-ordinate
* @retval None
*/
void HID_MOUSE_UpdatePosition (int8_t x, int8_t y)
{
//  
  if((x != 0) || (y != 0)) 
  {
    x_loc += x/2;
    y_loc += y/2;
    
    if(y_loc > MOUSE_WINDOW_HEIGHT - 12)
    {
      y_loc = MOUSE_WINDOW_HEIGHT - 12;
    }  
    if(x_loc > MOUSE_WINDOW_WIDTH - 10)
    {
      x_loc = MOUSE_WINDOW_WIDTH - 10;
    }  
    
    if(y_loc < 2)
    {
      y_loc = 2;
    }  
    if(x_loc < 2)
    {
      x_loc = 2;
    } 
    
    printf("Mouse Moving: %d, %d\r\n", x_loc,y_loc);
    
    prev_x = x_loc;
    prev_y = y_loc;
  }
}  

/**
* @brief  HID_MOUSE_ButtonPressed
*         The function is to handle mouse button press
* @param  button_idx : mouse button pressed
* @retval None
*/

void HID_MOUSE_ButtonPressed(uint8_t button_idx)
{
  /* Change the color of button pressed to indicate button press*/
  switch (button_idx)
  {
    /* Left Button Pressed */
  case 0 :
    printf("Left Button Pressed\r\n");
    break;
        
    /* Right Button Pressed */  
  case 1 :
    printf("Right Button Pressed\r\n");
    break; 
    
    /* Middle button Pressed */  
  case 2 :
    printf("Middle Button Pressed\r\n");
    break;    
  }
}

/**
* @brief  HID_Mouse_ButtonReleased
*         The function is to handle mouse button release
* @param  button_idx : mouse button released
* @retval None
*/

void HID_MOUSE_ButtonReleased(uint8_t button_idx)
{  
  /* Change the color of button released to default button color*/
  switch (button_idx)
  {
    /* Left Button Released */
  case 0 :
    printf("Left Button Released\r\n");
    break;
    
    /* Right Button Released */  
  case 1 :
    printf("Right Button Released\r\n");
    break;
    
    /* Middle Button Released */  
  case 2 :
    printf("Middle Button Released\r\n");    
    break;
  }
}

/**
* @}
*/ 

/**
* @}
*/ 

/**
* @}
*/

