/**
  ******************************************************************************
  * File   : USB_Device/Custom_HID/src/usb_desc.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description for Custom HID Demo
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"
/** @addtogroup AT32F413_StdPeriph_Examples
  * @{
  */

/** @addtogroup USB_Custom_HID
  * @{
  */ 
/* USB Standard Device Descriptor */
const uint8_t CustomHID_DeviceDescriptor[CUSTOMHID_SIZ_DEVICE_DESC] =
{
  0x12,                       /*bLength */
  USB_DEVICE_DESCRIPTOR_TYPE, /*bDescriptorType*/
  0x00,                       /*bcdUSB */
  0x02,
  0x00,                       /*bDeviceClass*/
  0x00,                       /*bDeviceSubClass*/
  0x00,                       /*bDeviceProtocol*/
  0x40,                       /*bMaxPacketSize40*/
  0x3C,                       /*idVendor (0x2E3C)*/
  0x2E,
  0x45,                       /*idProduct = 0x5745*/
  0x57,
  0x00,                       /*bcdDevice rel. 2.00*/
  0x02,
  1,                          /*Index of string descriptor describing
                                            manufacturer */
  2,                          /*Index of string descriptor describing
                                           product*/
  3,                          /*Index of string descriptor describing the
                                           device serial number */
  0x01                        /*bNumConfigurations*/
}
; /* CustomHID_DeviceDescriptor */

/* USB Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
const uint8_t CustomHID_ConfigDescriptor[CUSTOMHID_SIZ_CONFIG_DESC] =
{
  0x09, /* bLength: Configuration Descriptor size */
  USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType: Configuration */
  CUSTOMHID_SIZ_CONFIG_DESC,
  /* wTotalLength: Bytes returned */
  0x00,
  0x01,         /* bNumInterfaces: 2 interface */
  0x01,         /* bConfigurationValue: Configuration value */
  0x00,         /* iConfiguration: Index of string descriptor describing
                               the configuration*/
  0xC0,         /* bmAttributes: Self powered */
  0x32,         /* MaxPower 100 mA: this current is used for detecting Vbus */

  /************** Button + LED ****************/
  /************** Descriptor of Custom HID interface ****************/
  /* 09 */
  0x09,         /* bLength: Interface Descriptor size */
  USB_INTERFACE_DESCRIPTOR_TYPE,/* bDescriptorType: Interface descriptor type */
  0x00,         /* bInterfaceNumber: Number of Interface */
  0x00,         /* bAlternateSetting: Alternate setting */
  0x02,         /* bNumEndpoints */
  0x03,         /* bInterfaceClass: HID */
  0x00,         /* bInterfaceSubClass : 1=BOOT, 0=no boot */
  0x00,         /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
  0,            /* iInterface: Index of string descriptor */
  /******************** Descriptor of Custom HID HID ********************/
  /* 18 */
  0x09,         /* bLength: HID Descriptor size */
  HID_DESCRIPTOR_TYPE, /* bDescriptorType: HID */
  0x10,         /* bcdHID: HID Class Spec release number */
  0x01,
  0x00,         /* bCountryCode: Hardware target country */
  0x01,         /* bNumDescriptors: Number of HID class descriptors to follow */
  0x22,         /* bDescriptorType */
  sizeof(CustomHID_ReportDescriptor)&0xFF,/* wItemLength: Total length of Report descriptor */
  (sizeof(CustomHID_ReportDescriptor)>>8)&0xFF,
  /******************** Descriptor of Custom HID endpoints ******************/
  /* 27 */
  0x07,          /* bLength: Endpoint Descriptor size */
  USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: */
  0x81,          /* bEndpointAddress: Endpoint Address (IN) */
  0x03,          /* bmAttributes: Interrupt endpoint */
  0x40,          /* wMaxPacketSize: 64 Bytes max */
  0x00,
  0x20,          /* bInterval: Polling Interval (32 ms) */
  /* 34 */
    
  0x07,	/* bLength: Endpoint Descriptor size */
  USB_ENDPOINT_DESCRIPTOR_TYPE,	/* bDescriptorType: */
  0x01,	        /* bEndpointAddress: Endpoint Address (OUT) */
  0x03,	        /* bmAttributes: Interrupt endpoint */
  0x40,	        /* wMaxPacketSize: 64 Bytes max  */
  0x00,
  0x20,	        /* bInterval: Polling Interval (32 ms) */
  /* 41 */
}; /* CustomHID_ConfigDescriptor */

