/**
  **************************************************************************
  * File   : main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : SPI configuration file.
  **************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PLATFORM_CONFIG_H
#define __PLATFORM_CONFIG_H


/* Define the AT32F413 hardware depending on the used AT board */
  #define SPI_MASTER                   SPI1
  #define SPI_MASTER_CLK               RCC_APB2PERIPH_SPI1
  #define SPI_MASTER_GPIO              GPIOA
  #define SPI_MASTER_GPIO_CLK          RCC_APB2PERIPH_GPIOA  
  #define SPI_MASTER_PIN_SCK           GPIO_Pins_5
  #define SPI_MASTER_PIN_MISO          GPIO_Pins_6
  #define SPI_MASTER_PIN_MOSI          GPIO_Pins_7
  #define SPI_MASTER_DMA               DMA1
  #define SPI_MASTER_DMA_CLK           RCC_AHBPERIPH_DMA1  
  #define SPI_MASTER_Rx_DMA_Channel    DMA1_Channel2
  #define SPI_MASTER_Rx_DMA_FLAG       DMA1_FLAG_TC2
  #define SPI_MASTER_Tx_DMA_Channel    DMA1_Channel3
  #define SPI_MASTER_Tx_DMA_FLAG       DMA1_FLAG_TC3  
  #define SPI_MASTER_DR_Base           0x4001300C
  
  #define SPI_SLAVE                    SPI2
  #define SPI_SLAVE_CLK                RCC_APB1PERIPH_SPI2
  #define SPI_SLAVE_GPIO               GPIOB
  #define SPI_SLAVE_GPIO_CLK           RCC_APB2PERIPH_GPIOB 
  #define SPI_SLAVE_PIN_SCK            GPIO_Pins_13
  #define SPI_SLAVE_PIN_MISO           GPIO_Pins_14
  #define SPI_SLAVE_PIN_MOSI           GPIO_Pins_15 
  #define SPI_SLAVE_DMA                DMA1
  #define SPI_SLAVE_DMA_CLK            RCC_AHBPERIPH_DMA1  
  #define SPI_SLAVE_Rx_DMA_Channel     DMA1_Channel4
  #define SPI_SLAVE_Rx_DMA_FLAG        DMA1_FLAG_TC4
  #define SPI_SLAVE_Tx_DMA_Channel     DMA1_Channel5
  #define SPI_SLAVE_Tx_DMA_FLAG        DMA1_FLAG_TC5  
  #define SPI_SLAVE_DR_Base            0x4000380C
          

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __PLATFORM_CONFIG_H */

