/**
  ******************************************************************************
  * File   : usb_core.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Header of the Core Layer
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_CORE_H__
#define __USB_CORE_H__

/* Includes ------------------------------------------------------------------*/
#include "usb_conf.h"
#include "usb_regs.h"
#include "usb_defines.h"


/** @addtogroup USB_OTG_DRIVER
  * @{
  */
  
/** @defgroup USB_CORE
  * @brief usb otg driver core layer
  * @{
  */ 


/** @defgroup USB_CORE_Exported_Defines
  * @{
  */ 

#define USB_OTG_EPT0_IDLE                          0
#define USB_OTG_EPT0_SETUP                         1
#define USB_OTG_EPT0_DATAIN                       2
#define USB_OTG_EPT0_DATAOUT                      3
#define USB_OTG_EPT0_STATUSIN                     4
#define USB_OTG_EPT0_STATUSOUT                    5
#define USB_OTG_EPT0_STALL                         6

#define USB_OTG_EPT_TX_DIS       0x0000
#define USB_OTG_EPT_TX_STALL     0x0010
#define USB_OTG_EPT_TX_NAK       0x0020
#define USB_OTG_EPT_TX_VALID     0x0030
 
#define USB_OTG_EPT_RX_DIS       0x0000
#define USB_OTG_EPT_RX_STALL     0x1000
#define USB_OTG_EPT_RX_NAK       0x2000
#define USB_OTG_EPT_RX_VALID     0x3000
/**
  * @}
  */ 
#define   USB_MAX_DATA_LENGTH                        0x200

/** @defgroup USB_CORE_Exported_Types
  * @{
  */ 


typedef enum {
  USB_OTG_OK = 0,
  USB_OTG_FAIL
}USB_OTG_STS;

typedef enum {
  HC_IDLE = 0,
  HC_XFRC,
  HC_HALTED,
  HC_NAK,
  HC_NYET,
  HC_STALL,
  HC_XACTERR,  
  HC_BBLERR,   
  HC_DATATGLERR,  
}HCH_STS;

typedef enum {
  URB_IDLE = 0,
  URB_DONE,
  URB_NOTREADY,
  URB_ERROR,
  URB_STALL
}URB_STATE;

typedef enum {
  CTRL_START = 0,
  CTRL_XFRC,
  CTRL_HALTED,
  CTRL_NAK,
  CTRL_STALL,
  CTRL_XACTERR,  
  CTRL_BBLERR,   
  CTRL_DATATGLERR,  
  CTRL_FAIL
}CTRL_STATUS;


typedef struct USB_OTG_hc
{
  uint8_t       dev_addr ;
  uint8_t       ept_num;
  uint8_t       ept_is_in;
  uint8_t       speed;
  uint8_t       do_ping;  
  uint8_t       ept_type;
  uint16_t      max_packet;
  uint8_t       data_pid;
  uint8_t       *xfer_buff;
  uint32_t      xfer_len;
  uint32_t      xfer_count;  
  uint8_t       toggle_in;
  uint8_t       toggle_out;
  uint32_t       dma_adr;  
}
USB_OTG_HCH , *PUSB_OTG_HCH;

typedef struct USB_OTG_ept
{
  uint8_t        num;
  uint8_t        is_in;
  uint8_t        is_stall;  
  uint8_t        ept_type;
  uint8_t        data_pid_start;
  uint8_t        even_odd_frame;
  uint16_t       tx_fifo_num;
  uint32_t       maxpacket;
  /* transaction level variables*/
  uint8_t        *xfer_buff;
  uint32_t       dma_addr;  
  uint32_t       xfer_len;
  uint32_t       xfer_count;
  /* Transfer level variables*/  
  uint32_t       rem_data_len;
  uint32_t       total_data_len;
  uint32_t       ctl_data_len;  

}

USB_OTG_EPT , *PUSB_OTG_EPT;



typedef struct USB_OTG_core_cfg
{
  uint8_t       host_channels;
  uint8_t       dev_endpoints;
  uint8_t       speed;
//  uint8_t       dma_enable;
  uint16_t      maxpsize;
  uint16_t      totalfifosize;
  uint8_t       phy_itface;
  uint8_t       sof_output;
  uint8_t       low_power;
  uint8_t       coreid;
 
}
USB_OTG_CORE_CFGS, *PUSB_OTG_CORE_CFGS;



