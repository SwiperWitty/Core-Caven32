/**
  ******************************************************************************
  * File   : USB_Device/USB_Printer/src/usb_prop.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : All processing related to printer Demo
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

/** @addtogroup USB_USB_Printer
  * @{
  */ 
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t Request = 0;
uint8_t Printer_Port_status[1] = {0x18};
uint8_t USB_Printer_IDString[24] = 
  {
    0x00, 0x16,
    'M', 'F', 'G',':','A','r','t','e', 'r', 'y' ,' ',
    'C','M', 'D', ':', 'E', 'S', 'C', 'P', 'O', 'S',' ',
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
    USB_Printer_init,
    USB_Printer_Reset,
    USB_Printer_Status_In,
    USB_Printer_Status_Out,
    USB_Printer_Data_Setup,
    USB_Printer_NoData_Setup,
    USB_Printer_Get_Interface_Setting,
    USB_Printer_GetDeviceDescriptor,
    USB_Printer_GetConfigDescriptor,
    USB_Printer_GetStringDescriptor,
    0,
    0x40 /*MAX PACKET SIZE*/
  };
/** 
  * @brief  function pointers to usb standard requests
  */
USER_STANDARD_REQUESTS User_Standard_Requests =
  {
    USB_Printer_GetConfiguration,
    USB_Printer_SetConfiguration,
    USB_Printer_GetInterface,
    USB_Printer_SetInterface,
    USB_Printer_GetStatus,
    USB_Printer_ClearFeature,
    USB_Printer_SetEndPointFeature,
    USB_Printer_SetDeviceFeature,
    USB_Printer_SetDeviceAddress
  };
/** 
  * @brief  USB Device Descriptor pointer and length
  */
ONE_DESCRIPTOR Device_Descriptor =
  {
    (uint8_t*)USB_Printer_DeviceDescriptor,
    USB_Printer_SIZ_DEVICE_DESC
  };
/** 
  * @brief  USB Config Descriptor pointer and length
  */
ONE_DESCRIPTOR Config_Descriptor =
  {
    (uint8_t*)USB_Printer_ConfigDescriptor,
    USB_Printer_SIZ_CONFIG_DESC
  };
/** 
  * @brief  USB Sring Descriptor pointer and length
  */
ONE_DESCRIPTOR String_Descriptor[4] =
  {
    {(uint8_t*)USB_Printer_StringLangID, USB_Printer_SIZ_STRING_LANGID},    ///LangID Pointer and length
    {(uint8_t*)USB_Printer_StringVendor, USB_Printer_SIZ_STRING_VENDOR},    ///Vendor Pointer and length
    {(uint8_t*)USB_Printer_StringProduct, USB_Printer_SIZ_STRING_PRODUCT},  ///Product Pointer and length
    {(uint8_t*)USB_Printer_StringSerial, USB_Printer_SIZ_STRING_SERIAL}     ///Serial Pointer and length
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
void USB_Printer_init(void)
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
  * @brief  USB_Printer Mouse reset routine
  * @param  None
  * @retval None
  */
void USB_Printer_Reset(void)
{
  /* Set USB_Printer DEVICE as not configured */
  pInformation->Current_Configuration = 0;

  /* Current Feature initialization */
  pInformation->Current_Feature = USB_Printer_ConfigDescriptor[7];

  /* Set USB_Printer DEVICE with the default Interface*/
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

//  /* Initialize Endpoint 2 */
//  SetEPType(ENDP2, EP_INTERRUPT);
//  SetEPTxAddr(ENDP2, ENDP2_TXADDR);
//  SetEPRxStatus(ENDP2, EP_RX_DIS);
//  SetEPTxStatus(ENDP2, EP_TX_NAK);

  /* Initialize Endpoint 3 */
  SetEPType(ENDP3, EP_BULK);
  SetEPRxAddr(ENDP3, ENDP3_RXADDR);
  SetEPTxAddr(ENDP3, 0);
  SetEPRxCount(ENDP3, USB_Printer_DATA_SIZE);
  SetEPRxStatus(ENDP3, EP_RX_VALID);
  SetEPTxStatus(ENDP3, EP_TX_DIS);

  /* Set this device to response on default address */
  SetDeviceAddress(0);
  
  bDeviceState = ATTACHED;
}

/**
  * @brief  Update the device state to configured.
  * @param  None
  * @retval None
  */
void USB_Printer_SetConfiguration(void)
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
void USB_Printer_SetDeviceAddress (void)
{
  bDeviceState = ADDRESSED;
}

/**
  * @brief  Virtual COM Port Status In Routine.
  * @param  None
  * @retval None
  */
void USB_Printer_Status_In(void)
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
void USB_Printer_Status_Out(void)
{}

/**
  * @brief  Handle the data class specific requests.
  * @param  RequestNo: USB request number
  * @retval if support the request return Number USB_SUCCESS else return USB_UPSUPPORT
  */
RESULT USB_Printer_Data_Setup(uint8_t RequestNo)
{
  uint8_t    *(*CopyRoutine)(uint16_t);

  CopyRoutine = NULL;


  if (Type_Recipient == (0xA1 & (REQUEST_TYPE|RECIPIENT)))
  {
    if(RequestNo == GET_DEVICE_ID)
     {
       CopyRoutine = USB_Printer_ID;
     }
     else if (RequestNo == GET_PORT_STATUS)
     {
       CopyRoutine = USB_Printer_Port_Status;
     }
     else if (RequestNo == SOFT_RESET)
     {
       //do nothing.
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
RESULT USB_Printer_NoData_Setup(uint8_t RequestNo)
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
uint8_t *USB_Printer_GetDeviceDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Device_Descriptor);
}

/**
  * @brief  Gets the configuration descriptor.
  * @param  Length: the config descriptor length
  * @retval The address of the configuration descriptor.
  */
uint8_t *USB_Printer_GetConfigDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Config_Descriptor);
}

/**
  * @brief  Gets the string descriptors according to the needed index.
  * @param  Length: the string descriptor length
  * @retval The address of the string descriptor.
  */
uint8_t *USB_Printer_GetStringDescriptor(uint16_t Length)
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
RESULT USB_Printer_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting)
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
  * @brief  send the USB Printer ID .
  * @param  Length: LineCoding length
  * @retval linecodeing address pointer
  */
uint8_t *USB_Printer_ID(uint16_t Length)
{
  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = sizeof(USB_Printer_IDString);
    return NULL;
  }
  return(uint8_t *)&USB_Printer_IDString;
}

uint8_t *USB_Printer_Port_Status(uint16_t Length)
{
  pInformation->Ctrl_Info.Usb_wLength = 1;
  return Printer_Port_status;
}


/**
  * @}
  */

/**
  * @}
  */
