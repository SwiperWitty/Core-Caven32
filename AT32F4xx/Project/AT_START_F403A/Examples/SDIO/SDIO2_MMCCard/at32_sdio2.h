/**
  ******************************************************************************
  * File   : SDIO/SDIO2_MMCCard/at32_sdio.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file contains all the functions prototypes for the 
                SD/MMC Card at32_sdio driver firmware library.
  ******************************************************************************
  */
 
/* Define to prevent recursive inclusion -----------------------------------*/
#ifndef __AT32_SDIO_H
#define __AT32_SDIO_H

/* Includes ----------------------------------------------------------------*/                                                                             
#include "at32f4xx.h" 

/* Define SDIO2 remap mode, define only one --------------------------------*/ 
/* #define SDIO2_REMAP01 */
/* #define SDIO2_REMAP10 */
 #define SDIO2_REMAP11

/* Define SDIO Data transfer mode ------------------------------------------*/  
#define SD_POLLING_MODE         0   
#define SD_DMA_MODE                 1   

/* Define bus width---------------------------------------------------------*/
#define SDIO_BusWide_1b         0
#define SDIO_BusWide_4b         1
#define SDIO_BusWide_8b         2

/** 
 * SDIO error defines
 */
typedef enum
{    
    /* SDIO specific error defines ------------------------------------------*/   
    SD_CMD_FAIL                        = (1),  ///< Command response received (but CRC check failed)
    SD_DATA_FAIL                       = (2),  ///< Data bock sent/received (CRC check Failed)
    SD_CMD_RSP_TIMEOUT                 = (3),  ///< Command response timeout
    SD_DATA_TIMEOUT                    = (4),  ///< Data time out
    SD_TX_UNDERRUN                     = (5),  ///< Transmit FIFO under-run
    SD_RX_OVERRUN                      = (6),  ///< Receive FIFO over-run
    SD_START_BIT_ERR                   = (7),  ///< Start bit not detected on all data signals in widE bus mode
    SD_CMD_OUT_OF_RANGE                = (8),  ///< CMD's argument was out of range.
    SD_ADDR_MISALIGNED                 = (9),  ///< Misaligned address
    SD_BLK_LEN_ERR                     = (10), ///< Transferred block length is not allowed for the card or the number of transferred bytes does not match the block length
    SD_ERASE_SEQ_ERR                   = (11), ///< An error in the sequence of erase command occurs.
    SD_INVALID_ERASE_PARAM             = (12), ///< An Invalid selection for erase groups
    SD_WR_PROTECT_VIOLATION            = (13), ///< Attempt to program a write protect block
    SD_LOCK_UNLOCK_FAILED              = (14), ///< Sequence or password error has been detected in unlock command or if there was an attempt to access a locked card
    SD_CMD_CRC_FAILED                  = (15), ///< CRC check of the previous command failed
    SD_ILLEGAL_CMD                     = (16), ///< Command is not legal for the card state
    SD_CARD_ECC_FAILED                 = (17), ///< Card internal ECC was applied but failed to correct the data
    SD_CARD_CONTROLLER_ERR             = (18), ///< Internal card controller error
    SD_GENERAL_UNKNOWN_ERROR           = (19), ///< General or Unknown error
    SD_STREAM_RD_UNDERRUN              = (20), ///< The card could not sustain data transfer in stream read operation.
    SD_STREAM_WR_OVERRUN               = (21), ///< The card could not sustain data programming in stream mode
    SD_CID_CSD_OVERWRITE               = (22), ///< CID/CSD overwrite error
    SD_WP_ERASE_SKIP                   = (23), ///< only partial address space was erased
    SD_CARD_ECC_DISABLED               = (24), ///< Command has been executed without using internal ECC
    SD_ERASE_RESET                     = (25), ///< Erase sequence was cleared before executing because an out of erase sequence command was received
    SD_AKE_SEQ_ERROR                   = (26), ///< Error in sequence of authentication.
    SD_INVALID_VOLTRANGE               = (27), ///< Invalid voltage range
    SD_ADDR_OUT_OF_RANGE               = (28), ///< Address out of range
    SD_SWITCH_ERROR                    = (29), ///< switch error
    SD_SDIO_DISABLED                   = (30), ///< SDIO disabled
    SD_SDIO_FUNC_BUSY                  = (31), ///< function busy
    SD_SDIO_FUNC_FAILED                = (32), ///< function failed
    SD_SDIO_UNKNOWN_FUNC               = (33), ///< unknown function
    
/* Standard error defines --------------------------------------------*/
    SD_INTERNAL_ERROR,                         ///< internal error
    SD_NOT_CONFIGURED,                         ///< SDIO doesn't configuration
    SD_REQ_PENDING,                            ///< request pending
    SD_REQ_NOT_APPLICABLE,                     ///< request isn't applicable
    SD_INVALID_PARAMETER,                      ///< invalid parameter
    SD_UNSUPPORTED_FEATURE,                    ///< unsupported feature
    SD_UNSUPPORTED_HW,                         ///< unsupported hardware
    SD_ERROR,                                  ///< error
    SD_OK = 0                                  ///< pass
} SD_Error;       

