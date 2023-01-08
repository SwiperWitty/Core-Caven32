/********************************** (C) COPYRIGHT *******************************
 * File Name          : usb_desc.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/08/08
 * Description        : Descriptors for Virtual Com Port Demo.
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/ 
#include "usb_lib.h"
#include "usb_desc.h"

const uint8_t  USBD_DeviceDescriptor[USB_Device_Length] =
{
    USB_Device_Length,  //固定值
    USB_Device_Type,    //固定值
    0x10,0x01,          //USB1.1 即 0x0110,小端结构写成 0x10,0x01
    0x00,0x00,0x00,     //这是设备类，不想在设备描述中使用，打算放在【接口描述符】中
    64,                 //这个是端点0的最大数据包
    (USB_VID & 0xff),(USB_VID >> 8),          //VID
    (USB_PID & 0xff),(USB_PID >> 8),          //PID
    0x00,0x01,          //版本号
    0x01,               //【厂商字】符串索引
    0x02,               //【产品】字符串索引
    0x00,               //【设备的序列号】字符索引
    0x01,               //需要一个配置
};

const uint8_t  USBD_ConfigDescriptor[USBD_SIZE_CONFIG_ADD] =
{
    /*  --【配置描述符】-- */
    USB_Config_Length, USB_Config_Type, USBD_SIZE_CONFIG_ADD & 0x00ff,USBD_SIZE_CONFIG_ADD >> 8,
    0x02/*本配置只需要2个接口*/  ,0x01/*本配置的编号*/,0x04/*本配置有字符串索引*/ ,0xA0/*本配置属性是总线供电*/ ,   0x64/*本配置需要电流量*/,
    /*-----------------------------------------------配置------------------------------------------------------------------------*/
    /*  --【接口描述符0--KEY】-- */
    USB_Interface_Length,USB_Interface_Type,
    0x00/*本接口需要一个【编号】*/ ,0x00/*本接口备用编号*/    ,0x01/*本接口需要1个【端口数】*/    ,
    0x03/*本接口设置为 【HID类】*/   ,0x01/*本接口【键/鼠子类】*/    ,0x01/*本接口【键盘协议】*/  ,0x00/*本接口-字符串索引*/,
    /*  --【HID类描述符】--   */
    USB_HID_Class_Length,USB_HID_Class_Type,    0x11,0x01,  /*USB1.1 即 0x0110,小段结构写成 0x10,0x01*/
    0x21/*本类设置为 US国家*/    ,0x01/*本类有【一个下级描述符】*/   ,0x22/*本类的【下级描述】符为【报告描述符】*/ ,
    KEYBOARD_Report_Length & 0x00ff,KEYBOARD_Report_Length >> 8/*本配类【下级描述符】的长度*/,
    /*  --【端口类描述符】--    */
    //输入
    USB_Endpoint_Length,USB_Endpoint_Type,0x81/*本端口是输入的（0x80）,端口1*/ ,
    0x03/*本端口为中断接口 11b（0x03）*/  ,0x40,0x00/*本端口最大 0x0040包长*/    ,0x05/*本端口扫描时间1帧内*/,
    //输出
