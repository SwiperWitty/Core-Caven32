/**
  ******************************************************************************
  * File   : iap_user.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : IAP HID Demo protocol header
  ******************************************************************************
  */
#ifndef _IAP_USER_H
#define _IAP_USER_H
#include "at32f4xx.h"

#define IAP_RUN_IAP_MODE
//#define IAP_RUN_APP_MODE


/*Max Packet for IN & OUT*/
#define IAP_IN_MAXPKT_SIZE                      0x40
#define IAP_OUT_MAXPKT_SIZE                     0x40

/*IAP Download data FIFO max Size*/
#define IAP_FIFO_SIZE                           1024

 /*IAP device respond value*/
#define IAP_ACK        0xFF00
#define IAP_NACK       0x00FF

/*IAP Command list*/
#define IAP_CMD_IDLE                0x5AA0       /*Enter HID IAP mode*/
#define IAP_CMD_START               0x5AA1
#define IAP_CMD_ADDR                0x5AA2       /*Set download addr*/
#define IAP_CMD_DATA                0x5AA3       /*Download data*/
#define IAP_CMD_FINISH              0x5AA4       /*Download finish*/
#define IAP_CMD_CRC                 0x5AA5       /*Firmware CRC*/
#define IAP_CMD_JMP                 0x5AA6       /*Jump to user Code*/
#define IAP_CMD_GET                 0x5AA7       /*Get IAP User code address*/

#define IAP_CMD_SPIFLASH_GET        0x5AA8
#define IAP_CMD_SPIFLASH_ADDR       0x5AA9
#define IAP_CMD_SPIFLASH_DATA       0x5AAA
#define IAP_CMD_SPIFLASH_CRC        0x5AAB




#define FLASH_APP1_ADDR					0x08004000  /*User App Start address*/	

#define IAP_CMD_FINISH_FLAG_ADDR   (FLASH_APP1_ADDR - 0x800) /*用于断电保护*/
#define IAP_DOWNLOAD_FLAG               0x41544B38


#define FLASH_SIZE_REG()        ((*(UINT32 *)0x1FFFF7E0) & 0xFFFF) /*Get Flash size*/
#define KB_TO_B(kb)             ((kb) << 10)
#define FLASH_SIZE_256K         0x40000
#define INTERNAL_PAGE1K_BYTES                         ((UINT32)0x400)

/** Reverse the word byte order assembly code */
#define CONVERT_ENDIAN(dwValue)   ( (dwValue >> 24) | ((dwValue >> 8) & 0xFF00) | \
                                    ((dwValue << 8) & 0xFF0000) | (dwValue << 24) )


typedef enum
{
    IAP_IDLE,
    IAP_START,
    IAP_ADDR,
    IAP_DATA,
    IAP_FINISH,
    IAP_CRC,
    IAP_JMP_PRE,
    IAP_JUMP,
    IAP_SPI_ADDR,
    IAP_SPI_CRC
}IAP_STEP;

typedef enum 
{
  IAP_SUCCESS,
  IAP_FAILED    
    
}IAP_ResultType;

typedef struct __IAP_InfoType 
{
    uint8_t IAP_FiFo_Buffer[IAP_FIFO_SIZE * 4];          /*Fifo Buffer*/  
    uint8_t IAP_Rx_Buffer[IAP_OUT_MAXPKT_SIZE];      /*Rx buffer*/  
    uint8_t IAP_Tx_Buffer[IAP_IN_MAXPKT_SIZE];       /*Tx buffer*/ 
    uint16_t IAP_Current_Data_LEN;                   /*rx data len*/
    uint16_t IAP_Tx_Len;                             /*send data len*/
    uint32_t IAP_Addr;                               /*download address*/
    uint32_t IAP_APP_Addr;                           /*User App start address*/
    
    uint8_t IAP_Tx_status;                           /*IAP Send State,1 Have a send request, 0, no request*/
    uint8_t IAP_Step;                                /*IAP stat*/
    uint8_t IAP_Sector_size;                         /*1: 1Kbyte, 2:2Kbyte*/
}IAP_InfoType;

extern IAP_InfoType IAP_Info;

#define stohw(str) (((*str) << 8 & 0xFF00) | ((*(str+1)) & 0x00FF))
#define stow(str)  (((*str << 24) & 0xFF000000) | ((*(str+1) << 16) & 0x00FF0000) \
                    | ((*(str+2) << 8) & 0x0000FF00) | ((*(str+3)) & 0x000000FF)) 
                   


IAP_ResultType IAP_Command_Parse(uint8_t *iap_buf, uint32_t pkt_size);
void IAP_Jump(uint32_t dwAddr);
void IAP_handle_loop(void);
IAP_ResultType IAP_get_flag(void);


#endif


