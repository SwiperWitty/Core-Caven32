/**
  ******************************************************************************
  * File   : I2C/I2Cx_DMA_INT/i2c.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : The driver of Two Board Communication with i2c.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
#include "i2c.h"
#include "string.h"
#include "stdbool.h"
#include "at32_board.h"

/** @addtogroup AT32F415_StdPeriph_Examples
  * @{
  */

/** @addtogroup I2Cx_DMA_INT
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static u32 Timeout;

u8 * pRxBuffer;
u8 * pTxBuffer;

uint8_t I2C1_Buffer_Tx[BufferSize];
uint8_t I2C1_Buffer_Rx[BufferSize];

DMA_Channel_Type *I2C_TX_DMA1_Channelx;
DMA_Channel_Type *I2C_RX_DMA1_Channelx;
uint8_t I2C_TX_DMA1_Channelx_IRQn;
uint8_t I2C_RX_DMA1_Channelx_IRQn;
uint32_t I2Cx_DR_Address = 0x40005410;

uint8_t recv_1byte_flag = 0;
/* global state variable i2c_comm_state */
volatile I2C_STATE i2c_comm_state;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

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

/**
  * @brief  Timeout callback used by the I2C EEPROM driver.
  * @param  None
  * @retval None
  */
void TIMEOUT_UserCallback(I2C_Type *I2Cy, uint8_t I2C_Error)
{
  switch(I2C_Error)
  {
    case ERROR_BUSY:
      AT32_I2C_Init(I2Cy);
    break;
    case ERROR_START:
    break;
    case ERROR_ADDR:
      I2C_ClearFlag(I2Cy, I2C_FLAG_ACKFAIL);
      BIT_SET(I2Cy->CTRL1, I2C_CTRL1_STOPGEN);
    break;
    case ERROR_TDE:
    break;
    default:
    break;
  }
}

/**
  * @brief  Initializes peripherals used by the I2C EEPROM driver.
  * @param  None
  * @retval None
  */
void  AT32_I2C_Init(I2C_Type *I2Cy)
{
  /** GPIO configuration and clock enable */
  GPIO_InitType  GPIO_InitStructure;
  I2C_InitType  I2C_InitStructure;
  NVIC_InitType NVIC_InitStructure;
  DMA_InitType  DMA_InitStructure = {0};

  if(I2Cy == I2C1)
  {
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_I2C1, ENABLE);

    GPIO_InitStructure.GPIO_Pins =  GPIO_Pins_6 | GPIO_Pins_7;    //SCL | SDA
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    I2C_TX_DMA1_Channelx = DMA1_Channel6;
    I2C_RX_DMA1_Channelx = DMA1_Channel7;
    I2C_TX_DMA1_Channelx_IRQn = DMA1_Channel6_IRQn;
    I2C_RX_DMA1_Channelx_IRQn = DMA1_Channel7_IRQn;
    I2Cx_DR_Address = 0x40005410;
  }
  else
  {
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_I2C2, ENABLE);

    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_10 | GPIO_Pins_11;    //SCL | SDA
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    I2C_TX_DMA1_Channelx = DMA1_Channel4;
    I2C_RX_DMA1_Channelx = DMA1_Channel5;
    I2C_TX_DMA1_Channelx_IRQn = DMA1_Channel4_IRQn;
    I2C_RX_DMA1_Channelx_IRQn = DMA1_Channel5_IRQn;
    I2Cx_DR_Address = 0x40005810;
  }

  /** I2C periphral configuration */
  I2C_DeInit(I2Cy);
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2CDevice;
  I2C_InitStructure.I2C_FmDutyCycle = I2C_DUTYCYCLE;
  I2C_InitStructure.I2C_OwnAddr1 = 0xAA;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AddrMode = I2C_AddrMode_7bit;
  I2C_InitStructure.I2C_BitRate = I2C_SPEED;
  I2C_Init(I2Cy, &I2C_InitStructure);
  
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_DMA1, ENABLE);
  /* DMA1 channel5 configuration ----------------------------------------------*/
  DMA_Reset(I2C_RX_DMA1_Channelx);
  DMA_DefaultInitParaConfig(&DMA_InitStructure);
  DMA_InitStructure.DMA_PeripheralBaseAddr  = I2Cx_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr      = (uint32_t)I2C1_Buffer_Rx;
  DMA_InitStructure.DMA_Direction           = DMA_DIR_PERIPHERALSRC;
  DMA_InitStructure.DMA_BufferSize          = BufferSize;
  DMA_InitStructure.DMA_PeripheralInc       = DMA_PERIPHERALINC_DISABLE;
  DMA_InitStructure.DMA_MemoryInc           = DMA_MEMORYINC_ENABLE;
  DMA_InitStructure.DMA_PeripheralDataWidth = DMA_MEMORYDATAWIDTH_BYTE;
  DMA_InitStructure.DMA_MemoryDataWidth     = DMA_MEMORYDATAWIDTH_BYTE;
  DMA_InitStructure.DMA_Mode                = DMA_MODE_NORMAL;
  DMA_InitStructure.DMA_Priority            = DMA_PRIORITY_VERYHIGH;
  DMA_InitStructure.DMA_MTOM                = DMA_MEMTOMEM_DISABLE;
  DMA_Init(I2C_RX_DMA1_Channelx, &DMA_InitStructure);

  /* DMA1 channel6 configuration ----------------------------------------------*/
  DMA_Reset(I2C_TX_DMA1_Channelx);
  DMA_InitStructure.DMA_PeripheralBaseAddr = I2Cx_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)I2C1_Buffer_Tx;
  DMA_InitStructure.DMA_Direction          = DMA_DIR_PERIPHERALDST;
  DMA_InitStructure.DMA_Priority           = DMA_PRIORITY_HIGH;
  DMA_Init(I2C_TX_DMA1_Channelx, &DMA_InitStructure);

  I2C_DMACmd(I2Cy, DISABLE);

  /** I2C NVIC configuration */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  NVIC_InitStructure.NVIC_IRQChannel = I2C_TX_DMA1_Channelx_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = I2C_RX_DMA1_Channelx_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  i2c master transmit func.
  * @param  Addr  : slave addr
  *         tx_buf: transmit data buffer
  *         size  : transmit data size
  * @retval proc status
  */
