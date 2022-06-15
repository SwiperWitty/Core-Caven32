/**
  ******************************************************************************
  * File   : I2C/EEPROM_INT/at32f4xx_i2c_ex.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : The driver of the EEPROM example based on interrupt mode.
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
#include "at32f4xx_i2c_ex.h"
#include "at32_board.h"
/** @addtogroup AT32F421_StdPeriph_Examples
  * @{
  */

/** @addtogroup I2C EEPROM demo(Interrupt)
  * @brief  The communication is performed in the interrupt mode.The status of 
  *         all data processing is returned by the same function after 
  *         finishing transfer.
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/** 
  * @brief  I2C Timeout definition  
  */  

#define I2C_TIMEOUT_BUSY_FLAG     ((uint32_t)10000)  /* Timeout 10 s  */

/** 
  * @brief  I2C Event check flag definition  
  */  
#define I2C_EVT_CHECK_NONE                   ((uint32_t)0x00000000)
#define I2C_EVT_CHECK_ACKFAIL                ((uint32_t)0x00000001)
#define I2C_EVT_CHECK_STOP                   ((uint32_t)0x00000002)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** @defgroup I2C_EE_Private_Variables
  * @{
  */
uint16_t  sEEAddress = 0;   
uint8_t * sEEDataReadPointer;   
uint8_t*  sEEDataWritePointer;  
uint8_t   sEEDataNum;

static uint16_t I2C_Status;
/**
  * @}
  */ 
/* Private function prototypes -----------------------------------------------*/
I2C_StatusType I2C_EE_WaitEepromStandbyState(I2C_Type* I2Cx, uint32_t Timeout);
/* Exported functions --------------------------------------------------------*/

/**
  * @brief  us delay used by the I2C.
  * @param  us: delay x us.
  * @retval None
  */
void I2C_Delay(uint32_t us)
{
  Delay_us(us);
}

/**
  * @brief  Wait for the communication to end.
  * @param  Timeout Timeout duration.
  * @retval I2C status.
  */
static I2C_StatusType I2C_WaitEnd(uint32_t Timeout)
{
  /* delay 10 us = ms * 100 */
  Timeout *= 100;
  
  while(I2C_Status != I2C_END)
  {
    /* 10 us delay */
    I2C_Delay(10);
    
    /* Check for the Timeout */
    if((Timeout--) == 0)
    {
      return I2C_TIMEOUT;
    }
  }

  return I2C_OK;
}

/** @brief  Clears the I2C ADDR pending flag.
  * @param  I2Cx: where x can be 1, 2 or 3 to select the I2C peripheral.
  * @retval None.
  */
void I2C_ClearADDRFlag(I2C_Type* I2Cx)
{
  __IO uint32_t tmpreg;  
  
  tmpreg = I2Cx->STS1; 
  
  tmpreg = I2Cx->STS2; 
}  

/** @brief  Clears the I2C STOPF pending flag.
  * @param  I2Cx: where x can be 1, 2 or 3 to select the I2C peripheral.
  * @retval None.
  */
void I2C_ClearSTOPFlag(I2C_Type* I2Cx)
{
  __IO uint32_t tmpreg;  
                          
  tmpreg = I2Cx->STS1;
  
  tmpreg = I2Cx->CTRL1 |= I2C_CTRL1_PEN; 
}  

/**
  * @brief  This function handles I2C Communication Timeout.
  * @param  I2Cx: where x can be 1, 2 or 3 to select the I2C peripheral.
  * @param  Flag: specifies the I2C flag to check.
  * @param  Status: The new Flag status (SET or RESET).
  * @param  EventCheck: Check flags at the same time.
  * @param  Timeout Timeout duration.
  * @retval I2C status.
  */
static I2C_StatusType I2C_WaitOnFlagUntilTimeout(I2C_Type* I2Cx, uint32_t Flag, FlagStatus Status, uint32_t EventCheck, uint32_t Timeout)
{
  /* delay 10 us = ms * 100 */
  Timeout *= 100;
  
  while(I2C_GetFlagStatus(I2Cx, Flag) == Status)
  {
    /* Check ACKFAIL Flag */
    if(EventCheck & I2C_EVT_CHECK_ACKFAIL)
    {
      if(I2C_GetFlagStatus(I2Cx, I2C_FLAG_ACKFAIL) == SET)
      {
        /* Send STOP Condition */
        I2C_GenerateSTOP(I2Cx, ENABLE);

        /* Clear AF Flag */
        I2C_ClearFlag(I2Cx, I2C_FLAG_ACKFAIL);
        
        return I2C_ERROR;
      }    
    }

    /* Check STOP Flag */
    if(EventCheck & I2C_EVT_CHECK_STOP)
    {
      if(I2C_GetFlagStatus(I2Cx, I2C_FLAG_STOPF) == SET)
      {
        /* Clear STOP Flag */
        I2C_ClearSTOPFlag(I2Cx);

        return I2C_ERROR;
      }    
    }    

    /* 10 us delay */
    I2C_Delay(10);
    
    /* Check for the Timeout */
    if((Timeout--) == 0)
    {
      return I2C_TIMEOUT;
    }
  }
  
  return I2C_OK;
}

/**
  * @brief  Initializes peripherals used by the I2C.
  * @param  None.
  * @retval None.
  */
void LowLevel_Init(void)
{
  GPIO_InitType GPIO_InitStructure;
  NVIC_InitType NVIC_InitStructure; 
  
  /* I2C_SCL_GPIO_CLK and I2C_SDA_GPIO_CLK Periph clock enable */
  RCC_AHBPeriphClockCmd(I2C_SCL_GPIO_CLK | I2C_SDA_GPIO_CLK, ENABLE);

  /* I2C Periph clock enable */
  RCC_APB1PeriphClockCmd(I2C_CLK, ENABLE);
    
  /* GPIO configuration */  
  /* Configure I2C pins: SCL */
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_Mode     = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OutType  = GPIO_OutType_OD;
  GPIO_InitStructure.GPIO_Pull     = GPIO_Pull_NOPULL;  
  
  GPIO_InitStructure.GPIO_Pins     = I2C_SCL_PIN;
  GPIO_Init(I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

  /* Configure I2C pins: SDA */
  GPIO_InitStructure.GPIO_Pins     = I2C_SDA_PIN;
  GPIO_Init(I2C_SDA_GPIO_PORT, &GPIO_InitStructure); 
  
  GPIO_PinAFConfig(I2C_SCL_GPIO_PORT, I2C_SCL_GPIO_PinsSource, I2C_SCL_GPIO_GPIO_AF);
  
  GPIO_PinAFConfig(I2C_SDA_GPIO_PORT, I2C_SDA_GPIO_PinsSource, I2C_SDA_GPIO_GPIO_AF);
  
  NVIC_InitStructure.NVIC_IRQChannel = I2Cx_EV_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  

  NVIC_InitStructure.NVIC_IRQChannel = I2Cx_ER_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Initializes peripherals used by the I2C EEPROM driver.
  * @param  None.
  * @retval None.
  */
void I2Cx_Init(I2C_Type* I2Cx)
{ 
  I2C_InitType  I2C_InitStructure;
  
  LowLevel_Init();
  
  /* I2C configuration */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2CDevice;
  I2C_InitStructure.I2C_FmDutyCycle = I2C_FmDutyCycle_2_1;
  I2C_InitStructure.I2C_OwnAddr1 = I2C_SLAVE_ADDRESS7;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AddrMode = I2C_AddrMode_7bit;
  I2C_InitStructure.I2C_BitRate = I2C_SPEED;
  
  /* I2C Peripheral Enable */
  I2C_Cmd(I2Cx, ENABLE);
  
  /* Apply I2C configuration after enabling it */
  I2C_Init(I2Cx, &I2C_InitStructure);
}

/**
  * @brief  Reads a block of data from the EEPROM.
  * @param  I2Cx: where x can be 1, 2 or 3 to select the I2C peripheral.
  * @param  ReadAddr Internal memory address.
  * @param  pBuffer Pointer to data buffer.
  * @param  NumByteToRead : pointer to the variable holding number of bytes to 
  *         be read from the EEPROM.
  * @param  Timeout Timeout duration.
  * @retval I2C status.
  */
I2C_StatusType I2C_EE_ReadBuffer_INT(I2C_Type* I2Cx, uint16_t ReadAddr, uint8_t *pBuffer, uint16_t NumByteToRead, uint32_t Timeout)
{
  /* Update the number of send data*/
  sEEDataNum = NumByteToRead;  
  
  /* Update the cache of send data*/ 
  sEEDataReadPointer = pBuffer;
  
  /* Update the I2C Status*/  
  I2C_Status = I2C_START; 
    
  /* Wait until BUSY flag is reset */
  if(I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_BUSYF, SET, I2C_EVT_CHECK_NONE, I2C_TIMEOUT_BUSY_FLAG) != I2C_OK)
  {
    return I2C_ERROR_STEP_1;
  }

  /* Disable Pos */
  I2C_NACKPositionConfig(I2Cx, I2C_NACKPosition_Current);

  /* Enable Acknowledge */
  I2C_AcknowledgeConfig(I2Cx, ENABLE); 

  /* Send START condition */
  I2C_GenerateSTART(I2Cx, ENABLE);

  /* Wait until SB flag is set */
  if(I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_STARTF, RESET, I2C_EVT_CHECK_NONE, Timeout) != I2C_OK)
  {
    /* Send STOP Condition */
    I2C_GenerateSTOP(I2Cx, ENABLE);
    
    return I2C_ERROR_STEP_2;
  }

  /* Send slave address for write */
  I2C_Send7bitAddress(I2Cx, I2C_EEAddress, I2C_Direction_Transmit);
  
  /* Wait until ADDR flag is set */
  if(I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_ADDRF, RESET, I2C_EVT_CHECK_ACKFAIL, Timeout) != I2C_OK)
  {
    /* Send STOP Condition */
    I2C_GenerateSTOP(I2Cx, ENABLE);
    
    return I2C_ERROR_STEP_3;
  }
  
  /* Clear ADDR flag */
  I2C_ClearADDRFlag(I2Cx);  

  /* Wait until TDE flag is set */
  if(I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_TDE, RESET, I2C_EVT_CHECK_NONE, Timeout) != I2C_OK)
  {
    /* Send STOP Condition */
    I2C_GenerateSTOP(I2Cx, ENABLE);
    
    return I2C_ERROR_STEP_4;
  }  
  
  /* Send Memory Address */
  I2C_SendData(I2Cx, ReadAddr);
  
  /* Wait until TDE flag is set */
  if(I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_TDE, RESET, I2C_EVT_CHECK_ACKFAIL, Timeout) != I2C_OK)
  {
    /* Send STOP Condition */
    I2C_GenerateSTOP(I2Cx, ENABLE);
    
    return I2C_ERROR_STEP_5;
  }    
  
  /* Send START condition */
  I2C_GenerateSTART(I2Cx, ENABLE);

  /* Wait until SB flag is set */
  if(I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_STARTF, RESET, I2C_EVT_CHECK_NONE, Timeout) != I2C_OK)
  {
    /* Send STOP Condition */
    I2C_GenerateSTOP(I2Cx, ENABLE);
    
    return I2C_ERROR_STEP_6;
  }  
  
  /* Send slave address for read */
  I2C_Send7bitAddress(I2Cx, I2C_EEAddress, I2C_Direction_Receive);
  
  /* Wait until ADDR flag is set */
  if(I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_ADDRF, RESET, I2C_EVT_CHECK_ACKFAIL, Timeout) != I2C_OK)
  {
    /* Send STOP Condition */
    I2C_GenerateSTOP(I2Cx, ENABLE);
    
    return I2C_ERROR_STEP_7;
  }
  
  if(NumByteToRead == 1)
  {
    /* Disable Acknowledge */
    I2C_AcknowledgeConfig(I2Cx, DISABLE);

    /* Clear ADDR flag */
    I2C_ClearADDRFlag(I2Cx);

    /* Send STOP Condition */
    I2C_GenerateSTOP(I2Cx, ENABLE);
  }
  else if(NumByteToRead == 2)
  {
    /* Enable Pos */
    I2C_NACKPositionConfig(I2Cx, I2C_NACKPosition_Next);
    
    /* Clear ADDR flag */
    I2C_ClearADDRFlag(I2Cx);

    /* Disable Acknowledge */
    I2C_AcknowledgeConfig(I2Cx, DISABLE);
  }
  else
  {
    /* Enable Acknowledge */
    I2C_AcknowledgeConfig(I2Cx, ENABLE); 

    /* Clear ADDR flag */
    I2C_ClearADDRFlag(I2Cx);
  }
  
  /* Enable EVT, BUF and ERR interrupt */
  I2C_INTConfig(I2Cx, I2C_INT_EVT | I2C_INT_BUF | I2C_INT_ERR, ENABLE);
 
  return I2C_WaitEnd(Timeout);
}

