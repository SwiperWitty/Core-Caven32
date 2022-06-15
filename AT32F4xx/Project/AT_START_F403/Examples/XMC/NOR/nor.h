/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NOR_H
#define __NOR_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"

typedef struct
{
    uint16_t Manufacturer_Code;
    uint16_t Device_Code1;
    uint16_t Device_Code2;
    uint16_t Device_Code3;
}NOR_IDType;

/* NOR Status */
typedef enum
{
    NOR_SUCCESS = 0,
    NOR_ONGOING,
    NOR_ERROR,
    NOR_TIMEOUT
}NOR_Status;  

void NOR_Init(void);
void NOR_ReadID(NOR_IDType* NOR_ID);
NOR_Status NOR_EraseBlock(uint32_t BlockAddr);
NOR_Status NOR_EraseChip(void);
NOR_Status NOR_WriteHalfWord(uint32_t WriteAddr, uint16_t Data);
NOR_Status NOR_WriteBuffer(uint16_t* pBuffer, uint32_t WriteAddr, uint32_t NumHalfwordToWrite);
NOR_Status NOR_ProgramBuffer(uint16_t* pBuffer, uint32_t WriteAddr, uint32_t NumHalfwordToWrite);
uint16_t NOR_ReadHalfWord(uint32_t ReadAddr);
void NOR_ReadBuffer(uint16_t* pBuffer, uint32_t ReadAddr, uint32_t NumHalfwordToRead);
NOR_Status NOR_ReturnToReadMode(void);
NOR_Status NOR_Reset(void);
NOR_Status NOR_GetStatus(uint32_t Timeout);

#ifdef __cplusplus
}
#endif

#endif /* __NOR_H */
