/**
  ******************************************************************************
  * File   : I2C/DMA_MA_TX&SLA_RX/at32f4xx_i2c_ex.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : The header file of I2C driver.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AT32F4XX_I2C_EX_H
#define __AT32F4XX_I2C_EX_H

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"

/* Exported types ------------------------------------------------------------*/

/** @defgroup I2C_Exported_Types I2C Exported Types
  * @{
  */
  
/** 
  * @brief  I2C Status structures definition  
  */  
typedef enum 
{
  I2C_OK             = 0,
  
  I2C_ERROR_STEP_1   = 1,
  I2C_ERROR_STEP_2   = 2,
  I2C_ERROR_STEP_3   = 3,
  I2C_ERROR_STEP_4   = 4,
  I2C_ERROR_STEP_5   = 5,
  I2C_ERROR_STEP_6   = 6,
  I2C_ERROR_STEP_7   = 7,
  I2C_ERROR_STEP_8   = 8,  
  I2C_ERROR_STEP_9   = 9,
  I2C_ERROR_STEP_10  = 10,
  I2C_ERROR_STEP_11  = 11,
  I2C_ERROR_STEP_12  = 12,
  
  I2C_BUSY           = 100,
  I2C_TIMEOUT        = 101,
  I2C_ERROR          = 102,   

  I2C_START          = 110,  
  I2C_END            = 111,   
} I2C_StatusType;

/** 
  * @}
  */  

/* Exported constants --------------------------------------------------------*/

/** @defgroup I2C_Exported_Constants I2C Exported Constants
  * @{
  */

/** 
  * @brief  I2C Parameter definition  
  */  
#define I2C_SPEED                    100000
#define I2C_SLAVE_ADDRESS7           0xB0
#define I2C_EEAddress                0xB0

/**
  * @brief  I2C Interface pins
  */
#define I2C_PORT                     I2C1
#define I2C_CLK                      RCC_APB1PERIPH_I2C1
#define I2C_SCL_PIN                  GPIO_Pins_6                  /* PB.06 */
#define I2C_SCL_GPIO_PORT            GPIOB                        /* GPIOB */
#define I2C_SCL_GPIO_CLK             RCC_AHBPERIPH_GPIOB
#define I2C_SCL_GPIO_PinsSource      GPIO_PinsSource6             
#define I2C_SCL_GPIO_GPIO_AF         GPIO_AF_1     

#define I2C_SDA_PIN                  GPIO_Pins_7                  /* PB.07 */
#define I2C_SDA_GPIO_PORT            GPIOB                        /* GPIOB */
#define I2C_SDA_GPIO_CLK             RCC_AHBPERIPH_GPIOB
#define I2C_SDA_GPIO_PinsSource      GPIO_PinsSource7             
#define I2C_SDA_GPIO_GPIO_AF         GPIO_AF_1    

#define I2Cx_DMA                      DMA1   
#define I2Cx_DMA_CHANNEL_TX           DMA1_Channel2
#define I2Cx_DMA_CHANNEL_RX           DMA1_Channel3 
#define I2Cx_DMA_FLAG_TX_TC           DMA1_IT_TC2   
#define I2Cx_DMA_FLAG_TX_GL           DMA1_IT_GL2 
#define I2Cx_DMA_FLAG_RX_TC           DMA1_IT_TC3 
#define I2Cx_DMA_FLAG_RX_GL           DMA1_IT_GL3    
#define I2Cx_DMA_CLK                  RCC_AHBPERIPH_DMA1
      
#define I2Cx_DMA_TX_IRQn              DMA1_Channel3_2_IRQn
#define I2Cx_DMA_RX_IRQn              DMA1_Channel3_2_IRQn
#define I2Cx_DMA_IRQHandler           DMA1_Channel3_2_IRQHandler


#define I2Cx_DMA_TX_PREPRIO           0
#define I2Cx_DMA_TX_SUBPRIO           0  
#define I2Cx_DMA_TX_FLAG_TC           DMA1_FLAG_TC2  
#define I2Cx_DMA_TX_FLAG_HT           DMA1_FLAG_HT2  
#define I2Cx_DMA_TX_FLAG_ERR          DMA1_FLAG_ERR2

#define I2Cx_DMA_RX_PREPRIO           0
#define I2Cx_DMA_RX_SUBPRIO           0  
#define I2Cx_DMA_RX_FLAG_TC           DMA1_FLAG_TC3  
#define I2Cx_DMA_RX_FLAG_HT           DMA1_FLAG_HT3  
#define I2Cx_DMA_RX_FLAG_ERR          DMA1_FLAG_ERR3 
/**      
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
#define MASTER_BOARD
/** @defgroup I2C_Exported_Macros I2C Exported Macros
  * @{
  */
/** 
  * @}
  */
/* Exported variables ------------------------------------------------------- */    
/* Exported functions ------------------------------------------------------- */

/** @addtogroup I2C_Exported_Functions
  * @{  
  */
  
  
void I2Cx_Init(I2C_Type* I2Cx);

I2C_StatusType I2C_Slave_Receive_DMA(I2C_Type* I2Cx, uint8_t *pData, uint16_t Size, uint32_t Timeout);
I2C_StatusType I2C_Master_Transmit_DMA(I2C_Type* I2Cx, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);

void I2C_DMA_TX_IRQHandler(I2C_Type* I2Cx);
void I2C_DMA_RX_IRQHandler(I2C_Type* I2Cx);
/** 
  * @}
  */  

#endif /* __I2C_H */

