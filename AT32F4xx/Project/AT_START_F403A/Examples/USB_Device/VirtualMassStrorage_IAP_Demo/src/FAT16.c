/**
  ******************************************************************************
  * File   : USB_Device/VirtualMassStorage_IAP_Demo/src/FAT16.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : FAT16 Filesystem source file.
  ******************************************************************************
  */
#include "at32_board.h"
#include "FAT16.h"
#include "string.h"
#include "flash_if.h"
/** @addtogroup AT32F403A_StdPeriph_Examples
  * @{
  */

/** @addtogroup USB_VirtualMassStrorage_IAP_Demo
  * @{
  */ 

/********************************************************************
*********************************************************************
*       FAT16 Boot Sector DBR
*********************************************************************
********************************************************************/
const uint8_t FAT16_BootSector[FATBootSize]= 
{
    0xEB,           /*00 - BS_jmpBoot */
    0x3C,           /*01 - BS_jmpBoot */
    0x90,           /*02 - BS_jmpBoot */
    'M','S','D','O','S','5','.','0',    /* 03-10 - BS_OEMName */
    0x00,           /*11 - BPB_BytesPerSec = 2048 */
    0x08,           /*11 - BPB_BytesPerSec = 2048 */
    0x04,           /*13 - BPB_Sec_PerClus = 2K*4 = 8K*/
    2,              /*14 - BPB_RsvdSecCnt = 2 */
    0,              /*15 - BPB_RsvdSecCnt = 2 */
    2,              /*16 - BPB_NumFATs = 2 */
    0x0,            /*17 - BPB_RootEntCnt = 512 */
    0x2,            /*18 - BPB_RootEntCnt = 512 */
    0,              /*19 - BPB_TotSec16 = 0 */
    0,              /*20 - BPB_TotSec16 = 0 */
    0xF8,           /*21 - BPB_Media = 0xF8 */
    0x0D,           /*22 - BPBFATSz16 = 0x000D */
    0,              /*23 - BPBFATSz16 = 0x000D */
    0x3F,           /*24 - BPB_SecPerTrk = 0x003F */
    0,              /*25 - BPB_SecPerTrk = 0x003F */
    0xFF,           /*26 - BPB_NumHeads = 255 */
    0,              /*27 - BPB_NumHeads = 255 */
    0,              /*28 - BPB_HiddSec = 0 */
    0,              /*29 - BPB_HiddSec = 0 */
    0,              /*30 - BPB_HiddSec = 0 */
    0,              /*31 - BPB_HiddSec = 0 */
    0x00,           /*32 - BPB_TotSec32 = */
    0xC8,           /*33 - BPB_TotSec32 = 0x0000C800 100Mb*/
    0x00,           /*34 - BPB_TotSec32 = */
    0x00,           /*35 - BPB_TotSec32 = */
    0x80,           /*36 - BS_DrvNum = 0x80 */
    0,              /*37 - BS_Reserved1 = 0 , dirty bit = 0*/ /* Updated from FSL*/
    0x29,           /*38 - BS_BootSig = 0x29 */
    0xBD,           /*39 - BS_VolID = 0x02DDA5BD */
    0xA5,           /*40 - BS_VolID = 0x02DDA5BD */
    0xDD,           /*41 - BS_VolID = 0x02DDA5BD */
    0x02,           /*42 - BS_VolID = 0x02DDA5BD */
    'N','O',' ','N','A','M','E',' ',' ',' ',' ',  /*43-53 - BS_VolLab */
    'F','A','T','1','6',' ',' ',' '   /*54-61 - BS_FilSysType */
};

/********************************************************************
*********************************************************************
*       First Sector of FAT Table
*********************************************************************
********************************************************************/
const uint8_t FAT16_TableSector0[FATTableSize]= 
{
    0xF8,0xFF,0xFF,0xFF
};

/********************************************************************
*********************************************************************
*       FAT Root Directory Sector
*********************************************************************
********************************************************************/

uint8_t FAT16_ReadyFileName[FATFileNameSize]= 
{
    'R','e','a','d','y',' ',' ',' ','T','X','T'    /*00-10 - Short File Name */
};