PROC_StatusType I2C_Transmit_DMA(I2C_Type *I2Cy, uint8_t Addr, uint8_t *tx_buf, uint32_t size)
{
  Timeout = WAIT_TIMEOUT;

  while(I2C_GetFlagStatus(I2Cy, I2C_FLAG_BUSYF))
  {
    if((Timeout--) == 0)
    {
      TIMEOUT_UserCallback(I2Cy, ERROR_BUSY);
      return PROC_TIMEOUT;
    }
  }

  /* Disable Pos */
  BIT_CLEAR(I2Cy->CTRL1, I2C_CTRL1_POSEN);

  if(I2Cy == I2C1)
  {
    I2C_TX_DMA1_Channelx = DMA1_Channel6;
  }
  else
  {
    I2C_TX_DMA1_Channelx = DMA1_Channel4;
  }
  /* Disable the peripheral */
  BIT_CLEAR(I2C_TX_DMA1_Channelx->CHCTRL, DMA_CHCTRL6_CHEN);

  I2C_TX_DMA1_Channelx->TCNT = size;
  I2C_TX_DMA1_Channelx->CMBA = (uint32_t)tx_buf;

  /* Enable the transfer complete interrupt */
  DMA_INTConfig(I2C_TX_DMA1_Channelx, DMA_INT_TC, ENABLE);

  /* Enable the Peripheral */
  BIT_SET(I2C_TX_DMA1_Channelx->CHCTRL, DMA_CHCTRL6_CHEN);

  /* Generate Start */
  I2C_GenerateSTART(I2Cy, ENABLE);
  /* Wait until SB flag is set */
  Timeout = WAIT_TIMEOUT;

  while(I2C_CheckEvent(I2Cy, I2C_EVENT_MASTER_START_GENERATED) != SUCCESS)
  {
    if((Timeout--) == 0)
    {
      TIMEOUT_UserCallback(I2Cy, ERROR_START);
      return PROC_TIMEOUT;
    }
  }

  /* Send Slave Address */
  I2C_Send7bitAddress(I2Cy, Addr, I2C_Direction_Transmit);
  /* Wait until ADDR flag is set */
  Timeout = WAIT_TIMEOUT;

  while(I2C_CheckEvent(I2Cy, I2C_EVENT_MASTER_ADDRESS) != SUCCESS)
  {
    if((Timeout--) == 0)
    {
      TIMEOUT_UserCallback(I2Cy, ERROR_ADDR);
      return PROC_TIMEOUT;
    }
  }

  /* Enable DMA Request */
  BIT_SET(I2Cy->CTRL2, I2C_CTRL2_DMAEN);

  /* Clear ADDR flag */
  I2C_ClearADDRFlag(I2Cy);

  return PROC_OK;
}

