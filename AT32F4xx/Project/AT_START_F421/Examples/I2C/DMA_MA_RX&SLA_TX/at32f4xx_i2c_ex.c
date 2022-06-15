/**
  ******************************************************************************
  * File   : I2C/DMA_MA_RX&SLA_TX/at32f4xx_i2c_ex.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : The driver of the I2C communication example based on DMA mode.
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
#include "at32_board.h"
#include "at32f4xx_i2c_ex.h"

/** @addtogroup AT32F421_StdPeriph_Examples
  * @{
  */

/** @addtogroup DMA - Master Receiver & Slave Transmitter
  * @brief  The communication is performed in the DMA mode.The status of 
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
uint16_t TxCount;
uint16_t RxCount;
uint8_t *TxBuffer;
uint8_t *RxBuffer;

static uint16_t I2C_Status;
DMA_InitType  DMA_InitStructure;
/* Private function prototypes -----------------------------------------------*/
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
  while(I2C_Status != I2C_END)
  {
    /* 1 ms delay */
    I2C_Delay(1000);
    
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
  * @param  I2Cx: where x can be 1, 2 or 3 to select the I2C peripheral.
  * @retval None.
  */
void LowLevel_Init(I2C_Type* I2Cx)
{
  GPIO_InitType GPIO_InitStructure;
  NVIC_InitType NVIC_InitStructure;  
  
  /* NVIC group configuration */   
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
  
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

  /* Configure and enable I2C DMA TX Channel interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = I2Cx_DMA_TX_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = I2Cx_DMA_TX_PREPRIO;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = I2Cx_DMA_TX_SUBPRIO;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Configure and enable I2C DMA RX Channel interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = I2Cx_DMA_RX_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = I2Cx_DMA_RX_PREPRIO;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = I2Cx_DMA_RX_SUBPRIO;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  
  
  /* I2C DMA TX and RX channels configuration */
  /* Enable the DMA clock */
  RCC_AHBPeriphClockCmd(I2Cx_DMA_CLK, ENABLE);

  /* I2C TX DMA Channel configuration */
  DMA_Reset(I2Cx_DMA_CHANNEL_TX);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&I2Cx->DT;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)0;   /* This parameter will be configured durig communication */
  DMA_InitStructure.DMA_Direction = DMA_DIR_PERIPHERALDST;    /* This parameter will be configured durig communication */
  DMA_InitStructure.DMA_BufferSize = 0xFFFF;            /* This parameter will be configured durig communication */
  DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;
  DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_ENABLE;
  DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_BYTE;
  DMA_InitStructure.DMA_MemoryDataWidth = DMA_MEMORYDATAWIDTH_BYTE;
  DMA_InitStructure.DMA_Mode = DMA_MODE_NORMAL;
  DMA_InitStructure.DMA_Priority = DMA_PRIORITY_LOW;
  DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;
  DMA_Init(I2Cx_DMA_CHANNEL_TX, &DMA_InitStructure);  
  
  /* I2C RX DMA Channel configuration */
  DMA_Reset(I2Cx_DMA_CHANNEL_RX);
  DMA_Init(I2Cx_DMA_CHANNEL_RX, &DMA_InitStructure);  
  
  /* Enable the DMA Channels Interrupts */
  DMA_INTConfig(I2Cx_DMA_CHANNEL_TX, DMA_INT_TC, ENABLE);
  DMA_INTConfig(I2Cx_DMA_CHANNEL_RX, DMA_INT_TC, ENABLE);    
}

/**
  * @brief  Initializes peripherals used by the I2C EEPROM driver.
  * @param  I2Cx: where x can be 1, 2 or 3 to select the I2C peripheral.
  * @retval None.
  */
void I2Cx_Init(I2C_Type* I2Cx)
{ 
  I2C_InitType  I2C_InitStructure;
  
  LowLevel_Init(I2Cx);
  
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

  /* Disable the I2C peripheral DMA requests */
  I2C_DMACmd(I2Cx, DISABLE);
   
}

/**
  * @brief  Initializes DMA channel used by the I2C.
  * @param  DMAy_Channelx DMA transfer direction select.
  * @param  pData Pointer to data buffer.
  * @param  Size Amount of data to be sent.
  * @retval None
  */
void I2C_DMAConfig(DMA_Channel_Type* DMAy_Channelx, uint8_t *pData, uint16_t Size)
{ 
  /* Initialize the DMA with the new parameters */
  
  /* Disable the DMA Tx Channel */
  DMA_ChannelEnable(DMAy_Channelx, DISABLE);
  
  /* Disable the transfer complete interrupt */
  DMA_INTConfig(DMAy_Channelx, DMA_INT_TC , DISABLE);  
  
  /* Configure the DMA Tx Channel with the buffer address and the buffer size */
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)pData;
  DMA_InitStructure.DMA_Direction = (DMAy_Channelx == I2Cx_DMA_CHANNEL_TX) ? DMA_DIR_PERIPHERALDST : DMA_DIR_PERIPHERALSRC;    
  DMA_InitStructure.DMA_BufferSize = (uint32_t)Size;  
  DMA_Init(DMAy_Channelx, &DMA_InitStructure);  

  /* Enable the transfer complete interrupt */
  DMA_INTConfig(DMAy_Channelx, DMA_INT_TC , ENABLE);
  
  /* Enable the DMA Tx Channel */
  DMA_ChannelEnable(DMAy_Channelx, ENABLE);
}