/**
  * @brief  Write an amount of data in blocking mode to a specific memory address.
  * @param  I2Cx: where x can be 1, 2 or 3 to select the I2C peripheral.
  * @param  DevAddress Target device address.
  * @param  MemAddress Internal memory address.
  * @param  MemAddSize Size of internal memory address.
  * @param  pBuffer Pointer to data buffer.
  * @param  Size Amount of data to be sent.
  * @param  Timeout Timeout duration.
  * @retval I2C status.
  */
I2C_StatusType I2C_EE_WritePage_INT(I2C_Type* I2Cx, uint16_t WriteAddr, uint8_t *pBuffer, uint16_t NumByteToWrite, uint32_t Timeout)
{
  /* Update the number of send data*/
  sEEDataNum = NumByteToWrite;  
  
  /* Update the cache of send data*/ 
  sEEDataWritePointer = pBuffer;
  
  /* Update the I2C Status*/  
  I2C_Status = I2C_START; 
  
  /* Wait until BUSY flag is reset */
  if(I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_BUSYF, SET, I2C_EVT_CHECK_NONE, I2C_TIMEOUT_BUSY_FLAG) != I2C_OK)
  {
    return I2C_ERROR_STEP_1;
  }

  /* Disable Pos */
  I2C_NACKPositionConfig(I2Cx, I2C_NACKPosition_Current);
  
  /* Send START condition */
  I2C_GenerateSTART(I2Cx, ENABLE);

  /* Wait until SB flag is set */
  if(I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_STARTF, RESET, I2C_EVT_CHECK_NONE, Timeout) != I2C_OK)
  {
    /* Send STOP Condition */
    I2C_GenerateSTOP(I2Cx, ENABLE);
    
    return I2C_ERROR_STEP_2;
  }

  /* Send slave address for write */
  I2C_Send7bitAddress(I2Cx, I2C_EEAddress, I2C_Direction_Transmit);
  
  /* Wait until ADDR flag is set */
  if(I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_ADDRF, RESET, I2C_EVT_CHECK_ACKFAIL, Timeout) != I2C_OK)
  {
    /* Send STOP Condition */
    I2C_GenerateSTOP(I2Cx, ENABLE);
    
    return I2C_ERROR_STEP_3;
  }
  
  /* Clear ADDR flag */
  I2C_ClearADDRFlag(I2Cx);  

  /* Wait until TDE flag is set */
  if(I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_TDE, RESET, I2C_EVT_CHECK_NONE, Timeout) != I2C_OK)
  {
    /* Send STOP Condition */
    I2C_GenerateSTOP(I2Cx, ENABLE);
    
    return I2C_ERROR_STEP_4;
  }  
  
  /* Send Memory Address */
  I2C_SendData(I2Cx, WriteAddr);
  
  /* Wait until TDE flag is set */
  if(I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_TDE, RESET, I2C_EVT_CHECK_ACKFAIL, Timeout) != I2C_OK)
  {
    /* Send STOP Condition */
    I2C_GenerateSTOP(I2Cx, ENABLE);
    
    return I2C_ERROR_STEP_5;
  }

  /* Enable EVT, BUF and ERR interrupt */
  I2C_INTConfig(I2Cx, I2C_INT_EVT | I2C_INT_BUF | I2C_INT_ERR, ENABLE);
 
  return I2C_WaitEnd(Timeout);
}

