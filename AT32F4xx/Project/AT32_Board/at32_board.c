/**
  **************************************************************************
  * File   : at32_board.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : 1. Set of firmware functions to manage Leds, push-button and COM ports.
  *          2. initialize Delay Function and USB
  **************************************************************************
  */

#include "at32_board.h"
#include "stdio.h"
#include "at32f4xx_usart.h"
#include "misc.h"

/* Suport printf function, useMicroLib is unnecessary */
#ifdef __CC_ARM
  #pragma import(__use_no_semihosting)
  struct __FILE
  {
    int handle;
  };
  
  FILE __stdout;
  
  void _sys_exit(int x)
  {
    x = x;
  }
#endif

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
  
/*delay macros*/
#define STEP_DELAY_MS	50
  
/*AT-START LED resouce array*/
GPIO_Type *LED_GPIO_PORT[LED_NUM] = {LED1_GPIO, LED2_GPIO, LED3_GPIO, LED4_GPIO};
uint16_t LED_GPIO_PIN[LED_NUM]    = {LED1_PIN, LED2_PIN, LED3_PIN, LED4_PIN};
uint32_t LED_GPIO_RCC_CLK[LED_NUM] = {LED1_GPIO_RCC_CLK, LED2_GPIO_RCC_CLK, LED3_GPIO_RCC_CLK, LED4_GPIO_RCC_CLK};

/*AT-START Button resouce arry*/
GPIO_Type *BUTTON_GPIO_PORT[BUTTON_NUM] = {BUTTON_WAKEUP_GPIO, BUTTON_USER_KEY_GPIO};
uint16_t BUTTON_GPIO_PIN[BUTTON_NUM]    = {BUTTON_WAKEUP_PIN, BUTTON_USER_KEY_PIN};
uint32_t BUTTON_GPIO_RCC_CLK [BUTTON_NUM] = {BUTTON_WAKEUP_RCC_CLK, BUTTON_USER_KEY_RCC_CLK};

/*delay variable*/
static __IO uint32_t fac_us;
static __IO uint32_t fac_ms;
 
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{  
  USART_SendData(AT32_PRINT_UART, ch);
  while ( USART_GetFlagStatus(AT32_PRINT_UART, USART_FLAG_TRAC) == RESET );    
  return ch;
}

/**
  * @brief  Board initialize interface 
  *         init LED and BUTTON
  * @param  None
  * @retval None
  */
void AT32_Board_Init()
{
  /*initialize Delay Function*/
  Delay_init();

  /*Configure LED in AT_START_BOARD*/
  AT32_LEDn_Init(LED2);
  AT32_LEDn_Init(LED3);
  AT32_LEDn_Init(LED4);
  AT32_LEDn_OFF(LED2);
  AT32_LEDn_OFF(LED3);
  AT32_LEDn_OFF(LED4);

  /*Configure Button in AT_START_BOARD*/
  AT32_BUTTON_Init(BUTTON_WAKEUP); /*PA0*/   
  AT32_BUTTON_Init(BUTTON_USER_KEY); /*PC13*/
}

/**
  * @brief  USB GPIO initialize
  *         USB use DP->PA12, DM->PA11    
  * @param  None
  * @retval None
  */
void AT32_USB_GPIO_init()
{
  GPIO_InitType GPIO_InitStructure;
  /* Enable the USB Clock*/
  RCC_APB2PeriphClockCmd(USB_GPIO_RCC_CLK, ENABLE);

  /*Configure DP, DM pin as GPIO_Mode_OUT_PP*/
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins  = USB_DP_PIN | USB_DM_PIN;
#if !defined (AT32F421xx)
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
#else
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OutType = GPIO_OutType_PP;
  GPIO_InitStructure.GPIO_Pull = GPIO_Pull_NOPULL;
#endif
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_Init(USB_GPIO, &GPIO_InitStructure);
  GPIO_ResetBits(USB_GPIO, USB_DP_PIN);
}

/**
  * @brief  Configure Button GPIO   
  * @param  Button: Specifies the Button to be configured.
  * @retval None
  */