/**
  * @brief  i2c master receive func.
  * @param  Addr  : slave addr
  *         rx_buf: receive data buffer
  *         size  : receive data size
  * @retval proc status
  */
PROC_StatusType I2C_Receive_DMA(I2C_Type *I2Cy, uint8_t Addr, uint8_t *rx_buf, uint32_t size)
{
  Timeout = WAIT_TIMEOUT;

  while(I2C_GetFlagStatus(I2Cy, I2C_FLAG_BUSYF))
  {
    if((Timeout--) == 0)
    {
      TIMEOUT_UserCallback(I2Cy, ERROR_BUSY);
      return PROC_TIMEOUT;
    }
  }

  if(I2Cy == I2C1)
  {
    I2C_RX_DMA1_Channelx = DMA1_Channel7;
  }
  else
  {
    I2C_RX_DMA1_Channelx = DMA1_Channel5;
  }
  
  /* Disable Pos */
  BIT_CLEAR(I2Cy->CTRL1, I2C_CTRL1_POSEN);

  /* Disable the peripheral */
  BIT_CLEAR(I2C_RX_DMA1_Channelx->CHCTRL, DMA_CHCTRL7_CHEN);

  I2C_RX_DMA1_Channelx->TCNT = size;
  I2C_RX_DMA1_Channelx->CMBA = (uint32_t)rx_buf;

  /* Enable the transfer complete interrupt */
  DMA_INTConfig(I2C_RX_DMA1_Channelx, DMA_INT_TC, ENABLE);

  /* Enable the Peripheral */
  BIT_SET(I2C_RX_DMA1_Channelx->CHCTRL, DMA_CHCTRL7_CHEN);

  /* Enable Acknowledge */
  BIT_SET(I2Cy->CTRL1, I2C_CTRL1_ACKEN);

  /* Generate Start */
  I2C_GenerateSTART(I2Cy, ENABLE);
  /* Wait until SB flag is set */
  Timeout = WAIT_TIMEOUT;

  while(I2C_CheckEvent(I2Cy, I2C_EVENT_MASTER_START_GENERATED) != SUCCESS)
  {
    if((Timeout--) == 0)
    {
      TIMEOUT_UserCallback(I2Cy, ERROR_START);
      return PROC_TIMEOUT;
    }
  }

  /* Send Slave Address */
  I2C_Send7bitAddress(I2Cy, Addr, I2C_Direction_Receive);
  /* Wait until ADDR flag is set */
  Timeout = WAIT_TIMEOUT;

  while(I2C_CheckEvent(I2Cy, I2C_EVENT_MASTER_ADDRESS_WITH_RECEIVER) != SUCCESS)
  while(I2C_GetFlagStatus(I2Cy, I2C_FLAG_ADDRF) != SET)
  {
    if((Timeout--) == 0)
    {
      TIMEOUT_UserCallback(I2Cy, ERROR_ADDR);
      return PROC_TIMEOUT;
    }
  }

  if(size == 1)
  {
    /* Disable Acknowledge */
    BIT_CLEAR(I2Cy->CTRL1, I2C_CTRL1_ACKEN);
    
    /* Generate Stop */
    I2C_GenerateSTOP(I2Cy, ENABLE);
    
    recv_1byte_flag = 1;
  }
  else
  {
    /* Enable Last DMA bit */
    BIT_SET(I2Cy->CTRL2, I2C_CTRL2_DMALAST);
    
    recv_1byte_flag = 0;
  }


  /* Enable DMA Request */
  BIT_SET(I2Cy->CTRL2, I2C_CTRL2_DMAEN);

  /* Clear ADDR flag */
  I2C_ClearADDRFlag(I2Cy);

  return PROC_OK;
}

/**
  * @brief  i2c master receive func.
  * @param  Addr  : slave addr
  *         rx_buf: receive data buffer
  *         size  : receive data size
  * @retval proc status
  */
