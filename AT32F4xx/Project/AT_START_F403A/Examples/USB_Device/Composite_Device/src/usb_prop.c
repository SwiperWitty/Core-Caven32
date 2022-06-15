/**
  ******************************************************************************
  * File   : USB_Device/Composite_Device/src/usb_prop.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : All processing related to Composite Device Demo
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_conf.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "hw_config.h"

/** @addtogroup AT32F413_StdPeriph_Examples
  * @{
  */

/** @addtogroup USB_Composite_Device
  * @{
  */ 
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t Request = 0;
u32 ProtocolValue;

LINE_CODING linecoding =
  {
    115200, /* baud rate*/
    0x00,   /* stop bits-1*/
    0x00,   /* parity - none*/
    0x08    /* no. of bits 8*/
  };

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
    Virtual_Com_Port_init,
    Virtual_Com_Port_Reset,
    Virtual_Com_Port_Status_In,
    Virtual_Com_Port_Status_Out,
    Virtual_Com_Port_Data_Setup,
    Virtual_Com_Port_NoData_Setup,
    Virtual_Com_Port_Get_Interface_Setting,
    Virtual_Com_Port_GetDeviceDescriptor,
    Virtual_Com_Port_GetConfigDescriptor,
    Virtual_Com_Port_GetStringDescriptor,
    0,
    0x40 /*MAX PACKET SIZE*/
  };
/** 
  * @brief  function pointers to usb standard requests
  */
USER_STANDARD_REQUESTS User_Standard_Requests =
  {
    Virtual_Com_Port_GetConfiguration,
    Virtual_Com_Port_SetConfiguration,
    Virtual_Com_Port_GetInterface,
    Virtual_Com_Port_SetInterface,
    Virtual_Com_Port_GetStatus,
    Virtual_Com_Port_ClearFeature,
    Virtual_Com_Port_SetEndPointFeature,
    Virtual_Com_Port_SetDeviceFeature,
    Virtual_Com_Port_SetDeviceAddress
  };
/** 
  * @brief  USB Device Descriptor pointer and length
  */
ONE_DESCRIPTOR Device_Descriptor =
  {
    (uint8_t*)Composite_DeviceDescriptor,
    Composite_SIZ_DEVICE_DESC
  };
/** 
  * @brief  USB Config Descriptor pointer and length
  */
ONE_DESCRIPTOR Config_Descriptor =
  {
    (uint8_t*)Composite_ConfigDescriptor,
    Composite_SIZ_CONFIG_DESC
  };
  
/** 
  * @brief  USB HID Report Descriptor pointer and length
  */
ONE_DESCRIPTOR Mouse_Report_Descriptor =
  {
    (u8 *)MOUSE_ReportDescriptor,
    MOUSE_SIZ_REPORT_DESC
  };
/** 
  * @brief  USB HID Descriptor pointer and length
  */
ONE_DESCRIPTOR Mouse_Hid_Descriptor =
  {
    (u8*)Composite_ConfigDescriptor + MOUSE_OFF_HID_DESC,
    MOUSE_SIZ_HID_DESC
  };
  
/** 
  * @brief  USB Sring Descriptor pointer and length
  */
ONE_DESCRIPTOR String_Descriptor[4] =
  {
    {(uint8_t*)Composite_StringLangID, Composite_SIZ_STRING_LANGID},    ///LangID Pointer and length
    {(uint8_t*)Composite_StringVendor, Composite_SIZ_STRING_VENDOR},    ///Vendor Pointer and length
    {(uint8_t*)Composite_StringProduct, Composite_SIZ_STRING_PRODUCT},  ///Product Pointer and length
    {(uint8_t*)Composite_StringSerial, Composite_SIZ_STRING_SERIAL}     ///Serial Pointer and length
  };

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Virtual COM Port Mouse init routine.
  * @param  None
  * @retval None
  */
void Virtual_Com_Port_init(void)
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
  * @brief  Virtual_Com_Port Mouse reset routine
  * @param  None
  * @retval None
  */