void AT32_BUTTON_Init(BUTTON_Type button)
{
  GPIO_InitType GPIO_InitStructure;

  /*Enable the Button Clock*/
#if defined (AT32F421xx)
  RCC_AHBPeriphClockCmd(BUTTON_GPIO_RCC_CLK[button], ENABLE);
#else
  RCC_APB2PeriphClockCmd(BUTTON_GPIO_RCC_CLK[button], ENABLE);
#endif

  /*Configure Button pin as input with pull-up/pull-down*/
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins = BUTTON_GPIO_PIN[button];
#if !defined (AT32F421xx)
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PD;
#else
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Pull = GPIO_Pull_PD;
#endif
  GPIO_Init(BUTTON_GPIO_PORT[button], &GPIO_InitStructure);
}

/**
  * @brief  Returns the selected button state   
  * @param  Button: Specifies the Button to be Checked
  * @retval The Button GPIO Pin value
  */
uint8_t AT32_BUTTON_State(BUTTON_Type button)
{
  return GPIO_ReadInputDataBit(BUTTON_GPIO_PORT[button], BUTTON_GPIO_PIN[button]);
}

/**
  * @brief  Returns which Button have press down 
  * @param  None
  * @retval The Button have press down
  */
BUTTON_Type AT32_BUTTON_Press()
{
  static uint8_t Pressed = 1;
  /*Get Button state in AT_START board*/
  if ((Pressed == 1) && ((AT32_BUTTON_State(BUTTON_WAKEUP) == Bit_SET ) || (AT32_BUTTON_State(BUTTON_USER_KEY) == Bit_SET )))
  {
    /*debounce*/
    Pressed = 0;
    Delay_ms(10);
    if (AT32_BUTTON_State(BUTTON_WAKEUP) == Bit_SET)
      return BUTTON_WAKEUP;
    if (AT32_BUTTON_State(BUTTON_USER_KEY) == Bit_SET)
      return BUTTON_USER_KEY;
  }else if((AT32_BUTTON_State(BUTTON_USER_KEY) == Bit_RESET) && (AT32_BUTTON_State(BUTTON_WAKEUP) == Bit_RESET))
  {
    Pressed = 1;
  }
  return NO_BUTTON;
}

/**
  * @brief  Configure LED GPIO   
  * @param  led: Specifies the LED to be configured.
  * @retval None
  */
void AT32_LEDn_Init(LED_Type led)
{
  GPIO_InitType GPIO_InitStructure;
  /*Enable the LED Clock*/
#if defined (AT32F421xx)
  RCC_AHBPeriphClockCmd(LED_GPIO_RCC_CLK[led], ENABLE);
#else
  RCC_APB2PeriphClockCmd(LED_GPIO_RCC_CLK[led], ENABLE);
#endif

  /*Configure the LED pin as ouput push-pull*/
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins = LED_GPIO_PIN[led];				 
#if !defined (AT32F421xx)
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
#else
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OutType = GPIO_OutType_PP;
  GPIO_InitStructure.GPIO_Pull = GPIO_Pull_NOPULL;
#endif 
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;	
  GPIO_Init(LED_GPIO_PORT[led], &GPIO_InitStructure);
}

/**
  * @brief  Turns selected LED On.
  * @param  Led: Specifies the Led to be set on. 
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  *     @arg LED3
  *     @arg LED4  
  * @retval None
  */
void AT32_LEDn_ON(LED_Type led)
{
  if ( led > (LED_NUM - 1))
    return;
  if ( LED_GPIO_PIN[led] )
    LED_GPIO_PORT[led]->BRE = LED_GPIO_PIN[led];
}

/**
  * @brief  Turns selected LED Off.
  * @param  Led: Specifies the Led to be set off. 
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  *     @arg LED3
  *     @arg LED4 
  * @retval None
  */
void AT32_LEDn_OFF(LED_Type led)
{
  if ( led > (LED_NUM - 1))
    return;
  if ( LED_GPIO_PIN[led] )
    LED_GPIO_PORT[led]->BSRE = LED_GPIO_PIN[led];
}

/**
  * @brief  Turns selected LED Tooggle.
  * @param  Led: Specifies the Led to be set off. 
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  *     @arg LED3
  *     @arg LED4 
  * @retval None
  */
