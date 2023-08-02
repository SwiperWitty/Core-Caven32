
#if 0

#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "gd32f450i_eval.h"
#include "string.h"


#define SPI_CS1    GPIO_PIN_9  /*gpioB*/
#define SPI_CLK    GPIO_PIN_3  /*gpioD*/
#define SPI_MISO   GPIO_PIN_2  /*gpioC  gpioI*/
#define SPI_MOSI   GPIO_PIN_3  /*gpioC  gpioI*/


void spi1_init(void){
  /* GPIO configuration ------------------------------------------------------*/
    rcu_periph_clock_enable(RCU_SPI0);
    rcu_periph_clock_enable(RCU_SPI1);

    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_GPIOI);
    
  /* Configure SPI1 pins: SCK and MOSI and MISO ---------------------------------*/
    gpio_af_set(GPIOC, GPIO_AF_5, SPI_MOSI | SPI_MISO); //GPIOC GPIOI
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE,SPI_MOSI | SPI_MISO);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,SPI_MOSI | SPI_MISO);
  
    gpio_af_set(GPIOD, GPIO_AF_5, SPI_CLK);
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE,SPI_CLK);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,SPI_CLK);
  
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SPI_CS1);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, SPI_CS1);
  
  /* SPI1 Config -------------------------------------------------------------*/
  spi_parameter_struct spi_init_struct;

  /* configure SPI1 parameter */
  spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
  spi_init_struct.device_mode          = SPI_MASTER;
  spi_init_struct.frame_size           = SPI_FRAMESIZE_16BIT;
  spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_2EDGE;
  spi_init_struct.nss                  = SPI_NSS_SOFT;
  spi_init_struct.prescale             = SPI_PSC_8;
  spi_init_struct.endian               = SPI_ENDIAN_MSB;
  spi_init(SPI1, &spi_init_struct);
  
  /* Enable SPI1 */
  spi_enable(SPI1);
  
  GPIO_BC(GPIOB) = SPI_CS1;             //High-GPIO_BC Low-GPIO_BOP 
}

uint16_t array[64];

/*!
    \brief    main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{

//    gd_eval_led_init(LED1);
//    gd_eval_led_init(LED2);
    systick_config();
    /* enable the led clock */
    spi1_init();
    memcpy(array,"1234000000",sizeof("1234000000"));
    while(1) 
    {
//        led_spark();
        for(int i = 0;i < 5;i++)
        {
            while(RESET == spi_i2s_flag_get(SPI1, SPI_FLAG_TBE));
            spi_i2s_data_transmit(SPI1, array[i]);
        }
        delay_1ms(1);
    }
}
#endif