void Virtual_Com_Port_Reset(void)
{
  /* Set Virtual_Com_Port DEVICE as not configured */
  pInformation->Current_Configuration = 0;

  /* Current Feature initialization */
  pInformation->Current_Feature = Composite_ConfigDescriptor[7];

  /* Set Virtual_Com_Port DEVICE with the default Interface*/
  pInformation->Current_Interface = 0;

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
  SetEPType(ENDP1, EP_BULK);
  SetEPTxAddr(ENDP1, ENDP1_TXADDR);
  SetEPTxStatus(ENDP1, EP_TX_NAK);
  SetEPRxStatus(ENDP1, EP_RX_DIS);

  /* Initialize Endpoint 2 */
  SetEPType(ENDP2, EP_INTERRUPT);
  SetEPTxAddr(ENDP2, ENDP2_TXADDR);
  SetEPRxStatus(ENDP2, EP_RX_DIS);
  SetEPTxStatus(ENDP2, EP_TX_NAK);

  /* Initialize Endpoint 3 */
  SetEPType(ENDP3, EP_BULK);
  SetEPRxAddr(ENDP3, ENDP3_RXADDR);
  SetEPTxAddr(ENDP3, 0);
  SetEPRxCount(ENDP3, Composite_DATA_SIZE);
  SetEPRxStatus(ENDP3, EP_RX_VALID);
  SetEPTxStatus(ENDP3, EP_TX_DIS);

   /* Initialize Endpoint 4 */
  SetEPType(ENDP4, EP_INTERRUPT);
  SetEPTxAddr(ENDP4, ENDP4_TXADDR);
  SetEPTxCount(ENDP4, 4);
  SetEPRxStatus(ENDP4, EP_RX_DIS);
  SetEPTxStatus(ENDP4, EP_TX_NAK);
  
  /* Set this device to response on default address */
  SetDeviceAddress(0);
  
  bDeviceState = ATTACHED;
}

/**
  * @brief  Update the device state to configured.
  * @param  None
  * @retval None
  */
void Virtual_Com_Port_SetConfiguration(void)
{
  DEVICE_INFO *pInfo = &Device_Info;

  if (pInfo->Current_Configuration != 0)
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
void Virtual_Com_Port_SetDeviceAddress (void)
{
  bDeviceState = ADDRESSED;
}

/**
  * @brief  Virtual COM Port Status In Routine.
  * @param  None
  * @retval None
  */
void Virtual_Com_Port_Status_In(void)
{
  if (Request == SET_LINE_CODING)
  {
    Request = 0;
    usb_txfifo.wrpointer = 0;
	  usb_txfifo.curpointer = 0;
	  usb_rxfifo.wrpointer = 0;
	  usb_rxfifo.curpointer = 0;
  }
}

/**
  * @brief  Virtual COM Port Status OUT Routine.
  * @param  None
  * @retval None
  */
void Virtual_Com_Port_Status_Out(void)
{}

/**
  * @brief  Handle the data class specific requests.
  * @param  RequestNo: USB request number
  * @retval if support the request return Number USB_SUCCESS else return USB_UPSUPPORT
  */
RESULT Virtual_Com_Port_Data_Setup(uint8_t RequestNo)
{
  uint8_t    *(*CopyRoutine)(uint16_t);

  CopyRoutine = NULL;

//  if ((RequestNo == GET_DESCRIPTOR)
//      && (Type_Recipient == (STANDARD_REQUEST | INTERFACE_RECIPIENT))
//      && (pInformation->USBwIndex0 == 0))
//  {
//    if (pInformation->USBwValue1 == REPORT_DESCRIPTOR)
//    {
//      CopyRoutine = Mouse_GetReportDescriptor;
//    }
//    else if (pInformation->USBwValue1 == HID_DESCRIPTOR_TYPE)
//    {
//      CopyRoutine = Mouse_GetHIDDescriptor;
//    }

//  } /* End of GET_DESCRIPTOR */

  /*** GET_PROTOCOL ***/
//  else if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
//           && RequestNo == GET_PROTOCOL)
//  {
//    CopyRoutine = Mouse_GetProtocolValue;
//  }
  
  if (RequestNo == GET_LINE_CODING)
  {
    if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
    {
      CopyRoutine = Virtual_Com_Port_GetLineCoding;
    }
  }
  else if (RequestNo == SET_LINE_CODING)
  {
    if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
    {
      CopyRoutine = Virtual_Com_Port_SetLineCoding;
    }
    Request = SET_LINE_CODING;
  }
  else if(RequestNo == GET_DESCRIPTOR) //ÇëÇóHIDÃèÊö·û
  {
    if(Type_Recipient == (STANDARD_REQUEST | INTERFACE_RECIPIENT))
    {
      if(pInformation->USBwValue1 == REPORT_DESCRIPTOR)
      {
        CopyRoutine = Mouse_GetReportDescriptor;
      }
      else if (pInformation->USBwValue1 == HID_DESCRIPTOR_TYPE)
      {
        CopyRoutine = Mouse_GetHIDDescriptor;
      }
    }
    else if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
           && RequestNo == GET_PROTOCOL)
    {
      CopyRoutine = Mouse_GetProtocolValue;
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
  * @brief  handle the no data class specific requests.
  * @param  RequestNo: USB request number
  * @retval if support the request return Number USB_SUCCESS else return USB_UPSUPPORT
  */
RESULT Virtual_Com_Port_NoData_Setup(uint8_t RequestNo)
{

  if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
  {
    if (RequestNo == SET_COMM_FEATURE)
    {
      return USB_SUCCESS;
    }
    else if (RequestNo == SET_CONTROL_LINE_STATE)
    {
      return USB_SUCCESS;
    }
  }

  return USB_UNSUPPORT;
}

/**
  * @brief  Gets the device descriptor.
  * @param  Length: the device descriptor length
  * @retval The address of the device descriptor.
  */
uint8_t *Virtual_Com_Port_GetDeviceDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Device_Descriptor);
}

/**
  * @brief  Gets the configuration descriptor.
  * @param  Length: the config descriptor length
  * @retval The address of the configuration descriptor.
  */
uint8_t *Virtual_Com_Port_GetConfigDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Config_Descriptor);
}

