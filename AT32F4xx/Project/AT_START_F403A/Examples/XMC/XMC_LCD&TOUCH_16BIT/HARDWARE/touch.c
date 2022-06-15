/* Includes ------------------------------------------------------------------*/
#include "touch.h"
#include "xmc_lcd.h"

/** @addtogroup AT32F403A_StdPeriph_Examples
  * @{
  */

/** @addtogroup XMC_LCD&TOUCH_16BIT
  * @brief      This file provides a set of functions needed to drive the 
  *             TOUCH.
  * @{
  */ 
  
/** @defgroup XMC_LCD&TOUCH_16BIT_Private_Defines
  * @{
  */ 
  
_touch_dev  touch_dev = 
{
  TOUCH_PIN_Init,
  TOUCH_Read_XY,
  TOUCH_Scan,
  TOUCH_Adjust,
  0,
  0,
};

u8 CMD_RDX=0XD0;
u8 CMD_RDY=0X90;

_touch_dev *touch;
/**
  * @brief  This function is init touch pin.
  * @param  None
  * @retval None
  */
void TOUCH_PIN_Init(void)
{
  GPIO_InitType GPIO_Initstruct;
  SPI_InitType   SPI_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB|RCC_APB2PERIPH_GPIOD|RCC_APB2PERIPH_AFIO, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_SPI3,ENABLE);
  /* Disable JTAG */
  GPIO_PinsRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
  
  GPIO_Initstruct.GPIO_Pins = GPIO_Pins_3|GPIO_Pins_5;
  GPIO_Initstruct.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Initstruct.GPIO_MaxSpeed  = GPIO_MaxSpeed_50MHz;
  GPIO_Init(GPIOB,&GPIO_Initstruct);
  
  GPIO_Initstruct.GPIO_Pins = GPIO_Pins_8;
  GPIO_Initstruct.GPIO_Mode = GPIO_Mode_OUT_PP;
  GPIO_Initstruct.GPIO_MaxSpeed  = GPIO_MaxSpeed_50MHz;
  GPIO_Init(GPIOB,&GPIO_Initstruct);
  /* pull up */
  GPIO_SetBits(GPIOB,GPIO_Pins_3|GPIO_Pins_5|GPIO_Pins_8);
  
  GPIO_Initstruct.GPIO_Pins = GPIO_Pins_4;
  GPIO_Initstruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB,&GPIO_Initstruct);

  
  GPIO_Initstruct.GPIO_Pins = GPIO_Pins_5;
  GPIO_Initstruct.GPIO_Mode = GPIO_Mode_IN_PU;
  GPIO_Initstruct.GPIO_MaxSpeed  = GPIO_MaxSpeed_50MHz;
  GPIO_Init(GPIOD,&GPIO_Initstruct);
  /* pull down */
  GPIO_SetBits(GPIOD,GPIO_Pins_5);
  
  SPI_DefaultInitParaConfig(&SPI_InitStructure);
  SPI_InitStructure.SPI_TransMode = SPI_TRANSMODE_FULLDUPLEX;
  SPI_InitStructure.SPI_Mode = SPI_MODE_MASTER;
  SPI_InitStructure.SPI_FrameSize = SPI_FRAMESIZE_8BIT;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_HIGH;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2EDGE;
  SPI_InitStructure.SPI_NSSSEL = SPI_NSSSEL_SOFT;
  SPI_InitStructure.SPI_MCLKP = SPI_MCLKP_256;
  SPI_InitStructure.SPI_FirstBit = SPI_FIRSTBIT_MSB;
  SPI_InitStructure.SPI_CPOLY = 7;
  SPI_Init(SPI3, &SPI_InitStructure);
  
  SPI_Enable(SPI3, ENABLE);
}

/**
  * @brief  This function is read data from touch.
  * @param  x/y : coordinate vaule.
  * @retval state
  */
u8 TOUCH_Read_XY(u16 *x,u16 *y)
{
  u16 xtemp,ytemp;
  xtemp=TOUCH_Read_X_OR_Y(CMD_RDX);
	ytemp=TOUCH_Read_X_OR_Y(CMD_RDY);

  *x=xtemp;
	*y=ytemp;
  
  return 1;
}

/**
  * @brief  This function is processing data that read from touch.
  * @param  xy: data.
  * @retval processed data
  */
/* Read five times, remove Max and Min, and then take the average*/
#define READ_TIMES 5
#define LOST_VAL 1
u16 TOUCH_Read_X_OR_Y(u8 xy)
{
 	u16 i, j;
	u16 buf[READ_TIMES];
	u16 sum=0;
	u16 temp;
	for(i=0;i<READ_TIMES;i++)buf[i]=TOUCH_Read_AD(xy);		 		    
	for(i=0;i<READ_TIMES-1; i++)
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
      /* Increase the sorting */
			if(buf[i]>buf[j])
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	  
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
	temp=sum/(READ_TIMES-2*LOST_VAL);
	return temp; 
}

/**
  * @brief  This function is read data from touch.
  * @param  xy : command.
  * @retval data
  */
u16 TOUCH_Read_AD(u8 xy)
{	  
	u16 Num=0; 
  /* ENABLE touch IC */
	TCS_LOW;
  /* Send command */
	TOUCH_Write_1byte(xy);
  /* Touch IC conversion timer */
	Delay_us(6);

  Num = TOUCH_Read_1byte();
  Num = Num<<8;
  Num |= TOUCH_Read_1byte();
  Num>>=3;
  /* Disable touch IC */
	TCS_HIGH;
	return(Num);
}

/**
  * @brief  This function is write command to touch.
  * @param  CMD : command.
  * @retval None
  */
void TOUCH_Write_1byte(u8 CMD)
{
  while(SPI_I2S_GetFlagStatus(SPI3,SPI_I2S_FLAG_TE) == RESET);
  SPI_I2S_TxData(SPI3,CMD);
  while(SPI_I2S_GetFlagStatus(SPI3,SPI_I2S_FLAG_RNE) == RESET);
  SPI3->DT;
}

/**
  * @brief  This function is read data from touch.
  * @param  None
  * @retval data
  */
u8 TOUCH_Read_1byte(void)
{
  u8 touch;
  while(SPI_I2S_GetFlagStatus(SPI3,SPI_I2S_FLAG_TE) == RESET);
  SPI_I2S_TxData(SPI3,0XFF);
  while(SPI_I2S_GetFlagStatus(SPI3,SPI_I2S_FLAG_RNE) == RESET);
  touch = SPI_I2S_RxData(SPI3);
  return touch;
}

/**
  * @brief  This function is scanning the screen.
  * @param  None
  * @retval state
  */
u8 TOUCH_Scan(void)
{
  u16 x,y;
  if(GPIO_ReadInputDataBit(GPIOD,GPIO_Pins_5) == 0)
  {
    touch_dev.touch_read_xy(&touch_dev.X_P[1],&touch_dev.Y_P[1]);
    x = (240*touch_dev.X_P[1])/(0xed0);
    y = (320*touch_dev.Y_P[1])/(0xe60);
    
    LCD_DrawPoint(x,y,RED);
    LCD_DrawPoint(x-1,y,RED);
    LCD_DrawPoint(x+1,y,RED);
    LCD_DrawPoint(x,y+1,RED);
    LCD_DrawPoint(x,y-1,RED);
  }
  return 1;
}

/**
  * @brief  This function is adjust the screen.
  * @param  None
  * @retval state
  */
u8 TOUCH_Adjust(void)
{
  /* Touch adjust code */
  return 1;
}


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

















