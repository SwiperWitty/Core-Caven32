/**
  ******************************************************************************
  * File   : USB_Device/USB_Audio_Demo/src/usb_prop.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : All processing related to USB Audio Demo
  ******************************************************************************
  */

 /* Includes ------------------------------------------------------------------*/
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_conf.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "at32_audio_conf.h"

/** @addtogroup AT32F413_StdPeriph_Examples
  * @{
  */

/** @addtogroup USB_Audio_Demo
  * @{
  */ 
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t MUTE_DATA = 0;

DEVICE Device_Table =
  {
    EP_NUM,
    1
  };

DEVICE_PROP Device_Property =
  {
    USB_Audio_init,
    USB_Audio_Reset,
    USB_Audio_Status_In,
    USB_Audio_Status_Out,
    USB_Audio_Data_Setup,
    USB_Audio_NoData_Setup,
    USB_Audio_Get_Interface_Setting,
    USB_Audio_GetDeviceDescriptor,
    USB_Audio_GetConfigDescriptor,
    USB_Audio_GetStringDescriptor,
    0,
    0x40 /*MAX PACKET SIZE*/
  };

USER_STANDARD_REQUESTS User_Standard_Requests =
  {
    USB_Audio_GetConfiguration,
    USB_Audio_SetConfiguration,
    USB_Audio_GetInterface,
    USB_Audio_SetInterface,
    USB_Audio_GetStatus,
    USB_Audio_ClearFeature,
    USB_Audio_SetEndPointFeature,
    USB_Audio_SetDeviceFeature,
    USB_Audio_SetDeviceAddress
  };

ONE_DESCRIPTOR Device_Descriptor =
  {
    (uint8_t*)Audio_DeviceDescriptor,
    AUDIO_SIZ_DEVICE_DESC
  };

ONE_DESCRIPTOR Config_Descriptor =
  {
    (uint8_t*)Audio_ConfigDescriptor,
    AUDIO_SIZ_CONFIG_DESC
  };

ONE_DESCRIPTOR String_Descriptor[4] =
  {
    {(uint8_t*)Audio_StringLangID, AUDIO_SIZ_STRING_LANGID},
    {(uint8_t*)Audio_StringVendor, AUDIO_SIZ_STRING_VENDOR},
    {(uint8_t*)Audio_StringProduct, AUDIO_SIZ_STRING_PRODUCT},
    {(uint8_t*)Audio_StringSerial, AUDIO_SIZ_STRING_SERIAL},
  };


/*USB audio control struct*/
typedef struct __USB_Audio_Control_
{
    uint8_t u8Enpd;
    uint8_t u8Interface;
    uint8_t u8RquestNo;
    uint8_t u8SpkMute;
    uint8_t u8MicMute;
    uint16_t u16Volume;
    uint16_t u16MicVolume;
    uint32_t u32Freq;
    uint16_t u16SpkVolumeLimits[3]; /*[0] is mininum value, [1] is maxnum value, [2] is volume resolution */
    uint16_t u16MicVolumeLimits[3]; /*[0] is mininum value, [1] is maxnum value, [2] is volume resolution */
    
}USB_Audio_Control;

//USB_Audio_Control usb_audio = {0, 0, 0, 0, 0x40, 0, 0, {0x0000, 0x100, 0x040}, {0x0000, 0x100, 0x040}};
USB_Audio_Control usb_audio = {0, 0, 0, 0, 0, 0x1400, 0, 0, {0x0000, 0x1400, 0x0400}, {0x0000, 0x1400, 0x0400}};

/* Extern variables ----------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : USB_Audio_init.
* Description    : USB_Audio init routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_Audio_init()
{
  /* Update the serial number string descriptor with the data from the unique
  ID*/
  Get_SerialNum();

  /* Initialize the current configuration */
  pInformation->Current_Configuration = 0;

  /* Connect the device */
  PowerOn();

  /* Perform basic device initialization operations */
  USB_SIL_Init();

  bDeviceState = UNCONNECTED;
}

