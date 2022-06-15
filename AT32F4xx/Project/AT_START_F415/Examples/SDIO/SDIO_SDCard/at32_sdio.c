/**
  ******************************************************************************
  * File   : SDIO/SDIO_SDCard/at32_sdio.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : This file provides a set of functions needed to manage the
                SDIO/MMC Card memory.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "at32_sdio.h"
#include "string.h"

/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup SDIO_SDCard
  * @{
  */ 
  
/** @addtogroup SDIO_SD_Private_Variables
  * @{
  */
SDIO_InitType SDIO_InitStructure;
SDIO_CmdInitType SDIO_CmdInitStructure;
SDIO_DataInitType SDIO_DataInitStructure;

static u8 CardType = SDIO_STD_CAPACITY_SD_CARD_V1_1; ///< SD card type
static u32 CSD_Tab[4], CID_Tab[4], RCA = 0; ///< CSD, SID, RCA
static u8 DeviceMode = SD_POLLING_MODE; ///< working mode
static u8 StopCondition = 0; ///< transmit stop flag
volatile SD_Error TransferError = SD_OK; ///< transmit error flag
volatile u8 TransferEnd = 0; ///< transmit end flag
SD_CardInfo SDCardInfo; ///< SD card information

/**
  * @}
  */


/** @addtogroup SDIO_SD_Private_Function_Prototypes
  * @{
  */
SD_Error CmdError(void);
SD_Error CmdResp7Error(void);
SD_Error CmdResp1Error(u8 cmd);
SD_Error CmdResp3Error(void);
SD_Error CmdResp2Error(void);
SD_Error CmdResp4Error(u8 cmd);
SD_Error CmdResp5Error(u8 cmd);
SD_Error CmdResp6Error(u8 cmd, u16*prca);
SD_Error SDEnWideBus(u8 enx);
SD_Error MMC_Switch(u8 set, u8 index, u8 value);
SD_Error SD_Switch(u32 mode, u32 group, u8 value, u8 *resp);
SD_Error IsCardProgramming(u8 *pstatus);
SD_Error ChangeSpeed(u8 speed);
SD_Error FindSCR(void);
u8 convert_from_bytes_to_power_of_two(u16 NumberOfBytes);

/**
  * @brief  Initializes the SD Card and put it into StandBy State (Ready for data
  *         transfer).
  * @param  None
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_Init(void)
{
  GPIO_InitType  GPIO_InitStructure = {0};
  GPIO_StructInit(&GPIO_InitStructure);

  u16 clkdiv = 0;
  SD_Error errorstatus = SD_OK;

  /*!< GPIOC and GPIOD Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOC | RCC_APB2PERIPH_GPIOD, ENABLE);

  /*!< SDIO clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_SDIO1, ENABLE);

  /*!< Configure PC.08, PC.09, PC.10, PC.11, PC.12 pin: D0, D1, D2, D3, CLK pin */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_8 | GPIO_Pins_9 | GPIO_Pins_10 | GPIO_Pins_11 | GPIO_Pins_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /*!< Configure PD.02 CMD line */
  GPIO_InitStructure.GPIO_Pins = GPIO_Pins_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* Reset SDIO */
  SDIO_Reset(SDIO);

  errorstatus = SD_PowerON();   //上电

  if(errorstatus == SD_OK)
  {
    errorstatus = SD_InitializeCards();  //初始化SD卡
  }

  if(errorstatus == SD_OK)
  {
    errorstatus = SD_GetCardInfo(&SDCardInfo);  //获取SD卡信息
  }

  if(SDIO_MULTIMEDIA_CARD == CardType && SDCardInfo.SD_csd.SysSpecVersion >= 4)
  {
    CardType = SDIO_HIGH_SPEED_MULTIMEDIA_CARD;
  }

  if(errorstatus == SD_OK)
  {
    errorstatus = SD_SelectDeselect((u32)(SDCardInfo.RCA << 16));  //选中SD卡
  }

  if(errorstatus == SD_OK && ((SDIO_STD_CAPACITY_SD_CARD_V1_1 == CardType) || (SDIO_STD_CAPACITY_SD_CARD_V2_0 == CardType) || (SDIO_SECURE_DIGITAL_IO_COMBO_CARD == CardType) || (SDIO_HIGH_CAPACITY_SD_CARD == CardType)))
  {
    errorstatus = FindSCR();
  }

  if(errorstatus == SD_OK)
  {
    errorstatus = ChangeSpeed(0);  //Set to Normal Speed
  }

  if((errorstatus == SD_OK) || (SDIO_MULTIMEDIA_CARD == CardType))
  {
    if(SDCardInfo.CardType == SDIO_STD_CAPACITY_SD_CARD_V1_1 || SDCardInfo.CardType == SDIO_STD_CAPACITY_SD_CARD_V2_0)
    {
      clkdiv = SystemCoreClock / 6000000; //设置SD卡时钟为6MHz

      if (clkdiv > 2)
      {
        clkdiv -= 2;
      }
    }
    else
    {
      clkdiv = SystemCoreClock / 6000000; //设置SD卡时钟为6MHz

      if (clkdiv > 2)
      {
        clkdiv -= 2;
      }
    }

    SDIO_Clock_Set(clkdiv); //设置SDIO Clock分频系数
    errorstatus = SD_SetDeviceMode(SD_POLLING_MODE); //设置为轮询模式
  }

  return errorstatus;
}

/**
  * @brief  Set SDIO Clock Devision Factor.
  * @param  clkdiv: SDIO_CK = AHBCLK / (clkdiv+2)
  * @retval None
  */
void SDIO_Clock_Set(u32 clkdiv)
{
  u32 tmpreg = SDIO->CLKCTRL;
  tmpreg &= 0XFFFE7F00;
  tmpreg |= (clkdiv & 0X000000FF);
  tmpreg |= ((clkdiv & 0X00000300) << 7);
  SDIO->CLKCTRL = tmpreg;
}

/**
  * @brief  Enquires cards about their operating voltage and configures
  *         clock controls.
  * @param  None
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_PowerON(void)
{
  u8 i = 0;
  SD_Error errorstatus = SD_OK;
  u32 response = 0, count = 0, validvoltage = 0, clk_div;
  u32 SDType = SD_STD_CAPACITY;

  /*初始化时的时钟不能大于400KHz*/
  clk_div = SystemCoreClock / 200000 - 2; //初始化时钟200KHz

  if (clk_div > 0x3FF) //9 bits
  {
    clk_div = 0X3FF;
  }

  SDIO_InitStructure.SDIO_ClkPsc = clk_div;
  SDIO_InitStructure.SDIO_ClkEdge = SDIO_ClkEdge_Falling; //上升沿
  SDIO_InitStructure.SDIO_ClkBypass = SDIO_ClkBypass_Disable;  //关闭时钟旁路
  SDIO_InitStructure.SDIO_ClkPowerSave = SDIO_ClkPowerSave_Disable; //空闲时输出时钟
  SDIO_InitStructure.SDIO_BusWidth = SDIO_BusWidth_1b; //1-bit数据宽度
  SDIO_InitStructure.SDIO_FlowCtrl = SDIO_FlowCtrl_Disable; //关闭硬件流控
  SDIO_Init(SDIO, &SDIO_InitStructure);

  SDIO_SetPowerSaveState(SDIO, SDIO_PowerSave_ON); //上电状态,开启卡时钟

  SDIO_ClockCmd(SDIO, ENABLE); //SDIOCK使能

  for(i = 0; i < 10; i++)
  {
    SDIO_CmdInitStructure.SDIO_Argu = 0x0; //发送CMD0进入IDLE STAGE模式命令.
    SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_GO_IDLE_STATE; //CMD0
    SDIO_CmdInitStructure.SDIO_Resp = SDIO_Rsp_No; //无响应
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable; //则CPSM在开始发送命令之前等待数据传输结束。
    SDIO_SendCommand(SDIO, &SDIO_CmdInitStructure); //写命令进命令寄存器

    errorstatus = CmdError();

    if(errorstatus == SD_OK)
    {
      break;
    }
  }

  if(errorstatus)
  {
    return errorstatus;  //返回错误状态
  }

  SDIO_CmdInitStructure.SDIO_Argu = SD_CHECK_PATTERN;   //发送CMD8,短响应,检查SD卡接口特性
  SDIO_CmdInitStructure.SDIO_CmdIdx = SDIO_SEND_IF_COND;    //CMD8
  SDIO_CmdInitStructure.SDIO_Resp = SDIO_Rsp_Short;  //R7
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;   //关闭等待中断
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(SDIO, &SDIO_CmdInitStructure);

  errorstatus = CmdResp7Error(); //等待R7响应

  if(errorstatus == SD_OK) //R7响应正常
  {
    CardType = SDIO_STD_CAPACITY_SD_CARD_V2_0; //SD 2.0卡
    SDType = SD_HIGH_CAPACITY; //高容量卡
  }

  SDIO_CmdInitStructure.SDIO_Argu = 0x00; //发送CMD55,短响应
  SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_APP_CMD;
  SDIO_CmdInitStructure.SDIO_Resp = SDIO_Rsp_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(SDIO, &SDIO_CmdInitStructure);   //发送CMD55,短响应

  errorstatus = CmdResp1Error(SD_CMD_APP_CMD); //等待R1响应

  if(errorstatus == SD_OK) //SD2.0/SD 1.1,否则为MMC卡
  {
    /* SD卡,发送ACMD41 SD_APP_OP_COND,参数为:0x80100000 */
    while((!validvoltage) && (count < SD_MAX_VOLT_TRIAL))
    {
      SDIO_CmdInitStructure.SDIO_Argu = 0x00; //发送CMD55,短响应
      SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_APP_CMD;   //CMD55
      SDIO_CmdInitStructure.SDIO_Resp = SDIO_Rsp_Short;
      SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
      SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
      SDIO_SendCommand(SDIO, &SDIO_CmdInitStructure);   //发送CMD55,短响应

      errorstatus = CmdResp1Error(SD_CMD_APP_CMD); //等待R1响应

      if(errorstatus != SD_OK) //响应错误
      {
        return errorstatus;
      }

      /* acmd41，命令参数由支持的电压范围及HCS位组成，HCS位置一来区分卡是SDSc还是sdhc */
      SDIO_CmdInitStructure.SDIO_Argu = SD_VOLTAGE_WINDOW_SD | SDType; //发送ACMD41,短响应
      SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_SD_APP_OP_COND;
      SDIO_CmdInitStructure.SDIO_Resp = SDIO_Rsp_Short;  //r3
      SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
      SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
      SDIO_SendCommand(SDIO, &SDIO_CmdInitStructure);

      errorstatus = CmdResp3Error(); //等待R3响应

      if(errorstatus != SD_OK) //响应错误
      {
        return errorstatus;
      }

      response = SDIO->RSP1; //得到响应
      validvoltage = (((response >> 31) == 1) ? 1 : 0); //判断SD卡上电是否完成
      count++;
    }

    if(count >= SD_MAX_VOLT_TRIAL)
    {
      errorstatus = SD_INVALID_VOLTRANGE;
      return errorstatus;
    }

    if(response &= SD_HIGH_CAPACITY)
    {
      CardType = SDIO_HIGH_CAPACITY_SD_CARD;
    }
  }
  else //MMC卡
  {
    /* MMC卡,发送CMD1 SDIO_SEND_OP_COND,参数为:0x80FF8000 */
    while((!validvoltage) && (count < SD_MAX_VOLT_TRIAL))
    {
      SDIO_CmdInitStructure.SDIO_Argu = SD_VOLTAGE_WINDOW_MMC; //发送CMD1,短响应
      SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_SEND_OP_COND;
      SDIO_CmdInitStructure.SDIO_Resp = SDIO_Rsp_Short; //r3
      SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
      SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
      SDIO_SendCommand(SDIO, &SDIO_CmdInitStructure);

      errorstatus = CmdResp3Error(); //等待R3响应

      if(errorstatus != SD_OK) //响应错误
      {
        return errorstatus;
      }

      response = SDIO->RSP1; //得到响应
      validvoltage = (((response >> 31) == 1) ? 1 : 0);
      count++;
    }

    if(count >= SD_MAX_VOLT_TRIAL)
    {
      errorstatus = SD_INVALID_VOLTRANGE;
      return errorstatus;
    }

    CardType = SDIO_MULTIMEDIA_CARD;
  }

  return(errorstatus);
}

