/**
  **************************************************************************
  * File   : CAN/CAN_Normal/main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : Main program body
  **************************************************************************
  * @brief     : Initializes and turn on LED.Configure CAN in Normal mode.Then transmit and receive data.
  *              AT32_F413 contain only CAN1.
  *              SYSCLK = 144M,  APB1CLK = 72M, CAN Baudrate = 1000K
  *              User must use define AT32_USE_CAN1 to choose CAN1
  *              User must use define CAN1_REMAP10,CAN1_REMAP11 to choose which Pins be used
  *              Use USBCAN connect with user board,and then data will be received as loop
  *              Use USBCAN transmit data,user board will received.And when received data SID=0x400,LED2(CAN1 used)/LED3(CAN2 used) Toggle 
  *              User also can use LED4 Toggle to affirm system being running normaly
  */
 
 /* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "at32f4xx.h"
#include "at32_board.h"

/** @addtogroup AT32F415_StdPeriph_Examples
  * @{
  */

/** @addtogroup CAN_Normal
  * @{
  */ 
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/** The following one of the define AT32_USE_CAN1 must be choose*/
#define AT32_USE_CAN1      ///<CAN1 selected to be use

#ifdef AT32_USE_CAN1

	/** The following one of the define CAN1_NOREMAP and CAN1_REMAP10 must be choose*/
	#define CAN1_NOREMAP  ///<CAN1_REMAP10 mean CAN1 use pins PB8 and PB9;CAN1_NOREMAP mean CAN1 use pins PA11 and PA12
	
	#define CANx        CAN1 
	#define CANx_CLK_ENABLE()              	RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_CAN1, ENABLE);
	#ifdef CAN1_REMAP10
		#define CANx_TX    GPIO_Pins_9
		#define CANx_RX    GPIO_Pins_8
		#define CANx_TX_GPIO_CLK_ENABLE()       RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB, ENABLE);
		#define CANx_RX_GPIO_CLK_ENABLE()       RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB, ENABLE);
		#define CANx_TX_GPIO_PORT              GPIOB
		#define CANx_RX_GPIO_PORT              GPIOB
	#else
		#define CANx_TX    GPIO_Pins_12
		#define CANx_RX    GPIO_Pins_11
		#define CANx_TX_GPIO_CLK_ENABLE()       RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA, ENABLE);
		#define CANx_RX_GPIO_CLK_ENABLE()     	 RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA, ENABLE);
		#define CANx_TX_GPIO_PORT              GPIOA
		#define CANx_RX_GPIO_PORT              GPIOA
	#endif
 #endif

/* Private macro -------------------------------------------------------------*/
/*Extended identifier*/
#define FILTER_EXT_ID1    0x18F5f100
#define FILTER_EXT_ID2    0x18F5F200

/*Standard identifier*/
#define FILTER_STD_ID1    0x04F6
#define FILTER_STD_ID2    0x04F7
/* Private variables ---------------------------------------------------------*/
GPIO_InitType GPIO_InitStructure;
RCC_ClockType RccClkSource = {0};
CAN_InitType        	CAN_InitStructure;
CAN_FilterInitType  	CAN_FilterInitStructure = {0};
NVIC_InitType  NVIC_InitStructure = {0};
CanRxMsg RxMessage = {0};
CanTxMsg TxMessage = {0};

/* Private function prototypes -----------------------------------------------*/
static void CAN_GPIO_Configuration(void);
static void CAN_Configuration(void);
static void CAN_transmit_data(void);
static void CAN_NVIC_Config(void);

/* Private functions ---------------------------------------------------------*/

/**
 *	@brief  main function
 *	@param  None
 *	@retval None
 */
int main(void)
{
	GPIO_StructInit(&GPIO_InitStructure);
	CAN_StructInit(&CAN_InitStructure);
	AT32_Board_Init();
	AT32_LEDn_ON(LED2);
	AT32_LEDn_ON(LED3);
	AT32_LEDn_ON(LED4);
   
	CAN_GPIO_Configuration();
	CAN_Configuration();
	CAN_NVIC_Config();
	for(;;)
	{
		CAN_transmit_data();
		Delay_sec(1);
		AT32_LEDn_Toggle(LED4);
		Delay_sec(1);
	}
}

/**
 *	@brief  CAN GPIO Configiguration.Contain Mode,Speed and so on
 *	@param  None
 *	@retval None
 */
static void CAN_GPIO_Configuration(void)
{
	#ifdef AT32_USE_CAN1
		#ifdef CAN1_REMAP10
			RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO, ENABLE);
			GPIO_PinsRemapConfig(GPIO_Remap1_CAN1,ENABLE);
		#endif
	#endif

	CANx_TX_GPIO_CLK_ENABLE(); 
	CANx_RX_GPIO_CLK_ENABLE(); 
	GPIO_InitStructure.GPIO_Pins = CANx_TX;
	GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
	GPIO_Init(CANx_TX_GPIO_PORT, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pins = CANx_RX;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU;	
	GPIO_Init(CANx_RX_GPIO_PORT, &GPIO_InitStructure);
	CANx_CLK_ENABLE();  
}

/**
 *	@brief  CAN Configiguration.Contain Mode,Baudrate,Filter and so on
 *	@param  None
 *	@retval None
 */
