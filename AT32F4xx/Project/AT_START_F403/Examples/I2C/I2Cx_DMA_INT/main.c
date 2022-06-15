/**
  ******************************************************************************
  * File   : I2C/I2Cx_DMA_INT/main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
#include "i2c.h"
#include "at32_board.h"

/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup I2Cx_DMA_INT
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define I2Cx            I2C1    //choose I2C1 or I2C2
#define TX_SIZE         20
#define RX_SIZE         TX_SIZE
#define Slave_ADDRESS   0xA0

/* Private variables ---------------------------------------------------------*/
u8 tx_buf[TX_SIZE] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
                      0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x12, 0x13, 0x14, 0x15};
u8 rx_buf[RX_SIZE] = {0x00};
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Error handler program
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  printf("Proc error!!!\r\n");

  /* Block communication and all processes */
  while (1)
  {
  }
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  UART_Print_Init(115200);
  
  Delay_init();
  
  Delay_us(500);  //wait for SCL/SDA is pulled up

  AT32_I2C_Init(I2Cx);

  Delay_ms(10);
  
  while(1)
  {
    Delay_ms(20);
    if(I2C_Transmit_DMA(I2Cx, Slave_ADDRESS, tx_buf, 1) != PROC_OK)   //Test Transmit 1 byte
    {
        Error_Handler();
    }
    Delay_ms(5);
    if(I2C_Transmit_DMA(I2Cx, Slave_ADDRESS, tx_buf, 16) != PROC_OK)  //Test Transmit 16 byte
    {
        Error_Handler();
    }
    Delay_ms(5);
    if(I2C_Receive_DMA(I2Cx, Slave_ADDRESS, rx_buf, 1) != PROC_OK)    //Test Receive 1 byte
    {
        Error_Handler();
    }
    Delay_ms(5);
    if(I2C_Receive_DMA(I2Cx, Slave_ADDRESS, rx_buf, 16) != PROC_OK)   //Test Receive 16 byte
    {
        Error_Handler();
    }
    Delay_ms(5);
    if(I2C_Transmit_1Byte_Receive_nByte_DMA(I2Cx, Slave_ADDRESS, 0x10, rx_buf, 1) != PROC_OK)   //Test Transmit 1 byte Receive 1 byte
    {
        Error_Handler();
    }
    Delay_ms(5);
    if(I2C_Transmit_1Byte_Receive_nByte_DMA(I2Cx, Slave_ADDRESS, 0x10, rx_buf, 16) != PROC_OK)  //Test Transmit 1 byte Receive 16 byte
    {
        Error_Handler();
    }
  }
}

/**
  * @}
  */

/**
  * @}
  */