/**
  * @brief  Turns the SDIO output signals off.
  * @param  None
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_PowerOFF(void)
{
  SDIO_SetPowerSaveState(SDIO, SDIO_PowerSave_OFF); //SDIO电源关闭,时钟停止
  return SD_OK;
}

/**
  * @brief  Intialises all cards or single card as the case may be Card(s) come
  *         into standby state.
  * @param  None
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_InitializeCards(void)
{
  SD_Error errorstatus = SD_OK;
  u16 rca = 0x01;

  if(SDIO_GetPowerSaveState(SDIO) == 0) //检查电源状态,确保为上电状态
  {
    return SD_REQ_NOT_APPLICABLE;
  }

  if(SDIO_SECURE_DIGITAL_IO_CARD != CardType)   //非SECURE_DIGITAL_IO_CARD
  {
    SDIO_CmdInitStructure.SDIO_Argu = 0x0; //发送CMD2,取得CID,长响应
    SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_ALL_SEND_CID;
    SDIO_CmdInitStructure.SDIO_Resp = SDIO_Rsp_Long;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(SDIO, &SDIO_CmdInitStructure); //发送CMD2,取得CID,长响应

    errorstatus = CmdResp2Error(); //等待R2响应
    CID_Tab[0] = SDIO->RSP1;
    CID_Tab[1] = SDIO->RSP2;
    CID_Tab[2] = SDIO->RSP3;
    CID_Tab[3] = SDIO->RSP4;
  }

  if((SDIO_STD_CAPACITY_SD_CARD_V1_1 == CardType) || (SDIO_STD_CAPACITY_SD_CARD_V2_0 == CardType) || \
      (SDIO_SECURE_DIGITAL_IO_COMBO_CARD == CardType) || (SDIO_HIGH_CAPACITY_SD_CARD == CardType))          //判断卡类型
  {
    SDIO_CmdInitStructure.SDIO_Argu = 0x00; //发送CMD3,短响应
    SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_SET_REL_ADDR; //CMD3
    SDIO_CmdInitStructure.SDIO_Resp = SDIO_Rsp_Short; //r6
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(SDIO, &SDIO_CmdInitStructure); //发送CMD3,短响应

    errorstatus = CmdResp6Error(SD_CMD_SET_REL_ADDR, &rca); //等待R6响应

    if(errorstatus != SD_OK) //响应错误
    {
      return errorstatus;
    }
  }

  if (SDIO_MULTIMEDIA_CARD == CardType)
  {
    SDIO_CmdInitStructure.SDIO_Argu = (u32)(rca << 16); //发送CMD3,短响应
    SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_SET_REL_ADDR;//CMD3
    SDIO_CmdInitStructure.SDIO_Resp = SDIO_Rsp_Short; //r6
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(SDIO, &SDIO_CmdInitStructure); //发送CMD3,短响应

    errorstatus = CmdResp2Error(); //等待R2响应

    if(errorstatus != SD_OK) //响应错误
    {
      return errorstatus;
    }
  }

  if (SDIO_SECURE_DIGITAL_IO_CARD != CardType) //非SECURE_DIGITAL_IO_CARD
  {
    RCA = rca;
    SDIO_CmdInitStructure.SDIO_Argu = (uint32_t)(rca << 16); //发送CMD9+卡RCA,取得CSD,长响应
    SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_SEND_CSD;
    SDIO_CmdInitStructure.SDIO_Resp = SDIO_Rsp_Long;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(SDIO, &SDIO_CmdInitStructure);

    errorstatus = CmdResp2Error(); //等待R2响应

    if(errorstatus != SD_OK) //响应错误
    {
      return errorstatus;
    }

    CSD_Tab[0] = SDIO->RSP1;
    CSD_Tab[1] = SDIO->RSP2;
    CSD_Tab[2] = SDIO->RSP3;
    CSD_Tab[3] = SDIO->RSP4;
  }

  return SD_OK;//卡初始化成功
}


/**
  * @brief  Returns information about specific card.
  * @param  cardinfo: pointer to a SD_CardInfo structure that contains all SD card
  *         information.
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_GetCardInfo(SD_CardInfo *cardinfo)
{
  SD_Error errorstatus = SD_OK;
  uint8_t tmp = 0;

  cardinfo->CardType = (uint8_t)CardType;
  cardinfo->RCA = (uint16_t)RCA;

  /*!< Byte 0 */
  tmp = (uint8_t)((CSD_Tab[0] & 0xFF000000) >> 24);
  cardinfo->SD_csd.CSDStruct = (tmp & 0xC0) >> 6;
  cardinfo->SD_csd.SysSpecVersion = (tmp & 0x3C) >> 2;
  cardinfo->SD_csd.Reserved1 = tmp & 0x03;

  /*!< Byte 1 */
  tmp = (uint8_t)((CSD_Tab[0] & 0x00FF0000) >> 16);
  cardinfo->SD_csd.TAAC = tmp;

  /*!< Byte 2 */
  tmp = (uint8_t)((CSD_Tab[0] & 0x0000FF00) >> 8);
  cardinfo->SD_csd.NSAC = tmp;

  /*!< Byte 3 */
  tmp = (uint8_t)(CSD_Tab[0] & 0x000000FF);
  cardinfo->SD_csd.MaxBusClkFrec = tmp;

  /*!< Byte 4 */
  tmp = (uint8_t)((CSD_Tab[1] & 0xFF000000) >> 24);
  cardinfo->SD_csd.CardComdClasses = tmp << 4;

  /*!< Byte 5 */
  tmp = (uint8_t)((CSD_Tab[1] & 0x00FF0000) >> 16);
  cardinfo->SD_csd.CardComdClasses |= (tmp & 0xF0) >> 4;
  cardinfo->SD_csd.RdBlockLen = tmp & 0x0F;

  /*!< Byte 6 */
  tmp = (uint8_t)((CSD_Tab[1] & 0x0000FF00) >> 8);
  cardinfo->SD_csd.PartBlockRead = (tmp & 0x80) >> 7;
  cardinfo->SD_csd.WrBlockMisalign = (tmp & 0x40) >> 6;
  cardinfo->SD_csd.RdBlockMisalign = (tmp & 0x20) >> 5;
  cardinfo->SD_csd.DSRImpl = (tmp & 0x10) >> 4;
  cardinfo->SD_csd.Reserved2 = 0; /*!< Reserved */

  if ((CardType == SDIO_STD_CAPACITY_SD_CARD_V1_1) || (CardType == SDIO_STD_CAPACITY_SD_CARD_V2_0))
  {
    cardinfo->SD_csd.DeviceSize = (tmp & 0x03) << 10;

    /*!< Byte 7 */
    tmp = (uint8_t)(CSD_Tab[1] & 0x000000FF);
    cardinfo->SD_csd.DeviceSize |= (tmp) << 2;

    /*!< Byte 8 */
    tmp = (uint8_t)((CSD_Tab[2] & 0xFF000000) >> 24);
    cardinfo->SD_csd.DeviceSize |= (tmp & 0xC0) >> 6;

    cardinfo->SD_csd.MaxRdCurrentVDDMin = (tmp & 0x38) >> 3;
    cardinfo->SD_csd.MaxRdCurrentVDDMax = (tmp & 0x07);

    /*!< Byte 9 */
    tmp = (uint8_t)((CSD_Tab[2] & 0x00FF0000) >> 16);
    cardinfo->SD_csd.MaxWrCurrentVDDMin = (tmp & 0xE0) >> 5;
    cardinfo->SD_csd.MaxWrCurrentVDDMax = (tmp & 0x1C) >> 2;
    cardinfo->SD_csd.DeviceSizeMul = (tmp & 0x03) << 1;
    /*!< Byte 10 */
    tmp = (uint8_t)((CSD_Tab[2] & 0x0000FF00) >> 8);
    cardinfo->SD_csd.DeviceSizeMul |= (tmp & 0x80) >> 7;

    cardinfo->CardCapacity = (cardinfo->SD_csd.DeviceSize + 1) ;
    cardinfo->CardCapacity *= (1 << (cardinfo->SD_csd.DeviceSizeMul + 2));
    cardinfo->CardBlockSize = 1 << (cardinfo->SD_csd.RdBlockLen);
    cardinfo->CardCapacity *= cardinfo->CardBlockSize;
  }
  else if (CardType == SDIO_HIGH_CAPACITY_SD_CARD)
  {
    /*!< Byte 7 */
    tmp = (uint8_t)(CSD_Tab[1] & 0x000000FF);
    cardinfo->SD_csd.DeviceSize = (tmp & 0x3F) << 16;

    /*!< Byte 8 */
    tmp = (uint8_t)((CSD_Tab[2] & 0xFF000000) >> 24);

    cardinfo->SD_csd.DeviceSize |= (tmp << 8);

    /*!< Byte 9 */
    tmp = (uint8_t)((CSD_Tab[2] & 0x00FF0000) >> 16);

    cardinfo->SD_csd.DeviceSize |= (tmp);

    /*!< Byte 10 */
    tmp = (uint8_t)((CSD_Tab[2] & 0x0000FF00) >> 8);

    cardinfo->CardCapacity = (uint64_t)(cardinfo->SD_csd.DeviceSize + 1) * 512 * 1024;
    cardinfo->CardBlockSize = 512;
  }


  cardinfo->SD_csd.EraseGrSize = (tmp & 0x40) >> 6;
  cardinfo->SD_csd.EraseGrMul = (tmp & 0x3F) << 1;

  /*!< Byte 11 */
  tmp = (uint8_t)(CSD_Tab[2] & 0x000000FF);
  cardinfo->SD_csd.EraseGrMul |= (tmp & 0x80) >> 7;
  cardinfo->SD_csd.WrProtectGrSize = (tmp & 0x7F);

  /*!< Byte 12 */
  tmp = (uint8_t)((CSD_Tab[3] & 0xFF000000) >> 24);
  cardinfo->SD_csd.WrProtectGrEnable = (tmp & 0x80) >> 7;
  cardinfo->SD_csd.ManDeflECC = (tmp & 0x60) >> 5;
  cardinfo->SD_csd.WrSpeedFact = (tmp & 0x1C) >> 2;
  cardinfo->SD_csd.MaxWrBlockLen = (tmp & 0x03) << 2;

  /*!< Byte 13 */
  tmp = (uint8_t)((CSD_Tab[3] & 0x00FF0000) >> 16);
  cardinfo->SD_csd.MaxWrBlockLen |= (tmp & 0xC0) >> 6;
  cardinfo->SD_csd.WriteBlockPaPartial = (tmp & 0x20) >> 5;
  cardinfo->SD_csd.Reserved3 = 0;
  cardinfo->SD_csd.ContentProtectAppli = (tmp & 0x01);

  /*!< Byte 14 */
  tmp = (uint8_t)((CSD_Tab[3] & 0x0000FF00) >> 8);
  cardinfo->SD_csd.FileFormatGrouop = (tmp & 0x80) >> 7;
  cardinfo->SD_csd.CopyFlag = (tmp & 0x40) >> 6;
  cardinfo->SD_csd.PermWrProtect = (tmp & 0x20) >> 5;
  cardinfo->SD_csd.TempWrProtect = (tmp & 0x10) >> 4;
  cardinfo->SD_csd.FileFormat = (tmp & 0x0C) >> 2;
  cardinfo->SD_csd.ECC = (tmp & 0x03);

  /*!< Byte 15 */
  tmp = (uint8_t)(CSD_Tab[3] & 0x000000FF);
  cardinfo->SD_csd.CSD_CRC = (tmp & 0xFE) >> 1;
  cardinfo->SD_csd.Reserved4 = 1;


  /*!< Byte 0 */
  tmp = (uint8_t)((CID_Tab[0] & 0xFF000000) >> 24);
  cardinfo->SD_cid.ManufacturerID = tmp;

  /*!< Byte 1 */
  tmp = (uint8_t)((CID_Tab[0] & 0x00FF0000) >> 16);
  cardinfo->SD_cid.OEM_AppliID = tmp << 8;

  /*!< Byte 2 */
  tmp = (uint8_t)((CID_Tab[0] & 0x000000FF00) >> 8);
  cardinfo->SD_cid.OEM_AppliID |= tmp;

  /*!< Byte 3 */
  tmp = (uint8_t)(CID_Tab[0] & 0x000000FF);
  cardinfo->SD_cid.ProdName1 = tmp << 24;

  /*!< Byte 4 */
  tmp = (uint8_t)((CID_Tab[1] & 0xFF000000) >> 24);
  cardinfo->SD_cid.ProdName1 |= tmp << 16;

  /*!< Byte 5 */
  tmp = (uint8_t)((CID_Tab[1] & 0x00FF0000) >> 16);
  cardinfo->SD_cid.ProdName1 |= tmp << 8;

  /*!< Byte 6 */
  tmp = (uint8_t)((CID_Tab[1] & 0x0000FF00) >> 8);
  cardinfo->SD_cid.ProdName1 |= tmp;

  /*!< Byte 7 */
  tmp = (uint8_t)(CID_Tab[1] & 0x000000FF);
  cardinfo->SD_cid.ProdName2 = tmp;

  /*!< Byte 8 */
  tmp = (uint8_t)((CID_Tab[2] & 0xFF000000) >> 24);
  cardinfo->SD_cid.ProdRev = tmp;

  /*!< Byte 9 */
  tmp = (uint8_t)((CID_Tab[2] & 0x00FF0000) >> 16);
  cardinfo->SD_cid.ProdSN = tmp << 24;

  /*!< Byte 10 */
  tmp = (uint8_t)((CID_Tab[2] & 0x0000FF00) >> 8);
  cardinfo->SD_cid.ProdSN |= tmp << 16;

  /*!< Byte 11 */
  tmp = (uint8_t)(CID_Tab[2] & 0x000000FF);
  cardinfo->SD_cid.ProdSN |= tmp << 8;

  /*!< Byte 12 */
  tmp = (uint8_t)((CID_Tab[3] & 0xFF000000) >> 24);
  cardinfo->SD_cid.ProdSN |= tmp;

  /*!< Byte 13 */
  tmp = (uint8_t)((CID_Tab[3] & 0x00FF0000) >> 16);
  cardinfo->SD_cid.Reserved1 |= (tmp & 0xF0) >> 4;
  cardinfo->SD_cid.ManufactDate = (tmp & 0x0F) << 8;

  /*!< Byte 14 */
  tmp = (uint8_t)((CID_Tab[3] & 0x0000FF00) >> 8);
  cardinfo->SD_cid.ManufactDate |= tmp;

  /*!< Byte 15 */
  tmp = (uint8_t)(CID_Tab[3] & 0x000000FF);
  cardinfo->SD_cid.CID_CRC = (tmp & 0xFE) >> 1;
  cardinfo->SD_cid.Reserved2 = 1;

  return(errorstatus);
}