typedef  struct  usb_setup_reqest {
    
    uint8_t   bmRequest;                      
    uint8_t   bRequest;                           
    uint16_t  wValue;                             
    uint16_t  wIndex;                             
    uint16_t  wLength;                            
} USB_SETUP_REQEST;

typedef struct _Device_TypeDef
{
  uint8_t  *(*GetDeviceDescriptor)( uint8_t speed , uint16_t *length);  
  uint8_t  *(*GetLangIDStrDescriptor)( uint8_t speed , uint16_t *length); 
  uint8_t  *(*GetManufacturerStrDescriptor)( uint8_t speed , uint16_t *length);  
  uint8_t  *(*GetProductStrDescriptor)( uint8_t speed , uint16_t *length);  
  uint8_t  *(*GetSerialStrDescriptor)( uint8_t speed , uint16_t *length);  
  uint8_t  *(*GetConfigurationStrDescriptor)( uint8_t speed , uint16_t *length);  
  uint8_t  *(*GetInterfaceStrDescriptor)( uint8_t speed , uint16_t *length); 

#if (USBD_LPM_ENABLED == 1)
  uint8_t  *(*GetBOSDescriptor)( uint8_t speed , uint16_t *length); 
#endif   
} USB_DEVICE_USER, *pUSB_DEVICE_USER;

typedef struct _Device_cb
{
  uint8_t  (*Init)         (void *pusbdev , uint8_t cfgidx);
  uint8_t  (*Reset)       (void *pusbdev , uint8_t cfgidx);
 /* Control Endpoints*/
  uint8_t  (*Setup)        (void *pusbdev , USB_SETUP_REQEST  *req);  
  uint8_t  (*EPT0_TxSent)   (void *pusbdev );    
  uint8_t  (*EPT0_RxReady)  (void *pusbdev );  
  /* Class Specific Endpoints*/
  uint8_t  (*Data_In)       (void *pusbdev , uint8_t eptnum);   
  uint8_t  (*Data_Out)      (void *pusbdev , uint8_t eptnum); 
  uint8_t  (*SOF)          (void *pusbdev); 
  uint8_t  (*IsoINIncomplete)  (void *pusbdev); 
  uint8_t  (*IsoOUTIncomplete)  (void *pusbdev);   

  uint8_t  *(*GetConfigDescriptor)( uint8_t speed , uint16_t *length); 
//#ifdef USB_OTG_HS_CORE 
//  uint8_t  *(*GetOtherConfigDescriptor)( uint8_t speed , uint16_t *length);   
//#endif

#ifdef USB_SUPPORT_USER_STRING_DESC 
  uint8_t  *(*GetUsrStrDescriptor)( uint8_t speed ,uint8_t index,  uint16_t *length);   
#endif  
  
} USBD_Class_cb_Type;



typedef struct _USBD_USR_PROP
{
  void (*Init)(void);   
  void (*DeviceReset)(uint8_t speed); 
  void (*DeviceConfigured)(void);
  void (*DeviceSuspended)(void);
  void (*DeviceResumed)(void);  
  
  void (*DeviceConnected)(void);  
  void (*DeviceDisconnected)(void);    
  
}
USB_Device_User_cb_Type;

typedef struct _DCD
{
  uint8_t        device_config;
  uint8_t        device_state;
  uint8_t        device_status;
  uint8_t        device_old_status;
  uint8_t        device_address;
  uint8_t        connection_status;  
  uint8_t        test_mode;
  uint32_t       DeviceRemoteWakeup;
  USB_OTG_EPT     in_ept   [USB_OTG_MAX_TX_FIFO];
  USB_OTG_EPT     out_ept  [USB_OTG_MAX_TX_FIFO];
  uint8_t        setup_packet [8*3];
  USBD_Class_cb_Type         *class_cb;
  USB_Device_User_cb_Type           *user_cb;
  USB_DEVICE_USER                   *usr_device;  
  uint8_t        *pConfig_descriptor;
 }
DCD_DEVICE , *DCD_PDEVICE;


