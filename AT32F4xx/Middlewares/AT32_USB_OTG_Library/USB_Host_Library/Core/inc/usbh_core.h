/**
  ******************************************************************************
  * File   : usbh_core.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Header file for usbh_core.c
  ******************************************************************************
  */  
  

/* Define to prevent recursive  ----------------------------------------------*/
#ifndef __USBH_CORE_H
#define __USBH_CORE_H

/* Includes ------------------------------------------------------------------*/
#include "usb_hcd.h"
#include "usbh_def.h"
#include "usbh_conf.h"

/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_LIB_CORE
* @{
*/
  
/** @defgroup USBH_CORE
  * @brief This file is the Header file for usbh_core.c
  * @{
  */ 


/** @defgroup USBH_CORE_Exported_Defines
  * @{
  */ 

#define USB_MSC_CLASS                             0x08 /* MSC */
#define USB_HID_CLASS                             0x03 /* HID */

#define USB_AUDIO_CLASS                             0x01 /* Audio */
#define USB_CDCC_CLASS                              0x02  /* Communications and CDC Control */
#define USB_HID_CLASS                               0x03  /* HID (Human Interface Device) */
#define USB_PRINTER_CLASS                           0x07  /* Printer */
#define USB_MSC_CLASS                               0x08  /* Mass Storage */
#define USB_HUB_CLASS                               0x09  /* Hub */
#define USB_CDCD_CLASS                              0x0A  /* CDC-Data */
#define USB_SMARTCARD_CLASS                         0x0B  /* Smart Card */
#define USB_VIDEO_CLASS                             0x0E  /* Video */
#define USB_AVD_CLASS                               0x10  /* Audio/Video Devices */



#define MSC_PROTOCOL                      0x50
#define CBI_PROTOCOL                      0x01


#define USB_HOST_MAX_ERROR_COUNT                            2
#define USB_HOST_DEVICE_ADDRESS_DEFAULT                     0
#define USB_HOST_DEVICE_ADDRESS                             1
#define CFG_DESC_MAX_SIZE                               512


/**
  * @}
  */ 


/** @defgroup USBH_CORE_Exported_Types
  * @{
  */

typedef enum {
  USB_HOST_OK   = 0,
  USB_HOST_BUSY,
  USB_HOST_FAIL,
  USB_HOST_NOT_SUPPORTED,
  USB_HOST_UNRECOVERED_ERROR,
  USB_HOST_ERROR_SPEED_UNKNOWN,
  USB_HOST_APPLY_DEINIT
}USB_HOST_Status;

/* Following states are used for gState */
typedef enum {
  USB_HOST_IDLE =0,
  USB_HOST_WAIT_PRT_ENABLED,
  USB_HOST_DEV_ATTACHED,
  USB_HOST_DEV_DISCONNECTED,  
  USB_HOST_DETECT_DEVICE_SPEED,
  USB_HOST_ENUMERATION,
  USB_HOST_CLASS_REQUEST,  
  USB_HOST_CLASS,
  USB_HOST_CTRL_XFER,
  USB_HOST_USR_INPUT,
  USB_HOST_SUSPENDED,
  USB_HOST_WAKEUP,
  USB_HOST_ERROR_STATE  
}USB_HOST_State;  

/* Following states are used for EnumerationState */
typedef enum {
  USB_ENUM_IDLE = 0,
  USB_ENUM_GET_FULL_DEV_DESC,
  USB_ENUM_SET_ADDR,
  USB_ENUM_GET_CFG_DESC,
  USB_ENUM_GET_FULL_CFG_DESC,
  USB_ENUM_GET_MFC_STRING_DESC,
  USB_ENUM_GET_PRODUCT_STRING_DESC,
  USB_ENUM_GET_SERIALNUM_STRING_DESC,
  USB_ENUM_SET_CONFIGURATION,
  USB_ENUM_DEV_CONFIGURED
} USB_ENUM_State;  



/* Following states are used for CtrlXferStateMachine */
typedef enum {
  USB_CTRL_IDLE =0,
  USB_CTRL_SETUP,
  USB_CTRL_SETUP_WAIT,
  USB_CTRL_DATA_IN,
  USB_CTRL_DATA_IN_WAIT,
  USB_CTRL_DATA_OUT,
  USB_CTRL_DATA_OUT_WAIT,
  USB_CTRL_STATUS_IN,
  USB_CTRL_STATUS_IN_WAIT,
  USB_CTRL_STATUS_OUT,
  USB_CTRL_STATUS_OUT_WAIT,
  USB_CTRL_ERROR,
  USB_CTRL_STALLED,
  USB_CTRL_COMPLETE    
}
USB_CTRL_State;  

typedef enum {
  USB_Host_USR_NO_RESP   = 0,
  USB_Host_USR_RESP_OK = 1,
}
USB_Host_USR_Status;

/* Following states are used for RequestState */
typedef enum {
  CMD_IDLE =0,
  CMD_SEND,
  CMD_WAIT
} CMD_State;  



typedef struct _Ctrl
{
  uint8_t               hch_num_in; 
  uint8_t               hch_num_out; 
  uint8_t               ept0size;  
  uint8_t               *buff;
  uint16_t              length;
  uint8_t               errorcount;
  uint16_t              timer;  
  CTRL_STATUS           status;
  USB_Setup_Type        setup;
  USB_CTRL_State        state;  

} USB_Host_Ctrl_Type;



typedef struct _DeviceProp
{
  
  uint8_t                           address;
  uint8_t                           speed;
  USB_HOST_DevDesDesc_Type          Dev_Desc;
  USB_HOST_CfgDesc_Type             Cfg_Desc;  
  USB_HOST_InterfaceDesc_Type       Itf_Desc[USBH_MAX_NUM_INTERFACES];
  USB_HOST_EpDesc_Type              Ept_Desc[USBH_MAX_NUM_INTERFACES][USBH_MAX_NUM_ENDPOINTS];
  USB_HOST_HIDDesc_Type             HID_Desc;
  
}USB_Host_Device_Type;

typedef struct _USBH_Class_cb
{
  USB_HOST_Status  (*Init)\
    (USB_OTG_CORE_HANDLE *pusbdev , void *phost);
  void         (*Reset)\
    (USB_OTG_CORE_HANDLE *pusbdev , void *phost);
  USB_HOST_Status  (*Requests)\
    (USB_OTG_CORE_HANDLE *pusbdev ,void *phost);  
  USB_HOST_Status  (*Machine)\
    (USB_OTG_CORE_HANDLE *pusbdev, void *phost);     
  
} USB_Host_Class_cb_Type;


typedef struct _USBH_USR_PROP
{
  void (*Init)(void);       /* HostLibInitialized */
  void (*Reset)(void);       /* HostLibInitialized */  
  void (*DeviceAttached)(void);           /* DeviceAttached */
  void (*ResetDevice)(void);
  void (*DeviceDisconnected)(void); 
  void (*OverCurrentDetected)(void);  
  void (*DeviceSpeedDetected)(uint8_t DeviceSpeed);          /* DeviceSpeed */
  void (*DeviceDescAvailable)(void *);    /* DeviceDescriptor is available */
  void (*DeviceAddressAssigned)(void);  /* Address is assigned to USB Device */
  void (*ConfigurationDescAvailable)(USB_HOST_CfgDesc_Type *,
                                     USB_HOST_InterfaceDesc_Type *,
                                     USB_HOST_EpDesc_Type *); 
  /* Configuration Descriptor available */
  void (*ManufacturerString)(void *);     /* ManufacturerString*/
  void (*ProductString)(void *);          /* ProductString*/
  void (*SerialNumString)(void *);        /* SerialNubString*/
  void (*EnumerationDone)(void);           /* Enumeration finished */
  USB_Host_USR_Status (*UserInput)(void);
  int  (*UserApplication) (void);
  void (*DeviceNotSupported)(void); /* Device is not supported*/
  void (*UnrecoveredError)(void);

}
USB_Host_User_cb_Type;

typedef struct _Host_Type
{
  USB_HOST_State            gState;       /*  Host State Machine Value */
  USB_HOST_State            gStateBkp;    /* backup of previous State machine value */
  USB_ENUM_State            EnumState;    /* Enumeration state Machine */
  CMD_State             RequestState;       
  USB_Host_Ctrl_Type     Control;
  
  USB_Host_Device_Type   device_prop; 
  
  USB_Host_Class_cb_Type               *class_cb;  
  USB_Host_User_cb_Type  	              *user_cb;

  
} USB_HOST, *pUSB_HOST;

/**
  * @}
  */ 



/** @defgroup USBH_CORE_Exported_Macros
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup USBH_CORE_Exported_Variables
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup USBH_CORE_Exported_FunctionsPrototype
  * @{
  */ 
void USB_Host_Init(USB_OTG_CORE_HANDLE *pusbdev,
               USB_OTG_CORE_ID_TypeDef coreid, 
               USB_HOST *phost,                    
               USB_Host_Class_cb_Type *class_cb, 
               USB_Host_User_cb_Type *user_cb);
               
USB_HOST_Status USB_Host_Reset(USB_OTG_CORE_HANDLE *pusbdev, 
                        USB_HOST *phost);
void USB_Host_Process(USB_OTG_CORE_HANDLE *pusbdev , 
                  USB_HOST *phost);
void USB_Host_ErrorHandle(USB_HOST *phost, 
                      USB_HOST_Status errType);

/**
  * @}
  */ 

#endif /* __USBH_CORE_H */
/**
  * @}
  */ 

/**
  * @}
  */ 

/**
* @}
*/ 