/**
 * Card Specific Data: CSD Register
 */
typedef struct
{
    u8  CSDStruct;            ///< CSD structure
    u8  SysSpecVersion;       ///< System specification version
    u8  Reserved1;            ///< Reserved
    u8  TAAC;                 ///< Data read access-time 1
    u8  NSAC;                 ///< Data read access-time 2 in CLK cycles
    u8  MaxBusClkFrec;        ///< Max. bus clock frequency
    u16 CardComdClasses;      ///< Card command classes
    u8  RdBlockLen;           ///< Max. read data block length
    u8  PartBlockRead;        ///< Partial blocks for read allowed
    u8  WrBlockMisalign;      ///< Write block misalignment
    u8  RdBlockMisalign;      ///< Read block misalignment
    u8  DSRImpl;              ///< DSR implemented
    u8  Reserved2;            ///< Reserved
    u32 DeviceSize;           ///< Device Size
    u8  MaxRdCurrentVDDMin;   ///< Max. read current @ VDD min
    u8  MaxRdCurrentVDDMax;   ///< Max. read current @ VDD max
    u8  MaxWrCurrentVDDMin;   ///< Max. write current @ VDD min
    u8  MaxWrCurrentVDDMax;   ///< Max. write current @ VDD max
    u8  DeviceSizeMul;        ///< Device size multiplier
    u8  EraseGrSize;          ///< Erase group size
    u8  EraseGrMul;           ///< Erase group size multiplier
    u8  WrProtectGrSize;      ///< Write protect group size
    u8  WrProtectGrEnable;    ///< Write protect group enable
    u8  ManDeflECC;           ///< Manufacturer default ECC
    u8  WrSpeedFact;          ///< Write speed factor
    u8  MaxWrBlockLen;        ///< Max. write data block length
    u8  WriteBlockPaPartial;  ///< Partial blocks for write allowed
    u8  Reserved3;            ///< Reserded
    u8  ContentProtectAppli;  ///< Content protection application
    u8  FileFormatGrouop;     ///< File format group
    u8  CopyFlag;             ///< Copy flag (OTP)
    u8  PermWrProtect;        ///< Permanent write protection
    u8  TempWrProtect;        ///< Temporary write protection
    u8  FileFormat;           ///< File Format
    u8  ECC;                  ///< ECC code
    u8  CSD_CRC;              ///< CSD CRC
    u8  Reserved4;            ///< always
} SD_CSD;   

/**
 * Card Identification Data: CID Register
 */
typedef struct
{
    u8  ManufacturerID;       ///< ManufacturerID
    u16 OEM_AppliID;          ///< OEM/Application ID
    u32 ProdName1;            ///< Product Name part1
    u8  ProdName2;            ///< Product Name part2
    u8  ProdRev;              ///< Product Revision
    u32 ProdSN;               ///< Product Serial Number
    u8  Reserved1;            ///< Reserved1
    u16 ManufactDate;         ///< Manufacturing Date
    u8  CID_CRC;              ///< CID CRC
    u8  Reserved2;            ///< always 1
} SD_CID;
     
/**
 * SD Card Status
 */
