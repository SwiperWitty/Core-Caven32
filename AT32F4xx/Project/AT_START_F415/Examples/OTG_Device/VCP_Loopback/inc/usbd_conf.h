/**
  ******************************************************************************
  * File   : usbd_conf.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : USB Device configuration file
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_CONF__H__
#define __USBD_CONF__H__

/* Includes ------------------------------------------------------------------*/
#include "usb_conf.h"

/** @defgroup USB_CONF_Exported_Defines
  * @{
  */ 
#define USBD_CFG_MAX_NUM                1
#define USBD_ITF_MAX_NUM                1

#define USBD_SELF_POWERED               

#define USB_MAX_STR_DESC_SIZ            255 

/** @defgroup USB_VCP_Class_Layer_Parameter
  * @{
  */ 
#define CDC_IN_EP                       0x81  /* EP1 for data IN */
#define CDC_OUT_EP                      0x01  /* EP1 for data OUT */
#define CDC_CMD_EP                      0x82  /* EP2 for CDC commands */

/* CDC Endpoints parameters: you can fine tune these values depending on the needed baudrates and performance. */
#ifdef USE_USB_OTG_HS
 #define CDC_DATA_MAX_PACKET_SIZE       512  /* Endpoint IN & OUT Packet size */
 #define CDC_CMD_PACKET_SZE             8    /* Control Endpoint Packet size */

 #define CDC_IN_FRAME_INTERVAL          40   /* Number of micro-frames between IN transfers */
 #define APP_RX_DATA_SIZE               2048 /* Total size of IN buffer: 
                                                APP_RX_DATA_SIZE*8/MAX_BAUDARATE*1000 should be > CDC_IN_FRAME_INTERVAL*8 */
#else
 #define CDC_DATA_MAX_PACKET_SIZE       64   /* Endpoint IN & OUT Packet size */
 #define CDC_CMD_PACKET_SZE             8    /* Control Endpoint Packet size */

 #define CDC_IN_FRAME_INTERVAL          5    /* Number of frames between IN transfers */
 #define APP_RX_DATA_SIZE               2048 /* Total size of IN buffer: 
                                                APP_RX_DATA_SIZE*8/MAX_BAUDARATE*1000 should be > CDC_IN_FRAME_INTERVAL */
#endif /* USE_USB_OTG_HS */

#define APP_FOPS                        CDC_vcp_fops
/**
  * @}
  */ 

/** @defgroup USB_CONF_Exported_Types
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USB_CONF_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USB_CONF_Exported_Variables
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USB_CONF_Exported_FunctionsPrototype
  * @{
  */ 
/**
  * @}
  */ 


#endif //__USBD_CONF__H__


