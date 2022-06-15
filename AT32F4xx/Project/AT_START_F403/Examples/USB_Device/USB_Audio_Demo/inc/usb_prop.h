/*
  **************************************************************************
  * File   : usb_prop.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : All processing related to Audio USB_Audio Demo (Endpoint 0)
  **************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_PROP_H
#define __USB_PROP_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void USB_Audio_init(void);
void USB_Audio_Reset(void);
void USB_Audio_SetConfiguration(void);
void USB_Audio_SetDeviceAddress (void);
void USB_Audio_Status_In (void);
void USB_Audio_Status_Out (void);
void USB_Audio_SetInterface(void);
RESULT USB_Audio_Data_Setup(uint8_t);
RESULT USB_Audio_NoData_Setup(uint8_t);
RESULT USB_Audio_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting);
uint8_t *USB_Audio_GetDeviceDescriptor(uint16_t );
uint8_t *USB_Audio_GetConfigDescriptor(uint16_t);
uint8_t *USB_Audio_GetStringDescriptor(uint16_t);
uint8_t *Mute_Command(uint16_t Length);
uint8_t *Set_Freq_Command(uint16_t Length);
uint8_t *Set_Volume_Command(uint16_t Length);
uint8_t *Get_MinVolume_Command(uint16_t Length);
uint8_t *Get_CurVolume_Command(uint16_t Length);
uint8_t *Get_MaxVolume_Command(uint16_t Length);
uint8_t *Get_ResVolume_Command(uint16_t Length);

/* Exported define -----------------------------------------------------------*/
#define USB_Audio_GetConfiguration          NOP_Process
//#define USB_Audio_SetConfiguration          NOP_Process
#define USB_Audio_GetInterface              NOP_Process
//#define USB_Audio_SetInterface              NOP_Process
#define USB_Audio_GetStatus                 NOP_Process
#define USB_Audio_ClearFeature              NOP_Process
#define USB_Audio_SetEndPointFeature        NOP_Process
#define USB_Audio_SetDeviceFeature          NOP_Process
//#define USB_Audio_SetDeviceAddress          NOP_Process
#define GET_CUR                           0x81
#define SET_CUR                           0x01

#define GET_CUR                           0x81
#define GET_MIN                           0x82
#define GET_MAX                           0x83
#define GET_RES                           0x84


#define USB_AUDIO_SET_VOLUME                0x03
#define USB_AUDIO_SET_MUTE                  0x02
#define USB_AUDIO_SET_FREQ                  0x01

/*Feature Unit Control Selectors*/
#define AUDIO_FU_CONTROL_UNDEFINED        0x00
#define AUDIO_MUTE_CONTROL                0x01
#define AUDIO_VOLUME_CONTROL              0x02


#define AUDIO_CONTROL_FUNCTION            0x01
#define AUDIO_ENDPOINT_FUNCTION           0x02

#endif /* __USB_PROP_H */