/**
  * @brief  Writes buffer of data to the I2C EEPROM.
  * @param  I2Cx: where x can be 1, 2 or 3 to select the I2C peripheral.
  * @param  pBuffer : pointer to the buffer  containing the data to be written 
  *         to the EEPROM.
  * @param  WriteAddr : EEPROM's internal address to write to.
  * @param  NumByteToWrite : number of bytes to write to the EEPROM.
  * @param  Timeout Timeout duration
  * @retval I2C status
  */
I2C_StatusType I2C_EE_WriteBuffer_INT(I2C_Type* I2Cx, uint16_t WriteAddr, uint8_t* pBuffer, uint16_t NumByteToWrite, uint32_t Timeout)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, count = 0;
  uint16_t Addr = 0;

  Addr = WriteAddr % sEE_PAGESIZE;
  count = sEE_PAGESIZE - Addr;
  NumOfPage =  NumByteToWrite / sEE_PAGESIZE;
  NumOfSingle = NumByteToWrite % sEE_PAGESIZE;
 
  /*!< If WriteAddr is sEE_PAGESIZE aligned  */
  if(Addr == 0) 
  {
    /*!< If NumByteToWrite < sEE_PAGESIZE */
    if(NumOfPage == 0) 
    {
      /* Store the number of data to be written */
      sEEDataNum = NumOfSingle;
      
      /* Start writing data */
      I2C_EE_WritePage_INT(I2Cx, WriteAddr, pBuffer, sEEDataNum, Timeout);
      
      /* Wait eeprom write complete */
      I2C_EE_WaitEepromStandbyState(I2Cx, Timeout);
    }
    /*!< If NumByteToWrite > sEE_PAGESIZE */
    else  
    {
      while(NumOfPage--)
      {
        /* Store the number of data to be written */
        sEEDataNum = sEE_PAGESIZE;
        
        /* Start writing data */
        I2C_EE_WritePage_INT(I2Cx, WriteAddr, pBuffer, sEEDataNum, Timeout);
        
        /* Wait eeprom write complete */
        I2C_EE_WaitEepromStandbyState(I2Cx, Timeout);
        
        WriteAddr +=  sEE_PAGESIZE;
        pBuffer += sEE_PAGESIZE;
      }

      if(NumOfSingle!=0)
      {
        /* Store the number of data to be written */
        sEEDataNum = NumOfSingle;   
        
        /* Start writing data */
        I2C_EE_WritePage_INT(I2Cx, WriteAddr, pBuffer, sEEDataNum, Timeout);
        
        /* Wait eeprom write complete */
        I2C_EE_WaitEepromStandbyState(I2Cx, Timeout);
      }
    }
  }
  /*!< If WriteAddr is not sEE_PAGESIZE aligned  */
  else 
  {
    /*!< If NumByteToWrite < sEE_PAGESIZE */
    if(NumOfPage== 0) 
    {
      /*!< If the number of data to be written is more than the remaining space 
      in the current page: */
      if (NumByteToWrite > count)
      {
        /* Store the number of data to be written */
        sEEDataNum = count;     
        
        /*!< Write the data conained in same page */
        I2C_EE_WritePage_INT(I2Cx, WriteAddr, pBuffer, sEEDataNum, Timeout);
        
        /* Wait eeprom write complete */
        I2C_EE_WaitEepromStandbyState(I2Cx, Timeout);   
        
        /* Store the number of data to be written */
        sEEDataNum = (NumByteToWrite - count);        
        
        /*!< Write the remaining data in the following page */
        I2C_EE_WritePage_INT(I2Cx, (WriteAddr + count), (uint8_t*)(pBuffer + count), sEEDataNum, Timeout);
        
        /* Wait eeprom write complete */
        I2C_EE_WaitEepromStandbyState(I2Cx, Timeout);        
      }      
      else      
      {
        /* Store the number of data to be written */
        sEEDataNum = NumOfSingle;    
        
        /*!< Write the data conained in same page */
        I2C_EE_WritePage_INT(I2Cx, WriteAddr, pBuffer, sEEDataNum, Timeout);
        
        /* Wait eeprom write complete */
        I2C_EE_WaitEepromStandbyState(I2Cx, Timeout);     
      }     
    }
    /*!< If NumByteToWrite > sEE_PAGESIZE */
    else
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / sEE_PAGESIZE;
      NumOfSingle = NumByteToWrite % sEE_PAGESIZE;
      
      if(count != 0)
      {  
        /* Store the number of data to be written */
        sEEDataNum = count;    
        
        /*!< Write the data conained in same page */
        I2C_EE_WritePage_INT(I2Cx, WriteAddr, pBuffer, sEEDataNum, Timeout);
        
        /* Wait eeprom write complete */
        I2C_EE_WaitEepromStandbyState(I2Cx, Timeout);
        
        WriteAddr += count;
        pBuffer += count;
      } 
      
      while(NumOfPage--)
      {
        /* Store the number of data to be written */
        sEEDataNum = sEE_PAGESIZE;     
        
        /*!< Write the data conained in same page */
        I2C_EE_WritePage_INT(I2Cx, WriteAddr, pBuffer, sEEDataNum, Timeout);
        
        /* Wait eeprom write complete */
        I2C_EE_WaitEepromStandbyState(I2Cx, Timeout);
        
        WriteAddr +=  sEE_PAGESIZE;
        pBuffer += sEE_PAGESIZE;  
      }
      if(NumOfSingle != 0)
      {
        /* Store the number of data to be written */
        sEEDataNum = NumOfSingle;    
        
        /*!< Write the data conained in same page */
        I2C_EE_WritePage_INT(I2Cx, WriteAddr, pBuffer, sEEDataNum, Timeout);

        /* Wait eeprom write complete */
        I2C_EE_WaitEepromStandbyState(I2Cx, Timeout);
      }
    }
  }  
  
  return I2C_OK;
}

