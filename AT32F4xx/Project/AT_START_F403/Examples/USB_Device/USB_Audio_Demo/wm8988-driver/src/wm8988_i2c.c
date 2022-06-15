/*
  **************************************************************************
  * File   : wm8988_i2c.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : wm 8988 command driver
  **************************************************************************
 */
 
#include "at32f4xx.h"
#include "wm8988_i2c.h"

#define AT32_I2C_SDA_PIN        GPIO_Pins_9
#define AT32_I2C_SCL_PIN        GPIO_Pins_8
#define AT32_I2Cx               I2C1
#define AT32_I2C_SPEED          200000   /*i2c speed*/


static void I2C_Delay(uint32_t u32iTime);
static uint8_t AT32_I2C_Timeout(uint32_t I2C_Event);
/**
  * @brief  I2C GPIO initialize
  *         
  * @param  None
  * @retval None
  */
void AT32_I2C_Init(void)
{
    GPIO_InitType  GPIO_InitStructure; 
    I2C_InitType  I2C_InitStructure;
    
    /*i2c remap*/
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO, ENABLE);
    GPIO_PinsRemapConfig(GPIO_Remap_I2C1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_I2C1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB, ENABLE);
    /*i2c gpio configure*/
    GPIO_InitStructure.GPIO_Pins =  AT32_I2C_SDA_PIN | AT32_I2C_SCL_PIN;
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /*********** I2C periphral configuration **********/
    I2C_DeInit(AT32_I2Cx);
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2CDevice; 
    I2C_InitStructure.I2C_FmDutyCycle = I2C_FmDutyCycle_2_1;  
    I2C_InitStructure.I2C_OwnAddr1 = 0x00;  
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable; 
    I2C_InitStructure.I2C_AddrMode = I2C_AddrMode_7bit;
    I2C_InitStructure.I2C_BitRate = AT32_I2C_SPEED; 
    I2C_Init(AT32_I2Cx, &I2C_InitStructure);
    I2C_Cmd(AT32_I2Cx, ENABLE);
    
}
/**
  * @brief  I2C delay 
  *         
  * @param  u32iTime Delay times
  * @retval None
  */
void I2C_Delay(uint32_t u32iTime)
{
    uint32_t u32iTem;
    for ( u32iTem = 0; u32iTem < u32iTime; u32iTem ++);
}

/**
  * @brief  I2C Write data to reg
  *         
  * @param  None
  * @retval None
  */
uint8_t AT32_I2C_Check(void)
{
    /*send the start bit*/
    I2C_GenerateSTART(AT32_I2Cx, ENABLE);
    
    /*wait start generated*/
    while(!I2C_CheckEvent(AT32_I2Cx, I2C_EVENT_MASTER_START_GENERATED))	
    {
    }
    I2C_GenerateSTOP(AT32_I2Cx, ENABLE);
    return 0;
}

/**
  * @brief  I2C Chekc Timeout
  *         
  * @param  I2C_Event
  * @retval None
  */
static uint8_t AT32_I2C_Timeout(uint32_t I2C_Event)
{
    uint32_t u32Timeout = 0xFFFFFF, u32item = 0;
    while ( !I2C_CheckEvent(AT32_I2Cx, I2C_Event) )
    {
        u32item ++;
        if ( u32item >= u32Timeout )
          return I2C_FAILED;
    }
    return I2C_SUCCESS;
}

/**
  * @brief  I2C Write data to reg
  *         
  * @param  None
  * @retval None
  */
uint8_t AT32_I2C_WriteReg(uint8_t u8DevAddr, uint8_t u8RegAddr, uint8_t u8RegData)
{
    I2C_Delay(2000);
    if(I2C_GetFlagStatus(AT32_I2Cx, I2C_FLAG_BUSYF))
    {
        return I2C_FAILED;
    }
    /*send the start bit*/
    I2C_GenerateSTART(AT32_I2Cx, ENABLE);
    
    if ( AT32_I2C_Timeout(I2C_EVENT_MASTER_START_GENERATED) != I2C_SUCCESS )
    {
        return I2C_FAILED;
    }
    /*wait start generated*/
    /*send device address*/
    I2C_Send7bitAddress(AT32_I2Cx, u8DevAddr, I2C_Direction_Transmit); 

    if ( AT32_I2C_Timeout(I2C_EVENT_MASTER_ADDRESS | I2C_EVENT_MASTER_TRANSMITTER) != I2C_SUCCESS )
    {
        return I2C_FAILED;
    }
    /*send register address*/
    I2C_SendData(AT32_I2Cx, u8RegAddr); 
    
    /*wait ack*/
    if ( AT32_I2C_Timeout(I2C_EVENT_MASTER_DATA_TRANSMITTED) != I2C_SUCCESS )
    {
        return I2C_FAILED;
    }
    /*send data*/
    I2C_SendData(AT32_I2Cx, u8RegData); 
    /*wait ack*/
    if ( AT32_I2C_Timeout(I2C_EVENT_MASTER_DATA_TRANSMITTED) != I2C_SUCCESS )
    {
        return I2C_FAILED;
    }
    /* Send STOP condition */ 
    I2C_GenerateSTOP(AT32_I2Cx, ENABLE);
    I2C_Delay(8000);
    return I2C_SUCCESS;
}



/**
  * @brief  I2C read data from reg
  *         
  * @param  None
  * @retval None
  */
uint8_t AT32_I2C_ReadReg(uint8_t u8DevAddr, uint8_t u8RegAddr, uint8_t *u8RegData)
{

    return 0;
}