/**
  * @brief  Receive in master mode an amount of data in non-blocking mode with DMA
  * @param  I2Cx: where x can be 1, 2 or 3 to select the I2C peripheral.
  * @param  DevAddress Target device address.
  * @param  pData Pointer to data buffer.
  * @param  Size Amount of data to be sent.
  * @param  Timeout Timeout duration.
  * @retval I2C status.
  */
I2C_StatusType I2C_Master_Receive_DMA(I2C_Type* I2Cx, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  /* Update the I2C Status */  
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
  
  /* Disable DMA Request */
  I2C_DMACmd(I2Cx, DISABLE);
  
  /* Configure the DMA Rx Channel with the buffer address and the buffer size */
  I2C_DMAConfig(I2Cx_DMA_CHANNEL_RX, pData, Size);

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
  I2C_Send7bitAddress(I2Cx, DevAddress, I2C_Direction_Receive);
  
  /* Wait until ADDR flag is set */
  if(I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_ADDRF, RESET, I2C_EVT_CHECK_ACKFAIL, Timeout) != I2C_OK)
  {
    /* Send STOP Condition */
    I2C_GenerateSTOP(I2Cx, ENABLE);
    
    return I2C_ERROR_STEP_3;
  }

  if(Size == 1)
  {
    /* Disable Acknowledge */
    I2C_AcknowledgeConfig(I2Cx, DISABLE);
    
    /* Send STOP Condition */
    I2C_GenerateSTOP(I2Cx, ENABLE);    
  }
  else
  {
    /* Enable Last DMA bit */
    I2C_DMALastTransferCmd(I2Cx, ENABLE);
  }

  /* Enable DMA Request */
  I2C_DMACmd(I2Cx, ENABLE);

  /* Clear ADDR flag */
  I2C_ClearADDRFlag(I2Cx);

  /* Wait for the communication to end */  
  if(I2C_WaitEnd(Timeout) != I2C_OK)
  {
    /* Send STOP Condition */
    I2C_GenerateSTOP(I2Cx, ENABLE);
    
    return I2C_ERROR_STEP_4;  
  }
  
  return I2C_OK;
}

/**
  * @brief  Transmit in slave mode an amount of data in non-blocking mode with DMA
  * @param  I2Cx: where x can be 1, 2 or 3 to select the I2C peripheral.
  * @param  pData Pointer to data buffer
  * @param  Size Amount of data to be sent
  * @retval I2C status
  */
I2C_StatusType I2C_Slave_Transmit_DMA(I2C_Type* I2Cx, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  /* Update the I2C Status */  
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
  
  /* Disable DMA Request */
  I2C_DMACmd(I2Cx, DISABLE);
  
  /* Configure the DMA Rx Channel with the buffer address and the buffer size */
  I2C_DMAConfig(I2Cx_DMA_CHANNEL_TX, pData, Size);

  /* Wait until ADDR flag is set */
  if(I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_ADDRF, RESET, I2C_EVT_CHECK_NONE, Timeout) != I2C_OK)
  {
    /* Disable Acknowledge */
    I2C_AcknowledgeConfig(I2Cx, DISABLE);
    
    return I2C_ERROR_STEP_2;
  }
  
  /* Enable DMA Request */
  I2C_DMACmd(I2Cx, ENABLE);  
  
  /* Clear ADDR flag */
  I2C_ClearADDRFlag(I2Cx);

  /* Wait for the communication to end */  
  if(I2C_WaitEnd(Timeout) != I2C_OK)
  {
    /* Release the bus */
    I2C_GenerateSTOP(I2Cx, ENABLE);    
    
    return I2C_ERROR_STEP_3;  
  }
  
  return I2C_OK;
}

/**
  * @brief  Handles DMA TX interrupt request.
  * @param  None
  * @retval None
  */
void I2C_DMA_TX_IRQHandler(I2C_Type* I2Cx)
{
  /* Transfer Complete Interrupt management */
  if(DMA_GetFlagStatus(I2Cx_DMA_TX_FLAG_TC) != RESET)
  {
    /* Disable DMA Request */
    I2C_DMACmd(I2Cx, DISABLE);
    
    /* Disable the transfer complete interrupt */
    DMA_INTConfig(I2Cx_DMA_CHANNEL_TX, DMA_INT_TC, DISABLE);
  
    /* Clear the transfer complete flag */
    DMA_ClearFlag(I2Cx_DMA_TX_FLAG_TC);
    
    /* Wait until STOP flag is set */
    I2C_WaitOnFlagUntilTimeout(I2Cx, I2C_FLAG_ACKFAIL, RESET, I2C_EVT_CHECK_NONE, 100);  

    /* Clear ACKFAIL flag */
    I2C_ClearFlag(I2Cx, I2C_FLAG_ACKFAIL);
  
    /* End */
    I2C_Status = I2C_END;    
  }
}

/**
  * @brief  Handles DMA RX interrupt request.
  * @param  None
  * @retval None
  */
void I2C_DMA_RX_IRQHandler(I2C_Type* I2Cx)
{
  /* Transfer Complete Interrupt management */
  if(DMA_GetFlagStatus(I2Cx_DMA_RX_FLAG_TC) != RESET)
  {
    /* Disable DMA Request */
    I2C_DMACmd(I2Cx, DISABLE);    
    
    /* Disable the transfer complete interrupt */
    DMA_INTConfig(I2Cx_DMA_CHANNEL_RX, DMA_INT_TC, DISABLE);
  
    /* Clear the transfer complete flag */
    DMA_ClearFlag(I2Cx_DMA_RX_FLAG_TC);
    
    /* Send STOP Condition */
    I2C_GenerateSTOP(I2Cx, ENABLE);

    /* End */
    I2C_Status = I2C_END;
  }
}

/**
  * @}
  */

/**
  * @}
  */

