/********************************** (C) COPYRIGHT *******************************
 * File Name          : usb_desc.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/08/08
 * Description        : This file contains all the functions prototypes for the  
 *                      USB description firmware library.
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/ 
#ifndef __USB_DESC_H
#define __USB_DESC_H

#ifdef __cplusplus
 extern "C" {
#endif
	 
#include "ch32v30x.h"

 /* 自定义---*/
#define USB_Device_Length 0x12  //也就是 18
#define USB_Device_Type 0x01
#define USB_VID 0x03EB           // dd-dd
#define USB_PID 0x2421

#define USB_Config_Length       0x09    //【配置】描述符长度
#define USB_Interface_Length    0x09    //【接口】描述符长度
#define USB_Endpoint_Length     0x07    //【端点】描述符长度
#define USB_HID_Class_Length    0x09    //【HID类】描述符长度
#define USB_Report_Length       0x22    //【报告】HID描述符长度
#define KEYBOARD_Report_Length  0x3e    //【报告】KEY描述符长度       // dd-dd

#define USB_Config_Type     0x02
#define USB_String_TYPE     0x03
#define USB_Interface_Type  0x04
#define USB_Endpoint_Type   0x05
#define USB_HID_Class_Type  0x21

#define USBD_SIZE_CONFIG_ADD (9+9+9+7+7+9+9+7) //上面 Length 的和                // dd-dd

/*---自定义     */
	 
#define USB_DEVICE_DESCRIPTOR_TYPE              0x01
#define USB_CONFIGURATION_DESCRIPTOR_TYPE       0x02
#define USB_STRING_DESCRIPTOR_TYPE              0x03
#define USB_INTERFACE_DESCRIPTOR_TYPE           0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE            0x05


#define THIS_ENDP0_SIZE         0x40
#define ENDP1_IN_SIZE           8
#define ENDP1_OUT_SIZE          0x40          // dd-dd
#define ENDP2_IN_SIZE           0x40
#define ENDP2_OUT_SIZE          0x40
       
#define USBD_SIZE_DEVICE_DESC        18
#define USBD_SIZE_CONFIG_DESC        USBD_SIZE_CONFIG_ADD
#define USBD_SIZE_STRING_LANGID      4
#define USBD_SIZE_STRING_VENDOR      10
#define USBD_SIZE_STRING_PRODUCT     26
#define USBD_SIZE_STRING_SERIAL      12

#define USBD_SIZE_HID_DESC      USB_Report_Length
#define KEYBOARD_SIZE_HID_DESC      KEYBOARD_Report_Length

#define STANDARD_ENDPOINT_DESC_SIZE             0x09


extern const uint8_t USBD_DeviceDescriptor[USBD_SIZE_DEVICE_DESC];
extern const uint8_t USBD_ConfigDescriptor[USBD_SIZE_CONFIG_DESC];
                    
extern const uint8_t USBD_StringLangID [USBD_SIZE_STRING_LANGID];
extern const uint8_t USBD_StringVendor [USBD_SIZE_STRING_VENDOR];
extern const uint8_t USBD_StringProduct[USBD_SIZE_STRING_PRODUCT];
extern uint8_t USBD_StringSerial [USBD_SIZE_STRING_SERIAL];
extern const uint8_t USBD_HIDRepDesc[];
extern const uint8_t USBD_KeyRepDesc[];             // dd-dd

#ifdef __cplusplus
}
#endif

#endif /* __USB_DESC_H */
