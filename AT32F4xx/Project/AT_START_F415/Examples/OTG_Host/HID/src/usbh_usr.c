/**
  ******************************************************************************
  * File   : usbh_usr_show.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file includes the user application layer 
  ******************************************************************************
  */   

/* Includes ------------------------------------------------------------------*/
#include "usbh_usr.h"
#include "usbh_hid_mouse.h"
#include "usbh_hid_keybd.h"

/** @addtogroup USBH_USER
* @{
*/

/** @addtogroup USBH_HID_DEMO_USER_CALLBACKS
* @{
*/

/** @defgroup USBH_USR
* @brief This file is the Header file for usbh_usr.c
* @{
*/ 


/** @defgroup USBH_CORE_Exported_TypesDefinitions
* @{
*/ 

#define KYBRD_FIRST_COLUMN               (uint16_t)319
#define KYBRD_LAST_COLUMN                (uint16_t)7
#define KYBRD_FIRST_LINE                 (uint8_t)120
#define KYBRD_LAST_LINE                  (uint8_t)200


/**
* @}
*/ 
uint8_t  KeybrdCharXpos           = 0;
uint16_t KeybrdCharYpos           = 0;
extern  int16_t  x_loc, y_loc; 
extern __IO int16_t  prev_x, prev_y;

/** @addtogroup USER
* @{
*/

/** @defgroup USBH_USR 
* @brief    This file includes the user application layer
* @{
*/ 

/** @defgroup USBH_CORE_Exported_Types
* @{
*/ 



/** @defgroup USBH_USR_Private_Defines
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Macros
* @{
*/ 
/**
* @}
*/ 

/** @defgroup USBH_USR_Private_Variables
* @{
*/
extern USB_OTG_CORE_HANDLE           USB_OTG_Core_dev;
/*  Points to the DEVICE_PROP structure of current device */
/*  The purpose of this register is to speed up the execution */

USB_Host_User_cb_Type USR_Callbacks =
{
  USB_HOST_USER_Init,
  USB_HOST_USER_Reset,
  USB_HOST_USER_DeviceAttached,
  USB_HOST_USER_ResetDevice,
  USB_HOST_USER_DeviceDisconnected,
  USB_HOST_USER_OverCurrentDetected,
  USB_HOST_USER_DeviceSpeedDetected,
  USB_HOST_USER_Device_DescAvailable,
  USB_HOST_USER_DeviceAddressAssigned,
  USB_HOST_USER_Configuration_DescAvailable,
  USB_HOST_USER_Manufacturer_String,
  USB_HOST_USER_Product_String,
  USB_HOST_USER_SerialNum_String,
  USB_HOST_USER_EnumerationDone,
  USB_HOST_USER_UserInput,
  NULL,
  USB_HOST_USER_DeviceNotSupported,
  USB_HOST_USER_UnrecoveredError
};

/**
* @}
*/

/** @defgroup USBH_USR_Private_Constants
* @{
*/ 
/*--------------- SHOW Messages ---------------*/
const uint8_t SHOW_HOST_INIT[]          = "> Host Library V1.0.0 Initialized\r\n";
const uint8_t SHOW_DEV_ATTACHED[]       = "> Device Attached\r\n";
const uint8_t SHOW_DEV_DISCONNECTED[]   = "> Device Disconnected\r\n";
const uint8_t SHOW_DEV_RESET[]          = "> Device Reset\r\n";
const uint8_t SHOW_DEV_ENUMERATED[]     = "> Enumeration completed\r\n";
const uint8_t SHOW_DEV_HIGHSPEED[]      = "> High speed device detected\r\n";
const uint8_t SHOW_DEV_FULLSPEED[]      = "> Full speed device detected\r\n";
const uint8_t SHOW_DEV_LOWSPEED[]       = "> Low speed device detected\r\n";
const uint8_t SHOW_DEV_ERROR[]          = "> Device fault \r\n";

const uint8_t SHOW_MSC_CLASS[]          = "> Mass storage device connected\r\n";
const uint8_t SHOW_HID_CLASS[]          = "> HID device connected\r\n";

const uint8_t USB_HID_MouseStatus[]    = "> HID Demo Device : Mouse\r\n";
const uint8_t USB_HID_KeybrdStatus[]   = "> HID Demo Device : Keyboard\r\n";
const uint8_t SHOW_UNREC_ERROR[]        = "> UNRECOVERED ERROR STATE\r\n";
/**
* @}
*/



/** @defgroup USBH_USR_Private_FunctionPrototypes
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Functions
* @{
*/ 





/**
* @brief  USB_HOST_USER_Init 
*         Displays the message for host lib initialization
* @param  None
* @retval None
*/
void USB_HOST_USER_Init(void)
{
  printf("%s", SHOW_HOST_INIT);
}

/**
* @brief  USB_HOST_USER_DeviceAttached 
*         Displays the message on device attached
* @param  None
* @retval None
*/
void USB_HOST_USER_DeviceAttached(void)
{   
  printf("%s", SHOW_DEV_ATTACHED);
}

/**
* @brief  USB_HOST_USER_UnrecoveredError
* @param  None
* @retval None
*/
void USB_HOST_USER_UnrecoveredError (void)
{
  printf("%s", SHOW_UNREC_ERROR);
}

/**
* @brief  USBH_DisconnectEvent
*         Device disconint event
* @param  None
* @retval None
*/
void USB_HOST_USER_DeviceDisconnected (void)
{
  printf("%s", SHOW_DEV_DISCONNECTED);
}

/**
* @brief  USBH_USR_ResetUSBDevice 
*         Reset USB Device
* @param  None
* @retval None
*/
void USB_HOST_USER_ResetDevice(void)
{
  /* Users can do their application actions here for the USB-Reset */ 
  printf("%s", SHOW_DEV_RESET);
}


