/**
  ******************************************************************************
  * File   : I2C/Two_Board_Communication/i2c.c 
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

/** @addtogroup AT32F403A_StdPeriph_Examples
  * @{
  */

/** @addtogroup Two_Board_Communication
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static u32 Timeout;

u8 * pRxBuffer;
u8 * pTxBuffer;

static int NumToReceive;
static int NumToTransfer;

static I2C_DIRECTION Direction;
static uint8_t SlaveAddr;

/* global state variable i2c_comm_state */
volatile I2C_STATE i2c_comm_state;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Timeout callback used by the I2C EEPROM driver.
  * @param  None
  * @retval None
  */
u8 TIMEOUT_UserCallback(void)
{
  printf("Timeout error!!!\r\n");
  /* Block communication and all processes */
  while (1)
  {   
  }
}

/**
  * @brief  Initializes peripherals used by the I2C EEPROM driver.
  * @param  None
  * @retval None
  */
void  AT32_I2C_Init(void)
{
  /** GPIO configuration and clock enable */
  GPIO_InitType  GPIO_InitStructure; 
  I2C_InitType  I2C_InitStructure;
  NVIC_InitType NVIC_InitStructure;

  I2Cx_peripheral_clock();
  I2C_DeInit(I2Cx);

  I2Cx_scl_pin_clock();
  I2Cx_sda_pin_clock(); 
 
  GPIO_InitStructure.GPIO_Pins =  I2Cx_SCL_PIN | I2Cx_SDA_PIN;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_Init(GPIOx, &GPIO_InitStructure);

  /** I2C periphral configuration */
  I2C_DeInit(I2Cx);
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2CDevice; 
  I2C_InitStructure.I2C_FmDutyCycle = I2C_DUTYCYCLE;  
  I2C_InitStructure.I2C_OwnAddr1 = I2C_ADDRESS;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable; 
  I2C_InitStructure.I2C_AddrMode = I2C_AddrMode_7bit;
  I2C_InitStructure.I2C_BitRate = I2C_SPEED; 
  I2C_Init(I2Cx, &I2C_InitStructure);

  /** I2C NVIC configuration */  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  NVIC_InitStructure.NVIC_IRQChannel = I2C1_EV_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  NVIC_InitStructure.NVIC_IRQChannel = I2C1_ER_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  wait operation is completed.
  * @param  None
  * @retval None
  */
void I2C_WaitOperationIsCompleted(void)
{
  Timeout = WAIT_TIMEOUT;
  while(i2c_comm_state!=COMM_DONE)
  {
    if((Timeout--) == 0)  TIMEOUT_UserCallback();
  }
}

/**
  * @brief  i2c master transmit func.
  * @param  Addr  : slave addr
  *         tx_buf: transmit data buffer
  *         size  : transmit data size
  * @retval proc status
  */
PROC_StatusType I2C_Master_Transmit_INT(uint8_t Addr, uint8_t *tx_buf, uint32_t size)
{
  Timeout = WAIT_TIMEOUT;
  while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSYF))
  {
    if((Timeout--) == 0)  TIMEOUT_UserCallback();
  }
  pTxBuffer = tx_buf;
  NumToTransfer = size;
  Direction = Transmitter;
  SlaveAddr = Addr;

  i2c_comm_state = COMM_PRE;
  I2C_INTConfig(I2C1, I2C_INT_EVT | I2C_INT_BUF | I2C_INT_ERR, ENABLE);
  I2C_GenerateSTART(I2C1, ENABLE);

  I2C_WaitOperationIsCompleted();
  
  return PROC_OK;
}

/**
  * @brief  i2c master receive func.
  * @param  Addr  : slave addr
  *         rx_buf: receive data buffer
  *         size  : receive data size
  * @retval proc status
  */
PROC_StatusType I2C_Master_Receive_INT(uint8_t Addr, uint8_t *rx_buf, uint32_t size)
{
  Timeout = WAIT_TIMEOUT;
  while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSYF))
  {
    if((Timeout--) == 0)  TIMEOUT_UserCallback();
  }
  pRxBuffer = rx_buf;
  NumToReceive = size;
  Direction = Receiver;
  SlaveAddr = Addr;

  i2c_comm_state = COMM_PRE;

  I2C_AcknowledgeConfig(I2C1, ENABLE);
  I2C_INTConfig(I2C1, I2C_INT_EVT | I2C_INT_BUF | I2C_INT_ERR, ENABLE);
  I2C_GenerateSTART(I2C1, ENABLE);

  I2C_WaitOperationIsCompleted();
  
  return PROC_OK;
}

/**
  * @brief  i2c slave receive func.
  * @param  rx_buf: receive data buffer
  *         size  : receive data size
  * @retval proc status
  */
PROC_StatusType I2C_Slave_Receive_INT(uint8_t *rx_buf, uint32_t size)
{
  Timeout = WAIT_TIMEOUT;
  while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSYF))
  {
    if((Timeout--) == 0)  TIMEOUT_UserCallback();
  }

  pRxBuffer = rx_buf;
  NumToReceive = size;

  i2c_comm_state = COMM_PRE;
  I2C_AcknowledgeConfig(I2C1, ENABLE);
  I2C_INTConfig(I2C1, I2C_INT_EVT | I2C_INT_BUF | I2C_INT_ERR, ENABLE);

  I2C_WaitOperationIsCompleted();

  return PROC_OK;
}

/**
  * @brief  i2c slave transmit func.
  * @param  tx_buf: transmit data buffer
  *         size  : transmit data size
  * @retval proc status
  */