typedef enum
{
    SD_CARD_READY                  = ((uint32_t)0x00000001),
    SD_CARD_IDENTIFICATION         = ((uint32_t)0x00000002),
    SD_CARD_STANDBY                = ((uint32_t)0x00000003),
    SD_CARD_TRANSFER               = ((uint32_t)0x00000004),
    SD_CARD_SENDING                = ((uint32_t)0x00000005),
    SD_CARD_RECEIVING              = ((uint32_t)0x00000006),
    SD_CARD_PROGRAMMING            = ((uint32_t)0x00000007),
    SD_CARD_DISCONNECTED           = ((uint32_t)0x00000008),
    SD_CARD_ERROR                  = ((uint32_t)0x000000FF)
}SDCardState;

/**
 * SD Card SCR information
 */
typedef struct 
{
    u32 SD_SPEC:4;                  ///< [59:56]
    u32 SCR_STRUCTURE:4;            ///< [60:63]

    u32 SD_BUS_WIDTHS:4;            ///< [51:48]
    u32 SD_SECURITY:3;              ///< [52:54]
    u32 DATA_STAT_AFTER_ERASE:1;    ///< [55:55]

    u32 Reserved1:7;                ///< [46:40]
    u32 SD_SPEC3:1;                 ///< [47:47]

    u32 CMD20_SUPPORT:1;            ///< [32:32]
    u32 CMD23_SUPPORT:1;            ///< [33:33]
    u32 Reserverd2:6;               ///< [34:39]
    u32 Reserverd3;                 ///< [31:0]
} SD_SCR;

/**
 * SD Card information
 */
typedef struct
{
  SD_CSD SD_csd;
  SD_CID SD_cid;
    SD_SCR SD_scr;
  long long CardCapacity;   ///< Card Capacity
  u32 CardBlockSize;        ///< Card Block Size
  u16 RCA;                  
  u8 CardType;              
} SD_CardInfo;

extern SD_CardInfo SDCardInfo;       

/**
 * SDIO Commands  Index
 */
#define SD_CMD_GO_IDLE_STATE                       ((u8)0)
#define SD_CMD_SEND_OP_COND                        ((u8)1)
#define SD_CMD_ALL_SEND_CID                        ((u8)2)
#define SD_CMD_SET_REL_ADDR                        ((u8)3) ///< SDIO_SEND_REL_ADDR for SD Card
#define SD_CMD_SET_DSR                             ((u8)4)
#define SD_CMD_SDIO_SEN_OP_COND                    ((u8)5)
#define SD_CMD_HS_SWITCH                           ((u8)6)
#define SD_CMD_SEL_DESEL_CARD                      ((u8)7)
#define SD_CMD_HS_SEND_EXT_CSD                     ((u8)8)
#define SD_CMD_SEND_CSD                            ((u8)9)
#define SD_CMD_SEND_CID                            ((u8)10)
#define SD_CMD_READ_DAT_UNTIL_STOP                 ((u8)11) ///< SD Card doesn't support it
#define SD_CMD_STOP_TRANSMISSION                   ((u8)12)
#define SD_CMD_SEND_STATUS                         ((u8)13)
#define SD_CMD_HS_BUSTEST_READ                     ((u8)14)
#define SD_CMD_GO_INACTIVE_STATE                   ((u8)15)
#define SD_CMD_SET_BLOCKLEN                        ((u8)16)
#define SD_CMD_READ_SINGLE_BLOCK                   ((u8)17)
#define SD_CMD_READ_MULT_BLOCK                     ((u8)18)
#define SD_CMD_HS_BUSTEST_WRITE                    ((u8)19)
#define SD_CMD_WRITE_DAT_UNTIL_STOP                ((u8)20) 
#define SD_CMD_SET_BLOCK_COUNT                     ((u8)23) 
#define SD_CMD_WRITE_SINGLE_BLOCK                  ((u8)24)
#define SD_CMD_WRITE_MULT_BLOCK                    ((u8)25)
#define SD_CMD_PROG_CID                            ((u8)26)
#define SD_CMD_PROG_CSD                            ((u8)27)
#define SD_CMD_SET_WRITE_PROT                      ((u8)28)
#define SD_CMD_CLR_WRITE_PROT                      ((u8)29)
#define SD_CMD_SEND_WRITE_PROT                     ((u8)30)
#define SD_CMD_SD_ERASE_GRP_START                  ((u8)32) ///< To set the address of the first write block to be erased. (For SD card only)
#define SD_CMD_SD_ERASE_GRP_END                    ((u8)33) ///< To set the address of the last write block of the continuous range to be erased. (For SD card only)
#define SD_CMD_ERASE_GRP_START                     ((u8)35) ///< To set the address of the first write block to be erased. (For MMC card only spec 3.31)