/**
* @brief  USB_HOST_USER_DeviceSpeedDetected 
*         Displays the message for device speed
* @param  Devicespeed : Device Speed
* @retval None
*/
void USB_HOST_USER_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
  if(DeviceSpeed == HPRT0_PRTSPD_FULL_SPEED)
  {
    printf("%s", SHOW_DEV_FULLSPEED);
  }
  else if(DeviceSpeed == HPRT0_PRTSPD_LOW_SPEED)
  {
   printf("%s", SHOW_DEV_LOWSPEED);
  }
  else
  {
    printf("%s", SHOW_DEV_ERROR);
  }
}

/**
* @brief  USB_HOST_USER_Device_DescAvailable 
*         Displays the message for device descriptor
* @param  DeviceDesc : device descriptor
* @retval None
*/
void USB_HOST_USER_Device_DescAvailable(void *DeviceDesc)
{
}

/**
* @brief  USB_HOST_USER_DeviceAddressAssigned 
*         USB device is successfully assigned the Address 
* @param  None
* @retval None
*/
void USB_HOST_USER_DeviceAddressAssigned(void)
{
  
}


/**
* @brief  USBH_USR_Conf_Desc 
*         Displays the message for configuration descriptor
* @param  ConfDesc : Configuration descriptor
* @retval None
*/
void USB_HOST_USER_Configuration_DescAvailable(USB_HOST_CfgDesc_Type * cfgDesc,
                                          USB_HOST_InterfaceDesc_Type *itfDesc,
                                          USB_HOST_EpDesc_Type *epDesc)
{  
}

/**
* @brief  USB_HOST_USER_Manufacturer_String 
*         Displays the message for Manufacturer String 
* @param  ManufacturerString : Manufacturer String of Device
* @retval None
*/
void USB_HOST_USER_Manufacturer_String(void *ManufacturerString)
{
  
}

/**
* @brief  USB_HOST_USER_Product_String 
*         Displays the message for Product String
* @param  ProductString : Product String of Device
* @retval None
*/
void USB_HOST_USER_Product_String(void *ProductString)
{
  
}

/**
* @brief  USB_HOST_USER_SerialNum_String 
*         Displays the message for SerialNum_String 
* @param  SerialNumString : SerialNum_String of device
* @retval None
*/
void USB_HOST_USER_SerialNum_String(void *SerialNumString)
{
} 

/**
* @brief  EnumerationDone 
*         User response request is displayed to ask for
*         application jump to class
* @param  None
* @retval None
*/
void USB_HOST_USER_EnumerationDone(void)
{
  printf("%s", SHOW_DEV_ENUMERATED);
} 

/**
* @brief  USB_HOST_USER_DeviceNotSupported
*         Device is not supported
* @param  None
* @retval None
*/
void USB_HOST_USER_DeviceNotSupported(void)
{           
  
}  


/**
* @brief  USB_HOST_USER_UserInput
*         User Action for application state entry
* @param  None
* @retval USB_Host_USR_Status : User response for key button
*/
USB_Host_USR_Status USB_HOST_USER_UserInput(void)
{  
  return USB_Host_USR_RESP_OK;
  
} 

/**
* @brief  USB_HOST_USER_OverCurrentDetected
*         Device Overcurrent detection event
* @param  None
* @retval None
*/
void USB_HOST_USER_OverCurrentDetected (void)
{
  
}

/**
* @brief  USR_MOUSE_Init
*         Init Mouse window
* @param  None
* @retval None
*/
void USR_MOUSE_Init	(void)
{
  printf("%s", USB_HID_MouseStatus);
//  HID_MOUSE_ButtonReleased(0);
//  HID_MOUSE_ButtonReleased(1);
//  HID_MOUSE_ButtonReleased(2);
//  HID_MOUSE_UpdatePosition(0,0);
}

/**
* @brief  USR_MOUSE_ProcessData
*         Process Mouse data
* @param  data : Mouse data to be displayed
* @retval None
*/
void USR_MOUSE_ProcessData(HID_MOUSE_Data_TypeDef *data)
{
  
  uint8_t idx = 1;   
  static uint8_t b_state[3] = { 0, 0 , 0};
  
  if ((data->x != 0) && (data->y != 0))
  {
    HID_MOUSE_UpdatePosition(data->x , data->y);
  }
  
  for ( idx = 0 ; idx < 3 ; idx ++)
  {
    
    if(data->button & 1 << idx) 
    {
      if(b_state[idx] == 0)
      {
        HID_MOUSE_ButtonPressed (idx);
        b_state[idx] = 1;
      }
    }
    else
    {
      if(b_state[idx] == 1)
      {
        HID_MOUSE_ButtonReleased (idx);
        b_state[idx] = 0;
      }
    }
  }
  
  
}

/**
* @brief  USR_KEYBRD_Init
*         Init Keyboard window
* @param  None
* @retval None
*/
void  USR_KEYBRD_Init (void)
{

  printf("%s", USB_HID_KeybrdStatus);
  printf("> Use Keyboard to tape characters: \n\n");   
  printf("\n\n\n\n\n\n");
  
  KeybrdCharXpos = KYBRD_FIRST_LINE;
  KeybrdCharYpos = KYBRD_FIRST_COLUMN;
}


/**
* @brief  USR_KEYBRD_ProcessData
*         Process Keyboard data
* @param  data : Keyboard data to be displayed
* @retval None
*/
void  USR_KEYBRD_ProcessData (uint8_t data)
{
  printf("%c", data);
  
}

/**
* @brief  USB_HOST_USER_Reset
*         Deinit User state and associated variables
* @param  None
* @retval None
*/
void USB_HOST_USER_Reset(void)
{
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

/**
* @}
*/