/**
  * @brief  Enables wide bus opeartion for the requeseted card if supported by
  *         card.
  * @param  wmode: Specifies the SD card wide bus mode.
  *   This parameter can be one of the following values:
  *     @arg SDIO_BusWide_8b: 8-bit data transfer (Only for MMC)
  *     @arg SDIO_BusWide_4b: 4-bit data transfer
  *     @arg SDIO_BusWide_1b: 1-bit data transfer
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_EnableWideBusOperation(u32 wmode)
{
  SD_Error errorstatus = SD_OK;

  if((SDIO_MULTIMEDIA_CARD == CardType) || (SDIO_HIGH_SPEED_MULTIMEDIA_CARD == CardType))
  {
    errorstatus = MMC_Switch(EXT_CSD_CMD_SET_NORMAL, EXT_CSD_BUS_WIDTH, (u8)wmode);
  }
  else if((SDIO_STD_CAPACITY_SD_CARD_V1_1 == CardType) || (SDIO_STD_CAPACITY_SD_CARD_V2_0 == CardType) || (SDIO_HIGH_CAPACITY_SD_CARD == CardType))
  {
    if(wmode >= 2)
    {
      return SD_UNSUPPORTED_FEATURE;//不支持8位模式
    }

    errorstatus = SDEnWideBus(wmode);
  }

  if(SD_OK == errorstatus)
  {
    SDIO->CLKCTRL &= ~(3 << 11);    //清除之前的位宽设置
    SDIO->CLKCTRL |= (u16)wmode << 11; //设置总线宽度
  }

  return errorstatus;
}

/**
  * @brief  Set SDIO Work Mode.
  * @param  Mode
            SD_DMA_MODE    :  DMA Mode.
            SD_POLLING_MODE: Polling Mode.
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_SetDeviceMode(u32 Mode)
{
  SD_Error errorstatus = SD_OK;

  if((Mode == SD_DMA_MODE) || (Mode == SD_POLLING_MODE))
  {
    DeviceMode = Mode;
  }
  else
  {
    errorstatus = SD_INVALID_PARAMETER;
  }

  return errorstatus;
}

/**
  * @brief  Selects od Deselects the corresponding card.
  * @param  addr: Address of the Card to be selected.
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_SelectDeselect(u32 addr)
{
  SDIO_CmdInitStructure.SDIO_Argu =  addr; //发送CMD7,选择卡,短响应
  SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_SEL_DESEL_CARD;
  SDIO_CmdInitStructure.SDIO_Resp = SDIO_Rsp_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_INT;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(SDIO, &SDIO_CmdInitStructure); //发送CMD7,选择卡,短响应

  return CmdResp1Error(SD_CMD_SEL_DESEL_CARD);
}

/**
  * @brief  Read Data From or Write Data to SD Card.
  * @param  SDIO_CmdInitStruct: Pointer to SDIO_CmdInitType Structure.
  * @param  SDIO_DataInitStruct: Pointer to SDIO_DataInitType Structure.
  * @param  buf: Pointer to Data Buffer Read or Write.
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SDIO_SendDataCommand(SDIO_CmdInitType *SDIO_CmdInitStruct, SDIO_DataInitType* SDIO_DataInitStruct, u32 *buf)
{
  SD_Error errorstatus = SD_OK;
  u32 count;
  u32 timeout = SDIO_DATATIMEOUT;
  int tlen;

  if(NULL == buf)
  {
    return SD_INVALID_PARAMETER;
  }

  SDIO->DTCTRL = 0x0; //数据控制寄存器清零(关DMA)
  SDIO_DataConfig(SDIO, SDIO_DataInitStruct);
  SDIO_SendCommand(SDIO, SDIO_CmdInitStruct); //发送CMD17+从addr地址出读取数据,短响应
  errorstatus = CmdResp1Error(SDIO_CmdInitStruct->SDIO_CmdIdx); //等待R1响应

  if(errorstatus != SD_OK) //响应错误
  {
    return errorstatus;
  }

  tlen = SDIO_DataInitStruct->SDIO_DataLen;

  if(DeviceMode == SD_POLLING_MODE) /* 轮询模式 */
  {
    if (SDIO_DataInitStruct->SDIO_TransferDir == SDIO_TransferDir_ToSDIO)
    {
      while(!(SDIO->STS & (SDIO_INTR_STS_READ_MASK)))
      {
        if(SDIO_GetFlagStatus(SDIO, SDIO_FLG_RXBUF_H) != RESET)
        {
          for(count = 0; count < 8; count++, buf++)
          {
            *buf = SDIO->BUF;
          }

          timeout = 0X7FFFFF;
        }
        else
        {
          if(timeout == 0) //超时
          {
            SD_Init();
            return SD_DATA_TIMEOUT;
          }

          timeout--;
        }
      }

      while(SDIO_GetFlagStatus(SDIO, SDIO_FLG_RXBUF) != RESET)
      {
        *buf = SDIO->BUF;
        buf++;
      }
    }
    else
    {
      while(!(SDIO->STS & SDIO_INTR_STS_WRITE_MASK))
      {
        if(SDIO_GetFlagStatus(SDIO, SDIO_FLG_TXBUF_H) != RESET)
        {
          for(count = 0; count < 8 && tlen > 0; count++, buf++, tlen -= 4)
          {
            SDIO->BUF = *buf;
          }

          timeout = 0X3FFFFFFF;
        }
        else
        {
          if(timeout == 0) //超时
          {
            SD_Init();
            return SD_DATA_TIMEOUT;
          }

          timeout--;
        }
      }
    }

    if(SDIO_GetFlagStatus(SDIO, SDIO_FLG_DTTIMEOUT) != RESET)   //数据超时
    {
      SDIO_ClearFlag(SDIO, SDIO_FLG_DTTIMEOUT);
      return SD_DATA_TIMEOUT;
    }
    else if(SDIO_GetFlagStatus(SDIO, SDIO_FLG_DTFAIL) != RESET) //CRC错误
    {
      SDIO_ClearFlag(SDIO, SDIO_FLG_DTFAIL);
      return SD_DATA_FAIL;
    }
    else if(SDIO_GetFlagStatus(SDIO, SDIO_FLG_RXERRO) != RESET) //上溢错误
    {
      SDIO_ClearFlag(SDIO, SDIO_FLG_RXERRO);
      return SD_RX_OVERRUN;
    }
    else if(SDIO_GetFlagStatus(SDIO, SDIO_FLG_TXERRU) != RESET) //下溢错误
    {
      SDIO_ClearFlag(SDIO, SDIO_FLG_TXERRU);
      return SD_TX_UNDERRUN;
    }
    else if(SDIO_GetFlagStatus(SDIO, SDIO_FLG_SBITERR) != RESET) //起始位错误
    {
      SDIO_ClearFlag(SDIO, SDIO_FLG_SBITERR);
      return SD_START_BIT_ERR;
    }

    if(StopCondition == 1 && SDIO_GetFlagStatus(SDIO, SDIO_FLG_DTCMPL) != RESET) //接收结束
    {
      SDIO_CmdInitStruct->SDIO_Argu =  0; //发送CMD12+结束传输
      SDIO_CmdInitStruct->SDIO_CmdIdx = SD_CMD_STOP_TRANSMISSION;
      SDIO_CmdInitStruct->SDIO_Resp = SDIO_Rsp_Short;
      SDIO_CmdInitStruct->SDIO_Wait = SDIO_Wait_No;
      SDIO_CmdInitStruct->SDIO_CPSM = SDIO_CPSM_Enable;
      SDIO_SendCommand(SDIO, SDIO_CmdInitStruct);

      errorstatus = CmdResp1Error(SD_CMD_STOP_TRANSMISSION); //等待R1响应

      if(errorstatus != SD_OK)
      {
        return errorstatus;
      }
    }

    SDIO_ClearFlag(SDIO, SDIO_STATIC_FLAGS); //清除所有标记
  }
  else if(DeviceMode == SD_DMA_MODE) /* DMA模式 */
  {
    if (SDIO_DataInitStruct->SDIO_TransferDir == SDIO_TransferDir_ToSDIO)
    {
      SD_DMA_Config(buf, tlen, DMA_DIR_PERIPHERALSRC);
      SDIO->INTEN |= SDIO_INTR_STS_READ_MASK;

    }
    else
    {
      SD_DMA_Config(buf, tlen, DMA_DIR_PERIPHERALDST);
      SDIO->INTEN |= SDIO_INTR_STS_WRITE_MASK;
    }

    TransferError = SD_OK;
    TransferEnd = 0; //传输结束标置位，在中断服务置1
    SDIO->DTCTRL |= 1 << 3;

    while(!(SDIO->STS & SDIO->INTEN) && timeout)
    {
      timeout--;

      if (TransferEnd)
      {
        break;
      }
    }

    if(timeout == 0)
    {
      SD_Init();
      return SD_DATA_TIMEOUT;
    }

    if(TransferError != SD_OK)
    {
      errorstatus = TransferError;
    }
  }

  return errorstatus;
}

