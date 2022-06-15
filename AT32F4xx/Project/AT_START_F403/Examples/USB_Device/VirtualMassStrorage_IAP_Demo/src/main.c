/**
  ******************************************************************************
  * File   : USB_Device/VirtualMassStorage_IAP_Demo/src/main.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : VirtualMassStorage IAP Demo main file
  ******************************************************************************
  */
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "at32_board.h"
#include "FAT16.h"
#include "flash_if.h"
/** @addtogroup AT32F403_StdPeriph_Examples
  * @{
  */

/** @addtogroup USB_VirtualMassStrorage_IAP_Demo
  * @{
  */ 
extern uint32_t  Data_Buffer[];
uint8_t USBNeedReset = 0;

/**
  * @brief  Power on usb moudle.
  * @param  None
  * @retval USB_SUCCESS.
  */
void USB_PowerOn(void)
{
  uint16_t wRegVal;

  /*** cable plugged-in ? ***/
  //USB_Cable_Config(ENABLE);
  pInformation->Current_Configuration = 0;
  /*** CNTR_PWDN = 0 ***/
  wRegVal = CTRL_FRST;
  _SetCTRL(wRegVal);

  _SetCTRL(0);
  /*** Set interrupt mask ***/
  wInterrupt_Mask = IMR_MSK;
  _SetCTRL(wInterrupt_Mask);
  
}

/**
  * @brief  Power off usb moudle.
  * @param  None
  * @retval USB_SUCCESS.
  */
void USB_PowerOff()
{
  /* disable all interrupts and force USB reset */
  _SetCTRL(CTRL_FRST);
  /* clear interrupt status register */
  _SetINTSTS(0);
  /* Disable the Pull-Up*/
  /* switch-off device */
  _SetCTRL(CTRL_FRST + CTRL_PDWN);
  /* sw variables reset */
  /* ... */
}

/**
  * @brief  handles USB Reset
  * @param  None
  * @retval None.
  */
void USB_Reset()
{
    Delay_ms(1000);
    Delay_ms(1000);
    USB_PowerOff();
    Delay_ms(1000);
    Delay_ms(1000);
    pInformation->ControlState = 2;
    pInformation->Current_Configuration = 0;
    USB_PowerOn();
}

/**
  * @brief  loop msdStaus and create status file
  *         if download success or faild, need reset usb and return status file to PC
  * @param  None
  * @retval None
  */
void loop_msdStatus(void)
{
    static uint8_t msdTmp = UPGRADE_READY;
    if ( msdTmp == msd_UpStatus)
        return;
    else
        msdTmp = msd_UpStatus;
    
    switch (msd_UpStatus)
    {
        case UPGRADE_READY:
            FATSetStatusFileName("Ready");
            break;
        case UPGRADE_SUCCESS:
            FATSetStatusFileName("Success");
            msd_UpStatus = UPGREDE_DONE;
            USB_Reset();
            break;
        case UPGRADE_FAILED:
            FATSetStatusFileName("Failed");
            msd_UpStatus = UPGREDE_DONE;
            USB_Reset();
            break;
        case UPGRADE_LARGE:
            FATSetStatusFileName("Large");
            msd_UpStatus = UPGREDE_DONE;
            USB_Reset();
            break;
        case UPGRADE_UNKNOWN:
            FATSetStatusFileName("Unknown");
//            msd_UpStatus = UPGREDE_DONE;
//            USB_Reset();
            break;
        case UPGREDE_DONE:
            break;
        default:
            break;
    }
}

/**
  * @brief  main function
  * @param  None
  * @retval None
  */
int main(void)
{		
    /*AT START F4xx board initialize
    * Include LED, Button
    */
    AT32_Board_Init();
    
    
     /*Check if the key Push button*/
    if ( AT32_BUTTON_State(BUTTON_WAKEUP) == 0x00)
    {
        /*Check Flag*/
         if ( (AT_ReadOptionByte() & 0x1) == 0)
        {
            /*Jump to User Code*/
            IAP_Jump(FLASH_START_ADDR);
        }
    }
    /*USB GPIO configure*/
    AT32_USB_GPIO_init();
    
    /*Set USB Clock, USB Clock must 48MHz*/
    Set_USBClock();
    
    /*Enable USB Interrut*/
 	  USB_Interrupts_Config();    
   
    /*if use USB SRAM_Size = 768 Byte, default is 512 Byte*/
    //Set_USB768ByteMode();
    
    /* USB protocol and register initialize*/
 	  USB_Init(); 
    
    /*Set init status*/
    msd_UpStatus = UPGRADE_READY;

    
    /*Write optionbyte*/
    AT_WriteOptionByte(1);
 	  while(1)
	  {
        loop_msdStatus();
        Delay_ms(1000);
        AT32_LEDn_Toggle(LED4);
    }
}
/**
  * @}
  */ 

/**
  * @}
  */





















