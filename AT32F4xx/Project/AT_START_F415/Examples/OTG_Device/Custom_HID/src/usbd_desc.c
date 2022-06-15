/**
  ******************************************************************************
  * File   : OTG_Device/VCP_Loopback/src/usbd_desc.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file provides the USBD descriptors and string formating method.
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_req.h"
#include "usbd_conf.h"
#include "usb_regs.h"

/** @addtogroup AT32_USB_OTG_DEVICE_LIBRARY
  * @{
  */


/** @defgroup USBD_DESC 
  * @brief USBD descriptors module
  * @{
  */ 

/** @defgroup USBD_DESC_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_DESC_Private_Defines
  * @{
  */ 
#define USB_DEVICE_VID                        0x2E3C
#define USB_DEVICE_PID                        0x5745


/** @defgroup USB_String_Descriptors
  * @{
  */ 
#define USB_DEVICE_LANGID_STRING              0x409
#define USB_DEVICE_MANUFACTURER_STRING        "ArteryTek"
#define USB_DEVICE_PRODUCT_HS_STRING        "Custom HID"
#define USB_DEVICE_PRODUCT_FS_STRING        "Custom HID"
#define USB_DEVICE_CONFIGURATION_HS_STRING  "HID Config"
#define USB_DEVICE_INTERFACE_HS_STRING      "HID Interface"
#define USB_DEVICE_CONFIGURATION_FS_STRING  "HID Config"
#define USB_DEVICE_INTERFACE_FS_STRING      "HID Interface"
/**
  * @}
  */ 


/** @defgroup USBD_DESC_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_DESC_Private_Variables
  * @{
  */ 

USB_DEVICE_USER USER_desc =
{
  USB_DEVICE_USER_DeviceDescriptor,
  USB_DEVICE_USER_LangIDStrDescriptor, 
  USB_DEVICE_USER_ManufacturerStrDescriptor,
  USB_DEVICE_USER_ProductStrDescriptor,
  USB_DEVICE_USER_SerialStrDescriptor,
  USB_DEVICE_USER_ConfigStrDescriptor,
  USB_DEVICE_USER_InterfaceStrDescriptor,
};

/* USB Standard Device Descriptor */
uint8_t USB_Device_Desc[USB_SIZ_DEVICE_DESC] =
{
  0x12,                       /*bLength */
  USB_DEVICE_DESCRIPTOR_TYPE, /*bDescriptorType*/
  0x00,                       /*bcdUSB */
  0x02,
  0x00,                       /*bDeviceClass*/
  0x00,                       /*bDeviceSubClass*/
  0x00,                       /*bDeviceProtocol*/
  USB_OTG_MAX_EPT0_SIZE,      /*bMaxPacketSize*/
  LOBYTE(USB_DEVICE_VID),           /*idVendor*/
  HIBYTE(USB_DEVICE_VID),           /*idVendor*/
  LOBYTE(USB_DEVICE_PID),           /*idVendor*/
  HIBYTE(USB_DEVICE_PID),           /*idVendor*/
  0x00,                       /*bcdDevice rel. 2.00*/
  0x02,
  USB_DEVICE_IDX_MAFAC_STRING,           /*Index of manufacturer  string*/
  USB_DEVICE_IDX_PRODUCT_STRING,       /*Index of product string*/
  USB_DEVICE_IDX_SERIAL_STRING,        /*Index of serial number string*/
  USBD_CFG_MAX_NUM            /*bNumConfigurations*/
} ; /* USB_DeviceDescriptor */

/* USB Standard Device Descriptor */
uint8_t USB_Device_QualifierDesc[USB_DEV_QUALIFIER_DESC_LEN] =
{
  USB_DEV_QUALIFIER_DESC_LEN,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  0x40,
  0x01,
  0x00,
};

/* USB Standard Device Descriptor */
uint8_t USB_Device_LangIDDesc[USB_SIZ_STRING_LANGID] =
{
  USB_SIZ_STRING_LANGID,         
  USB_DESC_TYPE_STRING,       
  LOBYTE(USB_DEVICE_LANGID_STRING),
  HIBYTE(USB_DEVICE_LANGID_STRING), 
};

uint8_t USBD_StringSerial[USB_SIZ_STRING_SERIAL] =
{
  USB_SIZ_STRING_SERIAL,      
  USB_DESC_TYPE_STRING,    
};

uint8_t USB_Device_StrDesc[USB_MAX_STR_DESC_SIZ] ;

/**
  * @}
  */ 


/** @defgroup USBD_DESC_Private_FunctionPrototypes
  * @{
  */ 
static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len);
static void Get_SerialNum(void);
/**
  * @}
  */ 


/** @defgroup USBD_DESC_Private_Functions
  * @{
  */ 

