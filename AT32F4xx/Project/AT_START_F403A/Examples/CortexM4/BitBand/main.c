/**
  ******************************************************************************
  * File   : CortexM4/BitBand/main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 
 
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "at32f4xx.h"
#include "at32_board.h"

/** @addtogroup AT32F403A_StdPeriph_Examples
  * @{
  */

/** @addtogroup CortexM4_BitBand
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define RAM_BASE       0x20000000
#define RAM_BB_BASE    0x22000000
 
/* Private macro -------------------------------------------------------------*/
#define  Var_ResetBit_BB(VarAddr, BitNumber)    \
          (*(__IO uint32_t *) (RAM_BB_BASE | ((VarAddr - RAM_BASE) << 5) | ((BitNumber) << 2)) = 0)
   
#define Var_SetBit_BB(VarAddr, BitNumber)       \
          (*(__IO uint32_t *) (RAM_BB_BASE | ((VarAddr - RAM_BASE) << 5) | ((BitNumber) << 2)) = 1)

#define Var_GetBit_BB(VarAddr, BitNumber)       \
          (*(__IO uint32_t *) (RAM_BB_BASE | ((VarAddr - RAM_BASE) << 5) | ((BitNumber) << 2)))


/* Private variables ---------------------------------------------------------*/
__IO uint32_t Var, VarAddr = 0, VarBitValue = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  LED4 Flick For Error.
  * @param  None
  * @retval None
  */
void Variable_Error(void)
{
  while(1)
  {
    AT32_LEDn_Toggle(LED4);
    Delay_sec(1);
  }
}

/**
  * @brief  Main Function.
  * @param  None
  * @retval None
  */
int main(void)
{
  Var = 0x00005AA5;
  
  /* Get the variable address */ 
  VarAddr = (uint32_t)&Var; 
  
  /* LED Init */
  AT32_Board_Init();
  
  /* Modify Var variable bit 0 -----------------------------------------------*/
  Var_ResetBit_BB(VarAddr, 0);  /* Var = 0x00005AA4 */
  if(Var != 0x00005AA4)
  {
    Variable_Error();
  }
  
  Var_SetBit_BB(VarAddr, 0);    /* Var = 0x00005AA5 */
  if(Var != 0x00005AA5)
  {
    Variable_Error();
  }
  
  /* Modify Var variable bit 11 ----------------------------------------------*/
  Var_ResetBit_BB(VarAddr, 11);             /* Var = 0x000052A5 */
  if(Var != 0x000052A5)
  {
    Variable_Error();
  }
  
  /* Get Var variable bit 11 value */
  VarBitValue = Var_GetBit_BB(VarAddr, 11); /* VarBitValue = 0x00000000 */
  if(VarBitValue != 0)
  {
    Variable_Error();
  }
  
  Var_SetBit_BB(VarAddr, 11);               /* Var = 0x00005AA5 */
  if(Var != 0x00005AA5)
  {
    Variable_Error();
  }
  
  /* Get Var variable bit 11 value */
  VarBitValue = Var_GetBit_BB(VarAddr, 11);    /* VarBitValue = 0x00000001 */
  if(VarBitValue != 1)
  {
    Variable_Error();
  }
  
  /* Modify Var variable bit 31 ----------------------------------------------*/
  Var_SetBit_BB(VarAddr, 31);               /* Var = 0x80005AA5 */
  if(Var != 0x80005AA5)
  {
    Variable_Error();
  }
  
  /* Get Var variable bit 31 value */
  VarBitValue = Var_GetBit_BB(VarAddr, 31); /* VarBitValue = 0x00000001 */
  if(VarBitValue != 1)
  {
    Variable_Error();
  }
    
  Var_ResetBit_BB(VarAddr, 31);             /* Var = 0x00005AA5 */
  if(Var != 0x00005AA5)
  {
    Variable_Error();
  }
  
  /* Get Var variable bit 31 value */
  VarBitValue = Var_GetBit_BB(VarAddr, 31); /* VarBitValue = 0x00000000 */
  if(VarBitValue != 0)
  {
    Variable_Error();
  }
    
  for(;;)
  {
    AT32_LEDn_Toggle(LED2);
    Delay_sec(1);
    AT32_LEDn_Toggle(LED3);
    Delay_sec(1);
    AT32_LEDn_Toggle(LED4);
    Delay_sec(1);
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

