/**
  ******************************************************************************
  * File   : USB_Device/Mouse/src/usb_prop.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : All processing related to Mouse Demo
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_conf.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "hw_config.h"
/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup USB_Mouse
  * @{
  */ 
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u32 ProtocolValue;  ///the protocol value

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
    Joystick_init,
    Joystick_Reset,
    Joystick_Status_In,
    Joystick_Status_Out,
    Joystick_Data_Setup,
    Joystick_NoData_Setup,
    Joystick_Get_Interface_Setting,
    Joystick_GetDeviceDescriptor,
    Joystick_GetConfigDescriptor,
    Joystick_GetStringDescriptor,
    0,
    0x40 /*MAX PACKET SIZE*/
  };
  
/** 
  * @brief  function pointers to usb standard requests
  */
USER_STANDARD_REQUESTS User_Standard_Requests =
  {
    Joystick_GetConfiguration,
    Joystick_SetConfiguration,
    Joystick_GetInterface,
    Joystick_SetInterface,
    Joystick_GetStatus,
    Joystick_ClearFeature,
    Joystick_SetEndPointFeature,
    Joystick_SetDeviceFeature,
    Joystick_SetDeviceAddress
  };

/** 
  * @brief  USB Device Descriptor pointer and length
  */
ONE_DESCRIPTOR Device_Descriptor =
  {
    (u8*)Joystick_DeviceDescriptor,
    JOYSTICK_SIZ_DEVICE_DESC
  };
/** 
  * @brief  USB Config Descriptor pointer and length
  */
ONE_DESCRIPTOR Config_Descriptor =
  {
    (u8*)Joystick_ConfigDescriptor,
    JOYSTICK_SIZ_CONFIG_DESC
  };
/** 
  * @brief  USB HID Report Descriptor pointer and length
  */
ONE_DESCRIPTOR Joystick_Report_Descriptor =
  {
    (u8 *)Joystick_ReportDescriptor,
    JOYSTICK_SIZ_REPORT_DESC
  };
/** 
  * @brief  USB HID Descriptor pointer and length
  */
ONE_DESCRIPTOR Mouse_Hid_Descriptor =
  {
    (u8*)Joystick_ConfigDescriptor + JOYSTICK_OFF_HID_DESC,
    JOYSTICK_SIZ_HID_DESC
  };
/** 
  * @brief  USB Sring Descriptor pointer and length
  */
ONE_DESCRIPTOR String_Descriptor[4] =
  {
    {(u8*)Joystick_StringLangID, JOYSTICK_SIZ_STRING_LANGID},    ///LangID Pointer and length
    {(u8*)Joystick_StringVendor, JOYSTICK_SIZ_STRING_VENDOR},    ///Vendor Pointer and length
    {(u8*)Joystick_StringProduct, JOYSTICK_SIZ_STRING_PRODUCT},  ///Product Pointer and length
    {(u8*)Joystick_StringSerial, JOYSTICK_SIZ_STRING_SERIAL}     ///Serial Pointer and length
  };

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Joystick Mouse init.
  * @param  None
  * @retval None
  */
void Joystick_init(void)
{

  /* Update the serial number string descriptor with the data from the unique
  ID*/
  Get_SerialNum();

  pInformation->Current_Configuration = 0;
  /* Connect the device */
  PowerOn();
  /* USB interrupts initialization */
  _SetINTSTS(0);               /* clear pending interrupts */
  wInterrupt_Mask = IMR_MSK;
  _SetCTRL(wInterrupt_Mask); /* set interrupts mask */

  bDeviceState = UNCONNECTED;
}

/**
  * @brief  Joystick Mouse reset routine.
  * @param  None
  * @retval None
  */
void Joystick_Reset(void)
{
  /* Set Joystick_DEVICE as not configured */
  pInformation->Current_Configuration = 0;
  pInformation->Current_Interface = 0;/*the default Interface*/

  /* Current Feature initialization */
  pInformation->Current_Feature = Joystick_ConfigDescriptor[7];

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
  SetEPType(EP_IN, EP_INTERRUPT);
  SetEPTxAddr(EP_IN, EP_TXADDR);
  SetEPTxCount(EP_IN, 4);
  SetEPRxStatus(EP_IN, EP_RX_DIS);
  SetEPTxStatus(EP_IN, EP_TX_NAK);

  bDeviceState = ATTACHED;

  /* Set this device to response on default address */
  SetDeviceAddress(0);
}