// TODO
// Change Finish to Success

const uint8_t FAT16_RootDirSector[FATDirSize]= 
{
    0x20,           /*11 - Archive Attribute set */
    0x00,           /*12 - Reserved */
    0x4B,           /*13 - Create Time Tenth */
    0x9C,           /*14 - Create Time */
    0x42,           /*15 - Create Time */
    0x92,           /*16 - Create Date */
    0x38,           /*17 - Create Date */
    0x92,           /*18 - Last Access Date */
    0x38,           /*19 - Last Access Date */
    0x00,           /*20 - Not used in FAT16 */
    0x00,           /*21 - Not used in FAT16 */
    0x9D,           /*22 - Write Time */
    0x42,           /*23 - Write Time */
    0x92,           /*24 - Write Date */
    0x38,           /*25 - Write Date */
    0x00,           /*26 - First Cluster (none, because file is empty) */
    0x00,           /*27 - First Cluster (none, because file is empty) */
    0x00,           /*28 - File Size */
    0x00,           /*29 - File Size */
    0x00,           /*30 - File Size */
    0x00,           /*31 - File Size */
    'A','T','3','2',' ','I','A','P',' ',' ',' ',  /*32-42 - Volume label */
    0x08,           /*43 - File attribute = Volume label */
    0x00,           /*44 - Reserved */
    0x00,           /*45 - Create Time Tenth */
    0x00,           /*46 - Create Time */
    0x00,           /*47 - Create Time */
    0x00,           /*48 - Create Date */
    0x00,           /*49 - Create Date */
    0x00,           /*50 - Last Access Date */
    0x00,           /*51 - Last Access Date */
    0x00,           /*52 - Not used in FAT16 */
    0x00,           /*53 - Not used in FAT16 */
    0x9D,           /*54 - Write Time */
    0x42,           /*55 - Write Time */
    0x92,           /*56 - Write Date */
    0x38,           /*57 - Write Date */
};

FAT_DIR_t FileAttr;
static uint32_t FileWrNum = 0;
uint8_t msd_UpStatus;

/**
  * @brief  Read a Logical Block Address
  * @param  FAT_LBA: Logical Block Address to Read
  * @param  data: Pointer to array to store data read  
  * @param  len: data length  
  * @retval read length
  */
uint32_t FATReadLBA(uint32_t FAT_LBA,uint8_t* data, uint32_t len)
{
    /* Body */
    int32_t i;
    switch (FAT_LBA) 
    {
        /* Boot Sector */
        case FAT_DBR_ADDR:
            /* Write Boot Sector info */
            for(i=0;i<FATBootSize;i++)
            {
                *data++ = FAT16_BootSector[i];
            } /* EndFor */
            /* Rest of sector empty except last two bytes */
            i += 2;
            while (i++ < 512) 
            {
                *data++ = 0;
            } /* EndWhile */
                        
            /* Boot Sector requires these 2 bytes at end */
            //*data++ = 0x55;
            //*data++ = 0xAA;
            
            while(i++ < 2048)
            {
                *data++ = 0;
            }            
            break;
        /* FAT Table Sector */
        case FAT_FAT1_ADDR: //26K-FAT0
        case FAT_FAT2_ADDR: //26K-FAT1
            /* Write FAT Table Sector */
            for(i=0;i<FATTableSize;i++)
            {
                *data++ = FAT16_TableSector0[i];
            } /* EndFor */
            /* Rest of sector empty */
            while (i++ < FATBytesPerSec) 
            {
                *data++ = 0;
            } /*ENdWhile */
            break;
            
        /* Root Directory Sector */
        case FAT_ROOT_ADDR: //16K
            
            for(i=0;i<FATFileNameSize;i++) 
            {
                *data++ = FAT16_ReadyFileName[i];
            } /* EndFor */
                            
            /* Write rest of file FAT structure */
            for(i=0;i<FATDirSize;i++) 
            {
                *data++ = FAT16_RootDirSector[i];    
            } /* EndFor */
            /* Rest of sector empty to signify no more files */
            i += FATFileNameSize;
            while (i++ < FATBytesPerSec) 
            {
                *data++ = 0;
            } /* EndWhile */
            break;
            
          /* All other sectors empty */
        default:
            i = 0;
            while (i++ < FATBytesPerSec) 
            {
                *data++ = 0;
            } /* EndWhile */
            break;
    } /* EndSwitch */
    
    return FATBytesPerSec;
} /* EndBody */