/**
  * @brief  Checks if target device is ready for communication.
  * @note   This function is used with Memory devices.
  * @param  I2Cx: where x can be 1, 2 or 3 to select the I2C peripheral.
  * @param  DevAddress Target device address.
  * @param  Trials Number of trials.
  * @param  Timeout Timeout duration.
  * @retval I2C status.
  */
I2C_StatusType I2C_IsDeviceReady(I2C_Type* I2Cx, uint16_t DevAddress, uint32_t Timeout)
{
  /* Wait until BUSY flag is reset */
  if(I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_BUSYF, SET, I2C_EVT_CHECK_NONE, Timeout) != I2C_OK)
  {
    return I2C_ERROR_STEP_1;
  }

  /* Disable Pos */
  I2C_NACKPositionConfig(I2Cx, I2C_NACKPosition_Current);
 
  /* Send START condition */
  I2C_GenerateSTART(I2Cx, ENABLE);

  /* Wait until SB flag is set */
  if(I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_STARTF, RESET, I2C_EVT_CHECK_NONE, Timeout) != I2C_OK)
  {
    /* Send STOP Condition */
    I2C_GenerateSTOP(I2Cx, ENABLE);
    
    return I2C_ERROR_STEP_2;
  }

  /* Send slave address for write */
  I2C_Send7bitAddress(I2Cx, DevAddress, I2C_Direction_Transmit);
  
  /* Wait until ADDR flag is set */
  if(I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_ADDRF, RESET, I2C_EVT_CHECK_ACKFAIL, Timeout) != I2C_OK)
  {
    /* Send STOP Condition */
    I2C_GenerateSTOP(I2Cx, ENABLE);

    return I2C_ERROR_STEP_3;
  }
  
  /* Clear ADDR flag */
  I2C_ClearADDRFlag(I2Cx);  
  
  /* Send STOP Condition */
  I2C_GenerateSTOP(I2Cx, ENABLE);  
  
  /* Wait until BUSY flag is reset */
  if(I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_BUSYF, SET, I2C_EVT_CHECK_NONE, Timeout) != I2C_OK)
  {
    return I2C_ERROR_STEP_4;
  }  

  return I2C_OK;  
}