typedef struct _HCD
{
  uint8_t                  Rx_Buffer [USB_MAX_DATA_LENGTH];  
  __IO uint32_t            ConnectSts;
  __IO uint32_t            PortEnabled;
  __IO uint32_t            ErrCnt[USB_OTG_MAX_TX_FIFO];
  __IO uint32_t            XferCnt[USB_OTG_MAX_TX_FIFO];
  __IO HCH_STS           HCH_Status[USB_OTG_MAX_TX_FIFO];  
  __IO URB_STATE           URB_State[USB_OTG_MAX_TX_FIFO];
  USB_OTG_HCH               hc [USB_OTG_MAX_TX_FIFO];
  uint16_t                 channel [USB_OTG_MAX_TX_FIFO];
}
HCD_DEV , *USB_OTG_USBH_PDEV;


typedef struct _OTG
{
  uint8_t    OTG_State;
  uint8_t    OTG_PrevState;  
  uint8_t    OTG_Mode;    
}
OTG_DEV , *USB_OTG_USBO_PDEV;

typedef struct USB_OTG_handle
{
  USB_OTG_CORE_CFGS    cfg;
  USB_OTG_CORE_REGS    regs;
#ifdef USE_DEVICE_MODE
  DCD_DEVICE     dev;
#endif
//#ifdef USE_HOST_MODE
  HCD_DEV     host;
//#endif
#ifdef USE_OTG_MODE
  OTG_DEV     otg;
#endif
}
USB_OTG_CORE_HANDLE , *PUSB_OTG_CORE_HANDLE;

/**
  * @}
  */ 


/** @defgroup USB_CORE_Exported_Macros
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup USB_CORE_Exported_Variables
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USB_CORE_Exported_FunctionsPrototype
  * @{
  */ 


USB_OTG_STS  USB_OTG_CoreInit        (USB_OTG_CORE_HANDLE *pusbdev);
USB_OTG_STS  USB_OTG_SelectSpeed      (USB_OTG_CORE_HANDLE *pusbdev, 
                                      USB_OTG_CORE_ID_TypeDef coreid);
USB_OTG_STS  USB_OTG_EnableGlobalInt (USB_OTG_CORE_HANDLE *pusbdev);
USB_OTG_STS  USB_OTG_DisableGlobalInt(USB_OTG_CORE_HANDLE *pusbdev);
void*           USB_OTG_ReadPacket   (USB_OTG_CORE_HANDLE *pusbdev ,
    uint8_t *dest,
    uint16_t len);
USB_OTG_STS  USB_OTG_WritePacket     (USB_OTG_CORE_HANDLE *pusbdev ,
    uint8_t *src,
    uint8_t ch_ep_num,
    uint16_t len);
USB_OTG_STS  USB_OTG_FlushTxFifo     (USB_OTG_CORE_HANDLE *pusbdev , uint32_t num);
USB_OTG_STS  USB_OTG_FlushRxFifo     (USB_OTG_CORE_HANDLE *pusbdev);

uint32_t     USB_OTG_ReadCoreInterruptr     (USB_OTG_CORE_HANDLE *pusbdev);
uint32_t     USB_OTG_ReadOtgIterruptr      (USB_OTG_CORE_HANDLE *pusbdev);
uint8_t      USB_OTG_IsHostMode      (USB_OTG_CORE_HANDLE *pusbdev);
uint8_t      USB_OTG_IsDeviceMode    (USB_OTG_CORE_HANDLE *pusbdev);
uint32_t     USB_OTG_GetMode         (USB_OTG_CORE_HANDLE *pusbdev);
USB_OTG_STS  USB_OTG_PhyInit         (USB_OTG_CORE_HANDLE *pusbdev);
USB_OTG_STS  USB_OTG_SetCurrentMode  (USB_OTG_CORE_HANDLE *pusbdev,
    uint8_t mode);

