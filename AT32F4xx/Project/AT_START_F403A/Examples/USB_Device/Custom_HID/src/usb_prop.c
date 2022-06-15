/**
  ******************************************************************************
  * File   : USB_Device/Custom_HID/src/usb_prop.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : All processing related to Custom_HID
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_conf.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "hw_config.h"
#include "at32_board.h"
/** @addtogroup AT32F413_StdPeriph_Examples
  * @{
  */

/** @addtogroup USB_Custom_HID
  * @{
  */ 
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/ 
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t ProtocolValue;  ///the protocol value
__IO uint8_t EXTI_Enable;
__IO uint8_t Request = 0;
uint8_t Report_Buf[27];  
uint8_t idel_buf[2];
/* -------------------------------------------------------------------------- */
/*  Structures initializations */
/* -------------------------------------------------------------------------- */

DEVICE Device_Table =
{
  EP_NUM,
  1
};

/** 
  * @brief  function pointers to Device Property 
  */
DEVICE_PROP Device_Property =
{
  CustomHID_init,
  CustomHID_Reset,
  CustomHID_Status_In,
  CustomHID_Status_Out,
  CustomHID_Data_Setup,
  CustomHID_NoData_Setup,
  CustomHID_Get_Interface_Setting,
  CustomHID_GetDeviceDescriptor,
  CustomHID_GetConfigDescriptor,
  CustomHID_GetStringDescriptor,
  0,
  0x40 /*MAX PACKET SIZE*/
};
/** 
  * @brief  function pointers to usb standard requests
  */
USER_STANDARD_REQUESTS User_Standard_Requests =
{
  CustomHID_GetConfiguration,
  CustomHID_SetConfiguration,
  CustomHID_GetInterface,
  CustomHID_SetInterface,
  CustomHID_GetStatus,
  CustomHID_ClearFeature,
  CustomHID_SetEndPointFeature,
  CustomHID_SetDeviceFeature,
  CustomHID_SetDeviceAddress
};
/** 
  * @brief  USB Device Descriptor pointer and length
  */
ONE_DESCRIPTOR Device_Descriptor =
{
  (uint8_t*)CustomHID_DeviceDescriptor,
  CUSTOMHID_SIZ_DEVICE_DESC
};
/** 
  * @brief  USB Config Descriptor pointer and length
  */
ONE_DESCRIPTOR Config_Descriptor =
{
  (uint8_t*)CustomHID_ConfigDescriptor,
  CUSTOMHID_SIZ_CONFIG_DESC
};
/** 
  * @brief  USB HID Report Descriptor pointer and length
  */
ONE_DESCRIPTOR CustomHID_Report_Descriptor =
{
  (uint8_t *)CustomHID_ReportDescriptor,
  CUSTOMHID_SIZ_REPORT_DESC
};
/** 
  * @brief  USB HID Descriptor pointer and length
  */
ONE_DESCRIPTOR CustomHID_Hid_Descriptor =
{
  (uint8_t*)CustomHID_ConfigDescriptor + CUSTOMHID_OFF_HID_DESC,
  CUSTOMHID_SIZ_HID_DESC
};

/** 
  * @brief  USB Sring Descriptor pointer and length
  */
ONE_DESCRIPTOR String_Descriptor[4] =
{
  {(uint8_t*)CustomHID_StringLangID, CUSTOMHID_SIZ_STRING_LANGID},    ///LangID Pointer and length
  {(uint8_t*)CustomHID_StringVendor, CUSTOMHID_SIZ_STRING_VENDOR},    ///Vendor Pointer and length
  {(uint8_t*)CustomHID_StringProduct, CUSTOMHID_SIZ_STRING_PRODUCT},  ///Product Pointer and length
  {(uint8_t*)CustomHID_StringSerial, CUSTOMHID_SIZ_STRING_SERIAL}     ///Serial Pointer and length
};

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/*CustomHID_SetReport_Feature function prototypes*/
uint8_t *CustomHID_SetReport_Feature(uint16_t Length);

/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Custom HID init routine.
  * @param  None
  * @retval None
  */
