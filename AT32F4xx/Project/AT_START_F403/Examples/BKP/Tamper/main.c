/**
  ******************************************************************************
  * File   : BKP/Tamper/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body.
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
#include "at32_board.h"
#include "main.h"
#include <stdio.h>

/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup BKP_Tamper
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
typedef enum {FALSE = 0, TRUE = !FALSE} LogicStatus;

/* Private define ------------------------------------------------------------*/
#define BKP_DR_NUMBER              42
  
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint16_t BKP_Addr_Tab[BKP_DR_NUMBER] =
{
	BKP_DT1, BKP_DT2, BKP_DT3, BKP_DT4, BKP_DT5, BKP_DT6, BKP_DT7, BKP_DT8,
	BKP_DT9, BKP_DT10, BKP_DT11, BKP_DT12, BKP_DT13, BKP_DT14, BKP_DT15, BKP_DT16,
	BKP_DT17, BKP_DT18, BKP_DT19, BKP_DT20, BKP_DT21, BKP_DT22, BKP_DT23, BKP_DT24,
	BKP_DT25, BKP_DT26, BKP_DT27, BKP_DT28, BKP_DT29, BKP_DT30, BKP_DT31, BKP_DT32,
	BKP_DT33, BKP_DT34, BKP_DT35, BKP_DT36, BKP_DT37, BKP_DT38, BKP_DT39, BKP_DT40,
	BKP_DT41, BKP_DT42
};

/* Private function prototypes -----------------------------------------------*/
void NVIC_Configuration(void);
void WriteBackupReg(void);
uint8_t CheckBackupReg(void);
uint32_t IsBackupRegReset(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main Function.
  * @param  None
  * @retval None
  */
int main(void)
{
	/* USART configuration */
	UART_Print_Init(115200);

	/* NVIC configuration */
	NVIC_Configuration();

	/* Enable PWR and BKP clock */
	RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_PWR | RCC_APB1PERIPH_BKP, ENABLE);

	/* Enable write access to Backup domain */
	PWR_BackupAccessCtrl(ENABLE);

	/* Disable Tamper pin */
	BKP_TamperPinCmd(DISABLE);

	/* Disable Tamper interrupt */
	BKP_IntConfig(DISABLE);

	/* Tamper pin active on low level */
	BKP_TamperPinLvConfig(BKP_TamperPinLv_L);

	/* Clear Tamper pin Event pending flag */
	BKP_ClearFlag();

	/* Write data to Backup DRx registers */
	WriteBackupReg();

	/* Enable Tamper interrupt */
	BKP_IntConfig(ENABLE);

	/* Enable Tamper pin */
	BKP_TamperPinCmd(ENABLE);

	 /* Check if the written data are correct */
	if (CheckBackupReg() == TRUE)
	{
		printf("\r\nwrite Backup Reg ok");
	}
	else
	{
		printf("\r\nwrite Backup Reg fail");
	}

	while (1)
	{
	}
}

/**
  * @brief  Configures NVIC and Vector Table base location.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
	NVIC_InitType NVIC_InitStructure;

	/* Enable TAMPER IRQChannel */
	NVIC_InitStructure.NVIC_IRQChannel = TAMPER_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Writes data Backup DRx registers.
  * @param  FirstBackupData: data to be written to Backup data registers.
  * @retval None
  */
void WriteBackupReg(void)
{
	u32 index = 0;

	for (index = 0; index < BKP_DR_NUMBER; index++)
	{
		BKP_WriteBackupReg(BKP_Addr_Tab[index], 0x5A00 | BKP_Addr_Tab[index]);
	}
}

/**
  * @brief  Checks if the Backup DRx registers values are correct or not.
  * @param  FirstBackupData: data to be compared with Backup data registers.
  * @retval 
  *         - 0: All Backup DRx registers values are correct
  *         - Value different from 0: Number of the first Backup register
  *           which value is not correct
  */
uint8_t CheckBackupReg(void)
{
	u32 index = 0;

	for (index = 0; index < BKP_DR_NUMBER; index++)
	{
		if (BKP_ReadBackupReg(BKP_Addr_Tab[index]) != (0x5A00 | BKP_Addr_Tab[index]))
		{
			return FALSE;
		}
	}

	return TRUE;
}

/**
  * @brief  Checks if the Backup DRx registers are reset or not.
  * @param  None
  * @retval 
  *          - 0: All Backup DRx registers are reset
  *          - Value different from 0: Number of the first Backup register
  *            not reset
  */
uint32_t IsBackupRegReset(void)
{
	uint32_t index = 0;

	for (index = 0; index < BKP_DR_NUMBER; index++)
	{
		if (BKP_ReadBackupReg(BKP_Addr_Tab[index]) != 0x0000)
		{
			return (index + 1);
		}
	}

	return 0;
}

/**
  * @}
  */ 

/**
  * @}
  */ 

