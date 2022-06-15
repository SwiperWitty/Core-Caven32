/**
  ******************************************************************************
  * File   : I2C/I2Cx_DMA_INT/i2c_eeprom.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : The header file of eeprom driver.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __I2C_EEPROM_H
#define __I2C_EEPROM_H

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "at32f4xx.h"

/* Exported types ------------------------------------------------------------*/
typedef enum 
{
  PROC_OK       = 0x00,
  PROC_ERROR    = 0x01,
  PROC_BUSY     = 0x02,
  PROC_TIMEOUT  = 0x03
} PROC_StatusType;

typedef enum i2c_state
{
  COMM_DONE  = 0,    ///done successfully
  COMM_PRE = 1,
  COMM_IN_PROCESS = 2,
  COMM_EXIT = 3     ///exit since failure
}I2C_STATE;

typedef enum i2c_direction
{
  Transmitter=0x00,
  Receiver = 0x01
}I2C_DIRECTION;

/* Exported constants --------------------------------------------------------*/
#define BufferSize             8
/* Exported macro ------------------------------------------------------------*/
#define IIC_MODE   0

//#define I2Cy    I2C1

#define I2Cx_SCL_PIN    GPIO_Pins_6 
#define I2Cx_SDA_PIN    GPIO_Pins_7 
#define GPIOx           GPIOB 

#define I2Cx_peripheral_clock()  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_I2C1, ENABLE)
#define I2Cx_scl_pin_clock()     RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB, ENABLE)
#define I2Cx_sda_pin_clock()     RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB, ENABLE)

#define WAIT_TIMEOUT      0xffff

#define I2C_SPEED         400000
#define I2C_ADDRESS       0x30
#define I2C_DUTYCYCLE     I2C_FmDutyCycle_2_1

/** Maximum number of trials for sEE_WaitEepromStandbyState() function */
#define FALSE 0
#define TRUE  1

typedef enum
{
  ERROR_BUSY = 0,
  ERROR_START,
  ERROR_ADDR,
  ERROR_TDE,
}ERROR_STATE;

/* Exported functions ------------------------------------------------------- */
void  AT32_I2C_Init(I2C_Type *I2Cy);
void TIMEOUT_UserCallback(I2C_Type *I2Cy, uint8_t I2C_Error);
PROC_StatusType I2C_Transmit_DMA(I2C_Type *I2Cy, uint8_t Addr, uint8_t *tx_buf, uint32_t size);
PROC_StatusType I2C_Receive_DMA(I2C_Type *I2Cy, uint8_t Addr, uint8_t *rx_buf, uint32_t size);
PROC_StatusType I2C_Transmit_1Byte_Receive_nByte_DMA(I2C_Type *I2Cy, uint8_t Addr, uint8_t Reg_add, uint8_t *rx_buf, uint32_t size);

#endif /* __I2C_EEPROM_H */

