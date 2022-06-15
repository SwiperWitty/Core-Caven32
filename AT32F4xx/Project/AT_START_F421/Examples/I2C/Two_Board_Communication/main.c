/**
  ******************************************************************************
  * File   : I2C/Two_Board_Communication/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
#include "i2c.h"
#include "stdio.h"	
#include "string.h"	
#include "at32_board.h"

/** @addtogroup AT32F421_StdPeriph_Examples
  * @{
  */

/** @addtogroup I2C_Two_Board_Communication
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/*
 * 0   Slave Receive
 * 1   Slave Transmit
 * 2   Master Receive
 * 3   Master Transmit
 */
#define IIC_MODE   3

/* Private macro -------------------------------------------------------------*/
#define TX_SIZE      8
#define RX_SIZE      TX_SIZE

/* Private variables ---------------------------------------------------------*/
u8 tx_buf[TX_SIZE] = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8};
u8 rx_buf[RX_SIZE] = {0};

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
#if (IIC_MODE == 0) || (IIC_MODE == 2)
  int i = 0;
#endif

  AT32_Board_Init();
  UART_Print_Init(115200);

  AT32_I2C_Init();
  printf("I2C Two Board Communication demo.\r\n");		

  while(1)
  {
    if(AT32_BUTTON_Press() == BUTTON_WAKEUP)
    {
#if   (IIC_MODE == 0)
      if(I2C_Slave_Receive_INT(rx_buf, RX_SIZE) != PROC_OK)
      {
        Error_Handler();
      }
#elif (IIC_MODE == 1)
      if(I2C_Slave_Transmit_INT(tx_buf, TX_SIZE) != PROC_OK)
      {
        Error_Handler();
      }
#elif (IIC_MODE == 2)
      if(I2C_Master_Receive_INT(I2C_ADDRESS, rx_buf, RX_SIZE) != PROC_OK)
      {
        Error_Handler();
      }
#elif (IIC_MODE == 3)
      if(I2C_Master_Transmit_INT(I2C_ADDRESS, tx_buf, TX_SIZE) != PROC_OK)
      {
        Error_Handler();
      }
#endif

#if (IIC_MODE == 0) || (IIC_MODE == 2)
      printf("Receive OK!\r\n");
      printf("Data:");
      for(i=0; i<RX_SIZE; i++)
      {
        printf("%x ", rx_buf[i]);
      }
      printf("\r\n");
#else
      printf("Transmit OK!\r\n");
#endif
    }
  }   
}

/**
  * @}
  */ 

/**
  * @}
  */ 
  