/*******************************************************************************
* Function Name  : USB_Audio_Reset.
* Description    : USB_Audio reset routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_Audio_Reset()
{
  /* Set USB_Audio device as not configured state */
  pInformation->Current_Configuration = 0;

  /* Current Feature initialization */
  pInformation->Current_Feature = Audio_ConfigDescriptor[7];

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
  SetEPType(ENDP1, EP_ISOCHRONOUS);
  SetEPDblBuffAddr(ENDP1, ENDP1_BUF0Addr, ENDP1_BUF1Addr);
  SetEPDblBuffCount(ENDP1, EP_DBUF_IN, USB_EDNP_MAX_PACKET);
  ClearDTOG_RX(ENDP1);
  ClearDTOG_TX(ENDP1);
  ToggleDTOG_RX(ENDP1);
  SetEPRxStatus(ENDP1, EP_RX_DIS);
  SetEPTxStatus(ENDP1, EP_TX_VALID);


  /* Initialize Endpoint 2 */
  SetEPType(ENDP2, EP_ISOCHRONOUS);
  SetEPDblBuffAddr(ENDP2, ENDP2_BUF0Addr, ENDP2_BUF1Addr);
  SetEPDblBuffCount(ENDP2, EP_DBUF_OUT, USB_EDNP_MAX_PACKET);
  ClearDTOG_RX(ENDP2);
  ClearDTOG_TX(ENDP2);
  ToggleDTOG_TX(ENDP2);
  SetEPRxStatus(ENDP2, EP_RX_VALID);
  SetEPTxStatus(ENDP2, EP_TX_DIS);
  
  /* Initialize Endpoint 3 */
  SetEPType(ENDP3, EP_ISOCHRONOUS);
  SetEPDblBuffAddr(ENDP3, ENDP3_BUF0Addr, ENDP3_BUF1Addr);
  SetEPDblBuffCount(ENDP3, EP_DBUF_IN, USB_EDNP_MAX_PACKET);
  ClearDTOG_RX(ENDP3);
  ClearDTOG_TX(ENDP3);
  ToggleDTOG_RX(ENDP3);
  SetEPRxStatus(ENDP3, EP_RX_DIS);
  SetEPTxStatus(ENDP3, EP_TX_DIS);
  

  SetEPRxValid(ENDP0);
  /* Set this device to response on default address */
  SetDeviceAddress(0);

  bDeviceState = ATTACHED;
}
/*******************************************************************************
* Function Name  : USB_Audio_SetConfiguration.
* Description    : Update the device state to configured.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_Audio_SetConfiguration(void)
{
  DEVICE_INFO *pInfo = &Device_Info;

  if (pInfo->Current_Configuration != 0)
  {
    /* Device configured */
    bDeviceState = CONFIGURED;
    /*Send Feedback*/
#if (ENABLE_AT32_AUDIO_FEED_BACK == 1)
    UserToPMABufferCopy(feed_back, ENDP3_BUF1Addr, 3);
    SetEPDblBuf1Count(ENDP3, EP_DBUF_IN, 3);
    SetEPDblBuf0Count(ENDP3, EP_DBUF_IN, 3);
    SetEPTxStatus(ENDP3, EP_TX_VALID);
#endif
  }
}


/*******************************************************************************
* Function Name  : USB_Audio_SetInterface.
* Description    : Update the interface state to configured.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_Audio_SetInterface()
{
    
}
/*******************************************************************************
* Function Name  : USB_Audio_SetConfiguration.
* Description    : Update the device state to addressed.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_Audio_SetDeviceAddress (void)
{
  bDeviceState = ADDRESSED;
}
/*******************************************************************************
* Function Name  : USB_Audio_Status_In.
* Description    : USB_Audio Status In routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_Audio_Status_In(void)
{
    float fVol = 0;
    switch ( usb_audio.u8RquestNo )
    {
        case USB_AUDIO_SET_VOLUME:
            if ( usb_audio.u8Interface == AUDIO_SPK_FEATURE_UNIT_ID )
            {    /*Speaker Volume*/
                 fVol = (float)usb_audio.u16Volume / (float)usb_audio.u16SpkVolumeLimits[1] * 127;
                 Audio_SetVolume_Speaker(fVol);
            }
            if ( usb_audio.u8Interface == AUDIO_MIC_FEATURE_UNIT_ID ) 
            {    /*Microphone Volume*/
                fVol = (float)usb_audio.u16MicVolume / (float)usb_audio.u16MicVolumeLimits[1] * 200;
                Audio_SetVolume_Microphone(fVol);
            }
            break;
        case USB_AUDIO_SET_MUTE:
            if ( usb_audio.u8Interface == AUDIO_SPK_FEATURE_UNIT_ID ) 
            {   /*Speaker Volume*/
                Audio_SetMute_Speaker(usb_audio.u8SpkMute);
            }
            if ( usb_audio.u8Interface == AUDIO_MIC_FEATURE_UNIT_ID )
            {   /*Microphone Volume*/
                Audio_SetMute_Microphone(usb_audio.u8MicMute);
            }
            break;
        case USB_AUDIO_SET_FREQ:
            /*freq Microphone and Speaker must equal*/
            Audio_SetFreq(usb_audio.u32Freq, usb_audio.u8Interface);
            break;
        default:
            break;
        
    }  
        
}