/**
  * @brief  Erase continuous data block
  * @param  addr: starting address
  * @param  nblks: amount of erasing data block
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_EraseBlocks(long long addr, u32 nblks)
{
  SD_Error errorstatus = SD_OK;
  u32 start_addr, end_addr;
  u8 cardstate;

  if(CardType == SDIO_HIGH_CAPACITY_SD_CARD) //大容量卡
  {
    start_addr = addr >> 9;
    end_addr = start_addr + nblks - 1;
  }
  else
  {
    start_addr = addr;
    end_addr = start_addr + (nblks - 1) * 512;
  }

  SDIO_DataInitStructure.SDIO_DataBlkSize = SDIO_DataBlkSize_1b; //清除DPSM状态机配置
  SDIO_DataInitStructure.SDIO_DataLen = 0 ;
  SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT ;
  SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
  SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  SDIO_DataConfig(SDIO, &SDIO_DataInitStructure);

  if(SDIO->RSP1 & SD_CARD_LOCKED) //卡锁了
  {
    return SD_LOCK_UNLOCK_FAILED;
  }

  if(CardType == SDIO_MULTIMEDIA_CARD || CardType == SDIO_HIGH_SPEED_MULTIMEDIA_CARD)
  {
    SDIO_CmdInitStructure.SDIO_Argu =  start_addr;
    SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_ERASE_GRP_START; //CMD35
    SDIO_CmdInitStructure.SDIO_Resp = SDIO_Rsp_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(SDIO, &SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_ERASE_GRP_START); //等待R1响应

    if(errorstatus != SD_OK) //响应错误
    {
      return errorstatus;
    }

    SDIO_CmdInitStructure.SDIO_Argu =  end_addr;
    SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_ERASE_GRP_END;//CMD36
    SDIO_SendCommand(SDIO, &SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_ERASE_GRP_END); //等待R1响应

    if(errorstatus != SD_OK) //响应错误
    {
      return errorstatus;
    }

    SDIO_CmdInitStructure.SDIO_Argu =  0;
    SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_ERASE;//CMD38
    SDIO_SendCommand(SDIO, &SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_ERASE); //等待R1响应

    if(errorstatus != SD_OK) //响应错误
    {
      return errorstatus;
    }
  }
  else
  {
    SDIO_CmdInitStructure.SDIO_Argu =  start_addr;
    SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_SD_ERASE_GRP_START; //CMD32
    SDIO_CmdInitStructure.SDIO_Resp = SDIO_Rsp_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(SDIO, &SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_SD_ERASE_GRP_START); //等待R1响应

    if(errorstatus != SD_OK) //响应错误
    {
      return errorstatus;
    }

    SDIO_CmdInitStructure.SDIO_Argu =  end_addr;
    SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_SD_ERASE_GRP_END; //CMD33
    SDIO_SendCommand(SDIO, &SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_SD_ERASE_GRP_END);   //等待R1响应

    if(errorstatus != SD_OK) //响应错误
    {
      return errorstatus;
    }

    SDIO_CmdInitStructure.SDIO_Argu =  0;
    SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_ERASE; //CMD38
    SDIO_SendCommand(SDIO, &SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_ERASE); //等待R1响应

    if(errorstatus != SD_OK) //响应错误
    {
      return errorstatus;
    }
  }

  errorstatus = IsCardProgramming(&cardstate);

  while((errorstatus == SD_OK) && ((cardstate == SD_CARD_PROGRAMMING) || (cardstate == SD_CARD_RECEIVING)))
  {
    errorstatus = IsCardProgramming(&cardstate);
  }

  return errorstatus;
}

/**
  * @brief  Allows to read one block from a specified address in a card. The Data
  *         transfer can be managed by DMA mode or Polling mode.
  * @note   This operation should be followed by two functions to check if the
  *         DMA Controller and SD Card status.
  *          - SD_ReadWaitOperation(): this function insure that the DMA
  *            controller has finished all data transfer.
  *          - SD_GetStatus(): to check that the SD Card has finished the
  *            data transfer and it is ready for data.
  * @param  readbuff: pointer to the buffer that will contain the received data
  * @param  ReadAddr: Address from where data are to be read.
  * @param  BlockSize: the SD card Data block size. The Block size should be 512.
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_ReadBlock(u8 *readbuff, long long ReadAddr, u16 BlockSize)
{
  SD_Error errorstatus = SD_OK;
  u8 power;

  if(NULL == readbuff)
  {
    return SD_INVALID_PARAMETER;
  }

  SDIO->DTCTRL = 0x0; //数据控制寄存器清零(关DMA)

  if(CardType == SDIO_HIGH_CAPACITY_SD_CARD) //大容量卡
  {
    BlockSize = 512;
    ReadAddr >>= 9;
  }

  SDIO_DataInitStructure.SDIO_DataBlkSize = SDIO_DataBlkSize_1b; //清除DPSM状态机配置
  SDIO_DataInitStructure.SDIO_DataLen = 0;
  SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
  SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
  SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  SDIO_DataConfig(SDIO, &SDIO_DataInitStructure);

  if(SDIO->RSP1 & SD_CARD_LOCKED) //卡锁了
  {
    return SD_LOCK_UNLOCK_FAILED;
  }

  if((BlockSize > 0) && (BlockSize <= 2048) && ((BlockSize & (BlockSize - 1)) == 0))
  {
    power = convert_from_bytes_to_power_of_two(BlockSize);

    SDIO_CmdInitStructure.SDIO_Argu =  BlockSize;
    SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_SET_BLOCKLEN;
    SDIO_CmdInitStructure.SDIO_Resp = SDIO_Rsp_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(SDIO, &SDIO_CmdInitStructure); //发送CMD16+设置数据长度为blksize,短响应

    errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);   //等待R1响应

    if(errorstatus != SD_OK) //响应错误
    {
      return errorstatus;
    }
  }
  else
  {
    return SD_INVALID_PARAMETER;
  }

  SDIO_DataInitStructure.SDIO_DataBlkSize = power << 4 ; //清除DPSM状态机配置
  SDIO_DataInitStructure.SDIO_DataLen = BlockSize ;
  SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT ;
  SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
  SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;

  SDIO_CmdInitStructure.SDIO_Argu =  ReadAddr;
  SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_READ_SINGLE_BLOCK;
  SDIO_CmdInitStructure.SDIO_Resp = SDIO_Rsp_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;

  StopCondition = 0;
  return SDIO_SendDataCommand(&SDIO_CmdInitStructure, &SDIO_DataInitStructure, (u32 *)readbuff);
}

/**
  * @brief  Allows to read blocks from a specified address  in a card.  The Data
  *         transfer can be managed by DMA mode or Polling mode.
  * @note   This operation should be followed by two functions to check if the
  *         DMA Controller and SD Card status.
  *          - SD_ReadWaitOperation(): this function insure that the DMA
  *            controller has finished all data transfer.
  *          - SD_GetStatus(): to check that the SD Card has finished the
  *            data transfer and it is ready for data.
  * @param  readbuff: pointer to the buffer that will contain the received data.
  * @param  ReadAddr: Address from where data are to be read.
  * @param  BlockSize: the SD card Data block size. The Block size should be 512.
  * @param  NumberOfBlocks: number of blocks to be read.
  * @retval SD_Error: SD Card Error code.
  */