#define SD_CMD_ERASE_GRP_END                       ((u8)36) ///< To set the address of the last write block of the continuous range to be erased. (For MMC card only spec 3.31)

#define SD_CMD_ERASE                               ((u8)38)
#define SD_CMD_FAST_IO                             ((u8)39) ///< SD Card doesn't support it
#define SD_CMD_GO_IRQ_STATE                        ((u8)40) ///< SD Card doesn't support it
#define SD_CMD_LOCK_UNLOCK                         ((u8)42)
#define SD_CMD_APP_CMD                             ((u8)55)
#define SD_CMD_GEN_CMD                             ((u8)56)
#define SD_CMD_NO_CMD                              ((u8)64)

/** 
  * Following commands are SD Card Specific commands. SDIO_APP_CMD £ºCMD55 should be sent before sending these commands. 
  */
#define SD_CMD_APP_SD_SET_BUSWIDTH                 ((u8)6)  ///< For SD Card only
#define SD_CMD_SD_APP_STAUS                        ((u8)13) ///< For SD Card only
#define SD_CMD_SD_APP_SEND_NUM_WRITE_BLOCKS        ((u8)22) ///< For SD Card only
#define SD_CMD_SD_APP_OP_COND                      ((u8)41) ///< For SD Card only
#define SD_CMD_SD_APP_SET_CLR_CARD_DETECT          ((u8)42) ///< For SD Card only
#define SD_CMD_SD_APP_SEND_SCR                     ((u8)51) ///< For SD Card only
#define SD_CMD_SDIO_RW_DIRECT                      ((u8)52) ///< For SD I/O Card only
#define SD_CMD_SDIO_RW_EXTENDED                    ((u8)53) ///< For SD I/O Card only

/** 
  * Following commands are SD Card Specific security commands. SDIO_APP_CMD should be sent before sending these commands. 
  */
#define SD_CMD_SD_APP_GET_MKB                      ((u8)43) ///< For SD Card only
#define SD_CMD_SD_APP_GET_MID                      ((u8)44) ///< For SD Card only
#define SD_CMD_SD_APP_SET_CER_RN1                  ((u8)45) ///< For SD Card only
#define SD_CMD_SD_APP_GET_CER_RN2                  ((u8)46) ///< For SD Card only
#define SD_CMD_SD_APP_SET_CER_RES2                 ((u8)47) ///< For SD Card only
#define SD_CMD_SD_APP_GET_CER_RES1                 ((u8)48) ///< For SD Card only
#define SD_CMD_SD_APP_SECURE_READ_MULTIPLE_BLOCK   ((u8)18) ///< For SD Card only
#define SD_CMD_SD_APP_SECURE_WRITE_MULTIPLE_BLOCK  ((u8)25) ///< For SD Card only
#define SD_CMD_SD_APP_SECURE_ERASE                 ((u8)38) ///< For SD Card only
#define SD_CMD_SD_APP_CHANGE_SECURE_AREA           ((u8)49) ///< For SD Card only
#define SD_CMD_SD_APP_SECURE_WRITE_MKB             ((u8)48) ///< For SD Card only
               
/** 
 * Supported SD Memory Cards
 */