/*******************************************************************************
* Function Name  : USB_Audio_Status_Out.
* Description    : USB_Audio Status Out routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_Audio_Status_Out (void)
{}

/*******************************************************************************
* Function Name  : USB_Audio_Data_Setup
* Description    : Handle the data class specific requests.
* Input          : None.
* Output         : None.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT USB_Audio_Data_Setup(uint8_t RequestNo)
{
  uint8_t *(*CopyRoutine)(uint16_t);
  CopyRoutine = NULL;
  
  /*Current interface*/
  usb_audio.u8Interface = pInformation->USBwIndexs.bw.bb1;
  
    /*Set Configure*/
  if ( RequestNo == SET_CUR )
  {
      usb_audio.u8RquestNo = 0;
      switch( pInformation->USBbmRequestType & 0x3 )
      {
          case AUDIO_CONTROL_FUNCTION:
            if ( (pInformation->USBwValues.bw.bb1) == AUDIO_MUTE_CONTROL)
            {
                CopyRoutine = Mute_Command;
                usb_audio.u8RquestNo = USB_AUDIO_SET_MUTE;
            }
            else if ( (pInformation->USBwValues.bw.bb1) == AUDIO_VOLUME_CONTROL)
            {
                CopyRoutine = Set_Volume_Command;
                usb_audio.u8RquestNo = USB_AUDIO_SET_VOLUME;
            }
              break;
          case AUDIO_ENDPOINT_FUNCTION:
              CopyRoutine = Set_Freq_Command;
              usb_audio.u8RquestNo = USB_AUDIO_SET_FREQ;
              break;
      }
  }
  else if ( AUDIO_CONTROL_FUNCTION == (pInformation->USBbmRequestType & 0x3) )
  {
      if (RequestNo == GET_MIN)
      {
          CopyRoutine = Get_MinVolume_Command;
      }
      else if (RequestNo == GET_MAX)
      {
          CopyRoutine = Get_MaxVolume_Command;
      }
      else if (RequestNo == GET_RES)
      {
          CopyRoutine = Get_ResVolume_Command;
          
      }else if (RequestNo == GET_CUR )
      {
          if ( (pInformation->USBwValues.bw.bb1) == AUDIO_MUTE_CONTROL)
              CopyRoutine = Mute_Command;
          else
              CopyRoutine = Get_CurVolume_Command;
      }
      else
          return USB_UNSUPPORT;
  }
    
  
//  if ((RequestNo == GET_CUR) || (RequestNo == SET_CUR))
//  {
//    CopyRoutine = Mute_Command;
//  }

  else
  {
    return USB_UNSUPPORT;
  }

  pInformation->Ctrl_Info.CopyData = CopyRoutine;
  pInformation->Ctrl_Info.Usb_wOffset = 0;
  (*CopyRoutine)(0);
  return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : USB_Audio_NoData_Setup
* Description    : Handle the no data class specific requests.
* Input          : None.
* Output         : None.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT USB_Audio_NoData_Setup(uint8_t RequestNo)
{
  return USB_UNSUPPORT;
}

/*******************************************************************************
* Function Name  : USB_Audio_GetDeviceDescriptor.
* Description    : Get the device descriptor.
* Input          : Length : uint16_t.
* Output         : None.
* Return         : The address of the device descriptor.
*******************************************************************************/
uint8_t *USB_Audio_GetDeviceDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Device_Descriptor);
}

/*******************************************************************************
* Function Name  : USB_Audio_GetConfigDescriptor.
* Description    : Get the configuration descriptor.
* Input          : Length : uint16_t.
* Output         : None.
* Return         : The address of the configuration descriptor.
*******************************************************************************/
uint8_t *USB_Audio_GetConfigDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Config_Descriptor);
}

