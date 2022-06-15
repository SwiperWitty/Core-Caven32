/**
  ******************************************************************************
  * File   : usb_bsp.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file implements the board support package for the USB host library
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/

#include "usb_bsp.h"
#include "usb_conf.h"
#include "usb_core.h"
/** @addtogroup USBH_USER
* @{
*/

/** @defgroup USB_BSP
  * @brief This file is responsible to offer board support package
  * @{
  */ 

/** @defgroup USB_BSP_Private_Defines
  * @{
  */ 
//#define USE_ACCURATE_TIME
#define TIM_MSEC_DELAY                     0x01
#define TIM_USEC_DELAY                     0x02

#define HOST_POWERSW_PORT_RCC             RCC_APB2PERIPH_GPIOC
#define HOST_POWERSW_PORT                 GPIOC
#define HOST_POWERSW_VBUS                 GPIO_Pins_9

#define HOST_SOF_OUTPUT_RCC                RCC_APB2PERIPH_GPIOA
#define HOST_SOF_PORT                      GPIOA
#define HOST_SOF_SIGNAL                    GPIO_Pins_8

/**
  * @}
  */ 


/** @defgroup USB_BSP_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 



/** @defgroup USB_BSP_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBH_BSP_Private_Variables
  * @{
  */ 
//ErrorStatus HSEStartUpStatus;
#ifdef USE_ACCURATE_TIME 
__IO uint32_t BSP_delay = 0;
#endif
/**
  * @}
  */ 

/** @defgroup USBH_BSP_Private_FunctionPrototypes
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup USB_BSP_Private_Functions
  * @{
  */ 
static void USB_OTG_BSP_TimeInit ( void );
/**
  * @brief  USB_OTG_BSP_Init
  *         Initializes BSP configurations
  * @param  None
  * @retval None
  */

void USB_OTG_BSP_Init(USB_OTG_CORE_HANDLE *pusbdev)
{

//  GPIO_InitType GPIO_InitStructure;
//  
//  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA, ENABLE);
//  
//  GPIO_StructInit(&GPIO_InitStructure);
//  GPIO_InitStructure.GPIO_Pins  =  GPIO_Pins_12 | GPIO_Pins_11;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
//  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_2MHz;
//  GPIO_Init(USB_GPIO, &GPIO_InitStructure);
//  
//  while ( 1 )
//  {
//    GPIO_SetBits(GPIOA, GPIO_Pins_12 | GPIO_Pins_11);
//    Delay_ms(10);
//    GPIO_ResetBits(GPIOA,  GPIO_Pins_12 | GPIO_Pins_11);
////    Delay_ms(10);
//  }
//  RCC_CLKOUTConfig(RCC_CLKOUT_USB, RCC_MCOPRE_1);
//  RCC_CLKOUTConfig(RCC_CLKOUT_SYSCLK, RCC_MCOPRE_1);
  USB_OTG_BSP_TimeInit();
    
    
}
/**
  * @brief  USB_OTG_BSP_EnableInterrupt
  *         Configures USB Global interrupt
  * @param  None
  * @retval None
  */
void USB_OTG_BSP_EnableInterrupt(USB_OTG_CORE_HANDLE *pusbdev)
{
#ifdef USB_OTG_FS_LOW_PWR_MGMT_SUPPORT  
  EXTI_InitType EXTI_InitStructure;
#endif
  NVIC_InitType NVIC_InitStructure; 
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NVIC_InitStructure.NVIC_IRQChannel = USBOTG_IRQn;  
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  
  
#ifdef USB_OTG_FS_LOW_PWR_MGMT_SUPPORT  
  EXTI_ClearIntPendingBit(EXTI_Line18);

  EXTI_InitStructure.EXTI_Line    = EXTI_Line18; 
  EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineEnable = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  EXTI_ClearIntPendingBit(EXTI_Line18);   

  NVIC_InitStructure.NVIC_IRQChannel                      = USBWakeUp_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority           = 5;
  NVIC_InitStructure.NVIC_IRQChannelCmd                   = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  EXTI_ClearIntPendingBit(EXTI_Line18);

#endif
}

/**
  * @brief  BSP_Drive_VBUS
  *         Drives the Vbus signal through IO
  * @param  state : VBUS states
  * @retval None
  */

void USB_OTG_BSP_DriveVBUS(USB_OTG_CORE_HANDLE *pusbdev, uint8_t state)
{
  /*
  On-chip 5 V VBUS generation is not supported. For this reason, a charge pump 
  or, if 5 V are available on the application board, a basic power switch, must 
  be added externally to drive the 5 V VBUS line. The external charge pump can 
  be driven by any GPIO output. When the application decides to power on VBUS 
  using the chosen GPIO, it must also set the port power bit in the host port 
  control and status register (PPWR bit in OTG_FS_HPRT).
  
  Bit 12 PPWR: Port power
  The application uses this field to control power to this port, and the core 
  clears this bit on an overcurrent condition.
  */
  
  if (0 == state)
  { 
    /* DISABLE is needed on output of the Power Switch */
    GPIO_SetBits(HOST_POWERSW_PORT, HOST_POWERSW_VBUS);
  }
  else
  {
    /*ENABLE the Power Switch by driving the Enable LOW */
    GPIO_ResetBits(HOST_POWERSW_PORT, HOST_POWERSW_VBUS);
  }

}