#define SDIO_STD_CAPACITY_SD_CARD_V1_1             ((u32)0x00000000)
#define SDIO_STD_CAPACITY_SD_CARD_V2_0             ((u32)0x00000001)
#define SDIO_HIGH_CAPACITY_SD_CARD                 ((u32)0x00000002)
#define SDIO_MULTIMEDIA_CARD                       ((u32)0x00000003)
#define SDIO_SECURE_DIGITAL_IO_CARD                ((u32)0x00000004)
#define SDIO_HIGH_SPEED_MULTIMEDIA_CARD            ((u32)0x00000005)
#define SDIO_SECURE_DIGITAL_IO_COMBO_CARD          ((u32)0x00000006)
#define SDIO_HIGH_CAPACITY_MMC_CARD                ((u32)0x00000007)
#define SDIO_SDIO_CARD                                           ((u32)0x00000008)

/**
 * SDIO paremeters
 */
#define NULL 0
#define SDIO_STATIC_FLAGS               ((u32)0x000005FF)
#define SDIO_CMD0TIMEOUT                ((u32)0x00010000)     
#define SDIO_DATATIMEOUT                ((u32)0xFFFFFFFF)     
#define SDIO_FIFO_Address               ((u32)0x40018080)

/**
 * Mask for errors Card Status R1 (OCR Register)
 */  
#define SD_OCR_ADDR_OUT_OF_RANGE        ((u32)0x80000000)
#define SD_OCR_ADDR_MISALIGNED          ((u32)0x40000000)
#define SD_OCR_BLK_LEN_ERR              ((u32)0x20000000)
#define SD_OCR_ERASE_SEQ_ERR            ((u32)0x10000000)
#define SD_OCR_INVALID_ERASE_PARAM      ((u32)0x08000000)
#define SD_OCR_WR_PROTECT_VIOLATION     ((u32)0x04000000)
#define SD_OCR_LOCK_UNLOCK_FAILED       ((u32)0x01000000)
#define SD_OCR_CMD_CRC_FAILED           ((u32)0x00800000)
#define SD_OCR_ILLEGAL_CMD              ((u32)0x00400000)
#define SD_OCR_CARD_ECC_FAILED          ((u32)0x00200000)
#define SD_OCR_CARD_CONTROLLER_ERR      ((u32)0x00100000)
#define SD_OCR_GENERAL_UNKNOWN_ERROR    ((u32)0x00080000)
#define SD_OCR_STREAM_RD_UNDERRUN       ((u32)0x00040000)
#define SD_OCR_STREAM_WR_OVERRUN            ((u32)0x00020000)
#define SD_OCR_CID_CSD_OVERWRIETE       ((u32)0x00010000)
#define SD_OCR_WP_ERASE_SKIP            ((u32)0x00008000)
#define SD_OCR_CARD_ECC_DISABLED        ((u32)0x00004000)
#define SD_OCR_ERASE_RESET              ((u32)0x00002000)
#define MMC_SWITCH_ERROR                    ((u32)0x00000080)
#define SD_OCR_AKE_SEQ_ERROR            ((u32)0x00000008)
#define SD_OCR_ERRORBITS                ((u32)0xFDFFE008)

/**
 * Masks for R5 Response
 */
#define SD_R5_OUT_OF_RANGE                      ((u32)0x00000100)
#define SD_R5_FUNCTION_NUMBER           ((u32)0x00000200)
#define SD_R5_ERROR                             ((u32)0x00000800)

/**
 * Masks for R6 Response
 */
#define SD_R6_GENERAL_UNKNOWN_ERROR     ((u32)0x00002000)
#define SD_R6_ILLEGAL_CMD               ((u32)0x00004000)
#define SD_R6_CMD_CRC_FAILED            ((u32)0x00008000)

#define SD_VOLTAGE_WINDOW_SD            ((u32)0x80100000)
#define SD_HIGH_CAPACITY                ((u32)0x40000000)
#define SD_STD_CAPACITY                 ((u32)0x00000000)
#define SD_CHECK_PATTERN                ((u32)0x000001AA)
#define SD_VOLTAGE_WINDOW_MMC           ((u32)0x80FF8000)

#define SD_MAX_VOLT_TRIAL               ((u32)0x0000FFFF)
#define SD_ALLZERO                      ((u32)0x00000000)