/*******************************************************************************
* Function Name  : USB_Audio_GetStringDescriptor.
* Description    : Get the string descriptors according to the needed index.
* Input          : Length : uint16_t.
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
uint8_t *USB_Audio_GetStringDescriptor(uint16_t Length)
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

/*******************************************************************************
* Function Name  : USB_Audio_Get_Interface_Setting.
* Description    : test the interface and the alternate setting according to the
*                  supported one.
* Input1         : uint8_t: Interface : interface number.
* Input2         : uint8_t: AlternateSetting : Alternate Setting number.
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
RESULT USB_Audio_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting)
{
  if (AlternateSetting > 2)
  {
    return USB_UNSUPPORT;
  }
  else if (Interface > 2)
  {
    return USB_UNSUPPORT;
  }
  return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : Mute_Command
* Description    : Handle the GET MUTE and SET MUTE command.
* Input          : Length : uint16_t.
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
uint8_t *Mute_Command(uint16_t Length)
{

  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = pInformation->USBwLengths.w;
    return NULL;
  }
  else
  {
    if ( usb_audio.u8Interface == AUDIO_SPK_FEATURE_UNIT_ID ) /*Speaker Volume*/
        return (uint8_t *)(&usb_audio.u8SpkMute);
    else
        return (uint8_t *)(&usb_audio.u8MicMute);
//    return((uint8_t*)(&MUTE_DATA));
  }
}


/*******************************************************************************
* Function Name  : Set Freq
* Description    : Handle Set the Freq
* Input          : Length : uint16_t.
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
uint8_t *Set_Freq_Command(uint16_t Length)
{

  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = pInformation->USBwLengths.w;
    return NULL;
  }
  else
  {
    return (uint8_t *)(&usb_audio.u32Freq);
  }
}


/*******************************************************************************
* Function Name  : Set volume
* Description    : Handle Set the volume
* Input          : Length : uint16_t.
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
uint8_t *Set_Volume_Command(uint16_t Length)
{

  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = pInformation->USBwLengths.w;
    return NULL;
  }
  else
  {
    if ( usb_audio.u8Interface == AUDIO_SPK_FEATURE_UNIT_ID ) /*Speaker Volume*/
        return (uint8_t *)(&usb_audio.u16Volume);
    else
        return (uint8_t *)(&usb_audio.u16MicVolume);
  }
}


/*******************************************************************************
* Function Name  : MIN_Command
* Description    : Handle the GET Min  command.
* Input          : Length : uint16_t.
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
uint8_t *Get_MinVolume_Command(uint16_t Length)
{

  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = pInformation->USBwLengths.w;
    return NULL;
  }
  else
  {
    if ( usb_audio.u8Interface == AUDIO_SPK_FEATURE_UNIT_ID ) /*Speaker Volume*/
        return (uint8_t *)(&usb_audio.u16SpkVolumeLimits[0]);  
    else
        return (uint8_t *)(&usb_audio.u16MicVolumeLimits[0]);        
  }
}
/*******************************************************************************
* Function Name  : Get CurVolume Command
* Description    : Handle the GET CUR command.
* Input          : Length : uint16_t.
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
uint8_t *Get_CurVolume_Command(uint16_t Length)
{

  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = pInformation->USBwLengths.w;
    return NULL;
  }
  else
  {
    if ( usb_audio.u8Interface == AUDIO_SPK_FEATURE_UNIT_ID ) /*Speaker Volume*/
        return (uint8_t *)(&usb_audio.u16Volume);  
    else
        return (uint8_t *)(&usb_audio.u16MicVolume);         
  }
}
/*******************************************************************************
* Function Name  : Max_Command
* Description    : Handle the GET MaxVolume command.
* Input          : Length : uint16_t.
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
uint8_t *Get_MaxVolume_Command(uint16_t Length)
{

  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = pInformation->USBwLengths.w;
    return NULL;
  }
  else
  {
    if ( usb_audio.u8Interface == AUDIO_SPK_FEATURE_UNIT_ID ) /*Speaker Volume*/
        return (uint8_t *)(&usb_audio.u16SpkVolumeLimits[1]);
    else
        return (uint8_t *)(&usb_audio.u16MicVolumeLimits[1]);
  }
}

/*******************************************************************************
* Function Name  : Get Res Command
* Description    : Handle the Get res command.
* Input          : Length : uint16_t.
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
uint8_t *Get_ResVolume_Command(uint16_t Length)
{

  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = pInformation->USBwLengths.w;
    return NULL;
  }
  else
  { 
    if ( usb_audio.u8Interface == AUDIO_SPK_FEATURE_UNIT_ID ) /*Speaker Volume*/
        return (uint8_t *)(&usb_audio.u16SpkVolumeLimits[2]);
    else
        return (uint8_t *)(&usb_audio.u16MicVolumeLimits[2]);
  }
}

/**
  * @}
  */

/**
  * @}
  */
