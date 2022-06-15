/*
  **************************************************************************
  * File   : wm8988_i2c.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : header for i2c driver
  **************************************************************************
 */
 
 enum I2C_State{
    I2C_SUCCESS,
    I2C_FAILED    
};
 
 void AT32_I2C_Init(void);
 uint8_t AT32_I2C_WriteReg(uint8_t u8DevAddr, uint8_t u8RegAddr, uint8_t u8RegData);
 uint8_t AT32_I2C_ReadReg(uint8_t u8DevAddr, uint8_t u8RegAddr, uint8_t *u8RegData);
 uint8_t AT32_I2C_Check(void);