void AT32_LEDn_Toggle(LED_Type led)
{
  if ( led > (LED_NUM - 1))
    return;
  if ( LED_GPIO_PIN[led] )
    LED_GPIO_PORT[led]->OPTDT ^= LED_GPIO_PIN[led];
}

/**
  * @brief  initialize UART1   
  * @param  bound: UART BaudRate
  * @retval None
  */
void UART_Print_Init(uint32_t bound)
{
  GPIO_InitType GPIO_InitStructure;
  USART_InitType USART_InitStructure;

  /*Enable the UART Clock*/
#if defined (AT32F421xx)
  RCC_AHBPeriphClockCmd(AT32_PRINT_UARTTX_GPIO_RCC | AT32_PRINT_UARTRX_GPIO_RCC, ENABLE);	
#else
  RCC_APB2PeriphClockCmd(AT32_PRINT_UARTTX_GPIO_RCC | AT32_PRINT_UARTRX_GPIO_RCC, ENABLE);	
#endif
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_USART1, ENABLE);

  /* Configure the UART1 TX pin */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pins = AT32_PRINT_UARTTX_PIN; 
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
#if !defined (AT32F421xx)
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
#else
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OutType = GPIO_OutType_PP;
  GPIO_InitStructure.GPIO_Pull = GPIO_Pull_NOPULL;
#endif
  GPIO_Init(AT32_PRINT_UARTTX_GPIO, &GPIO_InitStructure);

  /* Configure the UART1 RX pin */
  GPIO_InitStructure.GPIO_Pins = AT32_PRINT_UARTRX_PIN;//PA10
#if !defined (AT32F421xx)
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PD;
#else
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pull = GPIO_Pull_NOPULL;
#endif
  GPIO_Init(AT32_PRINT_UARTRX_GPIO, &GPIO_InitStructure);

#if defined (AT32F421xx)
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource9, GPIO_AF_1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinsSource10, GPIO_AF_1);
#endif

  /*Configure UART param*/
  USART_StructInit(&USART_InitStructure);
  USART_InitStructure.USART_BaudRate = bound;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	

  USART_Init(AT32_PRINT_UART, &USART_InitStructure); 
  USART_INTConfig(AT32_PRINT_UART, USART_INT_RDNE, ENABLE);
  USART_Cmd(AT32_PRINT_UART, ENABLE);   
}

/**
  * @brief  initialize Delay function   
  * @param  None
  * @retval None
  */		   
void Delay_init()
{
  /*Config Systick*/
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
  fac_us = SystemCoreClock / (1000000U);
  fac_ms = fac_us * (1000U);
}

/**
  * @brief  Inserts a delay time.
  * @param  nus: specifies the delay time length, in microsecond.
  * @retval None
  */
void Delay_us(u32 nus)
{
  u32 temp;
  SysTick->LOAD = (u32)(nus*fac_us);
  SysTick->VAL = 0x00;
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk ;
  do
  {
    temp = SysTick->CTRL;
  }while((temp & 0x01) &&! (temp & (1<<16)));

  SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
  SysTick->VAL = 0X00;
}

/**
  * @brief  Inserts a delay time.
  * @param  nms: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay_ms(u16 nms)
{
  u32 temp;
  while(nms)
  {
    if(nms > STEP_DELAY_MS)
    {
      SysTick->LOAD = (u32)(STEP_DELAY_MS * fac_ms);
      nms -= STEP_DELAY_MS;
    }
    else
    {
      SysTick->LOAD = (u32)(nms * fac_ms);
      nms = 0;
    }
    SysTick->VAL = 0x00;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    do
    {
      temp = SysTick->CTRL;
    }while( (temp & 0x01) && !(temp & (1<<16)) );

    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = 0X00;
  }
}

/**
  * @brief  Inserts a delay time.
  * @param  sec: specifies the delay time length, in seconds.
  * @retval None
  */
void Delay_sec(u16 sec)
{
  u16 i;
  for(i=0; i<sec; i++)
  {
    Delay_ms(500);
    Delay_ms(500);
  }
}
	





