/**
  **************************************************************************
  * File   : main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  **************************************************************************
  */
 

/* Includes ------------------------------------------------------------------*/
#include "at32f403_nand.h"
#include "at32f4xx.h"
#include "at32_board.h"


/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */
  
/** @addtogroup XMC_NAND_ECC
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BUFFER_SIZE         0x800
#define NAND_ST_MakerID     0xAD
#define NAND_ST_DeviceID    0xF1

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
NAND_IDTypeDef NAND_ID;
NAND_ADDRESS WriteReadAddr;
uint8_t TxBuffer[BUFFER_SIZE], RxBuffer[BUFFER_SIZE] ,SpareBuffer[NAND_SPARE_AREA_SIZE];
uint32_t PageNumber = 1, WriteReadStatus = 0, status= 0;
uint32_t j = 0 , ECC_Value_Write=0 ,ECC_Value_Read=0 ,ECC_Value_Write_Last=0;

/* Private function prototypes -----------------------------------------------*/
void Fill_Buffer(uint8_t *pBuffer, uint16_t BufferLenght, uint32_t Offset);
void NAND_ECC_Correction(uint8_t *pBuffer,uint32_t TX_ECC_Value ,uint32_t RX_ECC_Value);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief   Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  /* Enable the XMC Clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_XMC, ENABLE);
    
  /* LED Initalization */
  AT32_Board_Init();    
  
  /* XMC Initialization */
  NAND_Init();

  /* NAND read ID command */
  NAND_ReadID(&NAND_ID);

  /* Verify the NAND ID */
  /* NAND support:Spansion:S34ML01G100TFI003  Micron:MT29F16G08CBACA  Sandisk:SDTNQGAMA-008G  Toshiba:TC58TEG6DDKTA00   SAMSUNG:K9GAG08U0E-SCB0   Hynix:H27U1G8F2CTR
                  ID      :0x01F1001D         ID    :0x2C48044A       ID     :0x45DE9493      ID     :0x98DE9493        ID     :0xECD58472        ID   :0xADF1801D
  */  
  if((NAND_ID.Maker_ID == NAND_ST_MakerID) && (NAND_ID.Device_ID == NAND_ST_DeviceID))    
  {

    /* NAND memory address to write to */
    WriteReadAddr.Zone = 0x00;
    WriteReadAddr.Block = 0x00;
    WriteReadAddr.Page = 0x00;

    /* Erase the NAND first Block */
    status = NAND_EraseBlock(WriteReadAddr);

    /* Write data to XMC NAND memory */
    /* Fill the buffer to send */
    Fill_Buffer(TxBuffer, BUFFER_SIZE , 0x0);
    
    /* Change the regular to make ECC Value isn't 0 */
    TxBuffer[10]=0x03;
 
    /* Calculate ECC Value while transmiting */
    XMC_NANDECCCmd(XMC_Bank2_NAND,ENABLE);
    status = NAND_WriteSmallPage(TxBuffer, WriteReadAddr, PageNumber);
    
    /* Save the right ECC_Value (in this case ,we suppose it as the true value) */
    ECC_Value_Write_Last = ECC_Value_Write;
    
    /* Change the 85 data like there is a 1 bit error happened */
    status = NAND_EraseBlock(WriteReadAddr);
    TxBuffer[10]=0x23;    
    status = NAND_WriteSmallPage(TxBuffer, WriteReadAddr, PageNumber);    

    /* Read back the wrong data and get ECC calculation value*/
    XMC_NANDECCCmd(XMC_Bank2_NAND,ENABLE);    
    status = NAND_ReadSmallPage (RxBuffer, WriteReadAddr, PageNumber);
   
   /* ECC check and correct 1 bit error */
    NAND_ECC_Correction(RxBuffer ,ECC_Value_Write_Last ,ECC_Value_Read);
  }
  
  if(RxBuffer[10]==0x03)
  {
    AT32_LEDn_OFF(LED2);
    AT32_LEDn_ON(LED3);
  }
  else
  {
    AT32_LEDn_ON(LED2);
    AT32_LEDn_ON(LED3);
  }
  
  while(1)
  {
  }
}

/**
  * @brief  Fill the buffer
  * @param  pBuffer: pointer on the Buffer to fill
  * @param  BufferSize: size of the buffer to fill
  * @param  Offset: first value to fill on the Buffer
  */
void Fill_Buffer(uint8_t *pBuffer, uint16_t BufferLenght, uint32_t Offset)
{
  uint16_t IndexTmp = 0;

  /* Put in global buffer same values */
  for (IndexTmp = 0; IndexTmp < BufferLenght; IndexTmp++ )
  {
    pBuffer[IndexTmp] = IndexTmp + Offset;
  }
}

/**
  * @brief  ECC Correct 1 bit error
  * @param  pBuffer: pointer on the Buffer to correct 
  * @param  TX_ECC_Value: ECC Value for transmiting to nand flash
  * @param  RX_ECC_Value: ECC Value for receiving from nand flash
  */
void NAND_ECC_Correction(uint8_t *pBuffer,uint32_t TX_ECC_Value ,uint32_t RX_ECC_Value)
{
  uint32_t ECC_Value, Position ,Byte_Position;
  uint8_t i,Compare_data;
  
  /* Check ECC Value */
  if(TX_ECC_Value!=RX_ECC_Value)
  {
    /* 2048 byte ---- 24bit ECC valid value */
    ECC_Value =(TX_ECC_Value^RX_ECC_Value)&0xffffff;
    
    /* 1 bit error correction */
    for(i=0;i<(24/2);i++)
    {
      Compare_data = (ECC_Value>>(i*2))&0x3;
      
      /* Find position */
      if(Compare_data==0x2)
      {
        Position |= (1<<i);
      }
      
      /* more than 1 bit erroc */
      else if(Compare_data!=0x1)
      {
        return;
      }
    }
    
    /* Correct receive value */
    Byte_Position = Position/8;
    pBuffer[Byte_Position] ^= 1 << (Position % 8);
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}

#endif
/**
  * @}
  */

/**
  * @}
  */

