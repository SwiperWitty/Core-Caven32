/**
  @page I2C example
  
  @verbatim
  * File   : I2C/DMA_MA_TX&SLA_RX/readme.txt 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Description of the I2C communication example based on DMA mode.
  ******************************************************************************
   @endverbatim

@par Example Description 

  This demo is based on the AT-START-F421 board,in this demo,use hardware I2C1 
  with two board communication and USART1 printf the result. change the macro 
  definition MASTER_BOARD can switch among master-transmit/slave-receive operation.

  ATTENTION:
    1. I2C bus must pull-up
	2. Press the master button to start communication.

 */
