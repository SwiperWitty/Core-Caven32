/**
  ******************************************************************************
  * File   : usb_regs.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : hardware registers
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_OTG_REGS_H__
#define __USB_OTG_REGS_H__

/* Includes ------------------------------------------------------------------*/
#include "usb_conf.h"


/** @addtogroup USB_OTG_DRIVER
  * @{
  */
  
/** @defgroup USB_REGS
  * @brief This file is the 
  * @{
  */ 


/** @defgroup USB_REGS_Exported_Defines
  * @{
  */ 

#define USBFS_OTG_BASE_ADDR                 0x50000000        

#define USB_OTG_GLOBAL_R_OFFSET      0x000
#define USB_OTG_DEV_R_OFFSET        0x800
#define USB_OTG_DEV_INEP_R_OFFSET         0x900
#define USB_OTG_EPT_R_OFFSET                0x20
#define USB_OTG_DEV_OUTEP_R_OFFSET        0xB00
#define USB_OTG_HOST_GLOBAL_R_OFFSET       0x400
#define USB_OTG_HOST_PORT_R_OFFSET        0x440
#define USB_OTG_HOST_CH_R_OFFSET        0x500
#define USB_OTG_CH_R_OFFSET             0x20
#define USB_OTG_PCGCCTL_OFFSET               0xE00
#define USB_OTG_DATA_FIFO_OFFSET             0x1000
#define USB_OTG_DATA_FIFO_SIZE               0x1000


#define USB_OTG_MAX_TX_FIFO                 15

#define USB_OTG_HS_MAX_PACKET_SIZE           512
#define USBFS_OTG_MAX_PACKET_SIZE           64
#define USB_OTG_MAX_EPT0_SIZE                 64
/**
  * @}
  */ 

/** @defgroup USB_REGS_Exported_Types
  * @{
  */ 

/**
  * @brief Universal Synchronous Asynchronous Receiver Transmitter
  */

/** @defgroup __USB_OTG_Core_register
  * @{
  */
typedef struct _USB_OTG_GLOBAL_REGS  /* 000h */
{
  __IO uint32_t GOTGCTRL;      /* USB_OTG Control and Status Register    000h*/
  __IO uint32_t GOTGINT;      /* USB_OTG Interrupt Register             004h*/
  __IO uint32_t GAHBCFG;      /* Core AHB Configuration Register    008h*/
  __IO uint32_t GUSBCFG;      /* Core USB Configuration Register    00Ch*/
  __IO uint32_t GRSTCTRL;      /* Core Reset Register                010h*/
  __IO uint32_t GINTSTS;      /* Core Interrupt Register            014h*/
  __IO uint32_t GINTMASK;      /* Core Interrupt Mask Register       018h*/
  __IO uint32_t GRXSTSR;      /* Receive Sts Q Read Register        01Ch*/
  __IO uint32_t GRXSTSP;      /* Receive Sts Q Read & POP Register  020h*/
  __IO uint32_t GRXFSIZE;      /* Receive FIFO Size Register         024h*/
  __IO uint32_t DINEPTTXF0_HNPTXFSIZE;   /* EP0 / Non Periodic Tx FIFO Size Register 028h*/
  __IO uint32_t HNPTXSTS;     /* Non Periodic Tx FIFO/Queue Sts reg 02Ch*/
  uint32_t Reserved30h[2];     /* Reserved                           030h*/
  __IO uint32_t GCCFG;        /* General Purpose IO Register        038h*/
  __IO uint32_t CID;          /* User ID Register                   03Ch*/
  uint32_t  Reserved40h[48];   /* Reserved                      040h-0FFh*/
  __IO uint32_t HPTXFSIZE; /* Host Periodic Tx FIFO Size Reg     100h*/
  __IO uint32_t DINEPTTXF[USB_OTG_MAX_TX_FIFO];/* dev Periodic Transmit FIFO */
}
USB_OTG_GLOBAL_REGS;
/**
  * @}
  */


/** @defgroup __device_Registers
  * @{
  */