//    USB_Endpoint_Length,USB_Endpoint_Type,0x01/*本端口是输出的（0x00）,端口1*/ ,
//    0x03/*本端口为中断接口 11b（0x03）*/  ,0x40,0x00/*本端口最大 64包长*/    ,0x0a/*本端口扫描时间1帧内*/,
    /*-----------------------------------------------接口------------------------------------------------------------------------*/
    /*  --【接口描述符1--HID】-- */              // dd-dd
    USB_Interface_Length,USB_Interface_Type,
    0x01/*本接口需要一个【编号】*/ ,0x00/*本接口备用编号*/    ,0x02/*本接口需要2个【端口数】*/    ,
    0x03/*本接口设置为 【HID类】*/   ,0x01/*本接口【数据子类】*/    ,0x02/*本接口【数据协议】*/  ,0x00/*本接口无字符串索引*/,
    /*  --【HID类描述符】--   */
    USB_HID_Class_Length,USB_HID_Class_Type,    0x10,0x01,  /*USB1.1 即 0x0110,小段结构写成 0x10,0x01*/
    0x00/*本类设置为 00*/    ,0x01/*本类有【一个下级描述符】*/   ,0x22/*本类的【下级描述】符为【报告描述符】*/ ,
    USB_Report_Length & 0x00ff,USB_Report_Length >> 8/*本配类【下级描述符】的长度*/,
    /*  --【端口类描述符】--    */
    //输入
    USB_Endpoint_Length,USB_Endpoint_Type,0x82/*本端口是输入的（0x80）,端口2*/ ,
    0x03/*本端口为中断接口 11b（0x03）*/  ,0x40,0x00/*本端口最大 0x0040包长*/    ,0x01/*本端口扫描时间1帧内*/,
    //输出
    USB_Endpoint_Length,USB_Endpoint_Type,0x02/*本端口是输出的（0x00）,端口2*/ ,
    0x03/*本端口为中断接口 11b（0x03）*/  ,0x40,0x00/*本端口最大 16包长*/    ,0x01/*本端口扫描时间1帧内*/,


};

/* USB String Descriptors */    //USB语言ID字符串
const uint8_t USBD_StringLangID[USBD_SIZE_STRING_LANGID] = {
    USBD_SIZE_STRING_LANGID,
    USB_String_TYPE,
    0x09,0x04                   //设置为美式英语
};

/* USB Device String Vendor */      //USB设备字符串厂商
const uint8_t USBD_StringVendor[USBD_SIZE_STRING_VENDOR] = {
    USBD_SIZE_STRING_VENDOR,
    USB_String_TYPE,
    'G', 0, 'X', 0, 'W', 0,'L', 0,
};

/* USB Device String Product */     //USB产品字符串
const uint8_t USBD_StringProduct[USBD_SIZE_STRING_PRODUCT] = {
    USBD_SIZE_STRING_PRODUCT,
    USB_String_TYPE,
    'H',0,'I',0,'D',0,'-',0,'K',0,'e',0,'y',0,'b',0,'o',0,'a',0,'r',0,'d',0,
};

/* USB Device String Serial */      //USB序列串字符串
uint8_t USBD_StringSerial[USBD_SIZE_STRING_SERIAL] = {
    USBD_SIZE_STRING_SERIAL,
    USB_String_TYPE,
    'W', 0, 'C', 0, 'H', 0, '3', 0, '2', 0
};

/* HID Report Descriptor */         //报告描述符
const uint8_t USBD_HIDRepDesc[USB_Report_Length] =
{
    0x06, 0x00,0xff,
    0x09, 0x01,
    0xa1, 0x01,
    0x09, 0x02,             /* Usage Page */
    0x15, 0x00,             /* Logical  Minimun */
    0x26, 0x00,0xff,        /* Logical  Maximun */
    0x75, 0x08,             /* Report Size */
    0x95, 0x40,             /* Report Counet */
    0x81, 0x06,             /* Input */
    0x09, 0x02,             /* Usage Page */
    0x15, 0x00,             /* Logical  Minimun */
    0x26, 0x00,0xff,        /* Logical  Maximun */
    0x75, 0x08,             /* Report Size */
    0x95, 0x40,             /* Report Counet */
    0x91, 0x06,             /* Output */
    0xC0,
};
const uint8_t USBD_KeyRepDesc[KEYBOARD_Report_Length] =   // dd-dd
{
    0x05,0x01,0x09,0x06,0xA1,0x01,0x05,0x07,
    0x19,0xe0,0x29,0xe7,0x15,0x00,0x25,0x01,
    0x75,0x01,0x95,0x08,0x81,0x02,0x95,0x01,
    0x75,0x08,0x81,0x01,0x95,0x03,0x75,0x01,
    0x05,0x08,0x19,0x01,0x29,0x03,0x91,0x02,
    0x95,0x05,0x75,0x01,0x91,0x01,0x95,0x06,
    0x75,0x08,0x26,0xff,0x00,0x05,0x07,0x19,
    0x00,0x29,0x91,0x81,0x00,0xC0,
};