__align(4) u32 *tempbuff;
SD_Error SD_ReadMultiBlocks(u8 *readbuff, long long ReadAddr, u16 BlockSize, u32 NumberOfBlocks)
{
  SD_Error errorstatus = SD_OK;
  u8 power;

  SDIO->DTCTRL = 0x0; //数据控制寄存器清零(关DMA)

  if(CardType == SDIO_HIGH_CAPACITY_SD_CARD) //大容量卡
  {
    BlockSize = 512;
    ReadAddr >>= 9;
  }

  SDIO_DataInitStructure.SDIO_DataBlkSize = 0; //清除DPSM状态机配置
  SDIO_DataInitStructure.SDIO_DataLen = 0;
  SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
  SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
  SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  SDIO_DataConfig(SDIO, &SDIO_DataInitStructure);

  if(SDIO->RSP1 & SD_CARD_LOCKED) //卡锁了
  {
    return SD_LOCK_UNLOCK_FAILED;
  }

  if((BlockSize > 0) && (BlockSize <= 2048) && ((BlockSize & (BlockSize - 1)) == 0))
  {
    power = convert_from_bytes_to_power_of_two(BlockSize);

    SDIO_CmdInitStructure.SDIO_Argu =  BlockSize; //发送CMD16+设置数据长度为blksize,短响应
    SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_SET_BLOCKLEN;
    SDIO_CmdInitStructure.SDIO_Resp = SDIO_Rsp_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(SDIO, &SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);   //等待R1响应

    if(errorstatus != SD_OK) //响应错误
    {
      return errorstatus;
    }
  }
  else
  {
    return SD_INVALID_PARAMETER;
  }

  if(NumberOfBlocks * BlockSize > SD_MAX_DATA_LENGTH) //判断是否超过最大接收长度
  {
    return SD_INVALID_PARAMETER;
  }

  SDIO_DataInitStructure.SDIO_DataBlkSize = power << 4; //nblks*blksize,512块大小,卡到控制器
  SDIO_DataInitStructure.SDIO_DataLen = NumberOfBlocks * BlockSize ;
  SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT ;
  SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
  SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;

  SDIO_CmdInitStructure.SDIO_Argu =  ReadAddr;//发送CMD18+从addr地址出读取数据,短响应
  SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_READ_MULT_BLOCK;
  SDIO_CmdInitStructure.SDIO_Resp = SDIO_Rsp_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;

  StopCondition = 1;
  return SDIO_SendDataCommand(&SDIO_CmdInitStructure, &SDIO_DataInitStructure, (u32 *)readbuff);
}

/**
  * @brief  Allows to write one block starting from a specified address in a card.
  *         The Data transfer can be managed by DMA mode or Polling mode.
  * @note   This operation should be followed by two functions to check if the
  *         DMA Controller and SD Card status.
  *          - SD_ReadWaitOperation(): this function insure that the DMA
  *            controller has finished all data transfer.
  *          - SD_GetStatus(): to check that the SD Card has finished the
  *            data transfer and it is ready for data.
  * @param  writebuff: pointer to the buffer that contain the data to be transferred.
  * @param  WriteAddr: Address from where data are to be read.
  * @param  BlockSize: the SD card Data block size. The Block size should be 512.
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_WriteBlock(u8 *writebuff, long long WriteAddr, u16 BlockSize)
{
  SD_Error errorstatus = SD_OK;
  u8  power = 0, cardstate = 0;
  u32 timeout = 0;
  u32 cardstatus = 0;

  if(writebuff == NULL) //参数错误
  {
    return SD_INVALID_PARAMETER;
  }

  SDIO->DTCTRL = 0x0; //数据控制寄存器清零(关DMA)

  SDIO_DataInitStructure.SDIO_DataBlkSize = 0; //清除DPSM状态机配置
  SDIO_DataInitStructure.SDIO_DataLen = 0;
  SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
  SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
  SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  SDIO_DataConfig(SDIO, &SDIO_DataInitStructure);

  if(SDIO->RSP1 & SD_CARD_LOCKED) //卡锁了
  {
    return SD_LOCK_UNLOCK_FAILED;
  }

  if(CardType == SDIO_HIGH_CAPACITY_SD_CARD) //大容量卡
  {
    BlockSize = 512;
    WriteAddr >>= 9;
  }

  if((BlockSize > 0) && (BlockSize <= 2048) && ((BlockSize & (BlockSize - 1)) == 0))
  {
    power = convert_from_bytes_to_power_of_two(BlockSize);

    SDIO_CmdInitStructure.SDIO_Argu = BlockSize; //发送CMD16+设置数据长度为blksize,短响应
    SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_SET_BLOCKLEN;
    SDIO_CmdInitStructure.SDIO_Resp = SDIO_Rsp_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(SDIO, &SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);   //等待R1响应

    if(errorstatus != SD_OK) //响应错误
    {
      return errorstatus;
    }

  }
  else
  {
    return SD_INVALID_PARAMETER;
  }

  timeout = SD_DATATIMEOUT;

  do
  {
    timeout--;
    errorstatus = SD_SendStatus(&cardstatus);
  }
  while(((cardstatus & 0x00000100) == 0) && (timeout > 0)); //检查READY_FOR_DATA位是否置位

  if(timeout == 0)
  {
    return SD_ERROR;
  }

  SDIO_CmdInitStructure.SDIO_Argu = WriteAddr; //发送CMD24,写单块指令,短响应
  SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_WRITE_SINGLE_BLOCK;
  SDIO_CmdInitStructure.SDIO_Resp = SDIO_Rsp_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;

  SDIO_DataInitStructure.SDIO_DataBlkSize = power << 4; //blksize, 控制器到卡
  SDIO_DataInitStructure.SDIO_DataLen = BlockSize;
  SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
  SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
  SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;

  StopCondition = 0; //单块写,不需要发送停止传输指令
  errorstatus = SDIO_SendDataCommand(&SDIO_CmdInitStructure, &SDIO_DataInitStructure, (u32 *)writebuff);

  if(errorstatus != SD_OK)
  {
    return errorstatus;
  }

  SDIO_ClearFlag(SDIO, SDIO_STATIC_FLAGS); //清除所有标记
  errorstatus = IsCardProgramming(&cardstate);

  while((errorstatus == SD_OK) && ((cardstate == SD_CARD_PROGRAMMING) || (cardstate == SD_CARD_RECEIVING)))
  {
    errorstatus = IsCardProgramming(&cardstate);
  }

  return errorstatus;
}

/**
  * @brief  Allows to write blocks starting from a specified address in a card.
  *         The Data transfer can be managed by DMA mode only.
  * @note   This operation should be followed by two functions to check if the
  *         DMA Controller and SD Card status.
  *          - SD_ReadWaitOperation(): this function insure that the DMA
  *            controller has finished all data transfer.
  *          - SD_GetStatus(): to check that the SD Card has finished the
  *            data transfer and it is ready for data.
  * @param  writebuff: pointer to the buffer that contain the data to be transferred.
  * @param  WriteAddr: Address from where data are to be read.
  * @param  BlockSize: the SD card Data block size. The Block size should be 512.
  * @param  NumberOfBlocks: number of blocks to be written.
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_WriteMultiBlocks(u8 *writebuff, long long WriteAddr, u16 BlockSize, u32 NumberOfBlocks)
{
  SD_Error errorstatus = SD_OK;
  u8  power = 0, cardstate = 0;
  u32 timeout = 0;
  u32 cardstatus = 0;

  if(writebuff == NULL)
  {
    return SD_INVALID_PARAMETER;
  }

  SDIO->DTCTRL = 0x0; //数据控制寄存器清零(关DMA)

  SDIO_DataInitStructure.SDIO_DataBlkSize = 0; //清除DPSM状态机配置
  SDIO_DataInitStructure.SDIO_DataLen = 0;
  SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
  SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
  SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  SDIO_DataConfig(SDIO, &SDIO_DataInitStructure);

  if(SDIO->RSP1 & SD_CARD_LOCKED) //卡锁了
  {
    return SD_LOCK_UNLOCK_FAILED;
  }

  if(CardType == SDIO_HIGH_CAPACITY_SD_CARD) //大容量卡
  {
    BlockSize = 512;
    WriteAddr >>= 9;
  }

  if((BlockSize > 0) && (BlockSize <= 2048) && ((BlockSize & (BlockSize - 1)) == 0))
  {
    power = convert_from_bytes_to_power_of_two(BlockSize);

    SDIO_CmdInitStructure.SDIO_Argu = BlockSize;//发送CMD16+设置数据长度为blksize,短响应
    SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_SET_BLOCKLEN;
    SDIO_CmdInitStructure.SDIO_Resp = SDIO_Rsp_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(SDIO, &SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);   //等待R1响应

    if(errorstatus != SD_OK) //响应错误
    {
      return errorstatus;
    }

  }
  else
  {
    return SD_INVALID_PARAMETER;
  }

  if(NumberOfBlocks * BlockSize > SD_MAX_DATA_LENGTH)
  {
    return SD_INVALID_PARAMETER;
  }

  timeout = SD_DATATIMEOUT;

  do
  {
    timeout--;
    errorstatus = SD_SendStatus(&cardstatus);
  }
  while(((cardstatus & 0x00000100) == 0) && (timeout > 0)); //检查READY_FOR_DATA位是否置位

  if(timeout == 0)
  {
    return SD_ERROR;
  }

  if((SDIO_STD_CAPACITY_SD_CARD_V1_1 == CardType) || (SDIO_STD_CAPACITY_SD_CARD_V2_0 == CardType) || (SDIO_HIGH_CAPACITY_SD_CARD == CardType))
  {
    SDIO_CmdInitStructure.SDIO_Argu = (u32)RCA << 16;   //发送ACMD55
    SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_APP_CMD;
    SDIO_CmdInitStructure.SDIO_Resp = SDIO_Rsp_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(SDIO, &SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_APP_CMD); //等待R1响应

    if(errorstatus != SD_OK)
    {
      return errorstatus;
    }

    SDIO_CmdInitStructure.SDIO_Argu = NumberOfBlocks; //发送CMD23
    SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_SET_BLOCK_COUNT;
    SDIO_CmdInitStructure.SDIO_Resp = SDIO_Rsp_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(SDIO, &SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_SET_BLOCK_COUNT); //等待R1响应

    if(errorstatus != SD_OK)
    {
      return errorstatus;
    }
  }

  SDIO_CmdInitStructure.SDIO_Argu = WriteAddr;  //发送CMD25
  SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_WRITE_MULT_BLOCK;
  SDIO_CmdInitStructure.SDIO_Resp = SDIO_Rsp_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;

  SDIO_DataInitStructure.SDIO_DataBlkSize = power << 4; //blksize, 控制器到卡
  SDIO_DataInitStructure.SDIO_DataLen = NumberOfBlocks * BlockSize ;
  SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT ;
  SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
  SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  StopCondition = 1;    //CMD12 is needed
  errorstatus = SDIO_SendDataCommand(&SDIO_CmdInitStructure, &SDIO_DataInitStructure, (u32 *)writebuff);

  if(errorstatus != SD_OK)
  {
    return errorstatus;
  }

  SDIO_ClearFlag(SDIO, SDIO_STATIC_FLAGS);
  errorstatus = IsCardProgramming(&cardstate);

  while((errorstatus == SD_OK) && ((cardstate == SD_CARD_PROGRAMMING) || (cardstate == SD_CARD_RECEIVING)))
  {
    errorstatus = IsCardProgramming(&cardstate);
  }

  return errorstatus;
}

/**
  * @brief  Read MMC card data stream.
  * @param  buf: buffer of saving data from MMC card
  * @param  addr: start address of data from MMC card
  * @param  tlen: data size
  * @retval SD_Error: SD Card Error code.
  */