typedef struct _USB_OTG_DEV_REGS /* 800h */
{
  __IO uint32_t DCFG;         /* dev Configuration Register   800h*/
  __IO uint32_t DCTRL;         /* dev Control Register         804h*/
  __IO uint32_t DSTS;         /* dev Status Register (RO)     808h*/
  uint32_t Reserved0Ch;           /* Reserved                     80Ch*/
  __IO uint32_t DINEPTMASK;   /* dev IN Endpoint Mask         810h*/
  __IO uint32_t DOUTEPTMASK;  /* dev OUT Endpoint Mask        814h*/
  __IO uint32_t DAINT;     /* dev All Endpoints Itr Reg    818h*/
  __IO uint32_t DAINTMASK; /* dev All Endpoints Itr Mask   81Ch*/
  uint32_t  Reserved20h;          /* Reserved                     820h*/
  uint32_t Reserved24h;       /* Reserved                     824h*/
  __IO uint32_t DVBUSDIS;    /* dev VBUS discharge Register  828h*/
  __IO uint32_t DVBUSPULSE;  /* dev VBUS Pulse Register      82Ch*/
  __IO uint32_t DTHRCTRL;     /* dev thr                      830h*/
  __IO uint32_t DINEPTEMPMASK; /* dev empty msk             834h*/
  __IO uint32_t DEACHINT;    /* dedicated EP interrupt       838h*/
  __IO uint32_t DEACHMASK;    /* dedicated EP msk             83Ch*/  
  uint32_t Reserved40h;      /* dedicated EP mask           840h*/
  __IO uint32_t DINEPT1MASK;  /* dedicated EP mask           844h*/
  uint32_t  Reserved44h[15];      /* Reserved                 844-87Ch*/
  __IO uint32_t DOUTEPT1MASK; /* dedicated EP msk            884h*/   
}
USB_OTG_DEV_REGS;
/**
  * @}
  */


/** @defgroup __IN_Endpoint-Specific_Register
  * @{
  */
typedef struct _USB_OTG_INEPREGS
{
  __IO uint32_t DINEPTCTRL; /* dev IN Endpoint Control Reg 900h + (ept_num * 20h) + 00h*/
  uint32_t Reserved04h;             /* Reserved                       900h + (ept_num * 20h) + 04h*/
  __IO uint32_t DINEPTINT; /* dev IN Endpoint Itr Reg     900h + (ept_num * 20h) + 08h*/
  uint32_t Reserved0Ch;             /* Reserved                       900h + (ept_num * 20h) + 0Ch*/
  __IO uint32_t DINEPTTXSIZE; /* IN Endpoint Txfer Size   900h + (ept_num * 20h) + 10h*/
  __IO uint32_t DINEPTDMA; /* IN Endpoint DMA Address Reg    900h + (ept_num * 20h) + 14h*/
  __IO uint32_t DTXFSTS;/*IN Endpoint Tx FIFO Status Reg 900h + (ept_num * 20h) + 18h*/
  uint32_t Reserved18h;             /* Reserved  900h+(ept_num*20h)+1Ch-900h+ (ept_num * 20h) + 1Ch*/
}
USB_OTG_INEPT_REGS;
/**
  * @}
  */


/** @defgroup __OUT_Endpoint-Specific_Registers
  * @{
  */
typedef struct _USB_OTG_OUTEPREGS
{
  __IO uint32_t DOUTEPTCTRL;       /* dev OUT Endpoint Control Reg  B00h + (ept_num * 20h) + 00h*/
  uint32_t Reserved04h;         /* Reserved                      B00h + (ept_num * 20h) + 04h*/
  __IO uint32_t DOUTEPTINT;       /* dev OUT Endpoint Itr Reg      B00h + (ept_num * 20h) + 08h*/
  uint32_t Reserved0Ch;         /* Reserved                      B00h + (ept_num * 20h) + 0Ch*/
  __IO uint32_t DOUTEPTTXSIZE;      /* dev OUT Endpoint Txfer Size   B00h + (ept_num * 20h) + 10h*/
  __IO uint32_t DOUTEPTDMA;       /* dev OUT Endpoint DMA Address  B00h + (ept_num * 20h) + 14h*/
  uint32_t Reserved18h[2];      /* Reserved B00h + (ept_num * 20h) + 18h - B00h + (ept_num * 20h) + 1Ch*/
}
USB_OTG_OUTEPT_REGS;
/**
  * @}
  */


/** @defgroup __Host_Mode_Register_Structures
  * @{
  */
