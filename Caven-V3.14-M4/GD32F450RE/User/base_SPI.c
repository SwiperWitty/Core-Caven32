
#include "base_SPI.h"

int spi_startup = 0;

void spi1_init(void)
{
  /* GPIO configuration ------------------------------------------------------*/
    rcu_periph_clock_enable(RCU_SPI1);

    rcu_periph_clock_enable(RCU_GPIOB);         //MOSI2         NSS1_1
    rcu_periph_clock_enable(RCU_GPIOD);         //SCLK1         NSS2_2 NSS2_3
    rcu_periph_clock_enable(RCU_GPIOI);         //MOSI1         MISO1
    
  /* Configure SPI1 pins: SCK and MOSI and MISO ---------------------------------*/
    gpio_af_set(GPIOI, GPIO_AF_5, SPI1_MOSI | SPI1_MISO); //GPIOC GPIOI
    gpio_mode_set(GPIOI, GPIO_MODE_AF, GPIO_PUPD_NONE,SPI1_MOSI | SPI1_MISO);
    gpio_output_options_set(GPIOI, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,SPI1_MOSI | SPI1_MISO);

    gpio_af_set(GPIOD, GPIO_AF_5, SPI1_CLK);
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE,SPI1_CLK);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,SPI1_CLK);
  
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SPI1_CS1);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, SPI1_CS1);
  
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
    spi_startup |= 0x01 << 0;
    SPI1_CS1_H();
}

void spi2_init(void)
{
    rcu_periph_clock_enable(RCU_SPI2);
    
    rcu_periph_clock_enable(RCU_GPIOA);         //NSS2_1
    rcu_periph_clock_enable(RCU_GPIOB);         //MOSI2 MISO2
    rcu_periph_clock_enable(RCU_GPIOC);         //SCLK2
    rcu_periph_clock_enable(RCU_GPIOD);         //NSS2_2 NSS2_3
    
    gpio_af_set(GPIOB, GPIO_AF_6, SPI2_MOSI | SPI2_MISO); 
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE,SPI2_MOSI | SPI2_MISO);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,SPI2_MOSI | SPI2_MISO);
    
    gpio_af_set(GPIOC, GPIO_AF_6, SPI2_CLK);
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE,SPI2_CLK);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,SPI2_CLK);
    
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SPI2_CS1);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, SPI2_CS1);
 
    gpio_mode_set(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SPI2_CS2 | SPI2_CS3);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, SPI2_CS2 | SPI2_CS3);

    /* SPI2 Config -------------------------------------------------------------*/
    spi_parameter_struct spi_init_struct;

    /* configure SPI2 parameter */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_16BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_4;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI2, &spi_init_struct);

    /* Enable SPI2 */
    spi_enable(SPI2);
    SPI2_CS1_H();   //CS
    SPI2_CS2_H();
    SPI2_CS3_H();
    spi_startup |= 0x01 << 1;
}


int spi_write_fpga_reg(uint16_t addr, uint16_t data)
{
    int retval = 0;
    int spi_temp = SPI1;
    if(spi_startup & (0x01 << 0))       //SPI1
    {
        SPI1_CS1_L();
//        Base_Delay (5,10);

        while(RESET == spi_i2s_flag_get(spi_temp, SPI_FLAG_TBE));
        spi_i2s_data_transmit(spi_temp,(addr & 0x0fff) | WRITE_FLAG);
//        Base_Delay (10,10);

        while(RESET == spi_i2s_flag_get(spi_temp, SPI_FLAG_TBE));
        spi_i2s_data_transmit(spi_temp,data);
//        Base_Delay (10,10);

        SPI1_CS1_H();
//        Base_Delay (5,10);
    }
    return retval;
}

uint32_t spi_read_fpga_reg(uint16_t addr, uint16_t* data)
{
    int retval = 0;
    uint16_t addr_get = 0xffff;
    int spi_temp = SPI1;
    if(spi_startup & (0x01 << 0))       //SPI1
    {
        //SPI_FLAG_RBNE
        SPI1_CS1_L();
//        Base_Delay (5,10);

        while(RESET == spi_i2s_flag_get(spi_temp, SPI_FLAG_TBE));
        spi_i2s_data_transmit(spi_temp,(addr & 0x0fff) | READ_FLAG);    //发送要读的地址    >>
        
//        Base_Delay (10,10);
        spi_i2s_data_receive(spi_temp);             //清空接收标志  <<
        
        while(RESET == spi_i2s_flag_get(spi_temp, SPI_FLAG_TBE));
        spi_i2s_data_transmit(spi_temp,0x0000);                         //发送时钟给MISO    >>
        while(RESET == spi_i2s_flag_get(spi_temp, SPI_FLAG_RBNE));
        addr_get = spi_i2s_data_receive(spi_temp);  //接收返回值（写入的地址）  <<
        
        addr_get &= 0x7fff;
        if(addr!=addr_get)
        {
            retval=1;
            return retval;
        }
//        Base_Delay (10,10);
        
        while(RESET == spi_i2s_flag_get(spi_temp, SPI_FLAG_TBE));
        spi_i2s_data_transmit(spi_temp,0x0000);                         //发送时钟给MISO    >>
        while(RESET == spi_i2s_flag_get(spi_temp, SPI_FLAG_RBNE));
        * data = spi_i2s_data_receive(spi_temp);  //接收返回值（数据）  <<
        
//        Base_Delay (10,10);
        SPI1_CS1_H();
//        Base_Delay (5,10);
    }
    return retval;
}

void spi2anachip(uint16_t data,uint16_t nss)
{
    int spi_temp = SPI2;
    if(spi_startup & (0x01 << 1))       //SPI2
    {
        SPI2_CS1_H(); SPI2_CS2_H(); SPI2_CS3_H();
  
        if(nss & (0x01 << 0))
        {SPI2_CS1_L();}
        if(nss & (0x01 << 1))
        {SPI2_CS2_L();}
        if(nss & (0x01 << 2))
        {SPI2_CS3_L();}
//        Base_Delay (1,10);

        while(RESET == spi_i2s_flag_get(spi_temp, SPI_FLAG_TBE));
        spi_i2s_data_transmit(spi_temp,data);
        
//        Base_Delay (4,10);

        SPI2_CS1_H(); SPI2_CS2_H(); SPI2_CS3_H();
//        Base_Delay (1,10);
    }
    
}