SD_Error MMC_StreamRead(u8 *buf, long long addr, u32 tlen)
{
  SDIO->DTCTRL = 0x0; //数据控制寄存器清零(关DMA)

  SDIO_DataInitStructure.SDIO_DataBlkSize = 0; //清除DPSM状态机配置
  SDIO_DataInitStructure.SDIO_DataLen = 0;
  SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
  SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
  SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Stream;
  SDIO_DataConfig(SDIO, &SDIO_DataInitStructure);

  if(SDIO->RSP1 & SD_CARD_LOCKED)
  {
    return SD_LOCK_UNLOCK_FAILED;  //卡锁了
  }

  SDIO_CmdInitStructure.SDIO_Argu =  addr; //发送CMD11+从addr地址出读取数据,短响应
  SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_READ_DAT_UNTIL_STOP;
  SDIO_CmdInitStructure.SDIO_Resp = SDIO_Rsp_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;

  SDIO_DataInitStructure.SDIO_DataBlkSize = 5 << 4;
  SDIO_DataInitStructure.SDIO_DataLen = tlen;
  SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT ;
  SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
  SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Stream;

  StopCondition = 1;    //CMD12 is needed
  return SDIO_SendDataCommand(&SDIO_CmdInitStructure, &SDIO_DataInitStructure, (u32 *)buf);
}

/**
  * @brief  Read MMC card data stream.
  * @param  buf: data that writing to MMC card
  * @param  addr: start address MMC card
  * @param  tlen: data size
  * @retval SD_Error: SD Card Error code.
  */
SD_Error MMC_StreamWrite(u8 *buf, long long addr, u32 tlen)
{
  SD_Error errorstatus = SD_OK;
  u8 cardstate = 0;

  if(buf == NULL)
  {
    return SD_INVALID_PARAMETER;  //参数错误
  }

  SDIO->DTCTRL = 0x0;                           //数据控制寄存器清零(关DMA)

  SDIO_DataInitStructure.SDIO_DataBlkSize = 0; ;    //清除DPSM状态机配置
  SDIO_DataInitStructure.SDIO_DataLen = 0 ;
  SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT ;
  SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
  SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Stream;
  SDIO_DataConfig(SDIO, &SDIO_DataInitStructure);

  if(SDIO->RSP1 & SD_CARD_LOCKED)
  {
    return SD_LOCK_UNLOCK_FAILED;  //卡锁了
  }

  SDIO_CmdInitStructure.SDIO_Argu = addr;   //发送CMD20,多块写指令,短响应
  SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_WRITE_DAT_UNTIL_STOP;
  SDIO_CmdInitStructure.SDIO_Resp = SDIO_Rsp_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;

  SDIO_DataInitStructure.SDIO_DataBlkSize = 15 << 4;
  SDIO_DataInitStructure.SDIO_DataLen = tlen;
  SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT ;
  SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
  SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Stream;

  StopCondition = 1;    //CMD12 is needed
  errorstatus = SDIO_SendDataCommand(&SDIO_CmdInitStructure, &SDIO_DataInitStructure, (u32 *)buf);

  if(errorstatus != SD_OK)
  {
    return errorstatus;
  }

  SDIO_ClearFlag(SDIO, SDIO_STATIC_FLAGS); //清除所有标记
  errorstatus = IsCardProgramming(&cardstate);

  while((errorstatus == SD_OK) && ((cardstate == SD_CARD_PROGRAMMING) || (cardstate == SD_CARD_RECEIVING)))
  {
    errorstatus = IsCardProgramming(&cardstate);
  }

  return errorstatus;
}

/**
  * @brief  SDIO ISR.
  * @param  None.
  * @retval None.
  */
void SDIO_IRQHandler(void)
{
  SD_ProcessIRQSrc();//处理所有SDIO相关中断
}

/**
  * @brief  Allows to process all the interrupts that are high.
  * @param  None
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_ProcessIRQSrc(void)
{
  if(SDIO->STS & (1 << 8)) //接收完成中断
  {
    if (StopCondition == 1)
    {
      SDIO_CmdInitStructure.SDIO_Argu = 0; //发送CMD12+结束传输
      SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_STOP_TRANSMISSION;
      SDIO_CmdInitStructure.SDIO_Resp = SDIO_Rsp_Short;
      SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
      SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
      SDIO_SendCommand(SDIO, &SDIO_CmdInitStructure);

      TransferError = CmdResp1Error(SD_CMD_STOP_TRANSMISSION);
    }
    else
    {
      TransferError = SD_OK;
    }

    SDIO->INTCLR |= 1 << 8;
    SDIO->INTEN &= ~((1 << 1) | (1 << 3) | (1 << 8) | (1 << 14) | (1 << 15) | (1 << 4) | (1 << 5) | (1 << 9)); //关中断
    TransferEnd = 1;
    return(TransferError);
  }

  if(SDIO_GetFlagStatus(SDIO, SDIO_FLG_DTFAIL) != RESET) //数据CRC错误
  {
    SDIO_ClearFlag(SDIO, SDIO_FLG_DTFAIL); //清错误标志
    SDIO->INTEN &= ~((1 << 1) | (1 << 3) | (1 << 8) | (1 << 14) | (1 << 15) | (1 << 4) | (1 << 5) | (1 << 9));
    TransferError = SD_DATA_FAIL;
    return(SD_DATA_FAIL);
  }

  if(SDIO_GetFlagStatus(SDIO, SDIO_FLG_DTTIMEOUT) != RESET) //数据超时错误
  {
    SDIO_ClearFlag(SDIO, SDIO_FLG_DTTIMEOUT); //清中断标志
    SDIO->INTEN &= ~((1 << 1) | (1 << 3) | (1 << 8) | (1 << 14) | (1 << 15) | (1 << 4) | (1 << 5) | (1 << 9)); //关中断
    TransferError = SD_DATA_TIMEOUT;
    return(SD_DATA_TIMEOUT);
  }

  if(SDIO_GetFlagStatus(SDIO, SDIO_FLG_RXERRO) != RESET) //FIFO上溢错误
  {
    SDIO_ClearFlag(SDIO, SDIO_FLG_RXERRO);              //清中断标志
    SDIO->INTEN &= ~((1 << 1) | (1 << 3) | (1 << 8) | (1 << 14) | (1 << 15) | (1 << 4) | (1 << 5) | (1 << 9)); //关中断
    TransferError = SD_RX_OVERRUN;
    return(SD_RX_OVERRUN);
  }

  if(SDIO_GetFlagStatus(SDIO, SDIO_FLG_TXERRU) != RESET) //FIFO下溢错误
  {
    SDIO_ClearFlag(SDIO, SDIO_FLG_TXERRU);              //清中断标志
    SDIO->INTEN &= ~((1 << 1) | (1 << 3) | (1 << 8) | (1 << 14) | (1 << 15) | (1 << 4) | (1 << 5) | (1 << 9)); //关中断
    TransferError = SD_TX_UNDERRUN;
    return(SD_TX_UNDERRUN);
  }

  if(SDIO_GetFlagStatus(SDIO, SDIO_FLG_SBITERR) != RESET) //起始位错误
  {
    SDIO_ClearFlag(SDIO, SDIO_FLG_SBITERR); //清中断标志
    SDIO->INTEN &= ~((1 << 1) | (1 << 3) | (1 << 8) | (1 << 14) | (1 << 15) | (1 << 4) | (1 << 5) | (1 << 9)); //关中断
    TransferError = SD_START_BIT_ERR;
    return(SD_START_BIT_ERR);
  }

  return(SD_OK);
}

/**
  * @brief  Checks for error conditions for CMD0.
  * @param  None
  * @retval SD_Error: SD Card Error code.
  */
SD_Error CmdError(void)
{
  SD_Error errorstatus = SD_OK;
  u32 timeout = SDIO_CMD0TIMEOUT;

  while(timeout--)
  {
    if(SDIO_GetFlagStatus(SDIO, SDIO_FLG_CMDCMPL) != RESET)
    {
      break;
    }
  }

  if(timeout == 0)
  {
    return SD_CMD_RSP_TIMEOUT;
  }

  SDIO_ClearFlag(SDIO, SDIO_STATIC_FLAGS); //清除所有标记
  return errorstatus;
}

/**
  * @brief  Checks for error conditions for R7 response.
  * @param  None
  * @retval SD_Error: SD Card Error code.
  */
SD_Error CmdResp7Error(void)
{
  SD_Error errorstatus = SD_OK;
  u32 status;
  u32 timeout = SDIO_CMD0TIMEOUT;

  while(timeout--)
  {
    status = SDIO->STS;

    if(status & ((1 << 0) | (1 << 2) | (1 << 6))) //CRC错误/命令响应超时/已经收到响应
    {
      break;
    }
  }

  if((timeout == 0) || (status & (1 << 2))) //响应超时
  {
    errorstatus = SD_CMD_RSP_TIMEOUT;
    SDIO_ClearFlag(SDIO, SDIO_FLG_CMDTIMEOUT);
    return errorstatus;
  }

  if(status & 1 << 6)   //收到响应
  {
    errorstatus = SD_OK;
    SDIO_ClearFlag(SDIO, SDIO_FLG_CMDRSPCMPL);
  }

  return errorstatus;
}

/**
  * @brief  Checks for error conditions for R1 response.
  * @param  cmd: The sent command index.
  * @retval SD_Error: SD Card Error code.
  */
SD_Error CmdResp1Error(u8 cmd)
{
  u32 status;

  while(1)
  {
    status = SDIO->STS;

    if(status & ((1 << 0) | (1 << 2) | (1 << 6))) //CRC错误/命令响应超时/已经收到响应
    {
      break;
    }
  }

  if(SDIO_GetFlagStatus(SDIO, SDIO_FLG_CMDTIMEOUT) != RESET) //响应超时
  {
    SDIO_ClearFlag(SDIO, SDIO_FLG_CMDTIMEOUT);
    return SD_CMD_RSP_TIMEOUT;
  }

  if(SDIO_GetFlagStatus(SDIO, SDIO_FLG_CMDFAIL) != RESET)   //CRC错误
  {
    SDIO_ClearFlag(SDIO, SDIO_FLG_CMDFAIL);
    return SD_CMD_FAIL;
  }

  if(SDIO->RSPCMD != cmd)
  {
    return SD_ILLEGAL_CMD;  //命令不匹配
  }

  SDIO->INTCLR = 0X5FF;
  return (SD_Error)(SDIO->RSP1 & SD_OCR_ERRORBITS);
}

/**
  * @brief  Checks for error conditions for R3 (OCR) response.
  * @param  None
  * @retval SD_Error: SD Card Error code.
  */
SD_Error CmdResp3Error(void)
{
  u32 status;

  while(1)
  {
    status = SDIO->STS;

    if(status & ((1 << 0) | (1 << 2) | (1 << 6))) //CRC错误/命令响应超时/已经收到响应
    {
      break;
    }
  }

  if(SDIO_GetFlagStatus(SDIO, SDIO_FLG_CMDTIMEOUT) != RESET) //响应超时
  {
    SDIO_ClearFlag(SDIO, SDIO_FLG_CMDTIMEOUT);
    return SD_CMD_RSP_TIMEOUT;
  }

  SDIO_ClearFlag(SDIO, SDIO_STATIC_FLAGS);
  return SD_OK;
}

