/**
  ******************************************************************************
  * File   : USB_Device/MassStorage/src/usb_prop.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : All processing related to MassStorage Demo
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_conf.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "hw_config.h"
#include "usb_bot.h"
/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup USB_MassStorage
  * @{
  */ 
uint32_t Max_Lun = 0;

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
    MASS_init,
    MASS_Reset,
    MASS_Status_In,
    MASS_Status_Out,
    MASS_Data_Setup,
    MASS_NoData_Setup,
    MASS_Get_Interface_Setting,
    MASS_GetDeviceDescriptor,
    MASS_GetConfigDescriptor,
    MASS_GetStringDescriptor,
    0,
    0x40 /*MAX PACKET SIZE*/
  };
/** 
  * @brief  function pointers to usb standard requests
  */
USER_STANDARD_REQUESTS User_Standard_Requests =
  {
    Mass_Storage_GetConfiguration,
    Mass_Storage_SetConfiguration,
    Mass_Storage_GetInterface,
    Mass_Storage_SetInterface,
    Mass_Storage_GetStatus,
    Mass_Storage_ClearFeature,
    Mass_Storage_SetEndPointFeature,
    Mass_Storage_SetDeviceFeature,
    Mass_Storage_SetDeviceAddress
  };
/** 
  * @brief  USB Device Descriptor pointer and length
  */
ONE_DESCRIPTOR Device_Descriptor =
  {
    (uint8_t*)MASS_DeviceDescriptor,
    MASS_SIZ_DEVICE_DESC
  };
/** 
  * @brief  USB Config Descriptor pointer and length
  */
ONE_DESCRIPTOR Config_Descriptor =
  {
    (uint8_t*)MASS_ConfigDescriptor,
    MASS_SIZ_CONFIG_DESC
  };
/** 
  * @brief  USB Sring Descriptor pointer and length
  */
ONE_DESCRIPTOR String_Descriptor[5] =
  {
    {(uint8_t*)MASS_StringLangID, MASS_SIZ_STRING_LANGID},    ///LangID Pointer and length
    {(uint8_t*)MASS_StringVendor, MASS_SIZ_STRING_VENDOR},    ///Vendor Pointer and length
    {(uint8_t*)MASS_StringProduct, MASS_SIZ_STRING_PRODUCT},  ///Product Pointer and length
    {(uint8_t*)MASS_StringSerial, MASS_SIZ_STRING_SERIAL},    ///Serial Pointer and length
    {(uint8_t*)MASS_StringInterface, MASS_SIZ_STRING_INTERFACE},///interface Pointer and length
  };

/* Extern variables ----------------------------------------------------------*/
extern unsigned char Bot_State;
extern Bulk_Only_CBW CBW;

/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Mass Storage init.
  * @param  None
  * @retval None
  */
void MASS_init()
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
  * @brief  Mass Storage reset routine.
  * @param  None
  * @retval None
  */
void MASS_Reset()
{
  /* Set the device as not configured */
  Device_Info.Current_Configuration = 0;

  /* Current Feature initialization */
  pInformation->Current_Feature = MASS_ConfigDescriptor[7];

  _SetBUFTBL(BTABLE_ADDRESS);

  /* Initialize Endpoint 0 */
  SetEPType(ENDP0, EP_CONTROL);
  SetEPTxStatus(ENDP0, EP_TX_NAK);
  SetEPRxAddr(ENDP0, ENDP0_RXADDR);
  SetEPRxCount(ENDP0, Device_Property.MaxPacketSize);
  SetEPTxAddr(ENDP0, ENDP0_TXADDR);
  Clear_Status_Out(ENDP0);
  SetEPRxValid(ENDP0);

  /* Initialize Endpoint 1 */
  SetEPType(ENDP1, EP_BULK);
  SetEPTxAddr(ENDP1, ENDP1_TXADDR);
  SetEPTxStatus(ENDP1, EP_TX_NAK);
  SetEPRxStatus(ENDP1, EP_RX_DIS);

  /* Initialize Endpoint 2 */
  SetEPType(ENDP2, EP_BULK);
  SetEPRxAddr(ENDP2, ENDP2_RXADDR);
  SetEPRxCount(ENDP2, Device_Property.MaxPacketSize);
  SetEPTxAddr(ENDP2, 0);
  SetEPTxCount(ENDP2, 0);
  SetEPRxStatus(ENDP2, EP_RX_VALID);
  SetEPTxStatus(ENDP2, EP_TX_DIS);


  SetEPRxCount(ENDP0, Device_Property.MaxPacketSize);
  SetEPRxValid(ENDP0);

  /* Set the device to response on default address */
  SetDeviceAddress(0);

  bDeviceState = ATTACHED;

  CBW.dSignature = BOT_CBW_SIGNATURE;
  Bot_State = BOT_IDLE;

//  USB_NotConfigured_LED();
}