void CustomHID_init(void)
{
  /* Update the serial number string descriptor with the data from the unique 
  ID*/
  Get_SerialNum();
    
  pInformation->Current_Configuration = 0;
  /* Connect the device */
  PowerOn();

  /* Perform basic device initialization operations */
  USB_SIL_Init();

  bDeviceState = UNCONNECTED;
}

/**
  * @brief  Custom HID reset routine.
  * @param  None
  * @retval None
  */
void CustomHID_Reset(void)
{
  /* Set CustomHID_DEVICE as not configured */
  pInformation->Current_Configuration = 0;
  pInformation->Current_Interface = 0;/*the default Interface*/
  
  /* Current Feature initialization */
  pInformation->Current_Feature = CustomHID_ConfigDescriptor[7];
 
  _SetBUFTBL(BTABLE_ADDRESS);

  /* Initialize Endpoint 0 */
  SetEPType(ENDP0, EP_CONTROL);
  SetEPTxStatus(ENDP0, EP_TX_STALL);
  SetEPRxAddr(ENDP0, ENDP0_RXADDR);
  SetEPTxAddr(ENDP0, ENDP0_TXADDR);
  Clear_Status_Out(ENDP0);
  SetEPRxCount(ENDP0, Device_Property.MaxPacketSize);
  SetEPRxValid(ENDP0);

  /* Initialize Endpoint 1 */
  SetEPType(ENDP1, EP_INTERRUPT);
  SetEPTxAddr(ENDP1, ENDP1_TXADDR);
  SetEPRxAddr(ENDP1, ENDP1_RXADDR);
  SetEPTxCount(ENDP1, 0x40);
  SetEPRxCount(ENDP1, 0x40);
  SetEPRxStatus(ENDP1, EP_RX_VALID);
  SetEPTxStatus(ENDP1, EP_TX_NAK);
  
  /* Set this device to response on default address */
  SetDeviceAddress(0);
  bDeviceState = ATTACHED;
}

/**
  * @brief  Updata the device state to configured.
  * @param  None
  * @retval None
  */
void CustomHID_SetConfiguration(void)
{
  if (pInformation->Current_Configuration != 0)
  {
    /* Device configured */
    bDeviceState = CONFIGURED;
  }
}
/**
  * @brief  Updata the device state to addressed.
  * @param  None
  * @retval None
  */
void CustomHID_SetDeviceAddress (void)
{
  bDeviceState = ADDRESSED;
}

/**
  * @brief  Custom HID status IN routine.
  * @param  None
  * @retval None
  */
void CustomHID_Status_In(void)
{  
}

/**
  * @brief  Custom HID status OUT routine.
  * @param  None
  * @retval None
  */
void CustomHID_Status_Out (void)
{
}

/**
  * @brief  Handle the data class specific requests.
  * @param  RequestNo: USB request number
  * @retval if support the request return Number USB_SUCCESS else return USB_UPSUPPORT
  */
RESULT CustomHID_Data_Setup(uint8_t RequestNo)
{
  uint8_t *(*CopyRoutine)(uint16_t);
  
  if (pInformation->USBwIndex != 0) 
    return USB_UNSUPPORT;    
  
  CopyRoutine = NULL;
  
  if ((RequestNo == GET_DESCRIPTOR)
      && (Type_Recipient == (STANDARD_REQUEST | INTERFACE_RECIPIENT))
        )
  {
    
    if (pInformation->USBwValue1 == REPORT_DESCRIPTOR)
    {
      CopyRoutine = CustomHID_GetReportDescriptor;
    }
    else if (pInformation->USBwValue1 == HID_DESCRIPTOR_TYPE)
    {
      CopyRoutine = CustomHID_GetHIDDescriptor;
    }
    
  } /* End of GET_DESCRIPTOR */
  
  /*** GET_PROTOCOL, GET_REPORT, SET_REPORT ***/
  else if ( (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT)) )
  {         
    switch( RequestNo )
    {
    case GET_PROTOCOL:
      CopyRoutine = CustomHID_GetProtocolValue;
      break;
    case SET_REPORT:
      CopyRoutine = CustomHID_SetReport_Feature;
      Request = SET_REPORT;
      break;
    default:
      break;
    }
  }
  
  if (CopyRoutine == NULL)
  {
    return USB_UNSUPPORT;
  }
  
  pInformation->Ctrl_Info.CopyData = CopyRoutine;
  pInformation->Ctrl_Info.Usb_wOffset = 0;
  (*CopyRoutine)(0);
  return USB_SUCCESS;
}