/**
  * @brief  Gets the HID report descriptor.
  * @param  Length: the Report descriptor length
  * @retval The address of the Report descriptor.
  */
u8 *Mouse_GetReportDescriptor(u16 Length)
{
  return Standard_GetDescriptorData(Length, &Mouse_Report_Descriptor);
}

/**
  * @brief  Gets the HID descriptor.
  * @param  Length: the HID descriptor length
  * @retval The address of the HID descriptor.
  */
u8 *Mouse_GetHIDDescriptor(u16 Length)
{
  return Standard_GetDescriptorData(Length, &Mouse_Hid_Descriptor);
}

/**
  * @brief  Gets the string descriptors according to the needed index.
  * @param  Length: the string descriptor length
  * @retval The address of the string descriptor.
  */
uint8_t *Virtual_Com_Port_GetStringDescriptor(uint16_t Length)
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
  * @brief  Test the interface and the alternate setting according to the
  *         supported one.
  * @param  Interface: interface number.
  * @param  AlternateSetting: alternate setting no.
  * @retval USB_SUCCESS/USB_UNSUPPORT
  */
RESULT Virtual_Com_Port_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting)
{
  if (AlternateSetting > 0)
  {
    return USB_UNSUPPORT;
  }
  else if (Interface > 1)
  {
    return USB_UNSUPPORT;
  }
  return USB_SUCCESS;
}

/**
  * @brief  send the linecoding structure to the PC host.
  * @param  Length: LineCoding length
  * @retval linecodeing address pointer
  */
uint8_t *Virtual_Com_Port_GetLineCoding(uint16_t Length)
{
  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = sizeof(linecoding);
    return NULL;
  }
  return(uint8_t *)&linecoding;
}

/**
  * @brief  Set the linecoding structure fields.
  * @param  Length: LineCoding length
  * @retval linecodeing address pointer
  */
uint8_t *Virtual_Com_Port_SetLineCoding(uint16_t Length)
{
  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = sizeof(linecoding);
    return NULL;
  }
  return(uint8_t *)&linecoding;
}

/**
  * @brief  Get the protocol value
  * @param  Length: the protocol value length
  * @retval address of the protcol value..
  */
u8 *Mouse_GetProtocolValue(u16 Length)
{
  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = 1;
    return NULL;
  }
  else
  {
    return (u8 *)(&ProtocolValue);
  }
}

/**
  * @}
  */

/**
  * @}
  */
