
#include "base_SPI.h"

void spi1_init(void)
{
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
    SPI1_CS1_H();
}

void spi2_init(void)
{
    SPI2_CS1_H();   //CS
    SPI2_CS2_H();
    SPI2_CS3_H();
}

int spix_write_fpga_data(char spix,char nss,uint16_t add,uint16_t data)
{
    int retval = 0;
    int spi_temp;
    switch (spix)
    {
    case 1:
    {
        SPI1_CS1_H();
        spi_temp = SPI1;
        switch (nss)
        {
        case 1:
        {
            SPI1_CS1_L();
            break;
        }
        default:
            retval = 1;
            break;
        }

        SPI1_CS1_H();
        break;
    }
    case 2:
    {
        SPI2_CS1_H();   //CS
        SPI2_CS2_H();
        SPI2_CS3_H();
        spi_temp = SPI2;
        switch (nss)
        {
        case 1:
        {
            SPI2_CS1_L();
            break;
        }
        case 2:
        {
            SPI2_CS2_L();
            break;
        }
        case 3:
        {
            SPI2_CS3_L();
            break;
        }
        default:
            retval = 1;
            break;
        }
        break;
    }
    default:
        retval = 2;
        break;
    }
    if (retval == 0)
    {
        while(RESET == spi_i2s_flag_get(spi_temp, SPI_FLAG_TBE));
        spi_i2s_data_transmit(spi_temp,(add & 0x0fff) | WRITE_FLAG);

        while(RESET == spi_i2s_flag_get(spi_temp, SPI_FLAG_TBE));
        spi_i2s_data_transmit(spi_temp,data);

    }
    SPI1_CS1_H();

    SPI2_CS1_H();
    SPI2_CS2_H();
    SPI2_CS3_H();

    return retval;
}


int spix_read_fpga_data(char spix,char nss,uint16_t add,uint16_t *data)
{
    int retval = 0;
    int spi_temp;
    switch (spix)
    {
    case 1:
    {
        SPI1_CS1_H();
        spi_temp = SPI1;
        switch (nss)
        {
        case 1:
        {
            SPI1_CS1_L();
            break;
        }
        default:
            retval = 1;
            break;
        }

        SPI1_CS1_H();
        break;
    }
    case 2:
    {
        SPI2_CS1_H();   //CS
        SPI2_CS2_H();
        SPI2_CS3_H();
        spi_temp = SPI2;
        switch (nss)
        {
        case 1:
        {
            SPI2_CS1_L();
            break;
        }
        case 2:
        {
            SPI2_CS2_L();
            break;
        }
        case 3:
        {
            SPI2_CS3_L();
            break;
        }
        default:
            retval = 1;
            break;
        }
        break;
    }
    default:
        retval = 2;
        break;
    }
    if (retval == 0)
    {
        while(RESET == spi_i2s_flag_get(spi_temp, SPI_FLAG_TBE));
        spi_i2s_data_transmit(spi_temp,(add & 0x0fff) | READ_FLAG);

        while(RESET == spi_i2s_flag_get(spi_temp, SPI_FLAG_TBE));
        spi_i2s_data_transmit(spi_temp,0x0000);

    }
    SPI1_CS1_H();

    SPI2_CS1_H();
    SPI2_CS2_H();
    SPI2_CS3_H();

    return retval;
}




