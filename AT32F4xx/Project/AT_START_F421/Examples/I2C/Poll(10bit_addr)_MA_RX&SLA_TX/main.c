/**
  ******************************************************************************
  * File   : I2C/Poll(10bit_addr)_MA_RX&SLA_TX/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
#include "at32f4xx_i2c_ex.h"
#include "stdio.h"	
#include "string.h"	
#include "at32_board.h"

/** @addtogroup AT32F421_StdPeriph_Examples
  * @{
  */

/** @addtogroup Poll(10bit_addr)_MA_RX&SLA_TX
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MASTER_BOARD

/* Private macro -------------------------------------------------------------*/
#define BUF_SIZE      8


/* Private variables ---------------------------------------------------------*/
u8 tx_buf[BUF_SIZE] = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8};
u8 rx_buf[BUF_SIZE] = {0};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Error handler program
  * @param  I2C_Status
  * @retval None
  */
void Error_Handler(I2C_StatusType I2C_Status)
{
  printf("Proc error I2C_ERROR_STEP_%d!!!\r\n", I2C_Status);
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  
  I2C_StatusType I2C_Status;
  
  AT32_Board_Init();
  
  UART_Print_Init(115200);

  I2Cx_Init(I2C_PORT);
  
  printf("Polling(10bit_address) - Master Receiver & Slave Transmitter.\r\n");		  

  while(1)
  {
    
#ifdef MASTER_BOARD   
    int i = 0;
  
    /* Wait for Key BUTTON_WAKEUP press before starting the Communication */
    /* Timeout is set to 10S  */
    while(AT32_BUTTON_Press() != BUTTON_WAKEUP)
    {
    }
    
    /* Start the request reception process */  
    if((I2C_Status = I2C_Master_Receive(I2C_PORT, I2C_SLAVE_ADDRESS10, rx_buf, BUF_SIZE, 10000)) != I2C_OK)
    {
      /* Error_Handler() function is called when error occurs */
      Error_Handler(I2C_Status);
    }       
    else
    {
      printf("Receive OK!\r\n");
      printf("Data:");
      for(i = 0; i < BUF_SIZE; i++)
      {
        printf("%x ", rx_buf[i]);
      }
      printf("\r\n");    
    }    
    
#else
    
    /* Start the transmission process */  
    /* Timeout is set to 10S  */
    if((I2C_Status = I2C_Slave_Transmit(I2C_PORT, tx_buf, BUF_SIZE, 10000)) != I2C_OK)
    {
      /* Error_Handler() function is called when error occurs */ 
      Error_Handler(I2C_Status);
    }
    else
    {
      printf("Transmit End!\r\n");    
    }
    
#endif
    
  }   
}

/**
  * @}
  */ 

/**
  * @}
  */ 
  