/**
  * @brief  Updata the device state to configured.
  * @param  None
  * @retval None
  */
void Joystick_SetConfiguration(void)
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
void Joystick_SetDeviceAddress (void)
{
  bDeviceState = ADDRESSED;
}

/**
  * @brief  Joystick status IN routine.
  * @param  None
  * @retval None
  */
void Joystick_Status_In(void)
{}

/**
  * @brief  Joystick status OUT routine.
  * @param  None
  * @retval None
  */
void Joystick_Status_Out (void)
{}

/**
  * @brief  Handle the data class specific requests.
  * @param  RequestNo: USB request number
  * @retval if support the request return Number USB_SUCCESS else return USB_UPSUPPORT
  */
RESULT Joystick_Data_Setup(u8 RequestNo)
{
  u8 *(*CopyRoutine)(u16);

  CopyRoutine = NULL;
  if ((RequestNo == GET_DESCRIPTOR)
      && (Type_Recipient == (STANDARD_REQUEST | INTERFACE_RECIPIENT))
      && (pInformation->USBwIndex0 == 0))
  {

    if (pInformation->USBwValue1 == REPORT_DESCRIPTOR)
    {
      CopyRoutine = Joystick_GetReportDescriptor;
    }
    else if (pInformation->USBwValue1 == HID_DESCRIPTOR_TYPE)
    {
      CopyRoutine = Joystick_GetHIDDescriptor;
    }

  } /* End of GET_DESCRIPTOR */

  /*** GET_PROTOCOL ***/
  else if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
           && RequestNo == GET_PROTOCOL)
  {
    CopyRoutine = Joystick_GetProtocolValue;
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
RESULT Joystick_NoData_Setup(u8 RequestNo)
{
  if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
      && (RequestNo == SET_PROTOCOL))
  {
    return Joystick_SetProtocol();
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
u8 *Joystick_GetDeviceDescriptor(u16 Length)
{
  return Standard_GetDescriptorData(Length, &Device_Descriptor);
}

/**
  * @brief  Gets the configuration descriptor.
  * @param  Length: the config descriptor length
  * @retval The address of the configuration descriptor.
  */
u8 *Joystick_GetConfigDescriptor(u16 Length)
{
  return Standard_GetDescriptorData(Length, &Config_Descriptor);
}

/**
  * @brief  Gets the string descriptors according to the needed index.
  * @param  Length: the string descriptor length
  * @retval The address of the string descriptor.
  */
u8 *Joystick_GetStringDescriptor(u16 Length)
{
  u8 wValue0 = pInformation->USBwValue0;
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
  * @brief  Gets the HID report descriptor.
  * @param  Length: the Report descriptor length
  * @retval The address of the Report descriptor.
  */
u8 *Joystick_GetReportDescriptor(u16 Length)
{
  return Standard_GetDescriptorData(Length, &Joystick_Report_Descriptor);
}

/**
  * @brief  Gets the HID descriptor.
  * @param  Length: the HID descriptor length
  * @retval The address of the HID descriptor.
  */
u8 *Joystick_GetHIDDescriptor(u16 Length)
{
  return Standard_GetDescriptorData(Length, &Mouse_Hid_Descriptor);
}

/**
  * @brief  tests the interface and the alternate setting according to the
  *         supported one.
  * @param  Interface: interface number
  * @param  AlternateSetting: Alternate Setting number.
  * @retval if support the interface and  AlternateSetting return Number USB_SUCCESS 
  *         else return USB_UPSUPPORT
  */
RESULT Joystick_Get_Interface_Setting(u8 Interface, u8 AlternateSetting)
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
  * @brief  Joystick Set Protocol request routine.
  * @param  None.
  * @retval USB_SUCCESS.
  */
RESULT Joystick_SetProtocol(void)
{
  u8 wValue0 = pInformation->USBwValue0;
  ProtocolValue = wValue0;
  return USB_SUCCESS;
}

/**
  * @brief  Get the protocol value
  * @param  Length: the protocol value length
  * @retval address of the protcol value..
  */
u8 *Joystick_GetProtocolValue(u16 Length)
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