typedef struct _USB_OTG_HREGS
{
  __IO uint32_t HCFG;             /* Host Configuration Register    400h*/
  __IO uint32_t HFIR;      /* Host Frame Interval Register   404h*/
  __IO uint32_t HFNUM;         /* Host Frame Nbr/Frame Remaining 408h*/
  uint32_t Reserved40Ch;                   /* Reserved                       40Ch*/
  __IO uint32_t HPTXSTS;   /* Host Periodic Tx FIFO/ Queue Status 410h*/
  __IO uint32_t HAINT;   /* Host All Channels Interrupt Register 414h*/
  __IO uint32_t HAINTMASK;   /* Host All Channels Interrupt Mask 418h*/
}
USB_OTG_HOST_REGS;
/**
  * @}
  */


/** @defgroup __Host_Channel_Specific_Registers
  * @{
  */
typedef struct _USB_OTG_HC_REGS
{
  __IO uint32_t HCCHAR;
  __IO uint32_t HCSPLT;
  __IO uint32_t HCINT;
  __IO uint32_t HCINTMASK;
  __IO uint32_t HCTSIZE;
  __IO uint32_t HCDMA;
  uint32_t Reserved[2];
}
USB_OTG_HOSTCH_REGS;
/**
  * @}
  */


/** @defgroup __otg_Core_registers
  * @{
  */
typedef struct _USB_OTG_CORE_REGS /* 000h */
{
  USB_OTG_GLOBAL_REGS         *GREGS;
  USB_OTG_DEV_REGS         *DREGS;
  USB_OTG_HOST_REGS         *HREGS;
  USB_OTG_INEPT_REGS      *INEP_REGS[USB_OTG_MAX_TX_FIFO];
  USB_OTG_OUTEPT_REGS     *OUTEP_REGS[USB_OTG_MAX_TX_FIFO];
  USB_OTG_HOSTCH_REGS       *HC_REGS[USB_OTG_MAX_TX_FIFO];
  __IO uint32_t         *HPRT0;
  __IO uint32_t         *DFIFO[USB_OTG_MAX_TX_FIFO];
  __IO uint32_t         *PCGCCTL;
}
USB_OTG_CORE_REGS , *PUSB_OTG_CORE_REGS;
typedef union _USB_OTG_GOTGCTL_Type 
{
  uint32_t u32val;
  struct
  {
uint32_t sreqscs :
    1;
uint32_t sreq :
    1;
uint32_t Reserved2_7 :
    6;
uint32_t hnegscs :
    1;
uint32_t hnpreq :
    1;
uint32_t hshnpen :
    1;
uint32_t dhnpen :
    1;
uint32_t Reserved12_15 :
    4;
uint32_t conidsts :
    1;
uint32_t dbtime :
    1;
uint32_t asesvld :
    1;
uint32_t bsesvld :
    1;
uint32_t Reserved20_31 :
    12;
  }
  b;
} USB_OTG_GOTGCTL_Type ;

