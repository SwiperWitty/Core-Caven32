/**
  ******************************************************************************
  * File   : USB_Device/HID_IAP_Demo/src/usb_desc.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description for HID IAP Demo
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"
#include "iap_user.h"
/** @addtogroup AT32F403A_StdPeriph_Examples
  * @{
  */

/** @addtogroup USB_HID_IAP_Demo
  * @{
  */ 
/* USB Standard Device Descriptor */
const uint8_t IapHID_DeviceDescriptor[IAPHID_SIZ_DEVICE_DESC] =
  {
    0x12,                       /*bLength */
    USB_DEVICE_DESCRIPTOR_TYPE, /*bDescriptorType*/
    0x00,                       /*bcdUSB */
    0x02,
    0x00,                       /*bDeviceClass*/
    0x00,                       /*bDeviceSubClass*/
    0x00,                       /*bDeviceProtocol*/
    0x40,                       /*bMaxPacketSize40*/
    USB_VENDER_ID & 0x00FF,                       /*idVendor (0x2E3C)*/
    (USB_VENDER_ID >> 8) & 0x00FF,
    USB_PRODUCT_ID & 0x00FF,                       /**/
    (USB_PRODUCT_ID >> 8) & 0x00FF,
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
  ; /* IapHID_DeviceDescriptor */



const uint8_t IapHID_ReportDescriptor[IAPHID_SIZ_REPORT_DESC] = {

    0x06, 0xFF, 0x00,      /* USAGE_PAGE (Vendor Page: 0xFF00) */                       
    0x09, 0x01,            /* USAGE (Demo Kit)               */    
    0xa1, 0x01,            /* COLLECTION (Application)       */            
    /* 6 */
    0x15, 0x00, //     LOGICAL_MINIMUM (0)
 	  0x25, 0xFF, //     LOGICAL_MAXIMUM (255)
    0x75, 0x08,
    0x95, 0x40,
    
    0x09, 0x01,
    0x81, 0x02,
    0x95, 0x40,
    
    0x09, 0x01,
    0x91, 0x02,
    0x95, 0x01,
    
    0x09, 0x01,
    0xB1, 0x02,
    0xC0
    
    
    
};

  
/* USB Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
const uint8_t IapHID_ConfigDescriptor[IAPHID_SIZ_CONFIG_DESC] =
  {
    0x09, /* bLength: Configuration Descriptor size */
    USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType: Configuration */
    IAPHID_SIZ_CONFIG_DESC,
    /* wTotalLength: Bytes returned */
    0x00,
    0x01,         /* bNumInterfaces: 1 interface */
    0x01,         /* bConfigurationValue: Configuration value */
    0x00,         /* iConfiguration: Index of string descriptor describing
                                 the configuration*/
    0x80,         /* bmAttributes: Self powered */
    0x32,         /* MaxPower 100 mA: this current is used for detecting Vbus */

    /************** Descriptor of IAP HID interface ****************/
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
    /******************** Descriptor of IAP HID HID ********************/
    /* 18 */
    0x09,         /* bLength: HID Descriptor size */
    HID_DESCRIPTOR_TYPE, /* bDescriptorType: HID */
    0x10,         /* bcdHID: HID Class Spec release number */
    0x01,
    0x00,         /* bCountryCode: Hardware target country */
    0x01,         /* bNumDescriptors: Number of HID class descriptors to follow */
    0x22,         /* bDescriptorType */
    IAPHID_SIZ_REPORT_DESC,/* wItemLength: Total length of Report descriptor */
    0x00,
    /******************** Descriptor of Custom HID endpoints ******************/
    /* 27 */
    0x07,          /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: */

    0x81,          /* bEndpointAddress: Endpoint Address (IN) */
    0x03,          /* bmAttributes: Interrupt endpoint */
    IAP_IN_MAXPKT_SIZE,          /* wMaxPacketSize: 2 Bytes max */
    0x00,
    0x01,          /* bInterval: Polling Interval (32 ms) */
    /* 34 */
    	
    0x07,	/* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE,	/* bDescriptorType: */
			/*	Endpoint descriptor type */
    0x02,	/* bEndpointAddress: */
			/*	Endpoint Address (OUT) */
    0x03,	/* bmAttributes: Interrupt endpoint */
    IAP_OUT_MAXPKT_SIZE,	/* wMaxPacketSize: 2 Bytes max  */
    0x00,
    0x01,	/* bInterval: Polling Interval (20 ms) */
    /* 41 */
  }
  ; /* IapHID_ConfigDescriptor */

/* USB String Descriptors (optional) */
const uint8_t IapHID_StringLangID[IAPHID_SIZ_STRING_LANGID] =
  {
    IAPHID_SIZ_STRING_LANGID,
    USB_STRING_DESCRIPTOR_TYPE,
    0x09,
    0x04
  }
  ; /* LangID = 0x0409: U.S. English */

const uint8_t IapHID_StringVendor[IAPHID_SIZ_STRING_VENDOR] =
  {
    IAPHID_SIZ_STRING_VENDOR, /* Size of Vendor string */
    USB_STRING_DESCRIPTOR_TYPE,  /* bDescriptorType*/
    'A', 0, 'r', 0, 't', 0, 'e', 0, 'r', 0, 'y', 0, ' ', 0, 
    'T', 0, 'e', 0, 'c', 0, 'h', 0, 'n', 0, 'o', 0, 'l', 0, 'o', 0, 'g', 0, 'y', 0,
  };

const uint8_t IapHID_StringProduct[IAPHID_SIZ_STRING_PRODUCT] =
  {
    IAPHID_SIZ_STRING_PRODUCT,          /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
    'A', 0, 'T', 0, '3', 0, '2', 0, ' ', 0, 'C', 0,
    'u', 0, 's', 0, 't', 0, 'm', 0, ' ', 0, 'H', 0, 'I', 0,
    'D', 0
  };
uint8_t IapHID_StringSerial[IAPHID_SIZ_STRING_SERIAL] =
  {
    IAPHID_SIZ_STRING_SERIAL,           /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
    'A', 0, 'T', 0,'3', 0,'2', 0
  };

/**
  * @}
  */ 

/**
  * @}
  */

  
  

  