#define SD_WIDE_BUS_SUPPORT             ((u32)0x00040000)
#define SD_SINGLE_BUS_SUPPORT           ((u32)0x00010000)
#define SD_CARD_LOCKED                  ((u32)0x02000000)
#define SD_CARD_PROGRAMMING             ((u32)0x00000007)
#define SD_CARD_RECEIVING               ((u32)0x00000006)
#define SD_DATATIMEOUT                  ((u32)0xFFFFFFFF)
#define SD_0TO7BITS                     ((u32)0x000000FF)
#define SD_8TO15BITS                    ((u32)0x0000FF00)
#define SD_16TO23BITS                   ((u32)0x00FF0000)
#define SD_24TO31BITS                   ((u32)0xFF000000)
#define SD_MAX_DATA_LENGTH              ((u32)0x01FFFFFF)

#define SD_HALFFIFO                     ((u32)0x00000008)
#define SD_HALFFIFOBYTES                ((u32)0x00000020)

/** 
 * Command Class Supported
 */ 
#define SD_CCCC_LOCK_UNLOCK             ((u32)0x00000080)
#define SD_CCCC_WRITE_PROT              ((u32)0x00000040)
#define SD_CCCC_ERASE                   ((u32)0x00000020)
                                                                     
/** 
 * CMD8
 */
#define SDIO_SEND_IF_COND               ((u32)0x00000008)

/**
 * MMC EXT_CSD operation
 */
#define EXT_CSD_Command_set             0x0
#define EXT_CSD_Set_bit                 0x1
#define EXT_CSD_Clear_byte              0x2
#define EXT_CSD_Write_byte              0x3

#define EXT_CSD_CMD_SET_NORMAL          (1<<0)
#define EXT_CSD_CMD_SET_SECURE          (1<<1)
#define EXT_CSD_CMD_SET_CPSECURE        (1<<2)

#define SDIO_INTR_STS_WRITE_MASK        (SDIO_FLG_DTFAIL|SDIO_FLG_DTTIMEOUT|SDIO_FLG_TXERRU|SDIO_FLG_DTCMPL|SDIO_FLG_SBITERR)
#define SDIO_INTR_STS_READ_MASK         (SDIO_FLG_DTFAIL|SDIO_FLG_DTTIMEOUT|SDIO_FLG_RXERRO|SDIO_FLG_DTCMPL|SDIO_FLG_SBITERR)

/** 
 * MMC EXT_CSD offset
 */
#define EXT_CSD_BUS_WIDTH                               183
#define EXT_CSD_HS_TIMING                               185

/* AT32_SDIO_SD_Exported_Functions ----------------------------------------*/ 
SD_Error SD_Init(void);
void SDIO_Clock_Set(u32 clkdiv);
void SDIO_Send_Cmd(u8 cmdindex,u8 waitrsp,u32 arg);
void SDIO_Send_Data_Cfg(u32 datatimeout,u32 datalen,u8 blksize,u8 dir);
SD_Error SD_PowerON(void);    
SD_Error SD_PowerOFF(void);
SD_Error SD_InitializeCards(void);
SD_Error SD_GetCardInfo(SD_CardInfo *cardinfo);       
SD_Error SD_EnableWideBusOperation(u32 wmode);
SD_Error SD_SetDeviceMode(u32 mode);
SD_Error SD_SelectDeselect(u32 addr); 
SD_Error SD_SendStatus(uint32_t *pcardstatus);
SDCardState SD_GetState(void);
SD_Error SD_EraseBlocks(long long addr,u32 nblks);
SD_Error SD_ReadBlock(u8 *buf,long long addr,u16 blksize);  
SD_Error SD_ReadMultiBlocks(u8 *buf,long long  addr,u16 blksize,u32 nblks);  
SD_Error SD_WriteBlock(u8 *buf,long long addr,  u16 blksize);   
SD_Error SD_WriteMultiBlocks(u8 *buf,long long addr,u16 blksize,u32 nblks);
SD_Error MMC_StreamRead(u8 *buf,long long addr,u32 tlen);
SD_Error MMC_StreamWrite(u8 *buf,long long addr,u32 tlen);
SD_Error SD_ProcessIRQSrc(void);
SD_Error ChangeSpeed(u8 speed);
void SD_DMA_Config(u32*mbuf,u32 bufsize,u32 dir); 

#endif /* __AT32_SDIO_H */