/**
  * @brief  Wait for EEPROM Standby state.
  * 
  * @note  This function allows to wait and check that EEPROM has finished the 
  *        last operation. It is mostly used after Write operation: after receiving
  *        the buffer to be written, the EEPROM may need additional time to actually
  *        perform the write operation. During this time, it doesn't answer to
  *        I2C packets addressed to it. Once the write operation is complete
  *        the EEPROM responds to its address.
  * 
  * @param  I2Cx: where x can be 1, 2 or 3 to select the I2C peripheral.
  * @param  Timeout Timeout duration.
  * @retval I2C status.
  */
I2C_StatusType I2C_EE_WaitEepromStandbyState(I2C_Type* I2Cx, uint32_t Timeout)
{
  uint32_t sEETrials = 0;

  while(1)
  {
    /* Check if the maximum allowed numbe of trials has bee reached */
    if (sEETrials++ == sEE_MAX_TRIALS_NUMBER)
    {
      /* If the maximum number of trials has been reached, exit the function */
      return I2C_BUSY;
    } 
    
    if(I2C_IsDeviceReady(I2Cx, I2C_EEAddress, Timeout) == I2C_OK)
    {
      return I2C_OK;
    }
  }
}

/**
  * @brief  This function handles I2C event interrupt request.
  * @param  I2Cx: where x can be 1, 2 or 3 to select the I2C peripheral.
  * @retval None.
  */