/**
  * @brief  Set Feature request handling.
  * @param  Length: length of report feature
  * @retval the pointer of report buffer
  */
uint8_t *CustomHID_SetReport_Feature(uint16_t Length)
{
  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = 2;
    return NULL;
  }
  else
  {
    return Report_Buf;
  }
}

/**
  * @brief  Set IDLE request handling.
  * @param  None.
  * @retval USB_SUCCESS
  */
RESULT CustomHID_SetIDLE_Feature()
{
  return USB_SUCCESS;
}

/**
  * @brief  handle the no data class specific requests.
  * @param  RequestNo: USB request number
  * @retval if support the request return Number USB_SUCCESS else return USB_UPSUPPORT
  */
RESULT CustomHID_NoData_Setup(uint8_t RequestNo)
{
  if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
      && (RequestNo == SET_PROTOCOL))
  {
    return CustomHID_SetProtocol();
  }
  else if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
      && (RequestNo == SET_IDLE))
  {
      return CustomHID_SetIDLE_Feature();

  }
  else
  {
    return USB_UNSUPPORT;
  }
}

/**
  * @brief  Gets the device descriptor.
  * @param  Length: the device descriptor length
  * @retval The address of the device descriptor.
  */
uint8_t *CustomHID_GetDeviceDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Device_Descriptor);
}

/**
  * @brief  Gets the configuration descriptor.
  * @param  Length: the config descriptor length
  * @retval The address of the configuration descriptor.
  */
uint8_t *CustomHID_GetConfigDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Config_Descriptor);
}

/**
  * @brief  Gets the string descriptors according to the needed index.
  * @param  Length: the string descriptor length
  * @retval The address of the string descriptor.
  */
uint8_t *CustomHID_GetStringDescriptor(uint16_t Length)
{
  uint8_t wValue0 = pInformation->USBwValue0;
  if (wValue0 > 4)
  {
    return NULL;
  }
  else 
  {
    return Standard_GetDescriptorData(Length, &String_Descriptor[wValue0]);
  }
}

/**
  * @brief  Gets the Custom HID report descriptor.
  * @param  Length: the Report descriptor length
  * @retval The address of the Report descriptor.
  */
uint8_t *CustomHID_GetReportDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &CustomHID_Report_Descriptor);
}

/**
  * @brief  Gets the HID descriptor.
  * @param  Length: the HID descriptor length
  * @retval The address of the HID descriptor.
  */
uint8_t *CustomHID_GetHIDDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &CustomHID_Hid_Descriptor);
}

/**
  * @brief  tests the interface and the alternate setting according to the
  *         supported one.
  * @param  Interface: interface number
  * @param  AlternateSetting: Alternate Setting number.
  * @retval if support the interface and  AlternateSetting return Number USB_SUCCESS 
  *         else return USB_UPSUPPORT
  */
RESULT CustomHID_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting)
{
  if (AlternateSetting > 0)
  {
    return USB_UNSUPPORT;
  }
  else if (Interface > 0)
  {
    return USB_UNSUPPORT;
  }
  return USB_SUCCESS;
}

/**
  * @brief  Custom HID Set Protocol request routine.
  * @param  None.
  * @retval USB_SUCCESS.
  */
RESULT CustomHID_SetProtocol(void)
{
  uint8_t wValue0 = pInformation->USBwValue0;
  ProtocolValue = wValue0;
  return USB_SUCCESS;
}

/**
  * @brief  Get the protocol value
  * @param  Length: the protocol value length
  * @retval address of the protcol value..
  */
uint8_t *CustomHID_GetProtocolValue(uint16_t Length)
{
  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = 1;
    return NULL;
  }
  else
  {
    return (uint8_t *)(&ProtocolValue);
  }
}

/**
  * @}
  */ 

/**
  * @}
  */