/*********************** HOST APIs ********************************************/
//#ifdef USE_HOST_MODE
USB_OTG_STS  USB_OTG_CoreInitHost    (USB_OTG_CORE_HANDLE *pusbdev);
USB_OTG_STS  USB_OTG_EnableHostInt   (USB_OTG_CORE_HANDLE *pusbdev);
USB_OTG_STS  USB_OTG_HCH_Init         (USB_OTG_CORE_HANDLE *pusbdev, uint8_t hc_num);
USB_OTG_STS  USB_OTG_HCH_Halt         (USB_OTG_CORE_HANDLE *pusbdev, uint8_t hc_num);
USB_OTG_STS  USB_OTG_HCH_StartXfer    (USB_OTG_CORE_HANDLE *pusbdev, uint8_t hc_num);
USB_OTG_STS  USB_OTG_HCH_DoPing       (USB_OTG_CORE_HANDLE *pusbdev , uint8_t hc_num);
uint32_t     USB_OTG_ReadHostAllChannels_intr    (USB_OTG_CORE_HANDLE *pusbdev);
uint32_t     USB_OTG_ResetPort       (USB_OTG_CORE_HANDLE *pusbdev);
uint32_t     USB_OTG_ReadHPRT0       (USB_OTG_CORE_HANDLE *pusbdev);
void         USB_OTG_DriveVbus       (USB_OTG_CORE_HANDLE *pusbdev, uint8_t state);
void         USB_OTG_InitFSLSPClkSel (USB_OTG_CORE_HANDLE *pusbdev ,uint8_t freq);
uint8_t      USB_OTG_IsEvenFrame     (USB_OTG_CORE_HANDLE *pusbdev) ;
void         USB_OTG_StopHost        (USB_OTG_CORE_HANDLE *pusbdev);
//#endif
/********************* DEVICE APIs ********************************************/
#ifdef USE_DEVICE_MODE
USB_OTG_STS  USB_OTG_CoreInitDev         (USB_OTG_CORE_HANDLE *pusbdev);
USB_OTG_STS  USB_OTG_EnableDevInt        (USB_OTG_CORE_HANDLE *pusbdev);
uint32_t     USB_OTG_ReadDevAllInEPItr           (USB_OTG_CORE_HANDLE *pusbdev);
enum USB_OTG_SPEED_MODE USB_OTG_GetDeviceSpeed (USB_OTG_CORE_HANDLE *pusbdev);
USB_OTG_STS  USB_OTG_EPT0Activate (USB_OTG_CORE_HANDLE *pusbdev);
USB_OTG_STS  USB_OTG_EPTActivate  (USB_OTG_CORE_HANDLE *pusbdev , USB_OTG_EPT *ept);
USB_OTG_STS  USB_OTG_EPTDeactivate(USB_OTG_CORE_HANDLE *pusbdev , USB_OTG_EPT *ept);
USB_OTG_STS  USB_OTG_EPTStartXfer (USB_OTG_CORE_HANDLE *pusbdev , USB_OTG_EPT *ept);
USB_OTG_STS  USB_OTG_EPT0StartXfer(USB_OTG_CORE_HANDLE *pusbdev , USB_OTG_EPT *ept);
USB_OTG_STS  USB_OTG_EPTSetStall          (USB_OTG_CORE_HANDLE *pusbdev , USB_OTG_EPT *ept);
USB_OTG_STS  USB_OTG_EPTClearStall        (USB_OTG_CORE_HANDLE *pusbdev , USB_OTG_EPT *ept);
uint32_t     USB_OTG_ReadDevAllOutEp_itr (USB_OTG_CORE_HANDLE *pusbdev);
uint32_t     USB_OTG_ReadDevOutEP_itr    (USB_OTG_CORE_HANDLE *pusbdev , uint8_t eptnum);
uint32_t     USB_OTG_ReadDevAllInEPItr   (USB_OTG_CORE_HANDLE *pusbdev);
void         USB_OTG_InitDevSpeed        (USB_OTG_CORE_HANDLE *pusbdev , uint8_t speed);
uint8_t      USB_HOST_IsEvenFrame (USB_OTG_CORE_HANDLE *pusbdev);
void         USB_OTG_EPT0_OutStart(USB_OTG_CORE_HANDLE *pusbdev);
void         USB_OTG_ActiveRemoteWakeup(USB_OTG_CORE_HANDLE *pusbdev);
void         USB_OTG_UngateClock(USB_OTG_CORE_HANDLE *pusbdev);
void         USB_OTG_StopDevice(USB_OTG_CORE_HANDLE *pusbdev);
void         USB_OTG_SetEPStatus (USB_OTG_CORE_HANDLE *pusbdev , USB_OTG_EPT *ept , uint32_t Status);
uint32_t     USB_OTG_GetEPStatus(USB_OTG_CORE_HANDLE *pusbdev ,USB_OTG_EPT *ept);
#endif
/**
  * @}
  */ 

#endif  /* __USB_CORE_H__ */


/**
  * @}
  */ 

/**
  * @}
  */ 