static void CAN_Configuration(void)
{  
    CANx_CLK_ENABLE();
    
	CAN_InitStructure.CAN_TTC = DISABLE;			
	CAN_InitStructure.CAN_ABO = ENABLE;			
	CAN_InitStructure.CAN_AWU = ENABLE;			
	CAN_InitStructure.CAN_NART = ENABLE;			
	CAN_InitStructure.CAN_RFL = DISABLE;		 	
	CAN_InitStructure.CAN_TFP = DISABLE;			
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;				
	CAN_InitStructure.CAN_BS1 = CAN_BS1_8tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;	
	CAN_InitStructure.CAN_Prescaler = 6;
	CAN_Init(CANx, &CAN_InitStructure);    ///<SYSCLK=144M,  APB1CLK=72M, CAN Baudrate=£¨144/2£©*1000K£©/£¨£¨1+5£©*£¨3+7+2£©£© = 1000K
   
  /*Config Filter Extended identifier*/
  CAN_FilterInitStructure.CAN_FilterNumber=0;	
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdList; 	
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; 	
	CAN_FilterInitStructure.CAN_FilterIdHigh=(((uint32_t)FILTER_EXT_ID1 << 3) >> 16) & 0xFFFF;	/*Extended identifier is 29 bit*/
	CAN_FilterInitStructure.CAN_FilterIdLow=(((uint32_t)FILTER_EXT_ID1 << 3) & 0xFFFF) | CAN_Id_Extended;
	CAN_FilterInitStructure.CAN_FilterMskIdHigh=((uint32_t)FILTER_EXT_ID2 << 3) >> 16;
	CAN_FilterInitStructure.CAN_FilterMskIdLow=(((uint32_t)FILTER_EXT_ID2 << 3) & 0xFFFF) | CAN_Id_Extended;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
	CAN_FilterInit(CANx, &CAN_FilterInitStructure);
	
  
  /*Config Filter Standard identifier*/
  CAN_FilterInitStructure.CAN_FilterNumber=1;	
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdList; 	
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; 	
	CAN_FilterInitStructure.CAN_FilterIdHigh=FILTER_STD_ID1 << 5;	/*Standard identifier is 11 bit*/
	CAN_FilterInitStructure.CAN_FilterIdLow=0x00;
	CAN_FilterInitStructure.CAN_FilterMskIdHigh=(FILTER_STD_ID2 << 5);
	CAN_FilterInitStructure.CAN_FilterMskIdLow=0x00;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
	CAN_FilterInit(CANx, &CAN_FilterInitStructure);
	
  CAN_INTConfig(CANx,CAN_INT_RFP0,ENABLE);
}

/**
 *	@brief  CAN transmit data.SID=0x400,Standard identifier,Data frame,data D0~D7 all 0xA5 
 *	@param  None
 *	@retval None
 */
static void CAN_transmit_data(void)
{
    uint8_t TransmitMailbox;
    uint32_t i = 0;
    TxMessage.Data[0] = 0xA7;
    TxMessage.Data[1] = 0xA7;
    TxMessage.Data[2] = 0xA7;
    TxMessage.Data[3] = 0xA7;
    TxMessage.Data[4] = 0xA7;
    TxMessage.Data[5] = 0xA5;
    TxMessage.Data[6] = 0xA5;
    TxMessage.Data[7] = 0xA5;
    
	TxMessage.StdId = 0x400;			
	TxMessage.ExtId = 0;			
	TxMessage.IDT = CAN_Id_Standard;
	TxMessage.RTR = CAN_RTR_Data;		
	TxMessage.DLC = 8;
    TransmitMailbox = CAN_Transmit(CANx, &TxMessage);
    while((CAN_TransmitStatus(CANx, TransmitMailbox) != CANTXOK) && (i != 0xFFFF))
    {
        i++;
    }
}

/**
 *	@brief  CAN NVIC Configiguration. Enable CAN receive NVIC,and Setting Priority
 *	@param  None
 *	@retval None
 */
static void CAN_NVIC_Config(void)
{
	#ifdef AT32_USE_CAN1
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
		NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	#endif
}

/**
 *	@brief  CAN1 Interrupt function RX0. Contain RFOV,RFFU and RFP,Wen received data SID=0x400,LED2 Toggle
 *	@param  None
 *	@retval None
 */
void CAN1_RX0_IRQHandler(void)
{
    if (CAN_GetINTStatus (CANx,CAN_INT_RFOV0) == SET ) 
    {
        CAN_ClearINTPendingBit (CANx, CAN_INT_RFOV0);
    }
    if (CAN_GetINTStatus (CANx,CAN_INT_RFFU0) == SET ) 
    {
        CAN_ClearINTPendingBit (CANx, CAN_INT_RFFU0);
    }
    if (CAN_GetINTStatus (CANx,CAN_INT_RFP0) == SET )
    {
      CAN_Receive(CANx, CAN_FIFO0, &RxMessage);
      /*Standard*/
      if(RxMessage.IDT == CAN_Id_Standard)
      {
        if ( RxMessage.StdId == FILTER_STD_ID1 )
            AT32_LEDn_Toggle(LED2);
        if ( RxMessage.StdId == FILTER_STD_ID2 )
            AT32_LEDn_Toggle(LED3);			
      }

      /*Extended*/
      if(RxMessage.IDT == CAN_Id_Extended)
      {
        if ( RxMessage.ExtId == FILTER_EXT_ID1 )
            AT32_LEDn_Toggle(LED2);
        if ( RxMessage.ExtId == FILTER_EXT_ID2 )
            AT32_LEDn_Toggle(LED3);			
      }

    }
}

/**
  * @}
  */ 

/**
  * @}
  */ 

