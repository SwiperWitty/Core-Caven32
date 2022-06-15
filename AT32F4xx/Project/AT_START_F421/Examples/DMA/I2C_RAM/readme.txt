/**
  @page DMA I2C_RAM example
  
  @verbatim
  * File   : DMA/I2C_RAM/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the DMA I2C_RAM example.
  ******************************************************************************
   @endverbatim

@par Example Description 
  This demo provides a description of how to use two DMA channels to transfer a 
  data buffer from memory to I2C2 through I2C1.If received data right ,the Three 
  leds will turn on.
  
  - Connect I2C1 SCL pin (PA.0) to I2C2 SCL pin (PA.9)
  - Connect I2C1 SDA pin (PA.1) to I2C2 SDA pin (PA.10)
  - Check that a pull-up resistor is connected on one I2C SDA pin
  - Check that a pull-up resistor is connected on one I2C SCL pin   
  
 *