/**
  * @brief  write File 
  * @param  FAT_LBA: Logical Block Address to write
  * @param  data: Pointer to array to store data write  
  * @param  len: data length  
  * @retval write len
  */
uint32_t FAT_RootDirWriteRequest(uint32_t FAT_LBA,uint8_t* data, uint32_t len)
{
    FAT_DIR_t* pFile = (FAT_DIR_t*) data;
    uint32_t   index = 2;
    
    pFile++; // Skip Root Dir
    pFile++; // Skip Status File
    
    while ( index++ < 512 )
    {
        if ( (pFile->DIR_Attr == 0x20) && ((pFile+1)->DIR_Attr == 0x00) )
            break;
        else
            pFile ++;
            
    }
    
    /* Get file name from buffer */
    if(index <= 512)
    {
        memcpy(&FileAttr, pFile, 32);
        FileAttr.DIR_WriteTime = 0;
        FileAttr.DIR_WriteDate = 0;
        FileWrNum = 0;
    }
    else
    {
        memset(&FileAttr, (int)0, 32);
    }
    
    return len;
}

/**
  * @brief  Parase Write offset address
  * @param  OfsetBuffer: offset address buffer
  * @retval Write Length
  */

uint32_t FAT_AddrOffsetParase(uint8_t *OfsetBuffer)
{
	uint32_t offset = 0, i, ofshift = 20;
    if ( *OfsetBuffer != 'A' )
        return 0xFFFFFFFF;
	OfsetBuffer += 1;
	for ( i = 0; i < 6; i ++ )
	{
		if ( *OfsetBuffer >= '0' && *OfsetBuffer <= '9' )
			offset |= ((*OfsetBuffer - '0') << ofshift);
		else if (*OfsetBuffer >= 'A' && *OfsetBuffer <= 'F' )
			offset |= ((*OfsetBuffer - 'A' + 0xA) << ofshift);
        else if (*OfsetBuffer >= 'a' && *OfsetBuffer <= 'f' )
            offset |= ((*OfsetBuffer - 'a' + 0xA) << ofshift);
        else /*Default write FLASH_START_ADDR*/
            return 0xAAAAAAAA;
		ofshift -= 4;
		OfsetBuffer ++;
	}
    if ( offset < (FLASH_START_ADDR - FLASH_BASE_ADDR)  )
        return 0xFFFFFFFF;
    else if (offset > (FLASH_BASE_ADDR + AT_FLASH_SIZE - FLASH_START_ADDR))
        return 0xFFFFFFFF;
    
	return offset;
}

/**
  * @brief  Write data to flash
  * @param  FAT_LBA: Logical Block Address to write
  * @param  data: Pointer to array to store data write  
  * @param  len: data length  
  * @retval Write Length
  */
