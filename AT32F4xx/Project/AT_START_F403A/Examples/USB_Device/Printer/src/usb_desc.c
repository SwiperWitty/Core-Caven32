/**
  ******************************************************************************
  * File   : USB_Device/USB_Printer/src/usb_desc.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description for printer Demo
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"

/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup USB_USB_Printer
  * @{
  */ 
/* USB Standard Device Descriptor */
const uint8_t USB_Printer_DeviceDescriptor[] =
  {
    0x12,   /* bLength */
    USB_DEVICE_DESCRIPTOR_TYPE,     /* bDescriptorType */
    0x00,
    0x02,   /* bcdUSB = 2.00 */
    0x00,   /* bDeviceClass: CDC */
    0x00,   /* bDeviceSubClass */
    0x00,   /* bDeviceProtocol */
    0x40,   /* bMaxPacketSize0 */
    0x3C,
    0x2E,   /* idVendor = 0x2E3C */
    0xFF,
    0x57,   /* idProduct = 0x7540 */
    0x00,
    0x02,   /* bcdDevice = 2.00 */
    1,              /* Index of string descriptor describing manufacturer */
    2,              /* Index of string descriptor describing product */
    3,              /* Index of string descriptor describing the device's serial number */
    0x01    /* bNumConfigurations */
  };

const uint8_t USB_Printer_ConfigDescriptor[] =
  {
    /*Configuration Descriptor*/
    0x09,   /* bLength: Configuration Descriptor size */
    USB_CONFIGURATION_DESCRIPTOR_TYPE,      /* bDescriptorType: Configuration */
    USB_Printer_SIZ_CONFIG_DESC,       /* wTotalLength:no of returned bytes */
    0x00,
    0x01,   /* bNumInterfaces: 2 interface */
    0x01,   /* bConfigurationValue: Configuration value */
    0x00,   /* iConfiguration: Index of string descriptor describing the configuration */
    0xC0,   /* bmAttributes: self powered */
    0x32,   /* MaxPower 100 mA */
    /*Data class interface descriptor*/
    0x09,   /* bLength: Endpoint Descriptor size */
    USB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: */
    0x00,   /* bInterfaceNumber: Number of Interface */
    0x00,   /* bAlternateSetting: Alternate setting */
    0x02,   /* bNumEndpoints: Two endpoints used */
    0x07,   /* bInterfaceClass:  USB Printer class */
    0x01,   /* bInterfaceSubClass: */
    0x02,   /* bInterfaceProtocol: */
    0x00,   /* iInterface: */
    /*Endpoint 3 Descriptor*/
    0x07,   /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
    0x03,   /* bEndpointAddress: (OUT3) */
    0x02,   /* bmAttributes: Bulk */
    USB_Printer_DATA_SIZE,             /* wMaxPacketSize: */
    0x00,
    0x00,   /* bInterval: ignore for Bulk transfer */
    /*Endpoint 1 Descriptor*/
    0x07,   /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
    0x81,   /* bEndpointAddress: (IN1) */
    0x02,   /* bmAttributes: Bulk */
    USB_Printer_DATA_SIZE,             /* wMaxPacketSize: */
    0x00,
    0x00    /* bInterval */
  };

/* USB String Descriptors */
const uint8_t USB_Printer_StringLangID[USB_Printer_SIZ_STRING_LANGID] =
  {
    USB_Printer_SIZ_STRING_LANGID,
    USB_STRING_DESCRIPTOR_TYPE,
    0x09,
    0x04 /* LangID = 0x0409: U.S. English */
  };

const uint8_t USB_Printer_StringVendor[USB_Printer_SIZ_STRING_VENDOR] =
  {
    USB_Printer_SIZ_STRING_VENDOR,     /* Size of Vendor string */
    USB_STRING_DESCRIPTOR_TYPE,             /* bDescriptorType*/
    'A', 0, 'r', 0, 't', 0, 'e', 0, 'r', 0, 'y', 0, ' ', 0, 
    'T', 0, 'e', 0, 'c', 0, 'h', 0, 'n', 0, 'o', 0, 'l', 0, 'o', 0, 'g', 0, 'y', 0
  };

const uint8_t USB_Printer_StringProduct[USB_Printer_SIZ_STRING_PRODUCT] =
  {
    USB_Printer_SIZ_STRING_PRODUCT,          /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
    /* Product name: "AT32 Virtual COM Port" */
    'A', 0, 'T', 0, '3', 0, '2', 0, ' ', 0, 'U', 0, 'S', 0,
    'B', 0, ' ', 0, 'P', 0, 'r', 0, 'i', 0, 'n', 0, 't', 0, 'e', 0,
    'r', 0
  };


uint8_t USB_Printer_StringSerial[USB_Printer_SIZ_STRING_SERIAL] =
  {
    USB_Printer_SIZ_STRING_SERIAL,           /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,                   /* bDescriptorType */
    'A', 0, 'T', 0, '3', 0, '2', 0
};

/**
  * @}
  */

/**
  * @}
  */


  
  

  

