/*

  File:
  gpio.c

  Author(s):
  Guo Shuqiang, NationRFID
  

  Description:
  实现需要GPIO控制的逻辑处理.
*/

#include "gpio.h"

u8 SwitchAntenna(u8 input_channel);
/**
 *  \brief Handler for Button 1 rising edge interrupt.
 *
 *  Handle process led1 status change.
 */


/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_GPIOC);
	rcu_periph_clock_enable(RCU_GPIOD);
	rcu_periph_clock_enable(RCU_GPIOE);
	rcu_periph_clock_enable(RCU_GPIOG);
        rcu_periph_clock_enable(RCU_GPIOH);
        
	gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, VACP | RF_AMP_EN);
	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, VACP | RF_AMP_EN);

	gpio_mode_set(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, chip_rst);
	gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, chip_rst);


	gpio_mode_set(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SHUTEN|AZEN|nPWREN);
	gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, SHUTEN|AZEN|nPWREN);

	gpio_mode_set(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PLL_CE);		//  /*ADF4360*/
	gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, PLL_CE);
	
	gpio_mode_set(GPIOD, GPIO_MODE_INPUT, GPIO_PUPD_NONE, PLL_LOCK);	//  /*ADF4360*/
        //***

	gpio_mode_set(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED1_WORK);          //GPIOC
	gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LED1_WORK);
        
	gpio_mode_set(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, BEEP|LED_WORK);          //GPIOE
	gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, BEEP|LED_WORK);
        
	gpio_mode_set(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, RF1_POWER|RF2_POWER);            //POWER
	gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, RF1_POWER|RF2_POWER);
        
	gpio_mode_set(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, VCO_CLK|VCO_DATA | PLL_LE);	//  /*ADF4360*/
	gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, VCO_CLK|VCO_DATA | PLL_LE);

	gpio_mode_set(GPIOH, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, RFID_EN);            //POWER
	gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, RFID_EN);
}

/********函数：void gpio_init(void)***********************************
  功能描述：
      本函数对用于天线选择开关、RF滤波器选择开关、
  LED指示灯的GPIO进行初始化。
*********************************************************************/

void gpio_init(void)
{    
	GPIO_Configuration();
	 
	AZEN_L(); 
	BEEP_OFF();
	LED_WORK_ON();
        LED1_WORK_ON();
        
        RF_AMP_EN_H();
        RF1_POWER_H();   
        RF2_POWER_H();   
        RFID_EN_H();   
        nPWREN_H();   
}       
        
        
/*正确读卡指示*/
void goodread_led(void)
{
//  LED_WORK_ON();
}

/*读卡错误指示*/
void closeread_led(int flag)
{
  if (flag)
  {
//    LED_WORK_OF();
  }
	
}

void BEEP_ON(void)
{
	BEEP_H();
}
void BEEP_OFF(void)
{
	BEEP_L();
}




/*天线选择函数*/
/******函数:SwitchAntenna() *******************************************
*
*   功能描述:
*       此函数为天线端口选择切换函数，在单端口模块中没有作用
*
*   调用参数:	
*        input_channel  调用函数指定的天线通道
*
*   返回值:
*		 0     通道切换操作完成；
*        1     给定天线号无效；
*   函数代码:
*/
u8 SwitchAntenna(u8 input_channel)
{
//  READER.index_antenna = input_channel; 
//  Ntf_current_antid(READER.index_antenna); 
  return 0;
}



void LED_rf_pwron(unsigned char antid)
{  

}
void LED_rf_pwroff(void)
{ 
  
}

u8 set_gpo_state(u8 port, u8 state)
{

  return 0;
}








