/**
  ******************************************************************************
  * File   : usbh_usr_show.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file includes the user application layer 
  ******************************************************************************
  */   

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "usbh_usr.h"
#include "ff.h"       /* FATFS */
#include "usbh_msc_core.h"
#include "usbh_msc_scsi.h"
#include "usbh_msc_bot.h"
//#include "usbh_hid_mouse.h"
//#include "usbh_hid_keybd.h"

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

enum
{
  USER_INIT = 0,
  USER_READLIST,
  USER_WRITE,
  USER_IDEL
};

/**
* @}
*/ 
static uint8_t user_appstate;
uint8_t filenameString[15]  = {0};
uint8_t line_idx = 0;  

FATFS fatfs;
FIL file;
static uint8_t Show_Disk (char* path , uint8_t recu_level);
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
  USB_HOST_USER_MSC_Application,
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

const uint8_t MSG_MSC_CLASS[]        = "> Mass storage device connected\r\n";
const uint8_t MSG_HID_CLASS[]        = "> HID device connected\r\n";
const uint8_t MSG_DISK_SIZE[]        = "> Size of the disk in MBytes: \r\n";
const uint8_t MSG_LUN[]              = "> LUN Available in the device:\r\n";
const uint8_t MSG_ROOT_CONT[]        = "> Exploring disk flash ...\r\n";
const uint8_t MSG_WR_PROTECT[]       = "> The disk is write protected\r\n";
const uint8_t MSG_UNREC_ERROR[]      = "> UNRECOVERED ERROR STATE\r\n";
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
  user_appstate = 0;
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
* @brief  USB_HOST_USER_MSC_Application
*         User Action for application state entry
* @param  None
* @retval USB_Host_USR_Status : User response for key button
*/

int USB_HOST_USER_MSC_Application(void)
{ 
  uint8_t writeTextBuff[] = "AT32F415 USB OTG Host Demo application using FAT_FS   ";
  uint16_t bytesWritten, bytesToWrite;
  FRESULT res;
  switch(user_appstate)
  {
    case USER_INIT:
      if ( f_mount(&fatfs, "", 0) != FR_OK)
      {
        printf("> Cannot initialize file system\r\n");
        return -1;
      }
      printf("> File System initialized.\r\n");
      printf("> Disk capacity : %llu Bytes\n",(uint64_t)USBH_MSC_Param.MSCapacity * (uint64_t)USBH_MSC_Param.MSPageLength);
      if(USBH_MSC_Param.MSWriteProtect == DISK_WRITE_PROTECTED)
      {
        printf("%s\r\n", MSG_WR_PROTECT);
      }
      user_appstate = USER_READLIST;
      break;
    case USER_READLIST:
      printf("%s", MSG_ROOT_CONT);
      Show_Disk("0:/", 1);
      user_appstate = USER_WRITE;
      break;
    case USER_WRITE:
      printf("> Writing File to disk flash ...\r\n");
      if (USBH_MSC_Param.MSWriteProtect == DISK_WRITE_PROTECTED)
      {
        printf("> Disk flash is write protected \r\n");
        break;
      }
       if(f_open(&file, "0:AT32.TXT",FA_CREATE_ALWAYS | FA_WRITE) == FR_OK)
       {
         bytesToWrite = sizeof(writeTextBuff); 
         res= f_write (&file, writeTextBuff, bytesToWrite, (void *)&bytesWritten); 
         if((bytesWritten == 0) || (res != FR_OK)) /*EOF or Error*/
         {
            printf("> AT32.TXT cannot be writen.\r\n");
         }
         else
         {
           printf("> 'AT32.TXT' file created.\r\n");  
         }
               /*close file and filesystem*/
         f_close(&file);
         f_mount(NULL, "", 0); 
       }
      user_appstate = USER_IDEL;
      break;
    case USER_IDEL:
      break;
    default:
      break;
  }    
  return 0;
  
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
* @brief  USB_HOST_USER_Reset
*         Deinit User state and associated variables
* @param  None
* @retval None
*/
void USB_HOST_USER_Reset(void)
{
}


/**
* @brief  Show_Disk 
*         Displays disk content
* @param  path: pointer to root path
* @retval None
*/
static uint8_t Show_Disk (char* path , uint8_t recu_level)
{

  FRESULT res;
  FILINFO fno;
  DIR dir;
  char *fn;
  char tmp[14];
  
  res = f_opendir(&dir, path);
  if (res == FR_OK) {
    while(USB_HCD_IsDeviceConnected(&USB_OTG_Core_dev)) 
    {
      res = f_readdir(&dir, &fno);
      if (res != FR_OK || fno.fname[0] == 0) 
      {
        break;
      }
      if (fno.fname[0] == '.')
      {
        continue;
      }

      fn = fno.fname;
      strcpy(tmp, fn); 

      line_idx++;
      if(line_idx > 9)
      {
        line_idx = 0;
      } 
      
      if(recu_level == 1)
      {
        printf("   |__");
      }
      else if(recu_level == 2)
      {
        printf("   |   |__");
      }
      if((fno.fattrib & AM_MASK) == AM_DIR)
      {
//        strcat(tmp, "\n"); 
        printf("%s\r\n", tmp);
      }
      else
      {
//        strcat(tmp, "\n"); 
         printf("%s\r\n", tmp);
      }

      if(((fno.fattrib & AM_MASK) == AM_DIR)&&(recu_level == 1))
      {
        Show_Disk(fn, 2);
      }
    }
  }
  return res;
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