const uint8_t CustomHID_ReportDescriptor[CUSTOMHID_SIZ_REPORT_DESC] = 
{                    
  0x06, 0xFF, 0x00,      /* USAGE_PAGE (Vendor Page: 0xFF00) */                       
  0x09, 0x01,            /* USAGE (Demo Kit)               */    
  0xa1, 0x01,            /* COLLECTION (Application)       */            
  /* 7 */
  
  /* Led 2 */
  0x85, 0x02,            /*     REPORT_ID 2		     */
  0x09, 0x02,            /*     USAGE (LED 2)	             */
  0x15, 0x00,            /*     LOGICAL_MINIMUM (0)        */          
  0x25, 0x01,            /*     LOGICAL_MAXIMUM (1)        */           
  0x75, 0x08,            /*     REPORT_SIZE (8)            */        
  0x95, 0x3F,            /*     REPORT_COUNT (1)           */       
  0xB1, 0x82,             /*    FEATURE (Data,Var,Abs,Vol) */     

  0x85, 0x02,            /*     REPORT_ID (2)              */
  0x09, 0x02,            /*     USAGE (LED 2)              */
  0x91, 0x82,            /*     OUTPUT (Data,Var,Abs,Vol)  */
  /* 27 */
  
  /* Led 3 */        
  0x85, 0x03,            /*     REPORT_ID (3)		     */
  0x09, 0x03,            /*     USAGE (LED 3)	             */
  0x15, 0x00,            /*     LOGICAL_MINIMUM (0)        */          
  0x25, 0x01,            /*     LOGICAL_MAXIMUM (1)        */           
  0x75, 0x08,            /*     REPORT_SIZE (8)            */        
  0x95, 0x3F,            /*     REPORT_COUNT (1)           */       
  0xB1, 0x82,             /*    FEATURE (Data,Var,Abs,Vol) */     

  0x85, 0x03,            /*     REPORT_ID (3)              */
  0x09, 0x03,            /*     USAGE (LED 3)              */
  0x91, 0x82,            /*     OUTPUT (Data,Var,Abs,Vol)  */
  /* 47 */
  
  /* Led 4 */
  0x85, 0x04,            /*     REPORT_ID 4)		     */
  0x09, 0x04,            /*     USAGE (LED 4)	             */
  0x15, 0x00,            /*     LOGICAL_MINIMUM (0)        */          
  0x25, 0x01,            /*     LOGICAL_MAXIMUM (1)        */           
  0x75, 0x08,            /*     REPORT_SIZE (8)            */        
  0x95, 0x3F,            /*     REPORT_COUNT (1)           */       
  0xB1, 0x82,            /*     FEATURE (Data,Var,Abs,Vol) */     

  0x85, 0x04,            /*     REPORT_ID (4)              */
  0x09, 0x04,            /*     USAGE (LED 4)              */
  0x91, 0x82,            /*     OUTPUT (Data,Var,Abs,Vol)  */
  /* 67 */
  
  /* key Push Button */  
  0x85, 0x05,            /*     REPORT_ID (5)              */
  0x09, 0x05,            /*     USAGE (Push Button)        */      
  0x15, 0x00,            /*     LOGICAL_MINIMUM (0)        */      
  0x25, 0x01,            /*     LOGICAL_MAXIMUM (1)        */      
  0x75, 0x01,            /*     REPORT_SIZE (1)            */  
  0x81, 0x82,            /*     INPUT (Data,Var,Abs,Vol)   */   
  
  0x09, 0x05,            /*     USAGE (Push Button)        */               
  0x75, 0x01,            /*     REPORT_SIZE (1)            */           
  0xb1, 0x82,            /*     FEATURE (Data,Var,Abs,Vol) */  
       
  0x75, 0x07,            /*     REPORT_SIZE (7)            */           
  0x81, 0x83,            /*     INPUT (Cnst,Var,Abs,Vol)   */                    
  0x85, 0x05,            /*     REPORT_ID (2)              */         
                  
  0x75, 0x07,            /*     REPORT_SIZE (7)            */           
  0xb1, 0x83,            /*     FEATURE (Cnst,Var,Abs,Vol) */                      
  /* 95 */
 
  /* Data OUT */  
  0x85, 0xF0,             /*     REPORT_ID (7)              */  
  0x09, 0x06,             /*     USAGE                      */ 
  0x15, 0x00,             /*     LOGICAL_MINIMUM (0)        */ 
  0x26, 0x00,0xff,        /*     LOGICAL_MAXIMUM (255)      */ 
  0x75, 0x08,             /*     REPORT_SIZE (8)            */ 
  0x95, 0x3F,             /*     REPORT_COUNT (64)          */ 
  0x91, 0x02,             /*     OUTPUT(Data,Var,Abs,Vol)   */ 
  /* 110 */
  
  /* Data IN */ 
  0x85, 0xF0,             /*     REPORT_ID (7)              */  
  0x09, 0x07,             /*     USAGE                      */ 
  0x15, 0x00,             /*     LOGICAL_MINIMUM (0)        */ 
  0x26, 0x00,0xff,        /*     LOGICAL_MAXIMUM (255)      */ 
  0x75, 0x08,             /*     REPORT_SIZE (8)            */ 
  0x95, 0x3F,             /*     REPORT_COUNT (64)          */ 
  0x81, 0x82,             /*     INPUT(Data,Var,Abs,Vol)    */ 
                
  /* 125 */
  0xc0 	                 /*     END_COLLECTION	             */
}; /* CustomHID_ReportDescriptor */