void I2C_EV_IRQHandler(I2C_Type* I2Cx)
{
  /* Write data */
  if(I2C_GetFlagStatus(I2Cx, I2C_FLAG_TDE) == SET)
  {
    if(sEEDataNum == 0)
    {
      /* End */
      I2C_Status = I2C_END;
      
      /* Disable BUF interrupt */
      I2C_INTConfig(I2Cx, I2C_INT_EVT | I2C_INT_BUF | I2C_INT_ERR, DISABLE);         
      
      /* Generate Stop */
      I2C_GenerateSTOP(I2Cx, ENABLE);      
    }
    else
    {
      /* Write data to DR */
      I2C_SendData(I2Cx, *sEEDataWritePointer++);
      sEEDataNum--;
    }
  }  
  
  /* Read data */  
  else if(I2C_GetFlagStatus(I2Cx, I2C_FLAG_RDNE) == SET)
  {
    if(sEEDataNum)
    {
      /* Read data from DR */
      (*sEEDataReadPointer++) = I2C_ReceiveData(I2Cx);
      sEEDataNum--;     

      if(sEEDataNum == 1)
      {
        /* Disable ack */
        I2C_AcknowledgeConfig(I2Cx, DISABLE);
        
        /* Generate Stop */
        I2C_GenerateSTOP(I2Cx, ENABLE);     
      }
      else if(sEEDataNum == 0)
      {
        /* Disable BUF interrupt */
        I2C_INTConfig(I2Cx, I2C_INT_EVT | I2C_INT_BUF | I2C_INT_ERR, DISABLE);     

        /* End */
        I2C_Status = I2C_END;
      }
    }
  }
}