typedef union _USB_OTG_GOTGINT_Type 
{
  uint32_t u32val;
  struct
  {
uint32_t Reserved0_1 :
    2;
uint32_t sendet :
    1;
uint32_t Reserved3_7 :
    5;
uint32_t sreqsschg :
    1;
uint32_t hnegsschg :
    1;
uint32_t reserver10_16 :
    7;
uint32_t hnegdet :
    1;
uint32_t adtochg :
    1;
uint32_t dbtdone :
    1;
uint32_t Reserved31_20 :
    12;
  }
  b;
} USB_OTG_GOTGINT_Type ;
typedef union _USB_OTG_GAHBCFG_Type 
{
  uint32_t u32val;
  struct
  {
uint32_t gintmsk :
    1;
uint32_t hbstlen :
    4;
uint32_t dmaen :
    1;
uint32_t Reserved :
    1;
uint32_t txfemlvl :
    1;
uint32_t ptxfemlvl :
    1;
uint32_t Reserved9_31 :
    23;
  }
  b;
} USB_OTG_GAHBCFG_Type ;
typedef union _USB_OTG_GUSBCFG_Type 
{
  uint32_t u32val;
  struct
  {
uint32_t toutcal :
    3;
uint32_t Reserved3_5 :
    3;
uint32_t physel :
    1;
uint32_t Reserved7 :
    1;
uint32_t srpcap :
    1;
uint32_t hnpcap :
    1;
uint32_t trdtim :
    4;
uint32_t Reserved14 :
    1;
uint32_t phylpwrclksel :
    1;
uint32_t Reserved16 :
    1;
uint32_t ulpifsls :
    1;
uint32_t ulpiautores :
    1;
uint32_t ulpiclksusm :
    1;
uint32_t ulpiextvbusdrv :
    1;
uint32_t ulpiintvbusind :
    1;
uint32_t termseldlpulse :
    1;
uint32_t ulpi_ind_cpl :
    1;
uint32_t ulpi_passthrough :
    1;       
uint32_t ulpi_protect_disable :
    1; 
uint32_t Reserved26_28 :
    3;     
uint32_t fhstmod :
    1;
uint32_t fdevmod :
    1;
uint32_t cotxpkt :
    1;
  }
  b;
} USB_OTG_GUSBCFG_Type ;
typedef union _USB_OTG_GRSTCTL_Type 
{
  uint32_t u32val;
  struct
  {
uint32_t csftrst :
    1;
uint32_t hsftrst :
    1;
uint32_t frcnrst :
    1;
uint32_t Reserved3 :
    1;
uint32_t rxfflsh :
    1;
uint32_t txfflsh :
    1;
uint32_t txfnum :
    5;
uint32_t Reserved11_29 :
    19;
uint32_t dmareq :
    1;
uint32_t ahbidle :
    1;
  }
  b;
} USB_OTG_GRSTCTL_Type ;
typedef union _USB_OTG_GINTMSK_Type 
{
  uint32_t u32val;
  struct
  {
uint32_t Reserved0 :
    1;
uint32_t modmism :
    1;
uint32_t otgintm :
    1;
uint32_t sofm :
    1;
uint32_t rxflvlm :
    1;
uint32_t nptxfempm :
    1;
uint32_t ginnakeffm :
    1;
uint32_t goutnakeffm :
    1;
uint32_t Reserved8_9 :
    2;
uint32_t erlysuspm :
    1;
uint32_t usbsuspm :
    1;
uint32_t usbrstm :
    1;
uint32_t enumdonem :
    1;
uint32_t isooutdrm :
    1;
uint32_t eopfm :
    1;
uint32_t Reserved16 :
    1;
uint32_t eptmism :
    1;
uint32_t ieptintm :
    1;
uint32_t oeptintm :
    1;
uint32_t incomisoinm :
    1;
uint32_t incompisooutm :
    1;
uint32_t Reserved22_23 :
    2;
uint32_t hportintm :
    1;
uint32_t hchintm :
    1;
uint32_t ptxfempm :
    1;
uint32_t Reserved27 :
    1;
uint32_t conidschgm :
    1;
uint32_t disconintm :
    1;
uint32_t sreqintm :
    1;
uint32_t wkupintm :
    1;
  }
  b;
} USB_OTG_GINTMSK_Type ;
typedef union _USB_OTG_GINTSTS_Type 
{
  uint32_t u32val;
  struct
  {
uint32_t curmod :
    1;
uint32_t modmis :
    1;
uint32_t otgint :
    1;
uint32_t sof :
    1;
uint32_t rxflvl :
    1;
uint32_t nptxfemp :
    1;
uint32_t ginnakeff :
    1;
uint32_t goutnakeffm :
    1;
uint32_t Reserved8_9 :
    2;
uint32_t erlysusp :
    1;
uint32_t usbsusp :
    1;
uint32_t usbrst :
    1;
uint32_t enumdone :
    1;
uint32_t isooutdr :
    1;
uint32_t eopf :
    1;
uint32_t Reserved16_17 :
    2;
uint32_t ieptint:
    1;
uint32_t oeptint :
    1;
uint32_t incomisoin :
    1;
uint32_t incompisoout :
    1;
uint32_t Reserved22_23 :
    2;
uint32_t hportint :
    1;
uint32_t hchint :
    1;
uint32_t ptxfemp :
    1;
uint32_t Reserved27 :
    1;
uint32_t conidschg :
    1;
uint32_t disconint :
    1;
uint32_t sreqint :
    1;
uint32_t wkupint :
    1;
  }
  b;
} USB_OTG_GINTSTS_Type ;
typedef union _USB_OTG_DRXSTS_Type 
{
  uint32_t u32val;
  struct
  {
uint32_t eptnum :
    4;
uint32_t bcnt :
    11;
uint32_t dpid :
    2;
uint32_t pktsts :
    4;
uint32_t frmnum :
    4;
uint32_t Reserved :
    7;
  }
  b;
} USB_OTG_DRXSTS_Type ;
typedef union _USB_OTG_GRXSTS_Type 
{
  uint32_t u32val;
  struct
  {
uint32_t chnum :
    4;
uint32_t bcnt :
    11;
uint32_t dpid :
    2;
uint32_t pktsts :
    4;
uint32_t Reserved :
    11;
  }
  b;
} USB_OTG_GRXFSTS_Type ;
typedef union _USB_OTG_FSIZ_Type 
{
  uint32_t u32val;
  struct
  {
uint32_t startaddr :
    16;
uint32_t depth :
    16;
  }
  b;
} USB_OTG_FSIZ_Type ;
typedef union _USB_OTG_HNPTXSTS_Type 
{
  uint32_t u32val;
  struct
  {
    uint32_t nptxfsavail :
      16;
    uint32_t nptreqxsavail :
      8;
      struct
        {
          uint32_t terminate :
            1;
          uint32_t token :
            2;
          uint32_t chnum :
            4; 
         } bnptxqtop;
     uint32_t Reserved :
        1;
  }
  b;
} USB_OTG_HNPTXSTS_Type ;
typedef union _USB_OTG_DTXFSTSn_Type 
{
  uint32_t u32val;
  struct
  {
uint32_t txfspcavail :
    16;
uint32_t Reserved :
    16;
  }
  b;
} USB_OTG_DTXFSTSn_Type ;