/**
  * @brief  Checks for error conditions for R2 (CID or CSD) response.
  * @param  None
  * @retval SD_Error: SD Card Error code.
  */
SD_Error CmdResp2Error(void)
{
  SD_Error errorstatus = SD_OK;
  u32 status;
  u32 timeout = SDIO_CMD0TIMEOUT;

  while(timeout--)
  {
    status = SDIO->STS;

    if(status & ((1 << 0) | (1 << 2) | (1 << 6))) //CRC错误/命令响应超时/已经收到响应
    {
      break;
    }
  }

  if((timeout == 0) || (status & (1 << 2))) //响应超时
  {
    errorstatus = SD_CMD_RSP_TIMEOUT;
    SDIO_ClearFlag(SDIO, SDIO_FLG_CMDTIMEOUT);
    return errorstatus;
  }

  if(SDIO_GetFlagStatus(SDIO, SDIO_FLG_CMDFAIL) != RESET)   //CRC错误
  {
    errorstatus = SD_CMD_FAIL;
    SDIO_ClearFlag(SDIO, SDIO_FLG_CMDFAIL);
  }

  SDIO_ClearFlag(SDIO, SDIO_STATIC_FLAGS); //清除所有标记
  return errorstatus;
}

/**
  * @brief  Checks for error conditions for R4 response.
  * @param  cmd: The sent command index.
  * @retval SD_Error: SD Card Error code.
  */
SD_Error CmdResp4Error(u8 cmd)
{
  u32 status;

  while(1)
  {
    status = SDIO->STS;

    if(status & ((1 << 0) | (1 << 2) | (1 << 6))) //CRC错误/命令响应超时/已经收到响应
    {
      break;
    }
  }

  if(SDIO_GetFlagStatus(SDIO, SDIO_FLG_CMDTIMEOUT) != RESET)    //响应超时
  {
    SDIO_ClearFlag(SDIO, SDIO_FLG_CMDTIMEOUT);
    return SD_CMD_RSP_TIMEOUT;
  }

  if(SDIO_GetFlagStatus(SDIO, SDIO_FLG_CMDFAIL) != RESET)   //CRC错误
  {
    SDIO_ClearFlag(SDIO, SDIO_FLG_CMDFAIL);
    return SD_CMD_FAIL;
  }

  if(SDIO->RSPCMD != cmd) //命令不匹配
  {
    return SD_ILLEGAL_CMD;
  }

  SDIO_ClearFlag(SDIO, SDIO_STATIC_FLAGS); //清除所有标记

  return SD_OK;
}

/**
  * @brief  Checks for error conditions for R5 response.
  * @param  cmd: The sent command index.
  * @retval SD_Error: SD Card Error code.
  */
SD_Error CmdResp5Error(u8 cmd)
{
  u32 status;

  while(1)
  {
    status = SDIO->STS;

    if(status & ((1 << 0) | (1 << 2) | (1 << 6))) //CRC错误/命令响应超时/已经收到响应
    {
      break;
    }
  }

  if(SDIO_GetFlagStatus(SDIO, SDIO_FLG_CMDTIMEOUT) != RESET) //响应超时
  {
    SDIO_ClearFlag(SDIO, SDIO_FLG_CMDTIMEOUT);
    return SD_CMD_RSP_TIMEOUT;
  }

  if(SDIO_GetFlagStatus(SDIO, SDIO_FLG_CMDFAIL) != RESET)   //CRC错误
  {
    SDIO_ClearFlag(SDIO, SDIO_FLG_CMDFAIL);
    return SD_CMD_FAIL;
  }

  if(SDIO->RSPCMD != cmd) //命令不匹配
  {
    return SD_ILLEGAL_CMD;
  }

  SDIO_ClearFlag(SDIO, SDIO_STATIC_FLAGS);

  if (SDIO->RSP1 & SD_R5_OUT_OF_RANGE)
  {
    return SD_CMD_OUT_OF_RANGE;
  }

  if (SDIO->RSP1 & SD_R5_FUNCTION_NUMBER)
  {
    return SD_SDIO_UNKNOWN_FUNC;
  }

  if (SDIO->RSP1 & SD_R5_ERROR)
  {
    return SD_GENERAL_UNKNOWN_ERROR;
  }

  return SD_OK;
}

/**
  * @brief  Checks for error conditions for R6 (RCA) response.
  * @param  cmd: The sent command index.
  * @param  prca: pointer to the variable that will contain the SD card relative
  *         address RCA.
  * @retval SD_Error: SD Card Error code.
  */
SD_Error CmdResp6Error(u8 cmd, u16*prca)
{
  SD_Error errorstatus = SD_OK;
  u32 status;
  u32 rspr1;

  while(1)
  {
    status = SDIO->STS;

    if(status & ((1 << 0) | (1 << 2) | (1 << 6))) //CRC错误/命令响应超时/已经收到响应
    {
      break;
    }
  }

  if(SDIO_GetFlagStatus(SDIO, SDIO_FLG_CMDTIMEOUT) != RESET) //响应超时
  {
    SDIO_ClearFlag(SDIO, SDIO_FLG_CMDTIMEOUT);
    return SD_CMD_RSP_TIMEOUT;
  }

  if(SDIO_GetFlagStatus(SDIO, SDIO_FLG_CMDFAIL) != RESET)   //CRC错误
  {
    SDIO_ClearFlag(SDIO, SDIO_FLG_CMDFAIL);
    return SD_CMD_FAIL;
  }

  if(SDIO->RSPCMD != cmd) //判断是否响应cmd命令
  {
    return SD_ILLEGAL_CMD;
  }

  SDIO_ClearFlag(SDIO, SDIO_STATIC_FLAGS); //清除所有标记

  rspr1 = SDIO->RSP1;

  if(SD_ALLZERO == (rspr1 & (SD_R6_GENERAL_UNKNOWN_ERROR | SD_R6_ILLEGAL_CMD | SD_R6_CMD_CRC_FAILED)))
  {
    *prca = (u16)(rspr1 >> 16);
    return errorstatus;
  }

  if(rspr1 & SD_R6_GENERAL_UNKNOWN_ERROR)
  {
    return SD_GENERAL_UNKNOWN_ERROR;
  }

  if(rspr1 & SD_R6_ILLEGAL_CMD)
  {
    return SD_ILLEGAL_CMD;
  }

  if(rspr1 & SD_R6_CMD_CRC_FAILED)
  {
    return SD_CMD_CRC_FAILED;
  }

  return errorstatus;
}

/**
  * @brief  Enables or disables the SDIO wide bus mode.
  * @param  enx: new state of the SDIO wide bus mode.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SDEnWideBus(u8 enx)
{
  SD_Error errorstatus = SD_OK;
  u8 arg = 0X00;

  if(enx)
  {
    arg = 0X02;
  }
  else
  {
    arg = 0X00;
  }

  if(SDIO->RSP1 & SD_CARD_LOCKED) //SD卡处于LOCKED状态
  {
    return SD_LOCK_UNLOCK_FAILED;
  }

  if(SDCardInfo.SD_scr.SD_BUS_WIDTHS)   //支持宽总线
  {
    SDIO_CmdInitStructure.SDIO_Argu = (uint32_t) SDCardInfo.RCA << 16;//发送CMD55+RCA,短响应
    SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_APP_CMD;
    SDIO_CmdInitStructure.SDIO_Resp = SDIO_Rsp_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(SDIO, &SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_APP_CMD);

    if(errorstatus != SD_OK)
    {
      return errorstatus;
    }

    SDIO_CmdInitStructure.SDIO_Argu = arg; //发送ACMD6,短响应,参数:10,4位;00,1位.
    SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_APP_SD_SET_BUSWIDTH;
    SDIO_CmdInitStructure.SDIO_Resp = SDIO_Rsp_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(SDIO, &SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_APP_SD_SET_BUSWIDTH);
    return errorstatus;
  }
  else
  {
    return SD_REQ_NOT_APPLICABLE;  //不支持宽总线设置
  }
}

/**
  * @brief  Switch MMC card speed to high speed
  * @param  set: new state of the SDIO wide bus mode.
  * @param  index: offset
  * @param  value: wanted speed
  * @retval SD_Error: SD Card Error code.
  */
SD_Error MMC_Switch(u8 set, u8 index, u8 value)
{
  SD_Error errorstatus = SD_OK;
  u32 cardstatus = 0;
  u32 timeout = 0;

  SDIO_CmdInitStructure.SDIO_Argu = (EXT_CSD_Write_byte << 24) | (index << 16) | (value << 8) | set;
  SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_HS_SWITCH;
  SDIO_CmdInitStructure.SDIO_Resp = SDIO_Rsp_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(SDIO, &SDIO_CmdInitStructure);

  errorstatus = CmdResp1Error(SD_CMD_HS_SWITCH);    //等待R1响应

  if(errorstatus != SD_OK)
  {
    ;
  }

  cardstatus = SDIO->RSP1;

  if(cardstatus & MMC_SWITCH_ERROR)
  {
    return SD_SWITCH_ERROR;
  }

  timeout = SD_DATATIMEOUT;

  do
  {
    timeout--;
    errorstatus = SD_SendStatus(&cardstatus);
  }
  while(((cardstatus & 0x00000100) == 0) && (timeout > 0)); //检查READY_FOR_DATA位是否置位

  if(timeout == 0)
  {
    return SD_ERROR;
  }

  return errorstatus;
}

/**
  * @brief  Switch SD card speed to high speed
  * @param  mode: polling mode or DMA mode
  * @param  group: selected group
  * @param  value: wanted speed
  * @param  resp: switch status
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_Switch(u32 mode, u32 group, u8 value, u8 *resp)
{
  SD_Error errorstatus = SD_OK;
  u8 power;
  u16 blksize;

  if(NULL == resp)
  {
    return SD_INVALID_PARAMETER;
  }

  SDIO->DTCTRL = 0x0;   //数据控制寄存器清零(关DMA)

  blksize = 64;
  power = convert_from_bytes_to_power_of_two(blksize);

  SDIO_CmdInitStructure.SDIO_Argu =  blksize;
  SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_SET_BLOCKLEN;
  SDIO_CmdInitStructure.SDIO_Resp = SDIO_Rsp_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(SDIO, &SDIO_CmdInitStructure); //发送CMD16+设置数据长度为blksize,短响应

  errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN); //等待R1响应

  if(errorstatus != SD_OK)
  {
    return errorstatus;  //响应错误
  }

  SDIO_DataInitStructure.SDIO_DataBlkSize = power << 4 ; //清除DPSM状态机配置
  SDIO_DataInitStructure.SDIO_DataLen = blksize ;
  SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT ;
  SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
  SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;

  SDIO_CmdInitStructure.SDIO_Argu = mode << 31 | 0x00FFFFFF;
  SDIO_CmdInitStructure.SDIO_Argu &= ~(0xF << (group * 4));
  SDIO_CmdInitStructure.SDIO_Argu |= value << (group * 4);
  SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_HS_SWITCH;
  SDIO_CmdInitStructure.SDIO_Resp = SDIO_Rsp_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;

  StopCondition = 0;
  return SDIO_SendDataCommand(&SDIO_CmdInitStructure, &SDIO_DataInitStructure, (u32 *)resp);
}


/**
  * @brief  Checks if the SD card is in programming state.
  * @param  pstatus: pointer to the variable that will contain the SD card state.
  * @retval SD_Error: SD Card Error code.
  */
