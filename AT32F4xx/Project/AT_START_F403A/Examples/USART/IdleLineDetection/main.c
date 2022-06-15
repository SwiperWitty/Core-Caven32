/**
  ******************************************************************************
  * File   : USART/MultiProcessor/main.c 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
#include "at32_board.h"

/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup Two Board Idle Line Detection
  * @{
  */ 

/* Private define ------------------------------------------------------------*/
#define MASTER
#define USART_REC_LEN  			200
#define KEY0  GPIO_ReadInputDataBit(GPIOE,GPIO_Pins_4)
#define KEY1  GPIO_ReadInputDataBit(GPIOE,GPIO_Pins_3)
#define KEY2  GPIO_ReadInputDataBit(GPIOE,GPIO_Pins_2)

#define KEY0_PRES 1
#define KEY1_PRES	2
#define KEY2_PRES	3
/* Private typedef -----------------------------------------------------------*/
typedef struct{
  uint16_t buf[USART_REC_LEN];
  uint16_t head;
  uint16_t tail;
  uint16_t count;	
}UARTSTRUCT;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
USART_InitType USART_InitStructure;
UARTSTRUCT UART_struct;
/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
uint8_t KEY_Scan(uint8_t mode);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  uint16_t i, tempVal;
  
  /* Initialize Leds, Wakeup and Key Buttons  */ 
  AT32_Board_Init();
  Delay_init();
  
  /* Initialize USART1 */
  UART_Print_Init(115200);
  NVIC_Configuration();
  
  while (1)
  {
    #ifdef MASTER
    while(KEY_Scan(0) != KEY1_PRES);

    while(USART_GetFlagStatus(USART1, USART_FLAG_TDE) != SET);
    USART_SendData(USART1, 0x7F);
    for(i = 0; i < 1; i++)
    {
        while(USART_GetFlagStatus(USART1, USART_FLAG_TDE) != SET);
        USART_SendData(USART1, 0x35);
    }
    while(USART_GetFlagStatus(USART1, USART_FLAG_TRAC) != SET);
    USART_SendBreak(USART1);
    for(i = 0; i < 1; i++)
    {
        while(USART_GetFlagStatus(USART1, USART_FLAG_TDE) != SET);
        USART_SendData(USART1, 0x36);
    }
    while(USART_GetFlagStatus(USART1, USART_FLAG_TRAC) != SET);
    /* Send Idle frame */
    USART1->CTRL1 &= ~0x08;
    USART1->CTRL1 |= 0x08;
    for(i = 0; i < 3; i++)
    {
        while(USART_GetFlagStatus(USART1, USART_FLAG_TDE) != SET);
        USART_SendData(USART1, 0x3A);
    }
    while(USART_GetFlagStatus(USART1, USART_FLAG_TRAC) != SET);
    USART_SendBreak(USART1);
    for(i=0;i<3;i++)
    {
        while(USART_GetFlagStatus(USART1, USART_FLAG_TDE) != SET);
        USART_SendData(USART1, 0x3B);
    }
    
    #else
    if(UART_struct.count > 0)
    {
        UART_struct.count--;
        tempVal = UART_struct.buf[UART_struct.tail++];
        if(UART_struct.tail > (USART_REC_LEN-1))  {
          UART_struct.tail = 0; 
        }
        while(USART_GetFlagStatus(USART1,USART_FLAG_TDE) != SET);
        USART_SendData(USART1, tempVal);
    }
    #endif
  }
}

/**
  * @brief  Configures NVIC
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
  NVIC_InitType NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  This function handles USART1 interrupt request.
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)
{
    u16 Res;
    u16 SRval=0;
    SRval = USART1->STS;

    if(USART_GetITStatus(USART1, USART_INT_IDLEF) != RESET)
    {
      UART_struct.tail=0;
    }
    if(USART_GetITStatus(USART1, USART_INT_RDNE) != RESET)
    {
       Res =USART_ReceiveData(USART1);
#ifndef MASTER
        if((Res&0x7f)==0x7f)
        {
            /* Receive one data, set mute mode */
            USART1->CTRL1 |= 0x02;            
        }
#endif
        if(UART_struct.count>(USART_REC_LEN-1))
        {
            /* Receiving data is error, re-receive */
            UART_struct.count = 0;
            UART_struct.head = 0;
            UART_struct.tail = 0;
        }			
        else  
        {
            UART_struct.count++;
            UART_struct.buf[UART_struct.head++] = Res;
            if(UART_struct.head>(USART_REC_LEN-1))  {
                UART_struct.head = 0;
            }
        }

    }
}

/**
  * @brief  This function scan which key is pressed
  * @param  mode: detect voltage level is high or low.
  *   This parameter can be one of the following values:
  *   0 or 1.
  * @retval None
  */
uint8_t KEY_Scan(uint8_t mode)
{	 
  static u8 key_up = 1;
	if(mode)key_up = 1;
	if(key_up && (KEY0 == 0 || KEY1 == 0 || KEY2 == 0))
	{
		Delay_ms(10);
		key_up = 0;
    
		if(KEY0==0)  {
      return KEY0_PRES;
    }
		else if(KEY1==0)  {
      return KEY1_PRES;
    }
		else if(KEY2==0)  {
      return KEY2_PRES;
    }
	}
  else if(KEY0 == 1 && KEY1 == 1 && KEY2 == 1)  {
    key_up = 1;
  }
 	return 0;
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