typedef union _USB_OTG_GCCFG_Type 
{
  uint32_t u32val;
  struct
  {
uint32_t Reserved_in :
    16;
uint32_t pwrdown :
    1;
uint32_t Reserved_17 :
    1;
uint32_t vbusasesen :
    1;
uint32_t vbusbsesen :
    1;
uint32_t sofouten :
    1;
uint32_t Reserved_1 :
    1;
uint32_t Reserved_out :
    10;
  }
  b;
} USB_OTG_GCCFG_Type ;

typedef union _USB_OTG_DCFG_Type 
{
  uint32_t u32val;
  struct
  {
uint32_t devspd :
    2;
uint32_t nzkstsohsk :
    1;
uint32_t Reserved3 :
    1;
uint32_t devaddr :
    7;
uint32_t perfivl :
    2;
uint32_t Reserved12_31 :
    19;
  }
  b;
} USB_OTG_DCFG_Type ;
typedef union _USB_OTG_DCTL_Type 
{
  uint32_t u32val;
  struct
  {
uint32_t rwkupsig :
    1;
uint32_t sftdis :
    1;
uint32_t gnpinnaksts :
    1;
uint32_t goutnaksts :
    1;
uint32_t tstctrl :
    3;
uint32_t sgnpinnak :
    1;
uint32_t cgnpinnak :
    1;
uint32_t sgoutnak :
    1;
uint32_t cgoutnak :
    1;
uint32_t pwroprgdone :
    1;    
uint32_t Reserved :
    20;
  }
  b;
} USB_OTG_DCTL_Type ;
typedef union _USB_OTG_DSTS_Type 
{
  uint32_t u32val;
  struct
  {
uint32_t suspsts :
    1;
uint32_t enumspd :
    2;
uint32_t eticerr :
    1;
uint32_t Reserved4_7:
    4;
uint32_t soffn :
    14;
uint32_t Reserved22_31 :
    10;
  }
  b;
} USB_OTG_DSTS_Type ;
typedef union _USB_OTG_DIEPINTn_Type 
{
  uint32_t u32val;
  struct
  {
uint32_t xferc :
    1;
uint32_t eptdis :
    1;
uint32_t Reserved2 :
    1;
uint32_t timeout :
    1;
uint32_t inttxfemp :
    1;
uint32_t Reserved5 :
    1;
uint32_t ineptnak :
    1;
uint32_t emptyint :
    1;
uint32_t txfifoudn :
    1;
uint32_t Reserved14_31 :
    23;
  }
  b;
} USB_OTG_DIEPINTn_Type ;
typedef union _USB_OTG_DIEPINTn_Type   USB_OTG_DIEPMSK_Type ;
typedef union _USB_OTG_DOEPINTn_Type 
{
  uint32_t u32val;
  struct
  {
uint32_t xferc :
    1;
uint32_t eptdis :
    1;
uint32_t Reserved2 :
    1;
uint32_t setup :
    1;
uint32_t Reserved04_31 :
    28;
  }
  b;
} USB_OTG_DOEPINTn_Type ;
typedef union _USB_OTG_DOEPINTn_Type   USB_OTG_DOEPMSK_Type ;