/**
  * @brief  Handle the SetConfiguration request.
  * @param  None
  * @retval None
  */
void Mass_Storage_SetConfiguration(void)
{
  if (pInformation->Current_Configuration != 0)
  {
    /* Device configured */
    bDeviceState = CONFIGURED;
   
    ClearDTOG_TX(ENDP1);
    ClearDTOG_RX(ENDP2);

    Bot_State = BOT_IDLE; /* set the Bot state machine to the IDLE state */
  }
}

/**
  * @brief  Handle the ClearFeature request.
  * @param  None
  * @retval None
  */
void Mass_Storage_ClearFeature(void)
{
  /* when the host send a CBW with invalid signature or invalid length the two
     Endpoints (IN & OUT) shall stall until receiving a Mass Storage Reset     */
  if (CBW.dSignature != BOT_CBW_SIGNATURE)
    Bot_Abort(BOTH_DIR);
}

/**
  * @brief  Updata the device state to addressed.
  * @param  None
  * @retval None
  */
void Mass_Storage_SetDeviceAddress (void)
{
  bDeviceState = ADDRESSED;
}

/**
  * @brief  Mass Storage status IN routine.
  * @param  None
  * @retval None
  */
void MASS_Status_In(void)
{
  return;
}

/**
  * @brief  Mass Storage status OUT routine.
  * @param  None
  * @retval None
  */
void MASS_Status_Out(void)
{
  return;
}

/**
  * @brief  Handle the data class specific requests.
  * @param  RequestNo: USB request number
  * @retval if support the request return Number USB_SUCCESS else return USB_UPSUPPORT
  */
RESULT MASS_Data_Setup(uint8_t RequestNo)
{
  uint8_t    *(*CopyRoutine)(uint16_t);

  CopyRoutine = NULL;
  if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
      && (RequestNo == GET_MAX_LUN) && (pInformation->USBwValue == 0)
      && (pInformation->USBwIndex == 0) && (pInformation->USBwLength == 0x01))
  {
    CopyRoutine = Get_Max_Lun;
  }
  else
  {
    return USB_UNSUPPORT;
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
RESULT MASS_NoData_Setup(uint8_t RequestNo)
{
  if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
      && (RequestNo == MASS_STORAGE_RESET) && (pInformation->USBwValue == 0)
      && (pInformation->USBwIndex == 0) && (pInformation->USBwLength == 0x00))
  {
    /* Initialize Endpoint 1 */
    ClearDTOG_TX(ENDP1);

    /* Initialize Endpoint 2 */
    ClearDTOG_RX(ENDP2);

    /*initialize the CBW signature to enable the clear feature*/
    CBW.dSignature = BOT_CBW_SIGNATURE;
    Bot_State = BOT_IDLE;

    return USB_SUCCESS;
  }
  return USB_UNSUPPORT;
}

/**
  * @brief  Test the interface and the alternate setting according to the
  *         supported one.
  * @param  Interface: interface number.
  * @param  AlternateSetting: alternate setting no.
  * @retval USB_SUCCESS/USB_UNSUPPORT
  */
RESULT MASS_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting)
{
  if (AlternateSetting > 0)
  {
    return USB_UNSUPPORT;/* in this application we don't have AlternateSetting*/
  }
  else if (Interface > 0)
  {
    return USB_UNSUPPORT;/*in this application we have only 1 interfaces*/
  }
  return USB_SUCCESS;
}

/**
  * @brief  Gets the device descriptor.
  * @param  Length: the device descriptor length
  * @retval The address of the device descriptor.
  */
uint8_t *MASS_GetDeviceDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Device_Descriptor );
}

/**
  * @brief  Gets the configuration descriptor.
  * @param  Length: the config descriptor length
  * @retval The address of the configuration descriptor.
  */
uint8_t *MASS_GetConfigDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Config_Descriptor );
}

/**
  * @brief  Gets the string descriptors according to the needed index.
  * @param  Length: the string descriptor length
  * @retval The address of the string descriptor.
  */
uint8_t *MASS_GetStringDescriptor(uint16_t Length)
{
  uint8_t wValue0 = pInformation->USBwValue0;

  if (wValue0 > 5)
  {
    return NULL;
  }
  else
  {
    return Standard_GetDescriptorData(Length, &String_Descriptor[wValue0]);
  }
}

/**
  * @brief  Handle the Get Max Lun request.
  * @param  Length: the lun length
  * @retval The address of the Max Lun.
  */
uint8_t *Get_Max_Lun(uint16_t Length)
{
  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = LUN_DATA_LENGTH;
    return 0;
  }
  else
  {
    return((uint8_t*)(&Max_Lun));
  }
}
/**
  * @}
  */

/**
  * @}
  */
  