/**
* @brief  USB_DEVICE_USER_DeviceDescriptor 
*         return the device descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t *  USB_DEVICE_USER_DeviceDescriptor( uint8_t speed , uint16_t *length)
{
  *length = sizeof(USB_Device_Desc);
  return (uint8_t*)USB_Device_Desc;
}

/**
* @brief  USB_DEVICE_USER_LangIDStrDescriptor 
*         return the LangID string descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t *  USB_DEVICE_USER_LangIDStrDescriptor( uint8_t speed , uint16_t *length)
{
  *length =  sizeof(USB_Device_LangIDDesc);  
  return (uint8_t*)USB_Device_LangIDDesc;
}


/**
* @brief  USB_DEVICE_USER_ProductStrDescriptor 
*         return the product string descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t *  USB_DEVICE_USER_ProductStrDescriptor( uint8_t speed , uint16_t *length)
{
  if(speed == 0)
  {   
    USB_Device_GetString((uint8_t *)(uint8_t *)USB_DEVICE_PRODUCT_HS_STRING, USB_Device_StrDesc, length);
  }
  else
  {
    USB_Device_GetString((uint8_t *)(uint8_t *)USB_DEVICE_PRODUCT_FS_STRING, USB_Device_StrDesc, length);    
  }
  return USB_Device_StrDesc;
}

/**
* @brief  USB_DEVICE_USER_ManufacturerStrDescriptor 
*         return the manufacturer string descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t *  USB_DEVICE_USER_ManufacturerStrDescriptor( uint8_t speed , uint16_t *length)
{
  USB_Device_GetString((uint8_t *)(uint8_t *)USB_DEVICE_MANUFACTURER_STRING, USB_Device_StrDesc, length);
  return USB_Device_StrDesc;
}

/**
* @brief  USB_DEVICE_USER_SerialStrDescriptor 
*         return the serial number string descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t *  USB_DEVICE_USER_SerialStrDescriptor( uint8_t speed , uint16_t *length)
{
  *length = USB_SIZ_STRING_SERIAL;
  
  /* Update the serial number string descriptor with the data from the unique ID*/
  Get_SerialNum();
  
  return (uint8_t*)USBD_StringSerial;
}

/**
* @brief  USB_DEVICE_USER_ConfigStrDescriptor 
*         return the configuration string descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t *  USB_DEVICE_USER_ConfigStrDescriptor( uint8_t speed , uint16_t *length)
{
  if(speed  == USB_OTG_SPEED_HIGH)
  {  
    USB_Device_GetString((uint8_t *)(uint8_t *)USB_DEVICE_CONFIGURATION_HS_STRING, USB_Device_StrDesc, length);
  }
  else
  {
    USB_Device_GetString((uint8_t *)(uint8_t *)USB_DEVICE_CONFIGURATION_FS_STRING, USB_Device_StrDesc, length); 
  }
  return USB_Device_StrDesc;  
}


/**
* @brief  USB_DEVICE_USER_InterfaceStrDescriptor 
*         return the interface string descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t *  USB_DEVICE_USER_InterfaceStrDescriptor( uint8_t speed , uint16_t *length)
{
  if(speed == 0)
  {
    USB_Device_GetString((uint8_t *)(uint8_t *)USB_DEVICE_INTERFACE_HS_STRING, USB_Device_StrDesc, length);
  }
  else
  {
    USB_Device_GetString((uint8_t *)(uint8_t *)USB_DEVICE_INTERFACE_FS_STRING, USB_Device_StrDesc, length);
  }
  return USB_Device_StrDesc;  
}

/**
  * @brief  Create the serial number string descriptor 
  * @param  None 
  * @retval None
  */
static void Get_SerialNum(void)
{
  uint32_t deviceserial0, deviceserial1, deviceserial2;
  
  deviceserial0 = *(uint32_t*)DEVICE_ID1;
  deviceserial1 = *(uint32_t*)DEVICE_ID2;
  deviceserial2 = *(uint32_t*)DEVICE_ID3;
  
  deviceserial0 += deviceserial2;
  
  if (deviceserial0 != 0)
  {
    IntToUnicode (deviceserial0, &USBD_StringSerial[2] ,8);
    IntToUnicode (deviceserial1, &USBD_StringSerial[18] ,4);
  }
}

/**
  * @brief  Convert Hex 32Bits value into char 
  * @param  value: value to convert
  * @param  pbuf: pointer to the buffer 
  * @param  len: buffer length
  * @retval None
  */
static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len)
{
  uint8_t idx = 0;
  
  for( idx = 0 ; idx < len ; idx ++)
  {
    if( ((value >> 28)) < 0xA )
    {
      pbuf[ 2* idx] = (value >> 28) + '0';
    }
    else
    {
      pbuf[2* idx] = (value >> 28) + 'A' - 10; 
    }
    
    value = value << 4;
    
    pbuf[ 2* idx + 1] = 0;
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

