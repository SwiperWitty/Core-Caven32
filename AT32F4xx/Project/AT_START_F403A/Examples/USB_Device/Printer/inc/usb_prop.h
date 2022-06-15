/**
  ******************************************************************************
  * File   : USB_Device/USB_Printer/src/usb_prop.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : All processing related to printer Demo
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_PROP_H
#define __USB_PROP_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef struct
{
  uint32_t bitrate;
  uint8_t format;
  uint8_t paritytype;
  uint8_t datatype;
}LINE_CODING;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/

#define USB_Printer_GetConfiguration          NOP_Process
//#define USB_Printer_SetConfiguration          NOP_Process
#define USB_Printer_GetInterface              NOP_Process
#define USB_Printer_SetInterface              NOP_Process
#define USB_Printer_GetStatus                 NOP_Process
#define USB_Printer_ClearFeature              NOP_Process
#define USB_Printer_SetEndPointFeature        NOP_Process
#define USB_Printer_SetDeviceFeature          NOP_Process
//#define USB_Printer_SetDeviceAddress          NOP_Process

#define SEND_ENCAPSULATED_COMMAND   0x00
#define GET_ENCAPSULATED_RESPONSE   0x01
#define SET_COMM_FEATURE            0x02
#define GET_COMM_FEATURE            0x03
#define CLEAR_COMM_FEATURE          0x04
#define SET_LINE_CODING             0x20
#define GET_LINE_CODING             0x21
#define SET_CONTROL_LINE_STATE      0x22
#define SEND_BREAK                  0x23

#define GET_DEVICE_ID               0x00
#define GET_PORT_STATUS             0x01
#define SOFT_RESET                  0x02

/* Exported functions ------------------------------------------------------- */
void USB_Printer_init(void);
void USB_Printer_Reset(void);
void USB_Printer_SetConfiguration(void);
void USB_Printer_SetDeviceAddress (void);
void USB_Printer_Status_In (void);
void USB_Printer_Status_Out (void);
RESULT USB_Printer_Data_Setup(uint8_t);
RESULT USB_Printer_NoData_Setup(uint8_t);
RESULT USB_Printer_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting);
uint8_t *USB_Printer_GetDeviceDescriptor(uint16_t );
uint8_t *USB_Printer_GetConfigDescriptor(uint16_t);
uint8_t *USB_Printer_GetStringDescriptor(uint16_t);
uint8_t *USB_Printer_ID(uint16_t Length);
uint8_t *USB_Printer_Port_Status(uint16_t Length);

uint8_t *USB_Printer_GetLineCoding(uint16_t Length);
uint8_t *USB_Printer_SetLineCoding(uint16_t Length);


#endif /* __USB_PROP_H */


