/**
  ******************************************************************************
  * File   : usbd_cdc_core_loopback.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : header file for the usbd_cdc_core_loopback.c file
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/

#ifndef __USB_CDC_CORE_H_
#define __USB_CDC_CORE_H_

#include  "usbd_ioreq.h"

/** @addtogroup AT32_USB_OTG_DEVICE_LIBRARY
  * @{
  */
  
/** @defgroup usbd_cdc
  * @brief This file is the Header file for USBD_cdc.c
  * @{
  */ 


/** @defgroup usbd_cdc_Exported_Defines
  * @{
  */ 
  
/**************************************************/
/* CDC Requests                                   */
/**************************************************/
#define SEND_ENCAPSULATED_COMMAND               0x00
#define GET_ENCAPSULATED_RESPONSE               0x01
#define SET_COMM_FEATURE                        0x02
#define GET_COMM_FEATURE                        0x03
#define CLEAR_COMM_FEATURE                      0x04
#define SET_AUX_LINE_STATE                      0x10
#define SET_HOOK_STATE                          0x11
#define PULSE_SETUP                             0x12
#define SEND_PULSE                              0x13
#define SET_PULSE_TIME                          0x14
#define RING_AUX_JACK                           0x15
#define SET_LINE_CODING                         0x20
#define GET_LINE_CODING                         0x21
#define SET_CONTROL_LINE_STATE                  0x22
#define SEND_BREAK                              0x23
#define SET_RINGER_PARMS                        0x30
#define GET_RINGER_PARMS                        0x31
#define SET_OPERATION_PARMS                     0x32
#define GET_OPERATION_PARMS                     0x33
#define SET_LINE_PARMS                          0x34
#define GET_LINE_PARMS                          0x35
#define DIAL_DIGITS                             0x36
#define SET_UNIT_PARAMETER                      0x37
#define GET_UNIT_PARAMETER                      0x38
#define CLEAR_UNIT_PARAMETER                    0x39
#define GET_PROFILE                             0x3A
#define SET_ETHERNET_MULTICAST_FILTERS          0x40
#define SET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER    0x41
#define GET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER    0x42
#define SET_ETHERNET_PACKET_FILTER              0x43
#define GET_ETHERNET_STATISTIC                  0x44
#define SET_ATM_DATA_FORMAT                     0x50
#define GET_ATM_DEVICE_STATISTICS               0x51
#define SET_ATM_DEFAULT_VC                      0x52
#define GET_ATM_VC_STATISTICS                   0x53
#define GET_NTB_PARAMETERS                      0x80
#define GET_NET_ADDRESS                         0x81
#define SET_NET_ADDRESS                         0x82
#define GET_NTB_FORMAT                          0x83
#define SET_NTB_FORMAT                          0x84
#define GET_NTB_INPUT_SIZE                      0x85
#define SET_NTB_INPUT_SIZE                      0x86
#define GET_MAX_DATAGRAM_SIZE                   0x87
#define SET_MAX_DATAGRAM_SIZE                   0x88
#define GET_CRC_MODE                            0x89
#define SET_CRC_MODE                            0x8A
#define NO_CMD                                  0xFF

#define USB_CDC_CONFIG_DESC_SIZ                (67)
#define USB_CDC_DESC_SIZ                       (67-9)

#define DEVICE_CLASS_CDC                        0x02
#define DEVICE_SUBCLASS_CDC                     0x00

#define CDC_DATA_IN_PACKET_SIZE                CDC_DATA_MAX_PACKET_SIZE
        
#define CDC_DATA_OUT_PACKET_SIZE               CDC_DATA_MAX_PACKET_SIZE

/**
  * @}
  */ 
/** @defgroup USBD_CORE_Exported_TypesDefinitions
  * @{
  */
typedef struct _CDC_IF_PROP
{
  uint16_t (*pcdc_Init)     (void);   
  uint16_t (*pcdc_Reset)   (void);   
  uint16_t (*pcdc_Ctrl)     (uint32_t Cmd, uint8_t* Buf, uint32_t Len);
  uint16_t (*pcdc_DataTx)   (void);
  uint16_t (*pcdc_DataRx)   (uint8_t *rx_buffer, uint32_t Len);
}
CDC_IF_Prop_Type;
/**
  * @}
  */ 



/** @defgroup USBD_CORE_Exported_Macros
  * @{
  */ 
  
/**
  * @}
  */ 

/** @defgroup USBD_CORE_Exported_Variables
  * @{
  */ 

extern USBD_Class_cb_Type  USB_DEVICE_CDC_cb;
/**
  * @}
  */ 

/** @defgroup USB_CORE_Exported_Functions
  * @{
  */
/**
  * @}
  */ 

#endif  /* __USB_CDC_CORE_H_ */
/**
  * @}
  */ 

/**
  * @}
  */ 
  