PROC_StatusType I2C_Transmit_1Byte_Receive_nByte_DMA(I2C_Type *I2Cy, uint8_t Addr, uint8_t Reg_add, uint8_t *rx_buf, uint32_t size)
{
  Timeout = WAIT_TIMEOUT;

  while(I2C_GetFlagStatus(I2Cy, I2C_FLAG_BUSYF))
  {
    if((Timeout--) == 0)
    {
      TIMEOUT_UserCallback(I2Cy, ERROR_BUSY);
      return PROC_TIMEOUT;
    }
  }

  /* Generate Start */
  I2C_GenerateSTART(I2Cy, ENABLE);
  /* Wait until SB flag is set */
  Timeout = WAIT_TIMEOUT;

  while(I2C_CheckEvent(I2Cy, I2C_EVENT_MASTER_START_GENERATED) != SUCCESS)
  {
    if((Timeout--) == 0)
    {
      TIMEOUT_UserCallback(I2Cy, ERROR_START);
      return PROC_TIMEOUT;
    }
  }

  /* Send Slave Address for write slave register*/
  I2C_Send7bitAddress(I2Cy, Addr, I2C_Direction_Transmit);
  /* Wait until ADDR flag is set */
  Timeout = WAIT_TIMEOUT;
  while(I2C_CheckEvent(I2Cy, I2C_EVENT_MASTER_ADDRESS) != SUCCESS)
  {
    if((Timeout--) == 0)
    {
      TIMEOUT_UserCallback(I2Cy, ERROR_ADDR);
      return PROC_TIMEOUT;
    }
  }
  
  I2C_SendData(I2Cy, Reg_add); 
  /* Wait until data transmitted */
  Timeout = WAIT_TIMEOUT;
  while(I2C_CheckEvent(I2Cy, I2C_EVENT_MASTER_DATA_TRANSMITTED) != SUCCESS)
  {
    if((Timeout--) == 0)
    {
      TIMEOUT_UserCallback(I2Cy, ERROR_TDE);
      return PROC_TIMEOUT;
    }
  }

  if(I2Cy == I2C1)
  {
    I2C_RX_DMA1_Channelx = DMA1_Channel7;
  }
  else
  {
    I2C_RX_DMA1_Channelx = DMA1_Channel5;
  }
  
  /* Disable Pos */
  BIT_CLEAR(I2Cy->CTRL1, I2C_CTRL1_POSEN);

  /* Disable the peripheral */
  BIT_CLEAR(I2C_RX_DMA1_Channelx->CHCTRL, DMA_CHCTRL7_CHEN);

  I2C_RX_DMA1_Channelx->TCNT = size;
  I2C_RX_DMA1_Channelx->CMBA = (uint32_t)rx_buf;

  /* Enable the transfer complete interrupt */
  DMA_INTConfig(I2C_RX_DMA1_Channelx, DMA_INT_TC, ENABLE);

  /* Enable the Peripheral */
  BIT_SET(I2C_RX_DMA1_Channelx->CHCTRL, DMA_CHCTRL7_CHEN);

  /* Enable Acknowledge */
  BIT_SET(I2Cy->CTRL1, I2C_CTRL1_ACKEN);
  
  /* Generate Re-Start */
  I2C_GenerateSTART(I2Cy, ENABLE);
  /* Wait until SB flag is set */
  Timeout = WAIT_TIMEOUT;
  while(I2C_CheckEvent(I2Cy, I2C_EVENT_MASTER_START_GENERATED) != SUCCESS)
  {
    if((Timeout--) == 0)
    {
      TIMEOUT_UserCallback(I2Cy, ERROR_START);
      return PROC_TIMEOUT;
    }
  }

  /* Send Slave Address */
  I2C_Send7bitAddress(I2Cy, Addr, I2C_Direction_Receive);
  /* Wait until ADDR flag is set */
  Timeout = WAIT_TIMEOUT;

  while(I2C_CheckEvent(I2Cy, I2C_EVENT_MASTER_ADDRESS_WITH_RECEIVER) != SUCCESS)
  while(I2C_GetFlagStatus(I2Cy, I2C_FLAG_ADDRF) != SET)
  {
    if((Timeout--) == 0)
    {
      TIMEOUT_UserCallback(I2Cy, ERROR_ADDR);
      return PROC_TIMEOUT;
    }
  }

  if(size == 1)
  {
    /* Disable Acknowledge */
    BIT_CLEAR(I2Cy->CTRL1, I2C_CTRL1_ACKEN);
    
    /* Generate Stop */
    I2C_GenerateSTOP(I2Cy, ENABLE);
    
    recv_1byte_flag = 1;
  }
  else
  {
    /* Enable Last DMA bit */
    BIT_SET(I2Cy->CTRL2, I2C_CTRL2_DMALAST);
    
    recv_1byte_flag = 0;
  }


  /* Enable DMA Request */
  BIT_SET(I2Cy->CTRL2, I2C_CTRL2_DMAEN);

  /* Clear ADDR flag */
  I2C_ClearADDRFlag(I2Cy);

  return PROC_OK;
}

/**
  * @}
  */

/**
  * @}
  */

