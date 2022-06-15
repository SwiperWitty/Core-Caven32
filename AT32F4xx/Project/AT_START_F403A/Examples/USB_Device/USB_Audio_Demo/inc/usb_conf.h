/**
  ******************************************************************************
  * File   : USB_Device/USB_Audio_Demo/inc/usb_conf.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : USB Endpoint config header.
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_CONF_H
#define __USB_CONF_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/* External variables --------------------------------------------------------*/

/*-------------------------------------------------------------*/
/* EP_NUM */
/* defines how many endpoints are used by the device */
/*-------------------------------------------------------------*/

#define EP_NUM              (4)

/*-------------------------------------------------------------*/
/* --------------   Buffer Description Table  -----------------*/
/*-------------------------------------------------------------*/
/* buffer table base address */
/* buffer table base address */
#define BTABLE_ADDRESS      (0x00)

/* EP0  */
/* rx/tx buffer base address */
#define ENDP0_RXADDR        (0x20)
#define ENDP0_TXADDR        (0x60)

/* EP1 */
/* buffer base address */
#define ENDP1_BUF0Addr      (0x220)  /*BUF0 and BUF1 same, need software control access*/
#define ENDP1_BUF1Addr      (0x220)

/* EP2 */
#define ENDP2_BUF0Addr      (0xA0)
#define ENDP2_BUF1Addr      (0x160)

/* EP3 */
#define ENDP3_BUF0Addr      (0x2E0)
#define ENDP3_BUF1Addr      (0x2E0)

#define AUDIO_IN_EP          0x81
#define AUDIO_OUT_EP         0x02
#define AUDIO_FEED_EP        0x83

#define USB_EDNP_MAX_PACKET 0x120
/*-------------------------------------------------------------*/
/* -------------------   ISTR events  -------------------------*/
/*-------------------------------------------------------------*/
/* IMR_MSK */
/* mask defining which events has to be handled */
/* by the device application software */

//#define IMR_MSK (CTRL_CTFR_IEN  | CTRL_WKUP_IEN | CTRL_SUSP_IEN | CTRL_ERR_IEN  | CTRL_SOF_IEN \
//                 | CTRL_ESOF_IEN | CTRL_RST_IEN)

#define IMR_MSK (CTRL_CTFR_IEN  | CTRL_RST_IEN)

/*#define CTR_CALLBACK*/
/*#define DOVR_CALLBACK*/
/*#define ERR_CALLBACK*/
/*#define WKUP_CALLBACK*/
/*#define SUSP_CALLBACK*/
/*#define RESET_CALLBACK*/
#define SOF_CALLBACK
/*#define ESOF_CALLBACK*/



/* CTR service routines */
/* associated to defined endpoints */
//#define  EP1_IN_Callback   NOP_Process
#define  EP2_IN_Callback   NOP_Process
//#define  EP3_IN_Callback   NOP_Process
#define  EP4_IN_Callback   NOP_Process
#define  EP5_IN_Callback   NOP_Process
#define  EP6_IN_Callback   NOP_Process
#define  EP7_IN_Callback   NOP_Process

#define  EP1_OUT_Callback   NOP_Process
//#define  EP2_OUT_Callback   NOP_Process
#define  EP3_OUT_Callback   NOP_Process
#define  EP4_OUT_Callback   NOP_Process
#define  EP5_OUT_Callback   NOP_Process
#define  EP6_OUT_Callback   NOP_Process
#define  EP7_OUT_Callback   NOP_Process

#endif /*__USB_CONF_H*/