SD_Error IsCardProgramming(u8 *pstatus)
{
  vu32 respR1 = 0, status = 0;


  SDIO_CmdInitStructure.SDIO_Argu = (uint32_t) RCA << 16; //卡相对地址参数
  SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_SEND_STATUS;//发送CMD13
  SDIO_CmdInitStructure.SDIO_Resp = SDIO_Rsp_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(SDIO, &SDIO_CmdInitStructure);

  status = SDIO->STS;

  while(!(status & ((1 << 0) | (1 << 6) | (1 << 2))))
  {
    status = SDIO->STS;  //等待操作完成
  }

  if(SDIO_GetFlagStatus(SDIO, SDIO_FLG_CMDFAIL) != RESET)   //CRC检测失败
  {
    SDIO_ClearFlag(SDIO, SDIO_FLG_CMDFAIL);
    return SD_CMD_FAIL;
  }

  if(SDIO_GetFlagStatus(SDIO, SDIO_FLG_CMDTIMEOUT) != RESET)    //命令超时
  {
    SDIO_ClearFlag(SDIO, SDIO_FLG_CMDTIMEOUT);
    return SD_CMD_RSP_TIMEOUT;
  }

  if(SDIO->RSPCMD != SD_CMD_SEND_STATUS)
  {
    return SD_ILLEGAL_CMD;
  }

  SDIO_ClearFlag(SDIO, SDIO_STATIC_FLAGS);

  respR1 = SDIO->RSP1;

  *pstatus = (u8)((respR1 >> 9) & 0x0000000F);

  return SD_OK;
}

/**
  * @brief  Read current card status.
  * @param  pcardstatus: Card status.
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_SendStatus(u32 *pcardstatus)
{
  SD_Error errorstatus = SD_OK;

  if(pcardstatus == NULL)
  {
    errorstatus = SD_INVALID_PARAMETER;
    return errorstatus;
  }

  SDIO_CmdInitStructure.SDIO_Argu = (uint32_t) RCA << 16;//发送CMD13,短响应
  SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_SEND_STATUS;
  SDIO_CmdInitStructure.SDIO_Resp = SDIO_Rsp_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(SDIO, &SDIO_CmdInitStructure);
  errorstatus = CmdResp1Error(SD_CMD_SEND_STATUS);  //查询响应状态

  if(errorstatus != SD_OK)
  {
    return errorstatus;
  }

  *pcardstatus = SDIO->RSP1; //读取响应值

  return errorstatus;
}

/**
  * @brief  Returns information about SD card.
  * @param  None.
  * @retval SDCardState: SD Card State code.
  */
SDCardState SD_GetState(void)
{
  u32 resp1 = 0;

  if(SD_SendStatus(&resp1) != SD_OK)
  {
    return SD_CARD_ERROR;
  }
  else
  {
    return (SDCardState)((resp1 >> 9) & 0x0F);
  }
}

/**
  * @brief  Find the SD card SCR register value.
  * @param  rca: selected card address.
  * @param  pscr: pointer to the buffer that will contain the SCR value.
  * @retval SD_Error: SD Card Error code.
  */
SD_Error FindSCR(void)
{
  u32 index = 0;
  SD_Error errorstatus = SD_OK;
  u32 *tempscr;

  tempscr = (u32 *) & (SDCardInfo.SD_scr);
  SDIO_CmdInitStructure.SDIO_Argu = (uint32_t)8;     //发送CMD16
  SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_SET_BLOCKLEN; //cmd16
  SDIO_CmdInitStructure.SDIO_Resp = SDIO_Rsp_Short;  //r1
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(SDIO, &SDIO_CmdInitStructure);

  errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);

  if(errorstatus != SD_OK)
  {
    return errorstatus;
  }

  SDIO_CmdInitStructure.SDIO_Argu = (uint32_t) SDCardInfo.RCA << 16;
  SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_APP_CMD;//发送CMD55
  SDIO_CmdInitStructure.SDIO_Resp = SDIO_Rsp_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(SDIO, &SDIO_CmdInitStructure);

  errorstatus = CmdResp1Error(SD_CMD_APP_CMD);

  if(errorstatus != SD_OK)
  {
    return errorstatus;
  }

  SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
  SDIO_DataInitStructure.SDIO_DataLen = 8;
  SDIO_DataInitStructure.SDIO_DataBlkSize = SDIO_DataBlkSize_8b;  //块大小8byte
  SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
  SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataConfig(SDIO, &SDIO_DataInitStructure);

  SDIO_CmdInitStructure.SDIO_Argu = 0x0;
  SDIO_CmdInitStructure.SDIO_CmdIdx = SD_CMD_SD_APP_SEND_SCR;   //发送ACMD51
  SDIO_CmdInitStructure.SDIO_Resp = SDIO_Rsp_Short;  //r1
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(SDIO, &SDIO_CmdInitStructure);

  errorstatus = CmdResp1Error(SD_CMD_SD_APP_SEND_SCR);

  if(errorstatus != SD_OK)
  {
    return errorstatus;
  }

  while(!(SDIO->STS & (SDIO_FLG_RXERRO | SDIO_FLG_DTFAIL | SDIO_FLG_DTTIMEOUT | SDIO_FLG_DTBLKCMPL | SDIO_FLG_SBITERR)))
  {
    if(SDIO_GetFlagStatus(SDIO, SDIO_FLG_RXBUF) != RESET) //接收FIFO数据可用
    {
      *(tempscr + index) = SDIO_ReadData(SDIO); //读取FIFO内容
      index++;
    }
  }

  if(SDIO_GetFlagStatus(SDIO, SDIO_FLG_DTTIMEOUT) != RESET) //数据超时错误
  {
    SDIO_ClearFlag(SDIO, SDIO_FLG_DTTIMEOUT);
    return SD_DATA_TIMEOUT;
  }
  else if(SDIO_GetFlagStatus(SDIO, SDIO_FLG_DTFAIL) != RESET)   //数据块CRC错误
  {
    SDIO_ClearFlag(SDIO, SDIO_FLG_DTFAIL);
    return SD_DATA_FAIL;
  }
  else if(SDIO_GetFlagStatus(SDIO, SDIO_FLG_RXERRO) != RESET)   //接收fifo上溢错误
  {
    SDIO_ClearFlag(SDIO, SDIO_FLG_RXERRO);
    return SD_RX_OVERRUN;
  }
  else if(SDIO_GetFlagStatus(SDIO, SDIO_FLG_SBITERR) != RESET)  //接收起始位错误
  {
    SDIO_ClearFlag(SDIO, SDIO_FLG_SBITERR);
    return SD_START_BIT_ERR;
  }

  SDIO_ClearFlag(SDIO, SDIO_STATIC_FLAGS); //清除所有标记

  return errorstatus;
}

/**
  * @brief  Set bus speed
  * @param  speed: 0,Normal speed; 1,High speed.
  * @retval SD_Error: SD Card Error code.
  */
SD_Error ChangeSpeed(u8 speed)
{
  SD_Error errorstatus = SD_OK;
  u8 SwitchSts[64];

  if (speed > 1)
  {
    return SD_ERROR;
  }

  if((SDIO_STD_CAPACITY_SD_CARD_V1_1 == CardType) || (SDIO_STD_CAPACITY_SD_CARD_V2_0 == CardType) || (SDIO_HIGH_CAPACITY_SD_CARD == CardType)) //判断卡类型
  {
    /* Version 1.01 Card does not support CMD6 */
    if (SDCardInfo.SD_scr.SD_SPEC == 0)
    {
      return SD_ERROR;
    }

    /* Check Group 1 function support speed*/
    errorstatus = SD_Switch(0, 0, speed, SwitchSts);

    if (errorstatus != 0)
    {
      return errorstatus;
    }

    if ((SwitchSts[13] & (1 << speed)) == 0)
    {
      return SD_ERROR;
    }

    errorstatus = SD_Switch(1, 0, speed, SwitchSts);

    if (errorstatus != 0)
    {
      return errorstatus;
    }

    /* Read it back for confirmation */
    if ((SwitchSts[16] & 0xF) != speed)
    {
      return SD_ERROR;
    }
  }
  else if(SDIO_HIGH_SPEED_MULTIMEDIA_CARD == CardType)
  {
    errorstatus = MMC_Switch(EXT_CSD_CMD_SET_NORMAL, EXT_CSD_BUS_WIDTH, (u8)speed);

    if (errorstatus != 0)
    {
      return errorstatus;
    }
  }

  return errorstatus;
}

/**
  * @brief  Converts the number of bytes in power of two and returns the power.
  * @param  NumberOfBytes: number of bytes.
  * @retval None
  */
u8 convert_from_bytes_to_power_of_two(u16 NumberOfBytes)
{
  u8 count = 0;

  while(NumberOfBytes != 1)
  {
    NumberOfBytes >>= 1;
    count++;
  }

  return count;
}

/**
  * @brief  Set DMA configuation for SDIO
  * @param  mbuf buffer address
  * @param  bufsize transmission data size
  * @param  DMA_DIR DMA direction, it is DMA_DIR_PERIPHERALDST(writing data) or DMA_DIR_PERIPHERALSRC(read data)
  * @retval None
  */
void SD_DMA_Config(u32*mbuf, u32 bufsize, u32 DMA_DIR)
{
  DMA_InitType DMA_InitStructure;

  RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_DMA2, ENABLE); //使能DMA2时钟

  DMA_Reset(DMA2_Channel4); //复位DMA2通道4
  DMA_ChannelEnable(DMA2_Channel4, DISABLE); //关闭DMA2通道4

  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&SDIO->BUF;  //DMA外设基地址
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)mbuf;  //DMA内存基地址
  DMA_InitStructure.DMA_Direction = DMA_DIR;  //数据传输方向，从内存读取发送到外设
  DMA_InitStructure.DMA_BufferSize = bufsize / 4; //DMA通道的DMA缓存的大小
  DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;  //外设地址寄存器不变
  DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_ENABLE;  //内存地址寄存器递增
  DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_WORD;  //数据宽度为32位
  DMA_InitStructure.DMA_MemoryDataWidth = DMA_MEMORYDATAWIDTH_WORD; //数据宽度为32位
  DMA_InitStructure.DMA_Mode = DMA_MODE_NORMAL;  //工作在正常缓存模式
  DMA_InitStructure.DMA_Priority = DMA_PRIORITY_HIGH; //DMA通道 x拥有高优先级
  DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;  //DMA通道x没有设置为内存到内存传输
  DMA_Init(DMA2_Channel4, &DMA_InitStructure);  //根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器

  DMA_ChannelEnable(DMA2_Channel4, ENABLE); //开启DMA2 通道4
}


/**
  * @}
  */

/**
  * @}
  */