uint32_t FAT_DataSectorWriteRequest(uint32_t FAT_LBA,uint8_t* WrData, uint32_t len)
{
    uint32_t FileSize = (int32_t)FileAttr.DIR_FileSize;
    uint32_t Remaind_Flash = AT_FLASH_SIZE - (FLASH_START_ADDR - FLASH_BASE_ADDR); /*Get the flash remaining space*/
    uint32_t FlashOffset = 0;
    static uint32_t FlashWrAddr = FLASH_START_ADDR;
    uint8_t WStatus = 0;
    
	/*if File expanded-name is BIN or bin , indicate this is a firmware*/
    if ((!memcmp(&(FileAttr.DIR_Name[8]), "BIN", 3) || !memcmp(&(FileAttr.DIR_Name[8]), "bin", 3)))
    {    /*Check the file size*/
            /*download one times, need reset device*/
        if ( msd_UpStatus == UPGREDE_DONE || msd_UpStatus == UPGRADE_SUCCESS)
            return 0;
        FlashOffset = FAT_AddrOffsetParase(FileAttr.DIR_Name);
        /*File not have offset address, default FLASH_START_ADDR*/
        if ( FlashOffset == 0xFFFFFFFF || FlashOffset == 0xAAAAAAAA )
            FlashWrAddr = FLASH_START_ADDR;
        else
            FlashWrAddr = FLASH_BASE_ADDR + FlashOffset;
        if(Remaind_Flash >= FileAttr.DIR_FileSize)
        {
            /*Write data to flash*/           
            if((FileWrNum + len) >= FileSize)
            {
                WStatus = AT_FLASH_WriteBuffer(FlashWrAddr + FileWrNum, WrData, (FileSize - FileWrNum));
                //WStatus = AT_FLASH_WriteBuffer(FlashWrAddr + FAT_LBA - FAT_FILE_START_ADDR, WrData, len - (FileWrNum - FileSize));
            }else
            {
                //WStatus = AT_FLASH_WriteBuffer(FlashWrAddr + FAT_LBA - FAT_FILE_START_ADDR, WrData, len);
                WStatus = AT_FLASH_WriteBuffer(FlashWrAddr + FileWrNum, WrData, len);
            }
            FileWrNum += len;
			if ( WStatus == 0 )
			{
				/*Write data finish*/
				if(FileWrNum >= FileSize)
				{
					FileWrNum = 0;
                    msd_UpStatus = UPGRADE_SUCCESS;
					AT_WriteOptionByte(0);
				}
			}else
			{
				FileWrNum = 0;
                msd_UpStatus = UPGRADE_FAILED;
			}
        }
        else
        {
            msd_UpStatus = UPGRADE_LARGE;
        }
    }
    else if (!memcmp(&(FileAttr.DIR_Name[0]), "JUMP", 4))
    {   /*JUMP to APP run*/
        IAP_Jump(FLASH_START_ADDR);
    }
    else if(!memcmp(&(FileAttr.DIR_Name[8]), "SEC", 3))
    {  /*This is a encryption firmware*/
            /*download one times, need reset device*/
        if ( msd_UpStatus == UPGREDE_DONE || msd_UpStatus == UPGRADE_SUCCESS)
            return 0;
    }
    else
    {   /*Unknown file type*/
        msd_UpStatus = UPGRADE_UNKNOWN;
    }

    return len;
}

/**
  * @brief  Write a Logical Block Address 
  * @param  FAT_LBA: Logical Block Address to write
  * @param  data: Pointer to array to store data write  
  * @param  len: data length  
  * @retval Write Length
  */
uint32_t FATWriteLBA(uint32_t FAT_LBA,uint8_t* data, uint32_t len)
{
    switch(FAT_LBA)
    {
        case FAT_DBR_ADDR: // 4K   Boot Sector
        case FAT_FAT1_ADDR: // 26K  FAT0
        case FAT_FAT2_ADDR: // 26K  FAT1
             break;
        case FAT_ROOT_ADDR: // 16K  Root Directory
             FAT_RootDirWriteRequest(FAT_LBA, data, len);
             break;
        default:
             {
                if(FAT_LBA >= FAT_FILE_START_ADDR)
                {    
                    FAT_DataSectorWriteRequest(FAT_LBA, data, len);
                }
             }
             break;
    }
    
    return FATBytesPerSec;
}

/**
  * @brief  Set a status file name
  * @param  name: status file name
  * @retval file name Length
  */
uint32_t FATSetStatusFileName(const char * name)
{
    uint8_t i;
    uint8_t len = (uint8_t)strlen(name);
    
    for(i=0; i<8 && i<len; i++)
    {
        FAT16_ReadyFileName[i] = name[i];
    }
    
    for(; i < 8; i++)
    {
        FAT16_ReadyFileName[i] = ' ';
    }
    
    return i;
}
/**
  * @}
  */ 

/**
  * @}
  */

