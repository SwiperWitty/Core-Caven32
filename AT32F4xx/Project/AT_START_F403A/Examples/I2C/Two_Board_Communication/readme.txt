/**
  @page I2C example
  
  @verbatim
  * File   : I2C/Two_Board_Communication/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the I2C Communication example.
  ******************************************************************************
   @endverbatim

@par Example Description 

  This demo is based on the AT-START-F403A board,in this demo,use hardware I2C1 
  with two board communication and USART1 printf the result. change the macro 
  definition IIC_MODE 0-3 can switch among slave-receive/slave-tranmit/master-
  receive and master-transmit operation.

  ATTENTION:
    1. I2C bus must pull-up
	2. Press slave button first, and then press master button.

 */