typedef union _USB_OTG_DAINT_Type 
{
  uint32_t u32val;
  struct
  {
uint32_t in :
    16;
uint32_t out :
    16;
  }
  ept;
} USB_OTG_DAINT_TypeDef ;

typedef union _USB_OTG_DTHRCTL_Type 
{
  uint32_t u32val;
  struct
  {
uint32_t nonisothren :
    1;
uint32_t isothren :
    1;
uint32_t txthrlen :
    9;
uint32_t Reserved11_15 :
    5;
uint32_t rxthren :
    1;
uint32_t rxthrlen :
    9;
uint32_t Reserved26 : 
    1;
uint32_t arbprken :  
    1;
uint32_t Reserved28_31 :
    4;   
  }
  b;
} USB_OTG_DTHRCTL_Type ;
typedef union _USB_OTG_DEPCTL_Type 
{
  uint32_t u32val;
  struct
  {
uint32_t maxpsize :
    11;
uint32_t reserved :
    4;
uint32_t usbaept :
    1;
uint32_t dpid :
    1;
uint32_t naksts :
    1;
uint32_t eptype :
    2;
uint32_t bsnp :
    1;
uint32_t stall :
    1;
uint32_t txfnum :
    4;
uint32_t cnak :
    1;
uint32_t snak :
    1;
uint32_t setd0pid :
    1;
uint32_t soddfrm :
    1;
uint32_t eptdis :
    1;
uint32_t eptena :
    1;
  }
  b;
} USB_OTG_DEPCTL_Type ;
typedef union _USB_OTG_DEPXFRSIZ_Type 
{
  uint32_t u32val;
  struct
  {
uint32_t xfersize :
    19;
uint32_t pktcnt :
    10;
uint32_t mcnt :
    2;
uint32_t Reserved :
    1;
  }
  b;
} USB_OTG_DEPXFRSIZ_Type ;
typedef union _USB_OTG_DEP0XFRSIZ_Type 
{
  uint32_t u32val;
  struct
  {
uint32_t xfersize :
    7;
uint32_t Reserved7_18 :
    12;
uint32_t pktcnt :
    1;
uint32_t Reserved20_28 :
    9;
uint32_t setupcnt :
    2;
uint32_t Reserved31 :
    1;
  }
  b;
} USB_OTG_DEP0XFRSIZ_Type ;
typedef union _USB_OTG_HCFG_Type 
{
  uint32_t u32val;
  struct
  {
uint32_t fslspclks :
    2;
uint32_t fslssupp :
    1;
  }
  b;
} USB_OTG_HCFG_Type ;
typedef union _USB_OTG_HFRMINTRVL_Type 
{
  uint32_t u32val;
  struct
  {
uint32_t fraint :
    16;
uint32_t Reserved :
    16;
  }
  b;
} USB_OTG_HFRMINTRVL_Type ;

