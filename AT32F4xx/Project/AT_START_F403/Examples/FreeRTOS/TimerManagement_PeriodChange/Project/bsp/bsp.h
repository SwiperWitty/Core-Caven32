/**
  ******************************************************************************
  * File   : Project/bsp/bsp.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Bsp Service Routines.
  ******************************************************************************
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/  
#ifndef _BSP_H_
#define _BSP_H

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define __AT32F403_BSP_VERSION		"1.0"

#define  USE_FreeRTOS      1

#if USE_FreeRTOS == 1
	#include "FreeRTOS.h"
	#include "task.h"
	#define DISABLE_INT()    taskENTER_CRITICAL()
	#define ENABLE_INT()     taskEXIT_CRITICAL()
#else
	#define ENABLE_INT()	__set_PRIMASK(0)	/* enable global interrupt */
	#define DISABLE_INT()	__set_PRIMASK(1)	/* disable global interrupt */
#endif

/* for debug */
#define BSP_Printf		printf

#include "at32f4xx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef enum
{
  FALSE = 0, TRUE  = !FALSE
}
bool;


/****** include driver *****/

#include "bsp_uart_fifo.h"
#include "bsp_led.h"
#include "bsp_timer.h"
#include "bsp_key.h"
//#include "bsp_dwt.h"

//#include "bsp_msg.h"

//#include "bsp_beep.h"

#include "bsp_tim_pwm.h"

//#include "bsp_cpu_flash.h"
//#include "bsp_sdio_sd.h"
//#include "bsp_i2c_gpio.h"
//#include "bsp_eeprom_24xx.h"
//#include "bsp_si4730.h"
//#include "bsp_hmc5883l.h"
//#include "bsp_mpu6050.h"
//#include "bsp_bh1750.h"
//#include "bsp_bmp180.h"
//#include "bsp_wm8978.h"
//#include "bsp_gt811.h"

//#include "bsp_fsmc_sram.h"
//#include "bsp_nand_flash.h"
//#include "bsp_nor_flash.h"

//#include "LCD_RA8875.h"
//#include "LCD_SPFD5420.h"
//#include "LCD_ILI9488.h"
//#include "bsp_ra8875_port.h"
//#include "bsp_tft_lcd.h"

//#include "bsp_touch.h"


//#include "bsp_oled.h"
//#include "bsp_sim800.h"
//#include "bsp_ra8875_flash.h"

//#include "bsp_spi_bus.h"
//#include "bsp_spi_flash.h"
//#include "bsp_tm7705.h"
//#include "bsp_vs1053b.h"
//#include "bsp_tsc2046.h"

//#include "bsp_ds18b20.h"
//#include "bsp_dac8501.h"
//#include "bsp_dht11.h"

//#include "bsp_ir_decode.h"
//#include "bsp_ps2.h"

//#include "bsp_modbus.h"
//#include "bsp_rs485_led.h"
//#include "bsp_user_lib.h"

//#include "bsp_dac8501.h"
//#include "bsp_dac8562.h"

//#include "bsp_esp8266.h"
//#include "bsp_step_moto.h"


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void bsp_Init(void);
void bsp_Idle(void);
void BSP_Tick_Init (void);

/* Private functions ---------------------------------------------------------*/

#endif

