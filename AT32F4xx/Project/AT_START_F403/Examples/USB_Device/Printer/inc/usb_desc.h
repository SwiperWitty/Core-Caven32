/**
  ******************************************************************************
  * File   : USB_Device/USB_Printer/src/usb_desc.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description for printer Demo
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_DESC_H
#define __USB_DESC_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define USB_DEVICE_DESCRIPTOR_TYPE              0x01
#define USB_CONFIGURATION_DESCRIPTOR_TYPE       0x02
#define USB_STRING_DESCRIPTOR_TYPE              0x03
#define USB_INTERFACE_DESCRIPTOR_TYPE           0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE            0x05

#define USB_Printer_DATA_SIZE              64
#define USB_Printer_INT_SIZE               8

#define USB_Printer_SIZ_DEVICE_DESC        18
//#define USB_Printer_SIZ_CONFIG_DESC        67
#define USB_Printer_SIZ_CONFIG_DESC        32
#define USB_Printer_SIZ_STRING_LANGID      4
#define USB_Printer_SIZ_STRING_VENDOR      36
#define USB_Printer_SIZ_STRING_PRODUCT     34
#define USB_Printer_SIZ_STRING_SERIAL      26

#define STANDARD_ENDPOINT_DESC_SIZE             0x09

/* Exported functions ------------------------------------------------------- */
extern const uint8_t USB_Printer_DeviceDescriptor[USB_Printer_SIZ_DEVICE_DESC];
extern const uint8_t USB_Printer_ConfigDescriptor[USB_Printer_SIZ_CONFIG_DESC];

extern const uint8_t USB_Printer_StringLangID[USB_Printer_SIZ_STRING_LANGID];
extern const uint8_t USB_Printer_StringVendor[USB_Printer_SIZ_STRING_VENDOR];
extern const uint8_t USB_Printer_StringProduct[USB_Printer_SIZ_STRING_PRODUCT];
extern uint8_t USB_Printer_StringSerial[USB_Printer_SIZ_STRING_SERIAL];


#endif /* __USB_DESC_H */