typedef union _USB_OTG_HFNUM_Type 
{
  uint32_t u32val;
  struct
  {
uint32_t franum :
    16;
uint32_t frarem :
    16;
  }
  b;
} USB_OTG_HFNUM_Type ;
typedef union _USB_OTG_HPTXSTS_Type 
{
  uint32_t u32val;
  struct
  {
uint32_t ptxfspcavl :
    16;
uint32_t ptxqspcavl :
    8;
      struct
        {
          uint32_t terminate :
            1;
          uint32_t token :
            2;
          uint32_t chnum :
            4; 
          uint32_t odd_even :
            1;            
         } ptxqtop;    
  }
  b;
} USB_OTG_HPTXSTS_Type ;
typedef union _USB_OTG_HPRT0_Type 
{
  uint32_t u32val;
  struct
  {
uint32_t prtconsts :
    1;
uint32_t prtcondet :
    1;
uint32_t prtena :
    1;
uint32_t prtenchng :
    1;
uint32_t prtovca :
    1;
uint32_t prtovcchng :
    1;
uint32_t prtres :
    1;
uint32_t prtsusp :
    1;
uint32_t prtrst :
    1;
uint32_t Reserved24h :
    1;
uint32_t prtlnsts :
    2;
uint32_t prtpwr :
    1;
uint32_t prttstctrl :
    4;
uint32_t prtspd :
    2;
uint32_t Reserved19_31 :
    13;
  }
  b;
} USB_OTG_HPRT0_Type ;
typedef union _USB_OTG_HAINT_Type 
{
  uint32_t u32val;
  struct
  {
uint32_t chaint :
    16;
uint32_t Reserved :
    16;
  }
  b;
} USB_OTG_HAINT_Type ;
typedef union _USB_OTG_HAINTMSK_Type 
{
  uint32_t u32val;
  struct
  {
uint32_t chaintm :
    16;
uint32_t Reserved :
    16;
  }
  b;
} USB_OTG_HAINTMSK_Type ;
typedef union _USB_OTG_HCCHAR_Type 
{
  uint32_t u32val;
  struct
  {
uint32_t maxpsize :
    11;
uint32_t eptnum :
    4;
uint32_t eptdir :
    1;
uint32_t Reserved :
    1;
uint32_t lspddev :
    1;
uint32_t eptype :
    2;
uint32_t mcnt :
    2;
uint32_t devaddr :
    7;
uint32_t oddfrm :
    1;
uint32_t chdis :
    1;
uint32_t chena :
    1;
  }
  b;
} USB_OTG_HCCHAR_Type ;
typedef union _USB_OTG_HCSPLT_Type 
{
  uint32_t u32val;
  struct
  {
uint32_t prtaddr :
    7;
uint32_t hubaddr :
    7;
uint32_t xactpos :
    2;
uint32_t compsplt :
    1;
uint32_t Reserved :
    14;
uint32_t spltena :
    1;
  }
  b;
} USB_OTG_HCSPLT_Type ;
typedef union _USB_OTG_HCINTn_Type 
{
  uint32_t u32val;
  struct
  {
uint32_t xferc :
    1;
uint32_t chhlt :
    1;
uint32_t ahberr :
    1;
uint32_t stall :
    1;
uint32_t nak :
    1;
uint32_t ack :
    1;
uint32_t nyet :
    1;
uint32_t xacterr :
    1;
uint32_t bblerr :
    1;
uint32_t frmovr :
    1;
uint32_t dtgerr :
    1;
uint32_t Reserved :
    21;
  }
  b;
} USB_OTG_HCINTn_Type ;
typedef union _USB_OTG_HCTSIZn_Type
{
  uint32_t u32val;
  struct
  {
uint32_t xfersize :
    19;
uint32_t pktcnt :
    10;
uint32_t dpid :
    2;
uint32_t dopng :
    1;
  }
  b;
} USB_OTG_HCTSIZn_Type ;
typedef union _USB_OTG_HCINTMSK_Type 
{
  uint32_t u32val;
  struct
  {
uint32_t xfercm :
    1;
uint32_t chhltm :
    1;
uint32_t ahberrm :
    1;
uint32_t stallm :
    1;
uint32_t nakm :
    1;
uint32_t ackm :
    1;
uint32_t nyetm :
    1;
uint32_t xacterrm :
    1;
uint32_t bblerrm :
    1;
uint32_t frmovrm :
    1;
uint32_t dtgerrm :
    1;
uint32_t Reserved :
    21;
  }
  b;
} USB_OTG_HCINTMSK_Type ;

typedef union _USB_OTG_PCGCCTL_Type 
{
  uint32_t u32val;
  struct
  {
uint32_t stoppclk :
    1;
uint32_t gatehclk :
    1;
uint32_t Reserved2_3 :
    2;
uint32_t physusp :
    1;    
uint32_t Reserved5_31 :
    27;
  }
  b;
} USB_OTG_PCGCCTL_Type ;

/**
  * @}
  */ 


/** @defgroup USB_REGS_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USB_REGS_Exported_Variables
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USB_REGS_Exported_FunctionsPrototype
  * @{
  */ 
/**
  * @}
  */ 


#endif /* __USB_OTG_REGS_H__ */


/**
  * @}
  */ 

/**
  * @}
  */ 