/**
  * @brief  This function handles I2C error interrupt request.
  * @param  I2Cx: where x can be 1, 2 or 3 to select the I2C peripheral.
  * @retval None.
  */
void I2C_ER_IRQHandler(I2C_Type* I2Cx)
{
  /* I2C Bus error interrupt occurred */
  if(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSERR) == SET)
  {
    I2C_ClearFlag(I2Cx, I2C_FLAG_BUSERR);
  }    
  
  /* I2C Arbitration Loss error interrupt occurred */
  if(I2C_GetFlagStatus(I2Cx, I2C_FLAG_ARLOST) == SET)
  {
    I2C_ClearFlag(I2Cx, I2C_FLAG_ARLOST);
  }  

  /* I2C Over-Run/Under-Run interrupt occurred */
  if(I2C_GetFlagStatus(I2Cx, I2C_FLAG_OVRUN) == SET)
  {
    I2C_ClearFlag(I2Cx, I2C_FLAG_OVRUN);
  }  

  /* I2C acknowledge fail occurred */
  if(I2C_GetFlagStatus(I2Cx, I2C_FLAG_ACKFAIL) == SET)
  {
    I2C_ClearFlag(I2Cx, I2C_FLAG_ACKFAIL);
  }
  
  /* I2C SMBus timeout occurred */
  if(I2C_GetFlagStatus(I2Cx, I2C_FLAG_TIMOUT) == SET)
  {
    I2C_ClearFlag(I2Cx, I2C_FLAG_TIMOUT);
  }  

  /* I2C PEC error occurred */
  if(I2C_GetFlagStatus(I2Cx, I2C_FLAG_PECERR) == SET)
  {
    I2C_ClearFlag(I2Cx, I2C_FLAG_PECERR);
  }  
  
  /* Disable the error interrupt */
  I2C_INTConfig(I2Cx, I2C_INT_EVT | I2C_INT_BUF | I2C_INT_ERR, DISABLE);     
}

/**
  * @}
  */

/**
  * @}
  */