PROC_StatusType I2C_Slave_Transmit_INT(uint8_t *tx_buf, uint32_t size)
{
  Timeout = WAIT_TIMEOUT;
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSYF))
	{
	  if((Timeout--) == 0)  TIMEOUT_UserCallback();
	}

  pTxBuffer = tx_buf;
  NumToTransfer = size;

	/** initialize static parameter */
	i2c_comm_state = COMM_PRE;

	I2C_AcknowledgeConfig(I2C1, ENABLE);
	I2C_INTConfig(I2C1, I2C_INT_EVT | I2C_INT_BUF | I2C_INT_ERR, ENABLE);

  I2C_WaitOperationIsCompleted();

  return PROC_OK;
}

/**
  * @brief  I2c1 event interrupt Service Routines.
  * @param  None
  * @retval None
  */
void I2C1_Evt_handle(void)
{  
  uint32_t lastevent=  I2C_GetLastEvent(I2C1);
  switch (lastevent)
  {
    case I2C_EVENT_MASTER_START_GENERATED:    /// EV5
      I2C_Send7bitAddress(I2C1, SlaveAddr, Direction);
    break;

    /* Master Receiver */
    case I2C_EVENT_MASTER_ADDRESS_WITH_RECEIVER:  /// EV6
    if(NumToReceive == 1)
    {
      I2C_AcknowledgeConfig(I2C1, DISABLE);
      I2C_GenerateSTOP(I2C1, ENABLE);
    }
    break;
    case I2C_EVENT_MASTER_DATA_RECEIVED:    /// EV7
      *pRxBuffer = I2C_ReceiveData(I2C1);
      pRxBuffer++;
      NumToReceive--;
      if(NumToReceive == 0) 
      {
        I2C_INTConfig(I2C1, I2C_INT_EVT | I2C_INT_BUF | I2C_INT_ERR, DISABLE);
        i2c_comm_state = COMM_DONE;
      }
      if(NumToReceive == 1) 
      {
        I2C_AcknowledgeConfig(I2C1, DISABLE); 
        I2C_GenerateSTOP(I2C1, ENABLE);
      }
    break;

    /* Master Transmitter */
    case I2C_EVENT_MASTER_ADDRESS|I2C_EVENT_MASTER_TRANSMITTER:  /// EV8
      I2C_SendData(I2C1, *pTxBuffer);
      pTxBuffer++;
      NumToTransfer--;
    break;
    case I2C_EVENT_MASTER_DATA_TRANSMITTING:   ///  EV8
    case I2C_EVENT_MASTER_DATA_TRANSMITTED:    /// EV8-2 
      if(NumToTransfer==0)
      {
        I2C_INTConfig(I2C1, I2C_INT_EVT | I2C_INT_BUF | I2C_INT_ERR, DISABLE);
        I2C_GenerateSTOP(I2C1, ENABLE);
        i2c_comm_state = COMM_DONE;
        Timeout = WAIT_TIMEOUT;
        while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSYF))
        {
          if((Timeout--) == 0) TIMEOUT_UserCallback();     
        }
      }else
      {
        I2C_SendData(I2C1, *pTxBuffer);
        pTxBuffer++;
        NumToTransfer--;
      }
    break;

    /* Slave Receiver */
    case I2C_EVENT_SLAVE_ADDRESS_RECEIVER_MATCHED:
      if(NumToReceive == 1)
      {
        I2C_AcknowledgeConfig(I2C1, DISABLE);
      }
    break;
    case I2C_EVENT_SLAVE_DATA_RECEIVED:
      *pRxBuffer = I2C_ReceiveData(I2C1);
      pRxBuffer++;
      NumToReceive--;
      if(NumToReceive == 1)
      {
        I2C_AcknowledgeConfig(I2C1, DISABLE);
      }
    break;
    case I2C_EVENT_SLAVE_STOP_DETECTED:
      I2C_INTConfig(I2C1, I2C_INT_EVT | I2C_INT_BUF | I2C_INT_ERR, DISABLE);
      i2c_comm_state = COMM_DONE;
    break;

    /* Slave Transmitter */
    case I2C_EVENT_SLAVE_ADDRESS_TRANSMITTER_MATCHED:
    break;
    case I2C_EVENT_SLAVE_DATA_TRANSMITTED:
    case I2C_EVENT_SLAVE_DATA_TRANSMITTING:
      if(NumToTransfer > 0)
      {
        I2C_SendData(I2C1, *pTxBuffer);
        pTxBuffer++;
        NumToTransfer--;
      }
      else if(NumToTransfer == 0)
      {
        I2C_INTConfig(I2C1, I2C_INT_EVT | I2C_INT_BUF | I2C_INT_ERR, DISABLE);
        i2c_comm_state = COMM_DONE;
      }
    break;
  }
}

/**
  * @brief  I2c1 error interrupt Service Routines.
  * @param  None
  * @retval None
  */
void I2C1_Err_handle(void)
{
  if (I2C_GetFlagStatus(I2C1, I2C_FLAG_ACKFAIL))
  {
    if (I2C1->STS2 &0x01) /// real fail
    {	
      I2C_GenerateSTOP(I2C1, ENABLE);
      i2c_comm_state = COMM_EXIT;
    }
    I2C_ClearFlag(I2C1, I2C_FLAG_ACKFAIL);
  }

  if (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSERR))
  {
    if (I2C1->STS2 &0x01)
    {
      I2C_GenerateSTOP(I2C1, ENABLE);
      i2c_comm_state = COMM_EXIT;
    }
    I2C_ClearFlag(I2C1, I2C_FLAG_BUSERR);
  }
}

/**
  * @}
  */ 

/**
  * @}
  */ 