/* USB String Descriptors (optional) */
const uint8_t CustomHID_StringLangID[CUSTOMHID_SIZ_STRING_LANGID] =
{
  CUSTOMHID_SIZ_STRING_LANGID,
  USB_STRING_DESCRIPTOR_TYPE,
  0x09,
  0x04
}
; /* LangID = 0x0409: U.S. English */

const uint8_t CustomHID_StringVendor[CUSTOMHID_SIZ_STRING_VENDOR] =
{
  CUSTOMHID_SIZ_STRING_VENDOR, /* Size of Vendor string */
  USB_STRING_DESCRIPTOR_TYPE,  /* bDescriptorType*/
  'A', 0, 'r', 0, 't', 0, 'e', 0, 'r', 0, 'y', 0, ' ', 0, 
  'T', 0, 'e', 0, 'c', 0, 'h', 0, 'n', 0, 'o', 0, 'l', 0, 'o', 0, 'g', 0, 'y', 0,
};

const uint8_t CustomHID_StringProduct[CUSTOMHID_SIZ_STRING_PRODUCT] =
{
  CUSTOMHID_SIZ_STRING_PRODUCT,          /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  'A', 0, 'T', 0, '3', 0, '2', 0, ' ', 0, 'C', 0,
  'u', 0, 's', 0, 't', 0, 'm', 0, ' ', 0, 'H', 0, 'I', 0,
  'D', 0
};
uint8_t CustomHID_StringSerial[CUSTOMHID_SIZ_STRING_SERIAL] =
{
  CUSTOMHID_SIZ_STRING_SERIAL,           /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  'A', 0, 'T', 0,'3', 0,'2', 0
};

/**
  * @}
  */ 

/**
  * @}
  */

  
  

  