/**
  * @brief  USB_OTG_BSP_ConfigVBUS
  *         Configures the IO for the Vbus and OverCurrent
  * @param  None
  * @retval None
  */

void  USB_OTG_BSP_ConfigVBUS(USB_OTG_CORE_HANDLE *pusbdev)
{ 
}



/**
  * @brief  USB_OTG_BSP_TimeInit
  *         Initializes delay unit using Timer2
  * @param  None
  * @retval None
  */
static void USB_OTG_BSP_TimeInit ( void )
{
#ifdef USE_ACCURATE_TIME   
  NVIC_InitType NVIC_InitStructure;
  
  /* Set the Vector Table base address at 0x08000000 */
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x00);
  
  /* Configure the Priority Group to 2 bits */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* Enable the TIM2 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TMR2_GLOBAL_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  
  NVIC_Init(&NVIC_InitStructure);
  
  RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR2, ENABLE);  
#endif  
}

/**
  * @brief  USB_OTG_BSP_TimerIRQ
  *         Time base IRQ
  * @param  None
  * @retval None
  */

void USB_OTG_BSP_TimerIRQ (void)
{
#ifdef USE_ACCURATE_TIME 
    
  if (TMR_GetINTStatus(TMR2, TMR_INT_Overflow) != RESET)
  {
    TMR_ClearITPendingBit(TMR2, TMR_INT_Overflow);
    if (BSP_delay > 0x00)
    { 
      BSP_delay--;
    }
    else
    {
      TMR_Cmd(TMR2,DISABLE);
    }
  }
#endif  
} 
#ifdef USE_ACCURATE_TIME
/**
  * @brief  BSP_SetTime
  *         Configures TIM2 for delay routine based on TIM2
  * @param  unit : msec /usec
  * @retval None
  */
static void BSP_SetTime(uint8_t unit)
{
  TMR_TimerBaseInitType  TMR_TimeBaseStructure;
  
  TMR_Cmd(TMR2,DISABLE);
  TMR_INTConfig(TMR2, TMR_INT_Overflow, DISABLE); 
  
  
  if(unit == TIM_USEC_DELAY)
  {  
    TMR_TimeBaseStructure.TMR_Period = 25;
  }
  else if(unit == TIM_MSEC_DELAY)
  {
    TMR_TimeBaseStructure.TMR_Period = 25999;
  }
  TMR_TimeBaseStructure.TMR_DIV = 5;
  TMR_TimeBaseStructure.TMR_ClockDivision = 0;
  TMR_TimeBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;
  
  TMR_TimeBaseInit(TMR2, &TMR_TimeBaseStructure);
  TMR_ClearITPendingBit(TMR2, TMR_INT_Overflow);
  
  TMR_ARPreloadConfig(TMR2, ENABLE);
  
  /* TIM IT enable */
  TMR_INTConfig(TMR2, TMR_INT_Overflow, ENABLE);
  
  /* TIM2 enable counter */ 
  TMR_Cmd(TMR2, ENABLE);  
} 
#endif

#ifdef USE_ACCURATE_TIME
/**
  * @brief  BSP_Delay
  *         Delay routine based on TIM2
  * @param  nTime : Delay Time 
  * @param  unit : Delay Time unit : mili sec / micro sec
  * @retval None
  */
static void BSP_Delay(uint32_t nTime, uint8_t unit)
{
#ifdef USE_ACCURATE_TIME 
  BSP_delay = nTime;
  BSP_SetTime(unit);  
  while(BSP_delay != 0);
  TMR_Cmd(TMR2,DISABLE);
#endif
}

#endif
/**
  * @brief  USB_OTG_BSP_mDelay
  *          This function provides delay time in milli sec
  * @param  msec : Value of delay required in milli sec
  * @retval None
  */
void USB_OTG_BSP_mDelay (const uint32_t msec)
{ 
#ifdef USE_ACCURATE_TIME
  BSP_Delay(msec,TIM_MSEC_DELAY); 
#else
  Delay_ms(msec);  
#endif  
}

/**
  * @brief  USB_OTG_BSP_uDelay
  *         This function provides delay time in micro sec
  * @param  usec : Value of delay required in micro sec
  * @retval None
  */
void USB_OTG_BSP_uDelay (const uint32_t usec)
{
#ifdef USE_ACCURATE_TIME
  BSP_Delay(usec,TIM_USEC_DELAY);  
#else
  Delay_us(usec);
#endif  
}
 
/**
  * @brief  TMR2_IRQHandler
  *         This function handles Timer2 Handler.
  * @param  None
  * @retval None
  */
void TMR2_GLOBAL_IRQHandler(void)
{
  USB_OTG_BSP_TimerIRQ();
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

